#[repr(C)]
pub struct WgpuString {
    pub ptr: *const std::ffi::c_char,
    pub len: usize,
}

impl WgpuString {
    pub fn as_str(&self) -> std::borrow::Cow<str> {
        let slice = unsafe { std::slice::from_raw_parts(self.ptr.cast(), self.len) };
        String::from_utf8_lossy(slice)
    }
}

pub use bind_group::*;
pub use buffer::*;
pub use commands::*;
pub use device::*;
pub use instance::*;
pub use pipeline::*;
pub use shader_module::*;
pub use surface::*;
pub use texture::*;

mod bind_group;
mod buffer;
mod commands;
mod device;
mod images;
mod instance;
mod pipeline;
mod shader_module;
mod surface;
mod texture;
