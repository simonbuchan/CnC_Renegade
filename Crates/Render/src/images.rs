use std::ffi::c_char;

#[repr(C)]
pub struct ImageRgbaData {
    pub width: u32,
    pub height: u32,
    pub data_ptr: *mut u8,
    pub data_len: usize,
}

// Currently unused, textures are loaded out of .mix files.
#[unsafe(no_mangle)]
pub extern "C" fn image_rgba_data_load(path: *const c_char) -> *mut ImageRgbaData {
    let path = unsafe { std::ffi::CStr::from_ptr(path) };
    let Ok(path) = path.to_str() else {
        return std::ptr::null_mut();
    };
    let Ok(image) = ::image::open(path) else {
        return std::ptr::null_mut();
    };
    let image = image.into_rgba8();
    let width = image.width();
    let height = image.height();
    let data = Box::into_raw(image.into_raw().into_boxed_slice());
    Box::into_raw(Box::new(ImageRgbaData {
        width,
        height,
        data_ptr: data.cast(),
        data_len: data.len(),
    }))
}

#[unsafe(no_mangle)]
pub extern "C" fn image_rgba_data_destroy(data: *mut ImageRgbaData) {
    let data = unsafe { Box::from_raw(data) };
    drop(unsafe {
        Box::from_raw(std::ptr::slice_from_raw_parts_mut(
            data.data_ptr,
            data.data_len,
        ))
    });
    drop(data);
}
