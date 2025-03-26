use crate::WgpuDevice;

pub struct WgpuTexture {
    pub(crate) queue: wgpu::Queue,
    pub(crate) texture: wgpu::Texture,
}

#[repr(C)]
pub enum WgpuTextureFormat {
    Rgba8Unorm,
    Bgra8Unorm,
    Depth32Float,
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
            WgpuTextureFormat::Bgra8Unorm => wgpu::TextureFormat::Bgra8Unorm,
            WgpuTextureFormat::Depth32Float => wgpu::TextureFormat::Depth32Float,
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
    Box::into_raw(Box::new(WgpuTexture { queue, texture }))
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_texture_clone(texture: *mut WgpuTexture) -> *mut WgpuTexture {
    // makes the C++ simpler
    if texture.is_null() {
        return std::ptr::null_mut();
    }
    let ptr_texture = unsafe { &*texture };
    let queue = ptr_texture.queue.clone();
    let texture = ptr_texture.texture.clone();
    Box::into_raw(Box::new(WgpuTexture { queue, texture }))
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
    let info = wgpu::TexelCopyTextureInfo {
        mip_level,
        ..texture.texture.as_image_copy()
    };
    let mip_size = texture
        .texture
        .size()
        .mip_level_size(mip_level, texture.texture.dimension());
    texture.queue.write_texture(
        info,
        data,
        // assume packed RGBA8 for now (we do have BCn formats to consider later)
        wgpu::TexelCopyBufferLayout {
            offset: 0,
            bytes_per_row: Some(mip_size.width * 4),
            rows_per_image: None,
        },
        mip_size,
    );
}
