use crate::imp::buffer::FramesMut;
use crate::imp::source::{RenderResult, Source};
use std::sync::mpsc;

#[derive(Copy, Clone, Debug, Eq, PartialEq)]
pub enum WaitResult {
    Complete,
    Dropped,
    AlreadyWaited,
}

pub struct MixerTrackHandle {
    complete: Option<mpsc::Receiver<()>>,
}

impl MixerTrackHandle {
    pub fn wait(&mut self) -> WaitResult {
        let Some(rx) = self.complete.take() else {
            return WaitResult::AlreadyWaited
        };
        match rx.recv() {
            Err(mpsc::RecvError) => WaitResult::Dropped,
            Ok(()) => WaitResult::Complete,
        }
    }
}

struct MixerTrack {
    source: Box<dyn Source>,
    complete: mpsc::Sender<()>,
}

enum MixerCommand {
    Play(MixerTrack),
}

pub(crate) struct Mixer {
    commands: mpsc::Receiver<MixerCommand>,
    channel_count: usize,
    tracks: Vec<MixerTrack>,
}

pub struct MixerHandle {
    commands: mpsc::Sender<MixerCommand>,
}

impl Mixer {
    pub(crate) fn new(channel_count: usize) -> (Self, MixerHandle) {
        let (tx, rx) = mpsc::channel::<MixerCommand>();
        let mixer = Self {
            channel_count,
            tracks: vec![],
            commands: rx,
        };
        let handle = MixerHandle {
            commands: tx,
        };
        (mixer, handle)
    }

    pub(crate) fn write(&mut self, output: &mut [f32]) {
        for command in self.commands.try_iter() {
            match command {
                MixerCommand::Play(track) => {
                    self.tracks.push(track);
                }
            }
        }

        output.fill(0.0);
        self.tracks.retain_mut(|track| {
            match track.source.render(FramesMut {
                samples: output,
                channels: self.channel_count,
            }) {
                RenderResult::Continue => true,
                RenderResult::Complete => {
                    let _ = track.complete.send(());
                    false
                },
            }
        });
    }
}

impl MixerHandle {
    pub fn play(
        &self,
        source: Box<dyn Source>,
    ) -> MixerTrackHandle {
        let (complete_tx, complete_rx) = mpsc::channel();
        
        let track = MixerTrack {
            source,
            complete: complete_tx,
        };
        self.commands
            .send(MixerCommand::Play(track))
            .expect("audio commands channel dropped");
        
        MixerTrackHandle {
            complete: Some(complete_rx),
        }
    }
}