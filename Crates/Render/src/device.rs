use crate::*;
use pollster::FutureExt;

pub struct WgpuDevice {
    pub(crate) adapter: wgpu::Adapter,
    pub(crate) device: wgpu::Device,
    pub(crate) queue: wgpu::Queue,
    pub(crate) pipeline_layout: wgpu::PipelineLayout,
    pub(crate) static_bind_group_layout: wgpu::BindGroupLayout,
    pub(crate) texture_bind_group_layout: wgpu::BindGroupLayout,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_device_create(instance: *mut WgpuInstance, adapter: u32) -> *mut WgpuDevice {
    let adapter = &unsafe { &*instance }.adapters[adapter as usize];
    match adapter
        .request_device(
            &wgpu::DeviceDescriptor {
                // DXTn support (known as BCn outside D3D)
                required_features: wgpu::Features::TEXTURE_COMPRESSION_BC,
                ..Default::default()
            },
            None,
        )
        .block_on()
    {
        Err(error) => {
            eprintln!("Failed to create device: {:?}", error);
            std::ptr::null_mut()
        }
        Ok((device, queue)) => {
            let adapter = adapter.clone();

            let static_bind_group_layout =
                device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
                    label: Some("static"),
                    entries: &[wgpu::BindGroupLayoutEntry {
                        binding: 0,
                        visibility: wgpu::ShaderStages::VERTEX | wgpu::ShaderStages::FRAGMENT,
                        ty: wgpu::BindingType::Buffer {
                            ty: wgpu::BufferBindingType::Uniform,
                            has_dynamic_offset: true,
                            min_binding_size: None,
                        },
                        count: None,
                    }],
                });

            let texture_bind_group_layout =
                device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
                    label: Some("texture"),
                    entries: &[
                        wgpu::BindGroupLayoutEntry {
                            binding: 0,
                            visibility: wgpu::ShaderStages::FRAGMENT,
                            ty: wgpu::BindingType::Texture {
                                sample_type: wgpu::TextureSampleType::Float { filterable: true },
                                view_dimension: wgpu::TextureViewDimension::D2,
                                multisampled: false,
                            },
                            count: None,
                        },
                        wgpu::BindGroupLayoutEntry {
                            binding: 1,
                            visibility: wgpu::ShaderStages::FRAGMENT,
                            ty: wgpu::BindingType::Sampler(wgpu::SamplerBindingType::Filtering),
                            count: None,
                        },
                    ],
                });

            let pipeline_layout = device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                label: None,
                bind_group_layouts: &[
                    &static_bind_group_layout,  // uniforms
                    &texture_bind_group_layout, // texture stage 0
                    &texture_bind_group_layout, // texture stage 1
                ],
                push_constant_ranges: &[],
            });

            Box::into_raw(Box::new(WgpuDevice {
                adapter,
                device,
                queue,
                pipeline_layout,
                static_bind_group_layout,
                texture_bind_group_layout,
            }))
        }
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_device_destroy(device: *mut WgpuDevice) {
    drop(unsafe { Box::from_raw(device) });
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_device_submit(device: *mut WgpuDevice, commands: *mut WgpuCommands) {
    let device = unsafe { &mut *device };
    let commands = unsafe { &mut *commands };
    drop(commands.render_pass.take());
    let commands = std::mem::replace(
        &mut commands.encoder,
        device
            .device
            .create_command_encoder(&wgpu::CommandEncoderDescriptor::default()),
    );
    device.queue.submit([commands.finish()]);
}
