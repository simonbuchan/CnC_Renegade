use pollster::FutureExt as _;
use std::ffi::c_char;
use std::num::NonZeroIsize;
use windows::Win32::Foundation::HWND;
use windows::Win32::UI::WindowsAndMessaging::{GWLP_HINSTANCE, GetWindowLongPtrW};

pub struct WgpuInstance {
    wgpu: wgpu::Instance,
    adapters: Vec<wgpu::Adapter>,
}

#[unsafe(no_mangle)]
pub extern "C" fn wgpu_instance_create() -> *mut WgpuInstance {
    let wgpu = wgpu::Instance::default();
    let adapters = wgpu.enumerate_adapters(wgpu::Backends::PRIMARY);
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
        .request_device(&wgpu::DeviceDescriptor::default(), None)
        .block_on()
    {
        Ok((device, queue)) => {
            let adapter = adapter.clone();
            Box::into_raw(Box::new(WgpuDevice {
                adapter,
                device,
                queue,
            }))
        }
        Err(error) => {
            eprintln!("Failed to create device: {:?}", error);
            std::ptr::null_mut()
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
        Ok(surface) => {
            Box::into_raw(Box::new(WgpuSurface {
                surface,
                texture: None,
                texture_view: None,
            }))
        }
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
        self.texture_view
            .get_or_insert_with(|| texture.texture.create_view(&wgpu::TextureViewDescriptor::default()))
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
