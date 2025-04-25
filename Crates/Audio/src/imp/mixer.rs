use crate::imp::source::{SourceChannels, SourceStaticData};
use std::sync::{mpsc, Arc};

struct MixerTrack {
    data: Arc<SourceStaticData>,
    pos: usize,
    gain: f32,
}

pub(crate) enum MixerCommand {
    Play {
        data: Arc<SourceStaticData>,
        gain: f32,
    },
}

pub(crate) struct Mixer {
    input: mpsc::Receiver<MixerCommand>,
    channel_count: usize,
    tracks: Vec<MixerTrack>,
}

impl Mixer {
    pub(crate) fn new(channel_count: usize) -> (mpsc::Sender<MixerCommand>, Self) {
        let (input, output) = mpsc::channel::<MixerCommand>();
        let result = Self {
            channel_count,
            tracks: vec![],
            input: output,
        };
        (input, result)
    }

    pub(crate) fn write(&mut self, output: &mut [f32]) {
        for command in self.input.try_iter() {
            match command {
                MixerCommand::Play { data, gain } => {
                    self.tracks.push(MixerTrack { data, gain, pos: 0 });
                }
            }
        }

        output.fill(0.0);
        let frames = output.chunks_exact_mut(self.channel_count);

        for frame in frames {
            self.tracks.retain_mut(|track| {
                match track.data.info.channels {
                    SourceChannels::Mono => {
                        frame[2] += track.data.samples[track.pos] * track.gain;
                    }
                    SourceChannels::Stereo => {
                        frame[0] += track.data.samples[track.pos * 2] * 0.5 * track.gain;
                        frame[1] += track.data.samples[track.pos * 2 + 1] * 0.5 * track.gain;
                    }
                }
                track.pos += 1;
                track.pos < track.data.samples.len() / track.data.info.channels.count()
            });
        }
    }
}
