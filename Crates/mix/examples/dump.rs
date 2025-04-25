use anyhow::{Context, Result};
use std::fs;
use std::path::PathBuf;

fn main() -> Result<()> {
    let mut args = std::env::args();
    args.next();
    let input_path = PathBuf::from(args.next().context("Missing input path")?);
    let output_dir_path = PathBuf::from(args.next().context("Missing output directory path")?);

    let data = fs::read(input_path).context("Reading input")?;
    let file = mix::File::new(data).context("Parsing input")?;

    fs::create_dir_all(&output_dir_path)?;
    for entry in &file.dir.entries {
        println!(
            "{:32?}: {:08x} {:8x} {}",
            entry.name, entry.crc, entry.data_range.start, entry.data_range.len(),
        );
        let output_path = output_dir_path.join(entry.name.replace("\\", "_"));
        let output_data = file.entry_data(&entry);
        fs::write(output_path, output_data)?;
    }
    Ok(())
}
