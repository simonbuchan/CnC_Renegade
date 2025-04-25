use crate::imp::mixer::{Mixer, MixerCommand};
use crate::imp::source::SourceStaticData;
use anyhow::{Context, Result};
use cpal::traits::{DeviceTrait, HostTrait, StreamTrait};
use std::sync::{mpsc, Arc};

pub struct Output {
    device: cpal::Device,
    output_config: cpal::StreamConfig,
    output_stream: cpal::Stream,
    commands: mpsc::Sender<MixerCommand>,
}

impl Output {
    pub fn new() -> Result<Self> {
        let host = cpal::default_host();
        let device = host
            .default_output_device()
            .context("failed to create default")?;
        let output_config = device.default_output_config()?;
        let output_config = output_config.config();
        // output_config.buffer_size = cpal::BufferSize::Fixed(16 * 1024);
        let (commands, mut mixer) = Mixer::new(output_config.channels.into());

        let output_stream = device
            .build_output_stream(
                &output_config,
                move |output: &mut [f32], _info: &cpal::OutputCallbackInfo| {
                    mixer.write(output);
                },
                |error| {
                    eprintln!("stream error: {error}");
                },
                None,
            )
            .context("failed to build output stream")?;

        output_stream
            .play()
            .context("failed to start output stream")?;

        Ok(Self {
            device,
            output_config,
            output_stream,
            commands,
        })
    }

    pub fn output_sample_rate(&self) -> u32 {
        self.output_config.sample_rate.0
    }

    pub fn output_channels(&self) -> usize {
        self.output_config.channels as usize
    }

    pub fn play(&mut self, data: Arc<SourceStaticData>, gain: f32) {
        assert_eq!(data.info.sample_rate, self.output_sample_rate());
        self.commands
            .send(MixerCommand::Play { data, gain })
            .expect("send play failed");
    }
}
