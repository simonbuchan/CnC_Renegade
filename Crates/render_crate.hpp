#pragma once

#include <memory>
#include <windows.h>

#include "render_crate.h"

namespace wgpu
{
    struct Instance;
    struct Device;
    struct Surface;
    struct Commands;

    struct Instance
    {
        std::unique_ptr<WgpuInstance, decltype(&wgpu_instance_destroy)> ptr;

        explicit Instance()
            : ptr(wgpu_instance_create(), &wgpu_instance_destroy)
        {
        }

        uint32_t adapter_count() const
        {
            return wgpu_instance_adapter_count(ptr.get());
        }

        bool adapter_supports_surface(uint32_t index, Surface& surface) const;

        WgpuAdapterInfo adapter_id(uint32_t index) const
        {
            return wgpu_instance_adapter_id(ptr.get(), index);
        }

        Device create_device(uint32_t adapter);
        Surface create_surface(HWND hwnd);
    };

    struct Device
    {
        std::unique_ptr<WgpuDevice, decltype(&wgpu_device_destroy)> ptr;

        Device(WgpuInstance* instance, uint32_t adapter)
            : ptr(wgpu_instance_device_create(instance, adapter), &wgpu_device_destroy)
        {
        }

        Commands create_commands();
        void submit(Commands&);
    };

    struct Surface
    {
        std::unique_ptr<WgpuSurface, decltype(&wgpu_surface_destroy)> ptr;

        Surface(WgpuInstance* instance, HWND hwnd)
            : ptr(wgpu_instance_surface_create(instance, reinterpret_cast<intptr_t>(hwnd)), &wgpu_surface_destroy)
        {
        }

        bool configure(WgpuDevice* device, uint32_t width, uint32_t height)
        {
            return wgpu_surface_configure(ptr.get(), device, width, height);
        }

        void present()
        {
            wgpu_surface_present(ptr.get());
        }
    };

    struct Commands
    {
        std::unique_ptr<WgpuCommands, decltype(&wgpu_commands_destroy)> ptr;

        explicit Commands(WgpuDevice* device)
            : ptr(wgpu_device_create_commands(device), &wgpu_commands_destroy)
        {
        }

        void begin_render_pass(WgpuSurface* surface, float (*clear_color)[4])
        {
            wgpu_commands_begin_render_pass(ptr.get(), surface, clear_color);
        }
    };

    inline bool Instance::adapter_supports_surface(uint32_t index, Surface& surface) const
    {
        return wgpu_instance_adapter_supports_surface(
            ptr.get(),
            index,
            surface.ptr.get());
    }

    inline Device Instance::create_device(uint32_t adapter)
    {
        return Device(ptr.get(), adapter);
    }

    inline Surface Instance::create_surface(HWND hwnd)
    {
        return Surface(ptr.get(), hwnd);
    }

    inline Commands Device::create_commands()
    {
        return Commands(ptr.get());
    }

    inline void Device::submit(Commands& commands)
    {
        wgpu_device_submit(ptr.get(), commands.ptr.get());
    }
}
