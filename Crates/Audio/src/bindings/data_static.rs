use std::sync::Arc;
use crate::imp;
use crate::imp::source::DecodeStreamOptions;

pub struct AudioStaticData(pub(crate) Arc<imp::source::StaticData>);

#[unsafe(no_mangle)]
pub unsafe extern "C" fn audio_static_data_create(
    ptr: *const std::ffi::c_void,
    len: usize,
    sample_rate: u32,
) -> *mut AudioStaticData {
    let slice = unsafe { std::slice::from_raw_parts(ptr as *const u8, len) };
    let cursor = Box::new(std::io::Cursor::new(slice));
    let result = imp::source::StaticData::from_io(cursor, DecodeStreamOptions {
        output_sample_rate: sample_rate,
    });

    match result {
        Ok(data) => {
            let sample = Box::new(AudioStaticData(Arc::new(data)));
            Box::into_raw(sample)
        }
        Err(e) => {
            eprintln!("Failed to create audio sample: {e}");
            std::ptr::null_mut()
        }
    }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn audio_static_data_destroy(data: *mut AudioStaticData) {
    drop(unsafe { Box::from_raw(data) });
}

#[repr(C)]
pub struct AudioStaticDataInfo {
    pub duration: f32,
    pub sample_rate: u32,
    pub bit_depth: u16,
    pub channels: u16,
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn audio_static_data_info(data: *mut AudioStaticData) -> AudioStaticDataInfo {
    let data = &unsafe { &*data }.0;
    AudioStaticDataInfo {
        duration: (data.samples.len() / data.info.channels.count()) as f32 / data.info.sample_rate as f32,
        sample_rate: data.info.sample_rate,
        channels: data.info.channels.count().try_into().expect("u16"),
        bit_depth: 32,
    }
}
