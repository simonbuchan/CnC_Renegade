use crate::imp::buffer::PlanarBuffer;
use crate::imp::source::SourceChannels;
use rubato::{FftFixedOut, Resampler as _};

pub struct ResamplerOptions {
    pub channels: SourceChannels,
    pub source_sample_rate: u32,
    pub output_sample_rate: u32,
}

pub struct Resampler {
    fft: FftFixedOut<f32>,
}

impl Resampler {
    pub fn new(options: &ResamplerOptions) -> Self {
        let fft = FftFixedOut::new(
            options.source_sample_rate.try_into().expect("usize"),
            options.output_sample_rate.try_into().expect("usize"),
            1024, // chunk_size_out
            1,    // sub_chunks,
            options.channels.count(),
        )
        .expect("fft should have valid options");
        Self { fft }
    }

    pub fn input_frames_max(&self) -> usize {
        self.fft.input_frames_max()
    }

    pub fn input_frames_next(&self) -> usize {
        self.fft.input_frames_next()
    }

    pub fn output_frames(&self) -> usize {
        self.fft.output_frames_max()
    }

    pub fn process(&mut self, input: &mut PlanarBuffer, output: &mut PlanarBuffer) {
        while self.process_once(input, output) {}
    }

    pub fn process_once(&mut self, input: &mut PlanarBuffer, output: &mut PlanarBuffer) -> bool {
        if self.fft.input_frames_next() > input.len() {
            return false;
        }
        let input_planes = input.read_planes(self.fft.input_frames_next());
        let mut output_planes = output.write_planes(self.fft.output_frames_next());
        let (input_read, _output_written) = self
            .fft
            .process_into_buffer(&input_planes, &mut output_planes, None)
            .expect("invalid buffers");
        drop(input_planes);
        input.trim_start(input_read);
        true
    }

    pub fn flush(&mut self, input: &mut PlanarBuffer, output: &mut PlanarBuffer) {
        for plane in input.planes_mut() {
            plane.resize(self.fft.input_frames_next(), 0.0);
        }
        let input_planes = input.read_planes(self.fft.input_frames_next());
        let mut output_planes = output.write_planes(self.fft.output_frames_next());
        let (input_read, _output_written) = self
            .fft
            .process_into_buffer(&input_planes, &mut output_planes, None)
            .expect("invalid buffers");
        drop(input_planes);
        input.trim_start(input_read);
    }
}
