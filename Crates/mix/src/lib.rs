use std::ops::Range;

use anyhow::{ensure, Context, Result};
use nom::bytes::*;
use nom::multi::*;
use nom::number::{le_u32, le_u8};
use nom::sequence::preceded;
use nom::{Finish, Parser};

#[derive(Debug)]
pub struct File {
    pub data: Vec<u8>,
    pub dir: Directory,
}

impl File {
    pub fn new(data: Vec<u8>) -> Result<Self> {
        let dir = Directory::from_slice(&data)?;
        Ok(File { data, dir })
    }

    pub fn get(&self, name: &str) -> Result<&[u8]> {
        let entry = self
            .dir
            .get_entry(name)
            .with_context(|| format!("file not found: {}", name))?;
        Ok(self.entry_data(&entry))
    }

    pub fn entry_data(&self, entry: &Entry) -> &[u8] {
        let start = entry.data_range.start.try_into().expect("usize");
        let end = entry.data_range.end.try_into().expect("usize");
        &self.data[start..end]
    }
}

#[derive(Debug)]
pub struct Directory {
    pub entries: Vec<Entry>,
}

impl Directory {
    pub fn get_entry(&self, name: &str) -> Option<&Entry> {
        self.entries.iter().find(|entry| entry.name == name)
    }
}

#[derive(Debug)]
pub struct Entry {
    pub name: String,
    pub data_range: Range<u32>,
    pub crc: u32,
}

impl Directory {
    pub fn from_slice(data: &[u8]) -> Result<Self> {
        let (_, header) = header(&data).finish().map_err(nom_err).context("parsing mix header")?;
        let (_, dir) = length_count(le_u32(), file_info)
            .parse(&data[header.dir_offset as usize..])
            .finish()
            .map_err(nom_err)
            .context("parsing mix file info")?;
        let (_, names) = length_count(le_u32(), file_name)
            .parse(&data[header.names_offset as usize..])
            .finish()
            .map_err(nom_err)
            .context("parsing mix file names")?;
        ensure!(
            dir.len() == names.len(),
            "directory and names length mismatch: {} != {}",
            dir.len(),
            names.len(),
        );
        let entries = dir
            .iter()
            .zip(names.iter())
            .map(|(info, name)| Entry {
                name: name.to_owned(),
                data_range: info.offset..info.offset + info.size,
                crc: info.crc,
            })
            .collect();
        Ok(Directory { entries })
    }
}

fn nom_err(error: nom::error::Error<&[u8]>) -> anyhow::Error {
    anyhow::anyhow!("nom error: {:?}", error)
}

type IResult<'input, T> = nom::IResult<&'input [u8], T>;

#[derive(Debug)]
struct Header {
    dir_offset: u32,
    names_offset: u32,
}

fn header(input: &[u8]) -> IResult<'_, Header> {
    let (input, (dir_offset, names_offset)) =
        preceded(tag("MIX1"), (le_u32(), le_u32())).parse(input)?;
    let result = Header {
        dir_offset,
        names_offset,
    };
    Ok((input, result))
}

#[derive(Debug)]
struct FileInfo {
    crc: u32,
    offset: u32,
    size: u32,
}

fn file_info(input: &[u8]) -> IResult<'_, FileInfo> {
    let (input, (crc, offset, size)) = (le_u32(), le_u32(), le_u32()).parse(input)?;
    let result = FileInfo { crc, offset, size };
    Ok((input, result))
}

fn file_name(input: &[u8]) -> IResult<'_, String> {
    let (input, data) = length_data(le_u8()).parse(input)?;
    // data includes trailing \0 (sometimes?)
    let data = data.strip_suffix(b"\0").expect("trailing \\0");
    let name = String::from_utf8(data.to_owned()).unwrap();
    Ok((input, name))
}
