use pollster::FutureExt as _;
use std::borrow::Cow;
use std::ffi::c_char;
use std::num::NonZeroIsize;
use windows::Win32::Foundation::HWND;
use windows::Win32::UI::WindowsAndMessaging::{GWLP_HINSTANCE, GetWindowLongPtrW};

#[repr(C)]
pub struct ImageRgbaData {
    pub width: u32,
    pub height: u32,
    pub data_ptr: *mut u8,
    pub data_len: usize,
}

#[unsafe(no_mangle)]
pub extern "C" fn image_rgba_data_load(path: *const c_char) -> *mut ImageRgbaData {
    let path = unsafe { std::ffi::CStr::from_ptr(path) };
    let Ok(path) = path.to_str() else {
        return std::ptr::null_mut();
    };
    let Ok(image) = image::open(path) else {
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

pub struct WgpuInstance {
    wgpu: wgpu::Instance,
    adapters: Vec<wgpu::Adapter>,
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

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_instance_device_create(
    instance: *mut WgpuInstance,
    adapter: u32,
) -> *mut WgpuDevice {
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
            let texture_bind_group_layout =
                device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
                    label: None,
                    entries: &[wgpu::BindGroupLayoutEntry {
                        binding: 0,
                        visibility: wgpu::ShaderStages::FRAGMENT,
                        ty: wgpu::BindingType::Texture {
                            sample_type: wgpu::TextureSampleType::Float { filterable: true },
                            view_dimension: wgpu::TextureViewDimension::D2,
                            multisampled: false,
                        },
                        count: None,
                    }],
                });
            Box::into_raw(Box::new(WgpuDevice {
                adapter,
                device,
                queue,
                texture_bind_group_layout
            }))
        }
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_instance_surface_create(
    instance: *mut WgpuInstance,
    hwnd: isize,
) -> *mut WgpuSurface {
    let instance = unsafe { &*instance };

    // required for Vulkan backend
    let hinstance = unsafe { GetWindowLongPtrW(HWND(hwnd as _), GWLP_HINSTANCE) };
    let Some(hinstance) = NonZeroIsize::new(hinstance.try_into().expect("isize from i32")) else {
        return std::ptr::null_mut();
    };

    let Some(hwnd) = NonZeroIsize::new(hwnd) else {
        return std::ptr::null_mut();
    };
    let mut hwnd = wgpu::rwh::Win32WindowHandle::new(hwnd);
    hwnd.hinstance = Some(hinstance);
    let target = wgpu::SurfaceTargetUnsafe::RawHandle {
        raw_display_handle: wgpu::rwh::RawDisplayHandle::Windows(
            wgpu::rwh::WindowsDisplayHandle::new(),
        ),
        raw_window_handle: wgpu::rwh::RawWindowHandle::Win32(hwnd),
    };
    let result = unsafe { instance.wgpu.create_surface_unsafe(target) };

    match result {
        Ok(surface) => Box::into_raw(Box::new(WgpuSurface {
            surface,
            texture: None,
            texture_view: None,
        })),
        Err(error) => {
            eprintln!("Failed to create surface: {:?}", error);
            std::ptr::null_mut()
        }
    }
}

pub struct WgpuDevice {
    adapter: wgpu::Adapter,
    device: wgpu::Device,
    queue: wgpu::Queue,
    texture_bind_group_layout: wgpu::BindGroupLayout,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_device_destroy(device: *mut WgpuDevice) {
    drop(unsafe { Box::from_raw(device) });
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_device_create_commands(device: *mut WgpuDevice) -> *mut WgpuCommands {
    let device = unsafe { &mut *device };
    let encoder = device
        .device
        .create_command_encoder(&wgpu::CommandEncoderDescriptor::default());
    Box::into_raw(Box::new(WgpuCommands {
        encoder,
        render_pass: None,
    }))
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
pub extern "C" fn wgpu_device_create_buffer(
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

#[repr(C)]
pub enum WgpuTextureFormat {
    Rgba8Unorm,
    Bc1RgbaUnorm,
    Bc2RgbaUnorm,
    Bc3RgbaUnorm,
}

pub const WGPU_TEXTURE_USAGE_RENDER_ATTACHMENT: u32 = 1 << 0;
pub const WGPU_TEXTURE_USAGE_TEXTURE_BINDING: u32 = 1 << 1;
pub const WGPU_TEXTURE_USAGE_COPY_SRC: u32 = 1 << 2;
pub const WGPU_TEXTURE_USAGE_COPY_DST: u32 = 1 << 3;

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_device_create_texture(
    device: *mut WgpuDevice,
    format: WgpuTextureFormat,
    width: u32,
    height: u32,
    mip_levels: u32,
    usages: u32,
) -> *mut WgpuTexture {
    let device = unsafe { &mut *device };
    let texture = device.device.create_texture(&wgpu::TextureDescriptor {
        label: None,
        size: wgpu::Extent3d {
            width,
            height,
            depth_or_array_layers: 1,
        },
        mip_level_count: mip_levels,
        sample_count: 1,
        dimension: wgpu::TextureDimension::D2,
        format: match format {
            WgpuTextureFormat::Rgba8Unorm => wgpu::TextureFormat::Rgba8Unorm,
            WgpuTextureFormat::Bc1RgbaUnorm => wgpu::TextureFormat::Bc1RgbaUnorm,
            WgpuTextureFormat::Bc2RgbaUnorm => wgpu::TextureFormat::Bc2RgbaUnorm,
            WgpuTextureFormat::Bc3RgbaUnorm => wgpu::TextureFormat::Bc3RgbaUnorm,
        },
        usage: {
            let mut usage = wgpu::TextureUsages::empty();
            if usages & WGPU_TEXTURE_USAGE_RENDER_ATTACHMENT != 0 {
                usage |= wgpu::TextureUsages::RENDER_ATTACHMENT;
            }
            if usages & WGPU_TEXTURE_USAGE_TEXTURE_BINDING != 0 {
                usage |= wgpu::TextureUsages::TEXTURE_BINDING;
            }
            if usages & WGPU_TEXTURE_USAGE_COPY_SRC != 0 {
                usage |= wgpu::TextureUsages::COPY_SRC;
            }
            if usages & WGPU_TEXTURE_USAGE_COPY_DST != 0 {
                usage |= wgpu::TextureUsages::COPY_DST;
            }
            usage
        },
        view_formats: &[],
    });
    let queue = device.queue.clone();
    let bind_group = device.device.create_bind_group(&wgpu::BindGroupDescriptor {
        label: None,
        layout: &device.texture_bind_group_layout,
        entries: &[wgpu::BindGroupEntry {
            binding: 0,
            resource: wgpu::BindingResource::TextureView(
                &texture.create_view(&wgpu::TextureViewDescriptor::default()),
            ),
        }],
    });
    Box::into_raw(Box::new(WgpuTexture { queue, texture, bind_group }))
}

#[repr(C)]
pub struct WgpuString {
    pub ptr: *const c_char,
    pub len: usize,
}

impl WgpuString {
    pub fn as_str(&self) -> Cow<str> {
        let slice = unsafe { std::slice::from_raw_parts(self.ptr.cast(), self.len) };
        String::from_utf8_lossy(slice)
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_device_create_wgsl_shader_module(
    device: *mut WgpuDevice,
    source: WgpuString,
) -> *mut WgpuShaderModule {
    let device = unsafe { &mut *device };
    let module = device
        .device
        .create_shader_module(wgpu::ShaderModuleDescriptor {
            label: None,
            source: wgpu::ShaderSource::Wgsl(source.as_str()),
        });
    Box::into_raw(Box::new(WgpuShaderModule { module }))
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
pub struct WgpuVertexBufferLayout {
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
    pub stride: u32,
    pub layout_ptr: *const WgpuVertexBufferLayout,
    pub layout_len: usize,
    pub vertex_shader: *const WgpuShaderDesc,
    pub fragment_shader: *const WgpuShaderDesc,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_device_create_pipeline(
    device: *mut WgpuDevice,
    desc: *const WgpuPipelineDesc,
) -> *mut WgpuPipeline {
    let device = unsafe { &mut *device };
    let desc = unsafe { &*desc };

    let layout_descs = unsafe { std::slice::from_raw_parts(desc.layout_ptr, desc.layout_len) };
    let mut attributes = vec![];

    for layout in layout_descs {
        let format = match layout.format {
            WgpuVertexFormat::Uint8x4 => wgpu::VertexFormat::Uint8x4,
            WgpuVertexFormat::Uint32 => wgpu::VertexFormat::Uint32,
            WgpuVertexFormat::Float32 => wgpu::VertexFormat::Float32,
            WgpuVertexFormat::Float32x2 => wgpu::VertexFormat::Float32x2,
            WgpuVertexFormat::Float32x3 => wgpu::VertexFormat::Float32x3,
            WgpuVertexFormat::Float32x4 => wgpu::VertexFormat::Float32x4,
        };
        attributes.push(wgpu::VertexAttribute {
            format,
            offset: layout.offset.into(),
            shader_location: layout.shader_location,
        });
    }

    let pipeline = device
        .device
        .create_render_pipeline(&wgpu::RenderPipelineDescriptor {
            label: None,
            layout: None,
            vertex: wgpu::VertexState {
                module: unsafe { &(&*(&*desc.vertex_shader).module).module },
                entry_point: Some(&*unsafe { (&*desc.vertex_shader).entry_point.as_str() }),
                buffers: &[wgpu::VertexBufferLayout {
                    array_stride: desc.stride.into(),
                    step_mode: wgpu::VertexStepMode::Vertex,
                    attributes: &attributes,
                }],
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
                    blend: Some(wgpu::BlendState::REPLACE),
                    write_mask: wgpu::ColorWrites::ALL,
                })],
            }),
            multiview: None,
            cache: None,
        });

    Box::into_raw(Box::new(WgpuPipeline { pipeline }))
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

pub struct WgpuBuffer {
    queue: wgpu::Queue,
    buffer: wgpu::Buffer,
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

pub struct WgpuTexture {
    queue: wgpu::Queue,
    texture: wgpu::Texture,
    // hack: one element bind group. Should move to bindless / index a texture binding array.
    bind_group: wgpu::BindGroup,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_texture_destroy(texture: *mut WgpuTexture) {
    drop(unsafe { Box::from_raw(texture) });
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_texture_write(
    texture: *mut WgpuTexture,
    mip_level: u32,
    data: *const u8,
    len: usize,
) {
    let texture = unsafe { &mut *texture };
    let data = unsafe { std::slice::from_raw_parts(data, len) };
    let size = texture.texture.size();
    texture.queue.write_texture(
        wgpu::TexelCopyTextureInfo {
            mip_level,
            ..texture.texture.as_image_copy()
        },
        data,
        // assume packed RGBA8 for now (we do have BCn formats to consider later)
        wgpu::TexelCopyBufferLayout {
            offset: 0,
            bytes_per_row: Some(
                size.mip_level_size(mip_level, texture.texture.dimension())
                    .width
                    * 4,
            ),
            rows_per_image: None,
        },
        size,
    );
}

pub struct WgpuShaderModule {
    module: wgpu::ShaderModule,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_shader_module_destroy(module: *mut WgpuShaderModule) {
    drop(unsafe { Box::from_raw(module) });
}

pub struct WgpuPipeline {
    pipeline: wgpu::RenderPipeline,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_pipeline_destroy(info: *mut WgpuPipeline) {
    drop(unsafe { Box::from_raw(info) });
}

pub struct WgpuSurface {
    surface: wgpu::Surface<'static>,
    texture: Option<wgpu::SurfaceTexture>,
    texture_view: Option<wgpu::TextureView>,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_surface_destroy(surface: *mut WgpuSurface) {
    drop(unsafe { Box::from_raw(surface) });
}

impl WgpuSurface {
    fn get_texture_view(&mut self) -> &wgpu::TextureView {
        let texture = self
            .texture
            .get_or_insert_with(|| self.surface.get_current_texture().unwrap());
        self.texture_view.get_or_insert_with(|| {
            texture
                .texture
                .create_view(&wgpu::TextureViewDescriptor::default())
        })
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_surface_configure(
    surface: *mut WgpuSurface,
    device: *mut WgpuDevice,
    width: u32,
    height: u32,
) -> bool {
    let surface = &unsafe { &mut *surface }.surface;
    let device = unsafe { &*device };
    let adapter = &device.adapter;
    let device = &device.device;
    if let Some(config) = surface.get_default_config(adapter, width, height) {
        surface.configure(device, &config);
        true
    } else {
        false
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_surface_present(surface: *mut WgpuSurface) {
    let surface = unsafe { &mut *surface };
    drop(surface.texture_view.take());
    if let Some(texture) = surface.texture.take() {
        texture.present();
    }
}

pub struct WgpuCommands {
    // dropped in struct order
    render_pass: Option<wgpu::RenderPass<'static>>,
    encoder: wgpu::CommandEncoder,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_destroy(commands: *mut WgpuCommands) {
    drop(unsafe { Box::from_raw(commands) });
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_begin_render_pass(
    commands: *mut WgpuCommands,
    surface: *mut WgpuSurface,
    clear_color: *mut [f32; 4],
) {
    let commands = unsafe { &mut *commands };
    let surface_view = unsafe { &mut *surface }.get_texture_view();

    if let Some(render_pass) = commands.render_pass.take() {
        drop(render_pass);
    }

    let load = if let Some(clear_color) = std::ptr::NonNull::new(clear_color) {
        let clear_color = unsafe { clear_color.as_ref() };
        wgpu::LoadOp::Clear(wgpu::Color {
            r: clear_color[0] as _,
            g: clear_color[1] as _,
            b: clear_color[2] as _,
            a: clear_color[3] as _,
        })
    } else {
        wgpu::LoadOp::Load
    };

    let render_pass = commands
        .encoder
        .begin_render_pass(&wgpu::RenderPassDescriptor {
            color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                view: surface_view,
                resolve_target: None,
                ops: wgpu::Operations {
                    load,
                    store: wgpu::StoreOp::Store,
                },
            })],
            ..Default::default()
        });
    commands.render_pass = Some(render_pass);
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_set_pipeline(
    commands: *mut WgpuCommands,
    pipeline: *mut WgpuPipeline,
) {
    let commands = unsafe { &mut *commands };
    let pipeline = unsafe { &*pipeline };
    let render_pass = commands.render_pass.as_mut().unwrap();
    render_pass.set_pipeline(&pipeline.pipeline);
}

pub const WGPU_SIZE_ALL: u64 = !0;

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_set_vertex_buffer(
    commands: *mut WgpuCommands,
    slot: u32,
    buffer: *mut WgpuBuffer,
    offset: u64,
    size: u64,
) {
    let commands = unsafe { &mut *commands };
    let buffer = &unsafe { &*buffer }.buffer;
    let render_pass = commands.render_pass.as_mut().unwrap();
    let size = if size == WGPU_SIZE_ALL {
        buffer.size() - offset
    } else {
        size
    };
    render_pass.set_vertex_buffer(slot, buffer.slice(offset..offset + size));
}

#[repr(C)]
#[non_exhaustive]
pub enum WgpuIndexFormat {
    Uint16,
    Uint32,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_set_index_buffer(
    commands: *mut WgpuCommands,
    buffer: *mut WgpuBuffer,
    format: WgpuIndexFormat,
    offset: u64,
    size: u64,
) {
    let commands = unsafe { &mut *commands };
    let buffer = &unsafe { &*buffer }.buffer;
    let render_pass = commands.render_pass.as_mut().unwrap();
    let size = if size == WGPU_SIZE_ALL {
        buffer.size() - offset
    } else {
        size
    };
    render_pass.set_index_buffer(
        buffer.slice(offset..offset + size),
        match format {
            WgpuIndexFormat::Uint16 => wgpu::IndexFormat::Uint16,
            WgpuIndexFormat::Uint32 => wgpu::IndexFormat::Uint32,
        },
    );
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_set_bind_group_to_texture(
    commands: *mut WgpuCommands,
    index: u32,
    texture: *mut WgpuTexture,
) {
    let commands = unsafe { &mut *commands };
    let texture = unsafe { &*texture };
    let render_pass = commands.render_pass.as_mut().unwrap();
    render_pass.set_bind_group(index, &texture.bind_group, &[]);
}

#[repr(C)]
pub struct WgpuDraw {
    pub vertex_first: u32,
    pub vertex_count: u32,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_draw(commands: *mut WgpuCommands, params: *const WgpuDraw) {
    let commands = unsafe { &mut *commands };
    let params = unsafe { &*params };
    let render_pass = commands.render_pass.as_mut().unwrap();
    let vertex_range = params.vertex_first..params.vertex_first + params.vertex_count;
    render_pass.draw(vertex_range, 0..1);
}

#[repr(C)]
pub struct WgpuDrawIndexed {
    pub index_first: u32,
    pub index_count: u32,
    pub base_vertex: i32,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_draw_indexed(
    commands: *mut WgpuCommands,
    params: *const WgpuDrawIndexed,
) {
    let commands = unsafe { &mut *commands };
    let params = unsafe { &*params };
    let render_pass = commands.render_pass.as_mut().unwrap();
    let index_range = params.index_first..params.index_first + params.index_count;
    render_pass.draw_indexed(index_range, params.base_vertex, 0..1);
}
