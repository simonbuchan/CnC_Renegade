use crate::{WgpuBuffer, WgpuDevice, WgpuTexture};

pub struct WgpuBindGroup {
    pub(crate) bind_group: wgpu::BindGroup,
}

// hack: should be able to define bind group layouts
#[unsafe(no_mangle)]
pub extern "C" fn wgpu_bind_group_create_static(
    device: *mut WgpuDevice,
    buffer: *mut WgpuBuffer,
    offset: u64,
    size: u64,
) -> *mut WgpuBindGroup {
    let device = unsafe { &mut *device };
    let buffer = unsafe { &*buffer };
    let bind_group = device.device.create_bind_group(&wgpu::BindGroupDescriptor {
        label: None,
        layout: &device.static_bind_group_layout,
        entries: &[wgpu::BindGroupEntry {
            binding: 0,
            resource: wgpu::BindingResource::Buffer(wgpu::BufferBinding {
                buffer: &buffer.buffer,
                offset,
                size: std::num::NonZeroU64::new(size),
            }),
        }],
    });
    Box::into_raw(Box::new(WgpuBindGroup { bind_group }))
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_bind_group_create_texture(
    device: *mut WgpuDevice,
    texture: *mut WgpuTexture,
) -> *mut WgpuBindGroup {
    let device = unsafe { &mut *device };
    let texture = unsafe { &*texture };
    let bind_group = device.device.create_bind_group(&wgpu::BindGroupDescriptor {
        label: None,
        layout: &device.texture_bind_group_layout,
        entries: &[
            wgpu::BindGroupEntry {
                binding: 0,
                resource: wgpu::BindingResource::TextureView(
                    &texture
                        .texture
                        .create_view(&wgpu::TextureViewDescriptor::default()),
                ),
            },
            wgpu::BindGroupEntry {
                binding: 1,
                resource: wgpu::BindingResource::Sampler(&device.device.create_sampler(
                    &wgpu::SamplerDescriptor {
                        address_mode_u: wgpu::AddressMode::Repeat,
                        address_mode_v: wgpu::AddressMode::Repeat,
                        ..Default::default()
                    },
                )),
            },
        ],
    });
    Box::into_raw(Box::new(WgpuBindGroup { bind_group }))
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_bind_group_destroy(bind_group: *mut WgpuBindGroup) {
    drop(unsafe { Box::from_raw(bind_group) });
}
