use anyhow::{Context, Result, anyhow};
use nom::bytes::*;
use nom::multi::*;
use nom::number::*;
use nom::sequence::preceded;
use nom::{Finish, Parser};
use std::fs;
use std::path::{Path, PathBuf};

fn main() -> Result<()> {
    let mut args = std::env::args();
    args.next();
    let input_path = PathBuf::from(args.next().context("Missing input path")?);
    let output_dir_path = PathBuf::from(args.next().context("Missing output directory path")?);

    let data = fs::read(input_path)?;
    let file = mix_file(&data).map_err(|err| {
        use nom::HexDisplay;
        anyhow!(
            "Failed to parse header: {:?}\n{}",
            err,
            err.input.to_hex(16)
        )
    })?;
    assert_eq!(file.dir.len(), file.names.len());

    fs::create_dir_all(&output_dir_path)?;
    for (info, name) in file.dir.iter().zip(file.names.iter()) {
        println!(
            "{:32?}: {:08x} {:8x} {}",
            name, info.crc, info.offset, info.size
        );
        let output_path = output_dir_path.join(name.replace("\\", "_"));
        let output_data = &data[info.offset as usize..][..info.size as usize];
        fs::write(output_path, output_data)?;
    }
    Ok(())
}

#[derive(Debug)]
struct MixFile {
    dir: Vec<FileInfo>,
    names: Vec<String>,
}

fn mix_file(data: &[u8]) -> Result<MixFile, nom::error::Error<&[u8]>> {
    let (_, header) = header(&data).finish()?;
    let (_, dir) = length_count(le_u32(), file_info)
        .parse(&data[header.dir_offset as usize..])
        .finish()?;
    let (_, names) = length_count(le_u32(), file_name)
        .parse(&data[header.names_offset as usize..])
        .finish()?;
    let result = MixFile { dir, names };
    Ok(result)
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
