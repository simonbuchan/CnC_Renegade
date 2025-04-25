use anyhow::Result;
use audio::imp::source::{DecodeStreamOptions, SourceStaticData, SourceStream};
use audio::imp::Output;
use std::fs;
use std::sync::Arc;

fn main() -> Result<()> {
    let mut engine = Output::new()?;
    let path = "Run/Unpacked/always_dat/00-n040e.wav";
    let file = fs::File::open(path)?;
    let mut stream = SourceStream::from_source(Box::new(file))?;

    let data = Arc::new(SourceStaticData::decode_from(
        &mut stream,
        DecodeStreamOptions {
            output_sample_rate: engine.output_sample_rate(),
        },
    )?);

    engine.play(data);

    // Wait for the sound to finish playing
    std::thread::sleep(std::time::Duration::from_secs(5));

    Ok(())
}
