use arrayvec::ArrayVec;
use polonius_the_crab::{polonius, polonius_return};
use std::collections::VecDeque;
use symphonia::core::audio::{AudioBuffer, AudioBufferRef, Signal};
use symphonia::core::conv::IntoSample;
use symphonia::core::sample::Sample;

pub struct Frames<'a> {
    pub samples: &'a [f32],
    pub channels: usize,
}

impl<'a> Frames<'a> {
    pub fn new(samples: &'a [f32], channels: usize) -> Self {
        Self { samples, channels }
    }
    
    pub fn len(&self) -> usize {
        self.samples.len() / self.channels
    }
}

impl<'a> IntoIterator for Frames<'a> {
    type Item = &'a [f32];
    type IntoIter = std::slice::ChunksExact<'a, f32>;

    fn into_iter(self) -> Self::IntoIter {
        self.samples.chunks_exact(self.channels)
    }
}

pub struct FramesMut<'a> {
    pub samples: &'a mut [f32],
    pub channels: usize,
}

impl<'a> FramesMut<'a> {
    pub fn new(samples: &'a mut [f32], channels: usize) -> Self {
        Self { samples, channels }
    }
    
    pub fn len(&self) -> usize {
        self.samples.len() / self.channels
    }
}

impl<'a> IntoIterator for FramesMut<'a> {
    type Item = &'a mut [f32];
    type IntoIter = std::slice::ChunksExactMut<'a, f32>;

    fn into_iter(self) -> Self::IntoIter {
        self.samples.chunks_exact_mut(self.channels)
    }
}

pub struct PlanarBuffer<const N: usize = 2> {
    planes: ArrayVec<VecDeque<f32>, N>,
}

impl<const N: usize> PlanarBuffer<N> {
    pub fn new(channels: usize) -> Self {
        assert!(channels > 0);
        assert!(channels <= N);
        let mut planes = ArrayVec::new();
        for _ in 0..channels {
            planes.push(VecDeque::new());
        }
        Self { planes }
    }

    pub fn with_capacity(channels: usize, capacity: usize) -> Self {
        assert!(channels > 0);
        assert!(channels <= N);
        let mut planes = ArrayVec::new();
        for _ in 0..channels {
            planes.push(VecDeque::with_capacity(capacity));
        }
        Self { planes }
    }

    pub fn len(&self) -> usize {
        self.planes[0].len()
    }

    pub fn planes(&self) -> &[VecDeque<f32>] {
        &self.planes
    }

    pub fn planes_mut(&mut self) -> &mut [VecDeque<f32>] {
        &mut self.planes
    }

    pub fn trim_start(&mut self, frames: usize) {
        for plane in self.planes.iter_mut() {
            let len = plane.len();
            if frames < len {
                plane.drain(0..frames);
            } else {
                plane.clear();
            }
        }
    }

    pub fn read_planes(&mut self, frames: usize) -> ArrayVec<&[f32], N> {
        fn head(mut plane: &mut VecDeque<f32>, min: usize) -> &[f32] {
            polonius!(|plane| -> &'polonius [f32] {
                let (head, _) = plane.as_slices();
                if min <= head.len() {
                    polonius_return!(head);
                }
            });
            plane.make_contiguous()
        }

        let mut result = ArrayVec::<&[f32], N>::new();
        for plane in self.planes.iter_mut() {
            result.push(head(plane, frames));
        }
        result
    }

    pub fn write_planes(&mut self, frames: usize) -> ArrayVec<&mut [f32], N> {
        fn tail(mut plane: &mut VecDeque<f32>, min: usize) -> &mut [f32] {
            polonius!(|plane| -> &'polonius mut [f32] {
                let (_, tail) = plane.as_mut_slices();
                if min <= tail.len() {
                    polonius_return!(tail);
                }
            });
            plane.make_contiguous()
        }

        let mut result = ArrayVec::<&mut [f32], N>::new();
        for plane in self.planes.iter_mut() {
            let len = plane.len();
            plane.resize(len + frames, 0.0);
            let tail = tail(plane, frames);
            let start = tail.len() - frames;
            result.push(&mut tail[start..]);
        }
        result
    }

    pub fn pop_front(&mut self) -> Option<ArrayVec<f32, N>> {
        let mut result = ArrayVec::<f32, N>::new();
        for plane in self.planes.iter_mut() {
            let sample = plane.pop_front()?;
            result.push(sample);
        }
        Some(result)
    }

    pub fn push_back(&mut self, frame: &[f32]) {
        for (plane, sample) in self.planes.iter_mut().zip(frame.into_iter().copied()) {
            plane.push_back(sample)
        }
    }

    pub fn append_decode_buffer_ref(&mut self, from: AudioBufferRef) {
        match from {
            AudioBufferRef::U8(buf) => self.append_decode_buffer(&buf),
            AudioBufferRef::U16(buf) => self.append_decode_buffer(&buf),
            AudioBufferRef::U24(buf) => self.append_decode_buffer(&buf),
            AudioBufferRef::U32(buf) => self.append_decode_buffer(&buf),
            AudioBufferRef::S8(buf) => self.append_decode_buffer(&buf),
            AudioBufferRef::S16(buf) => self.append_decode_buffer(&buf),
            AudioBufferRef::S24(buf) => self.append_decode_buffer(&buf),
            AudioBufferRef::S32(buf) => self.append_decode_buffer(&buf),
            AudioBufferRef::F32(buf) => self.append_decode_buffer(&buf),
            AudioBufferRef::F64(buf) => self.append_decode_buffer(&buf),
        }
    }

    fn append_decode_buffer<S: Sample + IntoSample<f32>>(&mut self, from: &AudioBuffer<S>) {
        assert_eq!(self.planes.len(), from.spec().channels.count());
        for (index, plane) in self.planes.iter_mut().enumerate() {
            let chan = from.chan(index);
            plane.reserve(chan.len());
            for sample in chan.iter().copied() {
                plane.push_back(sample.into_sample());
            }
        }
    }
}
