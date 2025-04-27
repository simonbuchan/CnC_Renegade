use crate::bindings::{AudioStaticData, AudioStream};
use crate::imp;

pub struct AudioOutput(pub(crate) imp::Output);

#[unsafe(no_mangle)]
pub unsafe extern "C" fn audio_output_create() -> *mut AudioOutput {
    match imp::Output::new() {
        Ok(output) => Box::into_raw(Box::new(AudioOutput(output))),
        Err(e) => {
            eprintln!("Failed to open audio output: {e}");
            std::ptr::null_mut()
        }
    }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn audio_output_destroy(ptr: *mut AudioOutput) {
    drop(unsafe { Box::from_raw(ptr) });
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn audio_output_sample_rate(ptr: *mut AudioOutput) -> u32 {
    let output = &unsafe { &*ptr }.0;
    output.output_sample_rate()
}

pub struct AudioTrackHandle(pub(crate) imp::MixerTrackHandle);

#[unsafe(no_mangle)]
pub unsafe extern "C" fn audio_output_play_static(
    ptr: *mut AudioOutput,
    data: *mut AudioStaticData,
) -> *mut AudioTrackHandle {
    let output = &mut unsafe { &mut *ptr }.0;
    let data = unsafe { &*data }.0.clone();
    let source = Box::new(data.into_source());
    let handle = output.mixer_handle().play(source);
    let handle = Box::new(AudioTrackHandle(handle));
    Box::into_raw(handle)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn audio_track_handle_destroy(ptr: *mut AudioTrackHandle) {
    drop(unsafe { Box::from_raw(ptr) });
}
