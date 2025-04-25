use crate::imp::buffer::PlanarBuffer;
use crate::imp::resampler::{Resampler, ResamplerOptions};
use anyhow::{bail, Context, Result};
use std::io;
use symphonia::core::codecs::{Decoder, DecoderOptions};
use symphonia::core::formats::{FormatOptions, FormatReader};
use symphonia::core::io::MediaSourceStream;
use symphonia::core::meta::MetadataOptions;
use symphonia::core::probe::Hint;
use symphonia::default::{get_codecs, get_probe};

#[derive(Debug, Copy, Clone)]
pub enum SourceChannels {
    Mono,
    Stereo,
}

impl SourceChannels {
    pub fn count(self) -> usize {
        match self {
            Self::Mono => 1,
            Self::Stereo => 2,
        }
    }
}

#[derive(Debug, Clone)]
pub struct SourceInfo {
    pub channels: SourceChannels,
    pub sample_rate: u32,
}

pub struct SourceStream {
    reader: Box<dyn FormatReader>,
    decoder: Box<dyn Decoder>,
    frames: usize,
    info: SourceInfo,
}

impl SourceStream {
    pub fn from_source(source: Box<dyn symphonia::core::io::MediaSource>) -> Result<Self> {
        let io = MediaSourceStream::new(source, Default::default());
        let probe = get_probe().format(
            &Hint::default(),
            io,
            &FormatOptions::default(),
            &MetadataOptions::default(),
        )?;
        let reader = probe.format;
        let track = reader.default_track().context("missing default track")?;
        let channels = track.codec_params.channels.context("no channels info")?;
        let channels = match channels.count() {
            1 => SourceChannels::Mono,
            2 => SourceChannels::Stereo,
            count => bail!("unsupported media channel count: {count}"),
        };
        let sample_rate = track
            .codec_params
            .sample_rate
            .context("no sample rate info")?;
        // let max_packet_frames = track
        //     .codec_params
        //     .max_frames_per_packet
        //     .context("no max frames per packet info")?
        //     .try_into()
        //     .expect("usize");
        let frames = track
            .codec_params
            .n_frames
            .context("no frames count info")?
            .try_into()
            .context("frame count as usize")?;

        let codec = get_codecs()
            .get_codec(track.codec_params.codec)
            .context("could not get codec")?;
        let decoder = (codec.inst_func)(&track.codec_params, &DecoderOptions::default())
            .context("could not create decoder")?;

        Ok(Self {
            reader,
            decoder,
            frames,
            info: SourceInfo {
                channels,
                sample_rate,
            },
        })
    }

    pub fn info(&self) -> &SourceInfo {
        &self.info
    }

    pub fn next_buffer(&mut self, output_buffer: &mut PlanarBuffer) -> Result<bool> {
        let packet = match self.reader.next_packet() {
            Err(symphonia::core::errors::Error::IoError(error))
                if error.kind() == io::ErrorKind::UnexpectedEof =>
            {
                return Ok(false);
            }
            Err(error) => Err(error).context("failed reading packet")?,
            Ok(packet) => packet,
        };
        let decode_buffer = self.decoder.decode(&packet)?;
        output_buffer.append_decode_buffer_ref(decode_buffer);
        Ok(true)
    }
}

pub struct DecodeStreamOptions {
    pub output_sample_rate: u32,
}

pub struct SourceStaticData {
    pub samples: Vec<f32>,
    pub info: SourceInfo,
}

impl SourceStaticData {
    pub fn from_source(
        source: Box<dyn symphonia::core::io::MediaSource>,
        options: DecodeStreamOptions,
    ) -> Result<Self> {
        let mut stream = SourceStream::from_source(source)?;
        Self::decode_from(&mut stream, options)
    }

    pub fn decode_from(stream: &mut SourceStream, options: DecodeStreamOptions) -> Result<Self> {
        let mut resampler = Resampler::new(&ResamplerOptions {
            channels: stream.info.channels,
            source_sample_rate: stream.info.sample_rate,
            output_sample_rate: options.output_sample_rate,
        });

        let mut output_buffer = PlanarBuffer::new(stream.info.channels.count());

        let mut samples = Vec::with_capacity(
            usize::try_from(
                stream.frames as u64 * u64::from(options.output_sample_rate)
                    / u64::from(stream.info.sample_rate),
            )
            .expect("samples usize"),
        );

        let mut input_buffer = PlanarBuffer::new(stream.info.channels.count());
        while stream.next_buffer(&mut input_buffer)? {
            resampler.process(&mut input_buffer, &mut output_buffer);
            while let Some(frame) = output_buffer.pop_front() {
                samples.extend_from_slice(&frame);
            }
        }

        if input_buffer.len() != 0 {
            // flush the resampler
            resampler.flush(&mut input_buffer, &mut output_buffer);
            while let Some(frame) = output_buffer.pop_front() {
                samples.extend_from_slice(&frame);
            }
        }

        Ok(Self {
            samples,
            info: SourceInfo {
                channels: stream.info.channels,
                sample_rate: options.output_sample_rate,
            },
        })
    }
}
