use pollster::FutureExt;
use crate::*;

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_shader_module_create_from_wgsl(
    device: *mut WgpuDevice,
    source: WgpuString,
) -> *mut WgpuShaderModule {
    let device = unsafe { &mut *device };
    device
        .device
        .push_error_scope(wgpu::ErrorFilter::Validation);
    let module = device
        .device
        .create_shader_module(wgpu::ShaderModuleDescriptor {
            label: None,
            source: wgpu::ShaderSource::Wgsl(source.as_str()),
        });
    if let Some(error) = device.device.pop_error_scope().block_on() {
        eprintln!("Failed to create shader module: {:?}", error);
        return std::ptr::null_mut();
    }
    Box::into_raw(Box::new(WgpuShaderModule { module }))
}

pub struct WgpuShaderModule {
    pub(crate) module: wgpu::ShaderModule,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_shader_module_destroy(module: *mut WgpuShaderModule) {
    drop(unsafe { Box::from_raw(module) });
}
