use crate::*;

pub struct WgpuCommands {
    // dropped in struct order
    pub(crate) render_pass: Option<wgpu::RenderPass<'static>>,
    pub(crate) encoder: wgpu::CommandEncoder,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_create(device: *mut WgpuDevice) -> *mut WgpuCommands {
    let device = unsafe { &mut *device };
    let encoder = device
        .device
        .create_command_encoder(&wgpu::CommandEncoderDescriptor::default());
    Box::into_raw(Box::new(WgpuCommands {
        encoder,
        render_pass: None,
    }))
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_destroy(commands: *mut WgpuCommands) {
    drop(unsafe { Box::from_raw(commands) });
}

#[repr(C)]
pub struct WgpuSize {
    pub width: u32,
    pub height: u32,
}

#[repr(C)]
pub struct WgpuTexelCopyOffset {
    pub x: u32,
    pub y: u32,
    pub level: u32,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_copy_texture_to_texture(
    commands: *mut WgpuCommands,
    dest_texture: *mut WgpuTexture,
    dest_offset: WgpuTexelCopyOffset,
    src_texture: *mut WgpuTexture,
    src_offset: WgpuTexelCopyOffset,
    size: *mut WgpuSize,
) {
    let commands = unsafe { &mut *commands };
    let dest_texture = unsafe { &*dest_texture };
    let src_texture = unsafe { &*src_texture };
    let size = std::ptr::NonNull::new(size);
    let extent = match size {
        Some(size) => {
            let size = unsafe { &*size.as_ref() };
            wgpu::Extent3d {
                width: size.width,
                height: size.height,
                depth_or_array_layers: 1,
            }
        }
        None => src_texture.texture.size(),
    };
    commands.encoder.copy_texture_to_texture(
        wgpu::TexelCopyTextureInfo {
            texture: &src_texture.texture,
            mip_level: src_offset.level,
            origin: wgpu::Origin3d {
                x: src_offset.x,
                y: src_offset.y,
                z: 0,
            },
            aspect: wgpu::TextureAspect::All,
        },
        wgpu::TexelCopyTextureInfo {
            texture: &dest_texture.texture,
            mip_level: dest_offset.level,
            origin: wgpu::Origin3d {
                x: dest_offset.x,
                y: dest_offset.y,
                z: 0,
            },
            aspect: wgpu::TextureAspect::All,
        },
        extent,
    );
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

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_commands_set_bind_group(
    commands: *mut WgpuCommands,
    index: u32,
    bind_group: *mut WgpuBindGroup,
    offsets_ptr: *const u32,
    offsets_len: usize,
) {
    let commands = unsafe { &mut *commands };
    let bind_group = unsafe { &*bind_group };
    let offsets = if offsets_ptr.is_null() {
        &[]
    } else {
        unsafe { std::slice::from_raw_parts(offsets_ptr, offsets_len) }
    };

    let render_pass = commands.render_pass.as_mut().unwrap();
    render_pass.set_bind_group(index, &bind_group.bind_group, offsets);
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
