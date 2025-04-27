use anyhow::Result;
use audio::imp::source::{DecodeStream, ResampleSource};
use audio::imp::Output;
use std::fs;

fn main() -> Result<()> {
    let engine = Output::new()?;
    let path = "Run/Unpacked/always_dat/menu.mp3";
    let file = fs::File::open(path)?;
    let stream = DecodeStream::from_io(Box::new(file))?;
    let source = stream.into_source();
    let source = ResampleSource::new(source, engine.output_sample_rate());
    let source = Box::new(source);

    let mut handle = engine.mixer_handle().play(source);
    println!("wait: {:?}", handle.wait());

    Ok(())
}
