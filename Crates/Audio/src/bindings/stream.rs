use super::file::AudioFile;
use super::{AudioFileApi, AudioFileHandle};
use crate::imp;

pub struct AudioStream(imp::source::SourceStream);

#[unsafe(no_mangle)]
pub unsafe extern "C" fn audio_stream_create(
    api: AudioFileApi,
    handle: AudioFileHandle,
) -> *mut AudioStream {
    let file = Box::new(AudioFile { api, handle });
    let result = imp::source::SourceStream::from_source(file);
    match result {
        Ok(stream) => {
            let stream = Box::new(AudioStream(stream));
            Box::into_raw(stream)
        }
        Err(e) => {
            eprintln!("Failed to create audio stream: {e}");
            std::ptr::null_mut()
        }
    }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn audio_stream_destroy(
    ptr: *mut AudioStream,
) {
    drop(unsafe { Box::from_raw(ptr) });
}
