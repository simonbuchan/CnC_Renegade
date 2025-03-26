use crate::*;
use std::ffi::c_char;

pub struct WgpuInstance {
    pub(crate) wgpu: wgpu::Instance,
    pub(crate) adapters: Vec<wgpu::Adapter>,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_instance_create() -> *mut WgpuInstance {
    let wgpu = wgpu::Instance::default();
    let adapters = wgpu.enumerate_adapters(wgpu::Backends::VULKAN);
    Box::into_raw(Box::new(WgpuInstance { wgpu, adapters }))
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_instance_destroy(instance: *mut WgpuInstance) {
    drop(unsafe { Box::from_raw(instance) });
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_instance_adapter_count(instance: *mut WgpuInstance) -> u32 {
    unsafe { &*instance }.adapters.len() as u32
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_instance_adapter_supports_surface(
    instance: *mut WgpuInstance,
    index: u32,
    surface: *mut WgpuSurface,
) -> bool {
    let instance = unsafe { &*instance };
    let adapter = &instance.adapters[index as usize];
    let surface = unsafe { &*surface };
    adapter.is_surface_supported(&surface.surface)
}
#[repr(C)]
pub struct WgpuAdapterInfo {
    pub driver: [c_char; 256],
    pub name: [c_char; 256],
    pub vendor_id: u32,
    pub device_id: u32,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_instance_adapter_id(
    instance: *mut WgpuInstance,
    index: u32,
) -> WgpuAdapterInfo {
    let adapter = &unsafe { &*instance }.adapters[index as usize];
    let info = adapter.get_info();
    let mut result = WgpuAdapterInfo {
        driver: [0; 256],
        name: [0; 256],
        vendor_id: info.vendor,
        device_id: info.device,
    };
    let driver = info.driver.as_bytes();
    let name = info.name.as_bytes();
    for i in 0..driver.len().min(256) {
        result.driver[i] = driver[i] as c_char;
    }
    for i in 0..name.len().min(256) {
        result.name[i] = name[i] as c_char;
    }
    result
}
