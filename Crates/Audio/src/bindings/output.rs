use crate::bindings::AudioStaticData;
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

#[unsafe(no_mangle)]
pub unsafe extern "C" fn audio_output_play(
    ptr: *mut AudioOutput,
    data: *mut AudioStaticData,
    gain: f32,
) {
    let output = &mut unsafe { &mut *ptr }.0;
    let data = &unsafe { &*data }.0;
    output.play(data.clone(), gain);
}
