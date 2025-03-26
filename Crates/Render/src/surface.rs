use std::num::NonZeroIsize;
use windows::Win32::Foundation::HWND;
use windows::Win32::UI::WindowsAndMessaging::{GetWindowLongPtrW, GWLP_HINSTANCE};
use crate::*;

pub struct WgpuSurface {
    pub(crate) surface: wgpu::Surface<'static>,
    pub(crate) texture: Option<wgpu::SurfaceTexture>,
    pub(crate) texture_view: Option<wgpu::TextureView>,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_surface_create(
    instance: *mut WgpuInstance,
    hwnd: isize,
) -> *mut WgpuSurface {
    let instance = &unsafe { &*instance }.wgpu;

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
    let result = unsafe { instance.create_surface_unsafe(target) };

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

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_surface_destroy(surface: *mut WgpuSurface) {
    drop(unsafe { Box::from_raw(surface) });
}

impl WgpuSurface {
    pub(crate) fn get_texture_view(&mut self) -> &wgpu::TextureView {
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
