use crate::*;

pub struct WgpuBuffer {
    pub(crate) queue: wgpu::Queue,
    pub(crate) buffer: wgpu::Buffer,
}

pub const WGPU_BUFFER_USAGE_UNIFORM: u32 = 1 << 0;
pub const WGPU_BUFFER_USAGE_VERTEX: u32 = 1 << 1;
pub const WGPU_BUFFER_USAGE_INDEX: u32 = 1 << 2;

fn to_buffer_usages(value: u32) -> wgpu::BufferUsages {
    let mut result = wgpu::BufferUsages::empty();
    if value & WGPU_BUFFER_USAGE_VERTEX != 0 {
        result |= wgpu::BufferUsages::VERTEX;
    }
    if value & WGPU_BUFFER_USAGE_INDEX != 0 {
        result |= wgpu::BufferUsages::INDEX;
    }
    if value & WGPU_BUFFER_USAGE_UNIFORM != 0 {
        result |= wgpu::BufferUsages::UNIFORM;
    }
    result
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_buffer_create(
    device: *mut WgpuDevice,
    size: u64,
    usage: u32,
) -> *mut WgpuBuffer {
    let device = unsafe { &mut *device };
    let queue = device.queue.clone();
    let buffer = device.device.create_buffer(&wgpu::BufferDescriptor {
        label: None,
        size,
        usage: to_buffer_usages(usage) | wgpu::BufferUsages::COPY_DST,
        mapped_at_creation: false,
    });
    Box::into_raw(Box::new(WgpuBuffer { queue, buffer }))
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_buffer_destroy(buffer: *mut WgpuBuffer) {
    drop(unsafe { Box::from_raw(buffer) });
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_buffer_write(
    buffer: *mut WgpuBuffer,
    offset: u64,
    data: *const u8,
    len: usize,
) {
    let buffer = unsafe { &mut *buffer };
    let data = unsafe { std::slice::from_raw_parts(data, len) };
    buffer.queue.write_buffer(&buffer.buffer, offset, data);
}
