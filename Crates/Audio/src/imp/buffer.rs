use arrayvec::ArrayVec;
use polonius_the_crab::{polonius, polonius_return};
use std::collections::VecDeque;
use symphonia::core::audio::{AudioBuffer, AudioBufferRef, Signal};
use symphonia::core::conv::IntoSample;
use symphonia::core::sample::Sample;

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

// multi-planar ring-buffer of samples, like a contiguous Vec<VecDeque<>>.
// Unfortunately, not worky very well right now.
#[derive(Default)]
struct Buffer {
    samples: Box<[f32]>,
    capacity: usize,
    // channels = samples.len() / capacity
    // 0 <= start < capacity
    start: usize,
    // start <= end <= capacity + start
    end: usize,
    // for channel 0, sample data is:
    //   end <= capacity: [start..end]
    //   capacity <= end: [start..capacity] then [0..end - capacity]
    // for channel n, offset by capacity * n
}

impl Buffer {
    pub fn with_capacity(capacity: usize, channels: usize) -> Self {
        if capacity == 0 {
            return Self::default();
        }
        Self {
            samples: vec![0f32; capacity * channels].into_boxed_slice(),
            capacity,
            start: 0,
            end: 0,
        }
    }

    fn ensure_capacity(&mut self, min_capacity: usize) {
        if min_capacity <= self.capacity {
            return;
        }
        let new_capacity = (self.capacity * 2).max(min_capacity.next_power_of_two());
        let channels = self.channels();
        let mut new_samples = vec![0f32; new_capacity * channels].into_boxed_slice();
        let len = self.end - self.start;

        for index in 0..channels {
            let old_chan = &self.samples[self.capacity * index..][..self.capacity];
            let new_chan = &mut new_samples[new_capacity * index..][..new_capacity];
            if let Some(wrap) = self.end.checked_sub(self.capacity) {
                let mid = self.capacity - self.start;
                new_chan[..mid].copy_from_slice(&old_chan[self.start..]);
                new_chan[mid..].copy_from_slice(&old_chan[..wrap]);
            } else {
                new_chan[..len].copy_from_slice(&old_chan[self.start..self.end]);
            }
        }
        self.samples = new_samples;
        self.capacity = new_capacity;
        self.start = 0;
        self.end = len;
    }

    fn append_decode_buffer_ref(&mut self, from: AudioBufferRef) {
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
        if self.capacity == 0 {
            // no channels to multiply
            self.capacity = from.frames().next_power_of_two();
            self.samples = vec![0.0f32; self.capacity].into_boxed_slice();
        } else {
            assert_eq!(self.channels(), from.spec().channels.count());
            let new_len = self.len() + from.frames();
            self.ensure_capacity(new_len);
        }
        // write to start..self.end (which may be entire before the wrap, straddle, on entirely after)
        let start = self.end;
        self.end += from.frames();
        if self.end <= self.capacity {
            let end = self.end;
            for index in 0..self.channels() {
                let from = from.chan(index);
                let to_raw = self.chan_raw_mut(index);
                let to = &mut to_raw[start..end];
                for (to_sample, &from_sample) in to.iter_mut().zip(from) {
                    *to_sample = from_sample.into_sample();
                }
            }
        } else if self.capacity <= start {
            let start = start - self.capacity;
            let end = self.end - self.capacity;
            for index in 0..self.channels() {
                let from = from.chan(index);
                let to_raw = self.chan_raw_mut(index);
                let to = &mut to_raw[start..end];
                for (to_sample, &from_sample) in to.iter_mut().zip(from) {
                    *to_sample = from_sample.into_sample();
                }
            }
        } else {
            // straddle, need to split the write
            let end = self.end - self.capacity;
            for index in 0..self.channels() {
                let (from_head, from_tail) = from.chan(index).split_at(self.capacity - start);
                let to_raw = self.chan_raw_mut(index);
                let to_head = &mut to_raw[start..];
                for (to_sample, &from_sample) in to_head.iter_mut().zip(from_head) {
                    *to_sample = from_sample.into_sample();
                }
                let to_tail = &mut to_raw[..end];
                for (to_sample, &from_sample) in to_tail.iter_mut().zip(from_tail) {
                    *to_sample = from_sample.into_sample();
                }
            }
        }
    }

    pub fn channels(&self) -> usize {
        assert_ne!(self.capacity, 0, "not initialized");
        self.samples.len() / self.capacity
    }

    pub fn capacity(&self) -> usize {
        self.capacity
    }

    pub fn len(&self) -> usize {
        self.end - self.start
    }

    pub fn head_len(&self) -> usize {
        self.capacity.min(self.end) - self.start
    }

    pub fn tail_len(&self) -> usize {
        self.end.saturating_sub(self.capacity)
    }

    pub fn chan_raw(&self, index: usize) -> &[f32] {
        let capacity = self.capacity;
        &self.samples[capacity * index..][..capacity]
    }

    pub fn chan_raw_mut(&mut self, index: usize) -> &mut [f32] {
        let capacity = self.capacity;
        &mut self.samples[capacity * index..][..capacity]
    }

    pub fn chan(&self, index: usize) -> (&[f32], &[f32]) {
        let capacity = self.capacity;
        let raw = self.chan_raw(index);
        if let Some(wrap) = self.end.checked_sub(capacity) {
            (&raw[self.start..], &raw[..wrap])
        } else {
            (&raw[self.start..self.end], &[])
        }
    }

    pub fn chan_mut(&mut self, index: usize) -> (&mut [f32], &mut [f32]) {
        let capacity = self.capacity;
        let start = self.start;
        let end = self.end;
        let raw = self.chan_raw_mut(index);
        if let Some(wrap) = end.checked_sub(capacity) {
            let (tail, head) = raw.split_at_mut(start);
            (head, &mut tail[..wrap])
        } else {
            (&mut raw[start..end], &mut [])
        }
    }

    /// Append to planes the first len samples removed from the start of each channel.
    pub fn read_planes<'buf, const N: usize>(
        &'buf mut self,
        planes: &mut ArrayVec<&'buf [f32], N>,
        len: usize,
    ) {
        assert!(len <= self.len());
        let channels = self.channels();
        if self.head_len() < len {
            let mut samples = &mut self.samples[..];
            for _ in 0..channels {
                let (raw, tail) = samples.split_at_mut(self.capacity);
                samples = tail;
                raw.rotate_left(self.start);
                planes.push(&raw[..len]);
            }
            self.end -= self.start;
            self.start = 0;
        } else {
            let mut samples = &self.samples[..];
            for _ in 0..channels {
                let (raw, tail) = samples.split_at(self.capacity);
                samples = tail;
                planes.push(&raw[self.start..][..len]);
            }
            self.start += len;
            // preserve the start and end invariants
            if self.start >= self.capacity {
                self.start -= self.capacity;
                self.end -= self.capacity;
            }
        }
    }

    /// Append to planes the first len samples after the end of each channel.
    /// Extends capacity if needed.
    pub fn write_planes<'buf, const N: usize>(
        &'buf mut self,
        planes: &mut ArrayVec<&'buf mut [f32], N>,
        len: usize,
    ) {
        self.ensure_capacity(self.len() + len);
        let end = self.end;
        self.end += len;
        let channels = self.channels();
        let mut samples = &mut self.samples[..];
        for _ in 0..channels {
            let (raw, tail) = samples.split_at_mut(self.capacity);
            samples = tail;
            planes.push(&mut raw[end..][..len]);
        }
    }

    pub fn clear(&mut self) {
        self.start = 0;
        self.end = 0;
    }

    pub fn trim(&mut self, len: usize) {
        if len >= self.len() {
            self.clear();
            return;
        }
        self.start += len;
        if self.start >= self.capacity {
            self.start -= self.capacity;
            self.end -= self.capacity;
        }
    }
}
