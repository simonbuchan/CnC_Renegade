#pragma once

#include <memory>
#include <string_view>
#include <windows.h>

#include "render_crate.h"

namespace image
{
    struct RgbaData
    {
        static RgbaData load(const char* path)
        {
            return RgbaData(image_rgba_data_load(path));
        }

        std::unique_ptr<ImageRgbaData, decltype(&image_rgba_data_destroy)> ptr;

        ImageRgbaData* operator->() const
        {
            return ptr.get();
        }

    private:
        explicit RgbaData(ImageRgbaData* ptr)
            : ptr(ptr, &image_rgba_data_destroy)
        {
        }
    };
}

namespace wgpu
{
    struct Instance;
    struct Device;
    struct Surface;
    struct Buffer;
    struct Texture;
    struct ShaderModule;
    struct Pipeline;
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

        Buffer create_buffer(uint64_t size, uint32_t usage);
        Texture create_texture(WgpuTextureFormat format, uint32_t width, uint32_t height, uint32_t mip_level_count,
                               uint32_t usage);
        Commands create_commands();
        ShaderModule create_shader_module(std::string_view wgsl_source);
        Pipeline create_pipeline(WgpuPipelineDesc const& desc);

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

    struct Buffer
    {
        std::unique_ptr<WgpuBuffer, decltype(&wgpu_buffer_destroy)> ptr;

        explicit Buffer(WgpuDevice* device, uint64_t size, uint32_t usage)
            : ptr(wgpu_device_create_buffer(device, size, usage), &wgpu_buffer_destroy)
        {
        }

        void write(uint64_t offset, const uint8_t* data, uintptr_t len)
        {
            wgpu_buffer_write(ptr.get(), offset, data, len);
        }
    };

    struct Texture
    {
        std::unique_ptr<WgpuTexture, decltype(&wgpu_texture_destroy)> ptr;

        explicit Texture(
            WgpuDevice* device,
            WgpuTextureFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mip_level_count,
            uint32_t usage)
            : ptr(wgpu_device_create_texture(device, format, width, height, mip_level_count, usage),
                  &wgpu_texture_destroy)
        {
        }

        void write(uint32_t level, uint8_t* data_ptr, uint32_t data_len)
        {
            wgpu_texture_write(ptr.get(), level, data_ptr, data_len);
        }
    };

    struct ShaderModule
    {
        std::unique_ptr<WgpuShaderModule, decltype(&wgpu_shader_module_destroy)> ptr;

        explicit ShaderModule(WgpuDevice* device, std::string_view wgsl_source)
            : ptr(wgpu_device_create_wgsl_shader_module(device, WgpuString{wgsl_source.data(), wgsl_source.length()}),
                  &wgpu_shader_module_destroy)
        {
        }

        WgpuShaderDesc desc(std::string_view entry_point)
        {
            return {ptr.get(), WgpuString{entry_point.data(), entry_point.length()}};
        }
    };

    struct Pipeline
    {
        std::unique_ptr<WgpuPipeline, decltype(&wgpu_pipeline_destroy)> ptr;

        explicit Pipeline(WgpuDevice* device, WgpuPipelineDesc const& desc)
            : ptr(wgpu_device_create_pipeline(device, &desc), &wgpu_pipeline_destroy)
        {
        }
    };

    struct Commands
    {
        std::unique_ptr<WgpuCommands, decltype(&wgpu_commands_destroy)> ptr;

        explicit Commands(WgpuDevice* device)
            : ptr(wgpu_device_create_commands(device), &wgpu_commands_destroy)
        {
        }

        void begin_render_pass(Surface& surface, float (*clear_color)[4])
        {
            wgpu_commands_begin_render_pass(ptr.get(), surface.ptr.get(), clear_color);
        }

        void set_pipeline(Pipeline& pipeline)
        {
            wgpu_commands_set_pipeline(ptr.get(), pipeline.ptr.get());
        }

        void set_vertex_buffer(uint32_t slot, Buffer& buffer, uint64_t offset = 0, uint64_t size = WGPU_SIZE_ALL)
        {
            wgpu_commands_set_vertex_buffer(ptr.get(), slot, buffer.ptr.get(), offset, size);
        }

        void set_index_buffer(Buffer& buffer, WgpuIndexFormat format, uint64_t offset = 0,
                              uint64_t size = WGPU_SIZE_ALL)
        {
            wgpu_commands_set_index_buffer(ptr.get(), buffer.ptr.get(), format, offset, size);
        }

        void set_bind_group_to_texture(uint32_t index, Texture& texture)
        {
            wgpu_commands_set_bind_group_to_texture(ptr.get(), index, texture.ptr.get());
        }

        void draw_indexed(WgpuDrawIndexed const& args)
        {
            wgpu_commands_draw_indexed(ptr.get(), &args);
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

    inline Buffer Device::create_buffer(uint64_t size, uint32_t usage)
    {
        return Buffer(ptr.get(), size, usage);
    }

    inline Texture Device::create_texture(WgpuTextureFormat format, uint32_t width, uint32_t height,
                                          uint32_t mip_level_count, uint32_t usage)
    {
        return Texture(ptr.get(), format, width, height, mip_level_count, usage);
    }

    inline Commands Device::create_commands()
    {
        return Commands(ptr.get());
    }

    inline ShaderModule Device::create_shader_module(std::string_view wgsl_source)
    {
        return ShaderModule(ptr.get(), wgsl_source);
    }

    inline Pipeline Device::create_pipeline(WgpuPipelineDesc const& desc)
    {
        return Pipeline(ptr.get(), desc);
    }

    inline void Device::submit(Commands& commands)
    {
        wgpu_device_submit(ptr.get(), commands.ptr.get());
    }
}
