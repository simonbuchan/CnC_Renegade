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
    struct BindGroup;
    struct Buffer;
    struct Texture;
    struct ShaderModule;
    struct Pipeline;
    struct Commands;

    struct Instance
    {
        static Instance create()
        {
            return Instance(wgpu_instance_create());
        }

        std::unique_ptr<WgpuInstance, decltype(&wgpu_instance_destroy)> ptr;

        explicit Instance(WgpuInstance* ptr)
            : ptr(ptr, &wgpu_instance_destroy)
        {
        }

        uint32_t adapter_count() const
        {
            return wgpu_instance_adapter_count(ptr.get());
        }

        bool adapter_supports_surface(uint32_t index, Surface const& surface) const;

        WgpuAdapterInfo adapter_id(uint32_t index) const
        {
            return wgpu_instance_adapter_id(ptr.get(), index);
        }
    };

    struct Device
    {
        static Device create(Instance const& instance, uint32_t adapter)
        {
            return Device(wgpu_device_create(instance.ptr.get(), adapter));
        }

        std::unique_ptr<WgpuDevice, decltype(&wgpu_device_destroy)> ptr;

        explicit Device(WgpuDevice* ptr)
            : ptr(ptr, &wgpu_device_destroy)
        {
        }

        void submit(Commands&);
    };

    struct Surface
    {
        static Surface create(Instance const& instance, HWND hwnd)
        {
            return Surface(wgpu_surface_create(instance.ptr.get(), reinterpret_cast<intptr_t>(hwnd)));
        }

        std::unique_ptr<WgpuSurface, decltype(&wgpu_surface_destroy)> ptr;

        explicit Surface(WgpuSurface* ptr)
            : ptr(ptr, &wgpu_surface_destroy)
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
        static Buffer create(Device const& device, uint64_t size, uint32_t usage)
        {
            return Buffer(wgpu_buffer_create(device.ptr.get(), size, usage));
        }

        std::unique_ptr<WgpuBuffer, decltype(&wgpu_buffer_destroy)> ptr;

        explicit Buffer(WgpuBuffer* ptr)
            : ptr(ptr, &wgpu_buffer_destroy)
        {
        }

        void write(uint64_t offset, const uint8_t* data, uintptr_t len)
        {
            wgpu_buffer_write(ptr.get(), offset, data, len);
        }
    };

    struct Texture
    {
        static Texture create(
            Device const& device,
            WgpuTextureFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mip_level_count,
            uint32_t usage)
        {
            return Texture(wgpu_device_create_texture(device.ptr.get(), format, width, height, mip_level_count, usage));
        }

        std::unique_ptr<WgpuTexture, decltype(&wgpu_texture_destroy)> ptr;

        Texture(Texture&& other) = default;
        Texture& operator=(Texture&& other) = default;

        explicit Texture(WgpuTexture* ptr)
            : ptr(ptr, &wgpu_texture_destroy)
        {
        }

        Texture(Texture const& other) = delete;
        Texture& operator=(Texture const& other) = delete;

        Texture clone() const
        {
            return Texture(wgpu_texture_clone(ptr.get()));
        }

        void write(uint32_t level, uint8_t const* data_ptr, uint32_t data_len)
        {
            wgpu_texture_write(ptr.get(), level, data_ptr, data_len);
        }
    };

    struct BindGroup
    {
        static BindGroup create_static(Device const& device, Buffer const& buffer, uint64_t offset, uint64_t size)
        {
            return BindGroup(wgpu_bind_group_create_static(device.ptr.get(), buffer.ptr.get(), offset, size));
        }

        static BindGroup create_texture(Device const& device, Texture const& texture)
        {
            return BindGroup(wgpu_bind_group_create_texture(device.ptr.get(), texture.ptr.get()));
        }

        std::unique_ptr<WgpuBindGroup, decltype(&wgpu_bind_group_destroy)> ptr;

        explicit BindGroup(WgpuBindGroup* ptr)
            : ptr(ptr, &wgpu_bind_group_destroy)
        {
        }
    };

    struct ShaderModule
    {
        static ShaderModule create_from_wsgl(Device const& device, std::string_view wgsl_source)
        {
            return ShaderModule(
                wgpu_shader_module_create_from_wgsl(
                    device.ptr.get(),
                    WgpuString{wgsl_source.data(), wgsl_source.length()}));
        }

        std::unique_ptr<WgpuShaderModule, decltype(&wgpu_shader_module_destroy)> ptr;

        explicit ShaderModule(WgpuShaderModule* ptr)
            : ptr(ptr, &wgpu_shader_module_destroy)
        {
        }

        WgpuShaderDesc desc(std::string_view entry_point)
        {
            return {ptr.get(), WgpuString{entry_point.data(), entry_point.length()}};
        }
    };

    struct Pipeline
    {
        static Pipeline create(Device const& device, WgpuPipelineDesc const& desc)
        {
            return Pipeline(wgpu_pipeline_create(device.ptr.get(), &desc));
        }

        std::unique_ptr<WgpuPipeline, decltype(&wgpu_pipeline_destroy)> ptr;

        explicit Pipeline(WgpuPipeline* ptr)
            : ptr(ptr, &wgpu_pipeline_destroy)
        {
        }
    };

    struct Commands
    {
        static Commands create(Device const& device)
        {
            return Commands(wgpu_commands_create(device.ptr.get()));
        }

        std::unique_ptr<WgpuCommands, decltype(&wgpu_commands_destroy)> ptr;

        explicit Commands(WgpuCommands* ptr)
            : ptr(ptr, &wgpu_commands_destroy)
        {
        }

        void copy_texture_to_texture(
            Texture& dest_texture,
            WgpuTexelCopyOffset dest_offset,
            Texture& source_texture,
            WgpuTexelCopyOffset source_offset,
            WgpuSize* copy_size = nullptr)
        {
            wgpu_commands_copy_texture_to_texture(
                ptr.get(),
                dest_texture.ptr.get(),
                dest_offset,
                source_texture.ptr.get(),
                source_offset,
                copy_size);
        }

        void begin_render_pass(Surface& surface, float (*clear_color)[4])
        {
            wgpu_commands_begin_render_pass(ptr.get(), surface.ptr.get(), clear_color);
        }

        void set_pipeline(Pipeline& pipeline)
        {
            wgpu_commands_set_pipeline(ptr.get(), pipeline.ptr.get());
        }

        void set_bind_group(uint32_t index, BindGroup& bind_group)
        {
            wgpu_commands_set_bind_group(ptr.get(), index, bind_group.ptr.get(), nullptr, 0);
        }

        void set_bind_group(uint32_t index, BindGroup& bind_group, uint32_t offset)
        {
            wgpu_commands_set_bind_group(ptr.get(), index, bind_group.ptr.get(), &offset, 1);
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

        void draw_indexed(WgpuDrawIndexed const& args)
        {
            wgpu_commands_draw_indexed(ptr.get(), &args);
        }
    };

    inline bool Instance::adapter_supports_surface(uint32_t index, Surface const& surface) const
    {
        return wgpu_instance_adapter_supports_surface(
            ptr.get(),
            index,
            surface.ptr.get());
    }

    inline void Device::submit(Commands& commands)
    {
        wgpu_device_submit(ptr.get(), commands.ptr.get());
    }
}
