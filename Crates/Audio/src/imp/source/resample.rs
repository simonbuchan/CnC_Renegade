use crate::imp::buffer::{Frames, FramesMut, PlanarBuffer};
use crate::imp::resampler::{Resampler, ResamplerOptions};
use crate::imp::source::{RenderResult, Source, SourceInfo};

pub struct ResampleSource<S> {
    source: S,
    output_info: SourceInfo,
    input_interleaved: Vec<f32>,
    input_buffer: PlanarBuffer,
    output_buffer: PlanarBuffer,
    resampler: Resampler,
}

impl<S: Source> ResampleSource<S> {
    pub fn new(source: S, output_sample_rate: u32) -> Self {
        let SourceInfo {
            channels,
            sample_rate,
        } = *source.info();
        let resampler = Resampler::new(&ResamplerOptions {
            channels,
            source_sample_rate: sample_rate,
            output_sample_rate,
        });
        Self {
            source,
            output_info: SourceInfo {
                channels,
                sample_rate: output_sample_rate,
            },
            input_interleaved: vec![0.0f32; resampler.input_frames_max() * channels.count()],
            input_buffer: PlanarBuffer::new(channels.count()),
            output_buffer: PlanarBuffer::new(channels.count()),
            resampler,
        }
    }

    fn fill_output(&mut self, output_len: usize) -> bool {
        let channels = self.output_info.channels.count();

        while self.output_buffer.len() < output_len {
            self.input_interleaved.fill(0.0);
            self.source.render(FramesMut {
                samples: &mut self.input_interleaved,
                channels,
            });

            for frame in Frames::new(&self.input_interleaved, channels) {
                self.input_buffer.push_back(frame);
            }

            if !self.resampler.process_once(&mut self.input_buffer, &mut self.output_buffer) {
                return false;
            }
            self.resampler.process(&mut self.input_buffer, &mut self.output_buffer);
        }

        true
    }
}

impl<S: Source> Source for ResampleSource<S> {
    fn info(&self) -> &SourceInfo {
        &self.output_info
    }

    fn render(&mut self, output_frames: FramesMut) -> RenderResult {
        self.fill_output(output_frames.len());

        for output_frame in output_frames.into_iter() {
            let Some(frame) = self.output_buffer.pop_front() else {
                return RenderResult::Complete
            };
            for (output_sample, input_sample) in output_frame.iter_mut().zip(frame) {
                *output_sample += input_sample;
            }
        }

        RenderResult::Continue
    }
}
