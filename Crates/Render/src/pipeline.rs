use crate::*;

pub struct WgpuPipeline {
    pub(crate) pipeline: wgpu::RenderPipeline,
}

#[repr(C)]
pub enum WgpuVertexFormat {
    Uint8x4,
    Uint32,
    Float32,
    Float32x2,
    Float32x3,
    Float32x4,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub enum WgpuBlendFactor {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstColor,
    OneMinusDstColor,
    DstAlpha,
    OneMinusDstAlpha,
}

impl From<WgpuBlendFactor> for wgpu::BlendFactor {
    fn from(value: WgpuBlendFactor) -> Self {
        match value {
            WgpuBlendFactor::Zero => wgpu::BlendFactor::Zero,
            WgpuBlendFactor::One => wgpu::BlendFactor::One,
            WgpuBlendFactor::SrcColor => wgpu::BlendFactor::Src,
            WgpuBlendFactor::OneMinusSrcColor => wgpu::BlendFactor::OneMinusSrc,
            WgpuBlendFactor::SrcAlpha => wgpu::BlendFactor::SrcAlpha,
            WgpuBlendFactor::OneMinusSrcAlpha => wgpu::BlendFactor::OneMinusSrcAlpha,
            WgpuBlendFactor::DstColor => wgpu::BlendFactor::Dst,
            WgpuBlendFactor::OneMinusDstColor => wgpu::BlendFactor::OneMinusDst,
            WgpuBlendFactor::DstAlpha => wgpu::BlendFactor::DstAlpha,
            WgpuBlendFactor::OneMinusDstAlpha => wgpu::BlendFactor::OneMinusDstAlpha,
        }
    }
}

#[repr(C)]
pub struct WgpuVertexBufferDesc {
    // should be u64, but C++ refuses to align correctly
    stride: u32,
}

#[repr(C)]
pub struct WgpuVertexAttributeDesc {
    pub buffer_index: usize,
    pub shader_location: u32,
    pub format: WgpuVertexFormat,
    // should be u64, but C++ refuses to align correctly
    // (might be the overridden allocator?)
    pub offset: u32,
}

#[repr(C)]
pub struct WgpuShaderDesc {
    pub module: *const WgpuShaderModule,
    pub entry_point: WgpuString,
}

#[repr(C)]
pub struct WgpuPipelineDesc {
    pub buffers_ptr: *const WgpuVertexBufferDesc,
    pub buffers_len: usize,
    pub attributes_ptr: *const WgpuVertexAttributeDesc,
    pub attributes_len: usize,
    pub vertex_shader: *const WgpuShaderDesc,
    pub fragment_shader: *const WgpuShaderDesc,
    pub alpha_blend_enable: bool,
    pub src_blend: WgpuBlendFactor,
    pub dst_blend: WgpuBlendFactor,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_pipeline_create(
    device: *mut WgpuDevice,
    desc: *const WgpuPipelineDesc,
) -> *mut WgpuPipeline {
    let device = unsafe { &mut *device };
    let desc = unsafe { &*desc };

    let buffer_descs = unsafe { std::slice::from_raw_parts(desc.buffers_ptr, desc.buffers_len) };
    let attribute_descs =
        unsafe { std::slice::from_raw_parts(desc.attributes_ptr, desc.attributes_len) };
    let mut buffer_attributes = vec![vec![]; desc.buffers_len];

    // vertex layout
    for layout in attribute_descs {
        let format = match layout.format {
            WgpuVertexFormat::Uint8x4 => wgpu::VertexFormat::Uint8x4,
            WgpuVertexFormat::Uint32 => wgpu::VertexFormat::Uint32,
            WgpuVertexFormat::Float32 => wgpu::VertexFormat::Float32,
            WgpuVertexFormat::Float32x2 => wgpu::VertexFormat::Float32x2,
            WgpuVertexFormat::Float32x3 => wgpu::VertexFormat::Float32x3,
            WgpuVertexFormat::Float32x4 => wgpu::VertexFormat::Float32x4,
        };
        buffer_attributes[layout.buffer_index].push(wgpu::VertexAttribute {
            format,
            offset: layout.offset.into(),
            shader_location: layout.shader_location,
        });
    }

    let mut buffers = vec![];
    for index in 0..desc.buffers_len as usize {
        buffers.push(wgpu::VertexBufferLayout {
            array_stride: buffer_descs[index].stride.into(),
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &buffer_attributes[index],
        });
    }

    let pipeline = device
        .device
        .create_render_pipeline(&wgpu::RenderPipelineDescriptor {
            label: None,
            layout: Some(&device.pipeline_layout),
            vertex: wgpu::VertexState {
                module: unsafe { &(&*(&*desc.vertex_shader).module).module },
                entry_point: Some(&*unsafe { (&*desc.vertex_shader).entry_point.as_str() }),
                buffers: &buffers,
                compilation_options: Default::default(),
            },
            primitive: wgpu::PrimitiveState {
                topology: wgpu::PrimitiveTopology::TriangleList,
                ..Default::default()
            },
            depth_stencil: None,
            multisample: Default::default(),
            fragment: Some(wgpu::FragmentState {
                module: unsafe { &(&*(&*desc.fragment_shader).module).module },
                entry_point: Some(&*unsafe { (&*desc.fragment_shader).entry_point.as_str() }),
                compilation_options: Default::default(),
                targets: &[Some(wgpu::ColorTargetState {
                    format: wgpu::TextureFormat::Bgra8UnormSrgb,
                    blend: desc.alpha_blend_enable.then(|| {
                        let operation = wgpu::BlendOperation::Add;
                        let src_factor = desc.src_blend.into();
                        let dst_factor = desc.dst_blend.into();

                        wgpu::BlendState {
                            color: wgpu::BlendComponent {
                                operation,
                                src_factor,
                                dst_factor,
                            },
                            alpha: wgpu::BlendComponent {
                                operation,
                                src_factor,
                                dst_factor,
                            },
                        }
                    }),
                    write_mask: wgpu::ColorWrites::ALL,
                })],
            }),
            multiview: None,
            cache: None,
        });

    Box::into_raw(Box::new(WgpuPipeline { pipeline }))
}
#[unsafe(no_mangle)]
pub extern "C" fn wgpu_pipeline_destroy(info: *mut WgpuPipeline) {
    drop(unsafe { Box::from_raw(info) });
}
