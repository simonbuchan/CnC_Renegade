#include "d3d8.h"

#include <array>
#include <cassert>
#include <fstream>

inline D3DMATRIX D3DMATRIX::IDENTITY = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
};


// offer both a 32 and 16 bit format so it can find either.
static const D3DDISPLAYMODE DummyModes[] = {
    {.Width = 800, .Height = 600, .Format = D3DFMT_A8R8G8B8},
    // {.Width = 800, .Height = 600, .Format = D3DFMT_R5G6B5},
};

static const D3DCAPS8 MockCaps = {
    .AdapterOrdinal = 0,
    .DeviceType = D3DDEVTYPE_HAL,
    .MaxTextureWidth = 4096,
    .MaxTextureHeight = 4096,
    .MaxSimultaneousTextures = 8,
    .DevCaps = D3DDEVCAPS_HWTRANSFORMANDLIGHT | D3DDEVCAPS_NPATCHES,
    .RasterCaps = D3DPRASTERCAPS_FOGRANGE | D3DPRASTERCAPS_ZBIAS,
    .TextureOpCaps =
    // for now disable bump maps
    // D3DTEXOPCAPS_BUMPENVMAP |
    // D3DTEXOPCAPS_BUMPENVMAPLUMINANCE |
    D3DTEXOPCAPS_ADD |
    D3DTEXOPCAPS_SUBTRACT |
    D3DTEXOPCAPS_MODULATE |
    D3DTEXOPCAPS_DOTPRODUCT3 |
    D3DTEXOPCAPS_SELECTARG1 |
    D3DTEXOPCAPS_ADDSMOOTH |
    D3DTEXOPCAPS_BLENDTEXTUREALPHA |
    D3DTEXOPCAPS_BLENDCURRENTALPHA,
    .TextureFilterCaps =
    D3DPTFILTERCAPS_MAGFLINEAR |
    D3DPTFILTERCAPS_MINFANISOTROPIC |
    D3DPTFILTERCAPS_MAGFANISOTROPIC |
    D3DPTFILTERCAPS_MINFLINEAR |
    D3DPTFILTERCAPS_MIPFLINEAR,
    .Caps2 = D3DCAPS2_FULLSCREENGAMMA,
    .VertexShaderVersion = 0,
    .PixelShaderVersion = 0,
};

D3D_U32 IDirect3DUnknown8::AddRef() { return ++count; }

D3D_U32 IDirect3DUnknown8::Release()
{
    if (--count)
        return count;
    delete this;
    return 0;
}

enum class WgslLocation : uint32_t
{
    // In FVF order. Must match the shader layout
    Position = 0,
    BlendWeights = 1,
    BlendIndices = 2,
    Normal = 3,
    Diffuse = 4,
    Specular = 5,
    Tex1 = 6,
    Tex2 = 7,
};

struct ConstVertexBufferData
{
    float vec4f_zero[4] = {0, 0, 0, 0};
    uint8_t black[4] = {0, 0, 0, 0};
    uint8_t white[4] = {0xFF, 0xFF, 0xFF, 0xFF};
};

wgpu::Buffer create_const_vertex_buffer(wgpu::Device& device)
{
    ConstVertexBufferData data;
    auto result = wgpu::Buffer::create(device, sizeof(data), WGPU_BUFFER_USAGE_VERTEX);
    result.write(0, (uint8_t const*)&data, sizeof(data));
    return result;
}

wgpu::Pipeline create_pipeline_for_state(
    wgpu::Device& device,
    wgpu::ShaderModule& shader_module,
    IDirect3DDevice8::PipelineState const& state)
{
    auto fvf = state.fvf;

    // build a new pipeline to match FVF
    auto vertex_shader = shader_module.desc("vs_main");
    auto fragment_shader = shader_module.desc("fs_main");

    // buffer 0 is the vertex buffer from ww3d2,
    constexpr auto input_buffer_index = 0;
    // buffer 1 is dummy values to match the shader layout, with stride 0 so all vertices get the same value
    constexpr auto const_buffer_index = 1;
    constexpr auto const_vec4f_zero_offset = offsetof(ConstVertexBufferData, vec4f_zero);
    constexpr auto const_black_offset = offsetof(ConstVertexBufferData, black);
    constexpr auto const_white_offset = offsetof(ConstVertexBufferData, white);

    std::array<WgpuVertexBufferDesc, 2> buffers = {};

    // buffer 0 stride
    auto buffer_stride = 0U;
    std::vector<WgpuVertexAttributeDesc> attrs;
    if (fvf & D3DFVF_XYZ)
    {
        attrs.push_back({
            .buffer_index = input_buffer_index,
            .shader_location = (uint32_t)WgslLocation::Position,
            .format = WgpuVertexFormat::Float32x3,
            .offset = buffer_stride,
        });
        buffer_stride += 12;
    }
    if (fvf & D3DFVF_B4)
    {
        if (fvf & D3DFVF_LASTBETA_UBYTE4)
        {
            attrs.push_back({
                .buffer_index = input_buffer_index,
                .shader_location = (uint32_t)WgslLocation::BlendWeights,
                .format = WgpuVertexFormat::Float32x3,
                .offset = buffer_stride,
            });
            attrs.push_back({
                .buffer_index = input_buffer_index,
                .shader_location = (uint32_t)WgslLocation::BlendIndices,
                .format = WgpuVertexFormat::Uint32,
                .offset = buffer_stride + 12,
            });
        }
        else
        {
            attrs.push_back({
                .buffer_index = input_buffer_index,
                .shader_location = (uint32_t)WgslLocation::BlendWeights,
                .format = WgpuVertexFormat::Float32x4,
                .offset = buffer_stride,
            });
        }
        buffer_stride += 16;
    }
    else
    {
        attrs.push_back({
            .buffer_index = const_buffer_index,
            .shader_location = (uint32_t)WgslLocation::BlendWeights,
            .format = WgpuVertexFormat::Float32x4,
            .offset = const_vec4f_zero_offset,
        });
    }
    if (fvf & D3DFVF_NORMAL)
    {
        attrs.push_back({
            .buffer_index = input_buffer_index,
            .shader_location = (uint32_t)WgslLocation::Normal,
            .format = WgpuVertexFormat::Float32x3,
            .offset = buffer_stride,
        });
        buffer_stride += 12;
    }
    else
    {
        attrs.push_back({
            .buffer_index = const_buffer_index,
            .shader_location = (uint32_t)WgslLocation::Normal,
            .format = WgpuVertexFormat::Float32x3,
            .offset = const_vec4f_zero_offset,
        });
    }
    if (fvf & D3DFVF_DIFFUSE)
    {
        attrs.push_back({
            .buffer_index = input_buffer_index,
            .shader_location = (uint32_t)WgslLocation::Diffuse,
            .format = WgpuVertexFormat::Uint8x4,
            .offset = buffer_stride,
        });
        buffer_stride += 4;
    }
    else
    {
        attrs.push_back({
            .buffer_index = const_buffer_index,
            .shader_location = (uint32_t)WgslLocation::Diffuse,
            .format = WgpuVertexFormat::Uint8x4,
            .offset = const_white_offset,
        });
    }
    if (fvf & D3DFVF_SPECULAR)
    {
        attrs.push_back({
            .buffer_index = input_buffer_index,
            .shader_location = (uint32_t)WgslLocation::Specular,
            .format = WgpuVertexFormat::Uint8x4,
            .offset = buffer_stride,
        });
        buffer_stride += 4;
    }
    else
    {
        attrs.push_back({
            .buffer_index = const_buffer_index,
            .shader_location = (uint32_t)WgslLocation::Specular,
            .format = WgpuVertexFormat::Uint8x4,
            .offset = const_black_offset,
        });
    }
    // FVFInfoClass texture coord logic is dumb, an
    // only works because the layout is last and only uses 2d
    // coordinates. Just check for the number of texture coordinates
    auto tex_count = (fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    if (tex_count > 0)
    {
        // #define D3DFVF_TEXCOORDSIZE1(CoordIndex) (3 << (CoordIndex*2 + 16))
        // #define D3DFVF_TEXCOORDSIZE2(CoordIndex) (0)
        // #define D3DFVF_TEXCOORDSIZE3(CoordIndex) (1 << (CoordIndex*2 + 16))
        // #define D3DFVF_TEXCOORDSIZE4(CoordIndex) (2 << (CoordIndex*2 + 16))
        // ensure only 2D coordinates
        assert(((fvf >> 16) & 3) == 0);
        attrs.push_back({
            .buffer_index = input_buffer_index,
            .shader_location = (uint32_t)WgslLocation::Tex1,
            .format = WgpuVertexFormat::Float32x2,
            .offset = buffer_stride,
        });
        buffer_stride += 8;
    }
    else
    {
        attrs.push_back({
            .buffer_index = const_buffer_index,
            .shader_location = (uint32_t)WgslLocation::Tex1,
            .format = WgpuVertexFormat::Float32x2,
            .offset = const_vec4f_zero_offset,
        });
    }
    if (tex_count > 1)
    {
        assert(((fvf >> 18) & 3) == 0);
        attrs.push_back({
            .shader_location = (uint32_t)WgslLocation::Tex2,
            .format = WgpuVertexFormat::Float32x2,
            .offset = buffer_stride,
        });
        buffer_stride += 8;
    }
    else
    {
        attrs.push_back({
            .buffer_index = const_buffer_index,
            .shader_location = (uint32_t)WgslLocation::Tex2,
            .format = WgpuVertexFormat::Float32x2,
            .offset = const_vec4f_zero_offset,
        });
    }
    buffers[0].stride = buffer_stride;
    buffers[1].stride = 0; // all vertices get the same value

    return wgpu::Pipeline::create(device, {
                                      .buffers_ptr = buffers.data(),
                                      .buffers_len = buffers.size(),
                                      .attributes_ptr = attrs.data(),
                                      .attributes_len = attrs.size(),
                                      .vertex_shader = &vertex_shader,
                                      .fragment_shader = &fragment_shader,
                                      .alpha_blend_enable = state.alpha_blend_enable,
                                      .src_blend = state.src_blend,
                                      .dst_blend = state.dest_blend,
                                      .z_write_enable = state.z_write_enable,
                                      .z_bias = state.z_bias,
                                      .z_func = state.z_func,
                                  });
}

std::string read_shader()
{
    std::ifstream file("shader.wgsl");
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

wgpu::BindGroup create_white_texture_bind_group(wgpu::Device& device)
{
    auto white_texture = wgpu::Texture::create(device,
                                               WgpuTextureFormat::Rgba8Unorm,
                                               1,
                                               1,
                                               1,
                                               WGPU_TEXTURE_USAGE_TEXTURE_BINDING | WGPU_TEXTURE_USAGE_COPY_DST);
    uint8_t white[] = {0xff, 0xff, 0xff, 0xff};
    white_texture.write(0, white, sizeof(white));
    return wgpu::BindGroup::create_texture(device, white_texture);
}

constexpr uint32_t max_states_before_flush = 1000;
constexpr uint32_t uniform_state_size_aligned = (sizeof(IDirect3DDevice8::UniformState) + 255) & ~255;

IDirect3DDevice8::IDirect3DDevice8(wgpu::Device device_, wgpu::Surface surface, wgpu::Texture depth_buffer)
    : device(std::move(device_)),
      surface(std::move(surface)),
      depth_buffer(std::move(depth_buffer)),
      commands(wgpu::Commands::create(device)),
      commands_copy(wgpu::Commands::create(device)),
      shader_module(wgpu::ShaderModule::create_from_wsgl(device, read_shader())),
      state_buffer(wgpu::Buffer::create(device, uniform_state_size_aligned * max_states_before_flush,
                                        WGPU_BUFFER_USAGE_UNIFORM)),
      static_bind_group(wgpu::BindGroup::create_static(device, state_buffer, 0, sizeof(UniformState))),
      const_vertex_buffer(create_const_vertex_buffer(device)),
      white_texture_bind_group(create_white_texture_bind_group(device))
{
}

IDirect3D8* Direct3DCreate8(int)
{
    auto instance = new IDirect3D8();
    instance->AddRef();
    return instance;
}

D3D_U32 IDirect3D8::GetAdapterCount()
{
    return wgpu.adapter_count();
}

D3D_U32 IDirect3D8::GetAdapterModeCount(D3D_U32 index)
{
    return ARRAYSIZE(DummyModes);
}

D3D_RESULT IDirect3D8::EnumAdapterModes(D3D_U32 adapter, D3D_U32 index, D3DDISPLAYMODE* mode)
{
    if (adapter != 0)
        return D3DERR_INVALIDCALL;
    if (index >= ARRAYSIZE(DummyModes))
        return D3DERR_INVALIDCALL;
    *mode = DummyModes[index];
    return D3D_OK;
}

D3D_RESULT IDirect3D8::GetAdapterDisplayMode(D3D_U32 adapter, D3DDISPLAYMODE* mode)
{
    if (adapter != 0)
        return D3DERR_INVALIDCALL;
    *mode = DummyModes[0];
    return D3D_OK;
}

D3D_RESULT IDirect3D8::GetAdapterIdentifier(D3D_U32 index, D3DENUM, D3DADAPTER_IDENTIFIER8* id)
{
    auto info = wgpu.adapter_id(index);
    *id = D3DADAPTER_IDENTIFIER8{
        .VendorId = info.vendor_id,
        .DeviceId = info.device_id,
    };
    memcpy(id->Driver, info.driver, ARRAYSIZE(id->Driver));
    memcpy(id->Description, info.name, ARRAYSIZE(id->Description));
    return D3D_OK;
}

D3D_RESULT IDirect3D8::GetDeviceCaps(D3D_U32, D3DDEVTYPE, D3DCAPS8* caps)
{
    *caps = MockCaps;
    return D3D_OK;
}

D3D_RESULT IDirect3D8::CheckDeviceFormat(
    D3D_U32,
    D3DDEVTYPE,
    D3DFORMAT,
    D3D_U32,
    D3DRESOURCETYPE,
    D3DFORMAT format)
{
    // only support these formats for now (which are natively supported by wgpu)
    if (format == D3DFMT_A8R8G8B8) // actually WgpuTextureFormat::Bgra8Unorm
        return D3D_OK;
    if (format == D3DFMT_D32)
        return D3D_OK;
    if (format == D3DFMT_D24S8)
        return D3D_OK;
    return D3DERR_WRONGTEXTUREFORMAT;
}

D3D_RESULT IDirect3D8::CheckDepthStencilMatch(D3D_U32, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, D3DFORMAT)
{
    return D3D_OK;
}

D3D_RESULT IDirect3D8::CreateDevice(
    D3D_U32 index,
    D3DDEVTYPE,
    HWND hwnd,
    D3D_U32,
    D3DPRESENT_PARAMETERS* present_params,
    IDirect3DDevice8** result)
{
    auto wgpu_device = wgpu::Device::create(wgpu, index);
    auto wgpu_surface = wgpu::Surface::create(wgpu, hwnd);

    D3D_U32 width = present_params->BackBufferWidth;
    D3D_U32 height = present_params->BackBufferHeight;
    // D3DFORMAT format = present_params->BackBufferFormat;

    if (!wgpu_surface.configure(wgpu_device.ptr.get(), width, height))
        return D3DERR_WRONGTEXTUREFORMAT;

    auto depth_buffer = wgpu::Texture::create(wgpu_device, WgpuTextureFormat::Depth32Float,
                                              width, height, 1,
                                              WGPU_TEXTURE_USAGE_RENDER_ATTACHMENT);

    *result = IDirect3DDevice8::Create(std::move(wgpu_device), std::move(wgpu_surface), std::move(depth_buffer)).
        Detach();
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::GetDeviceCaps(D3DCAPS8* caps)
{
    *caps = MockCaps;
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::ValidateDevice(D3D_U32*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::Reset(const D3DPRESENT_PARAMETERS*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::GetDisplayMode(D3DDISPLAYMODE*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::TestCooperativeLevel()
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::ResourceManagerDiscardBytes(D3D_U32)
{
    return D3D_OK;
}

D3D_U32 IDirect3DDevice8::GetAvailableTextureMem()
{
    return 0x1000000;
}

D3D_RESULT IDirect3DDevice8::CreateVertexBuffer(D3D_U32 length, D3D_U32 usage, D3D_U32 fvf, D3D_U32 pool,
                                                IDirect3DVertexBuffer8** result)
{
    auto vertex_buffer = wgpu::Buffer::create(device, length, WGPU_BUFFER_USAGE_VERTEX);
    *result = IDirect3DVertexBuffer8::Create(std::move(vertex_buffer)).Detach();
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::CreateIndexBuffer(D3D_U32 length, D3D_U32 usage, D3D_U32 format, D3D_U32 pool,
                                               IDirect3DIndexBuffer8** result)
{
    WgpuIndexFormat wgpu_format;
    switch (format)
    {
    case D3DFMT_INDEX16:
        wgpu_format = WgpuIndexFormat::Uint16;
        break;
    // case D3DFMT_INDEX32:
    //     wgpu_format = WgpuIndexFormat::Uint32;
    //     break;
    default:
        return D3DERR_INVALIDCALL;
    }

    auto index_buffer = wgpu::Buffer::create(device, length, WGPU_BUFFER_USAGE_INDEX);
    *result = IDirect3DIndexBuffer8::Create(std::move(index_buffer), wgpu_format).Detach();
    return D3D_OK;
}

D3D_U32 FormatBitsPerPixel(D3DFORMAT format)
{
    switch (format)
    {
    default:
        return 0;

    // 8-bit
    case D3DFMT_L8:
    case D3DFMT_A8:
    case D3DFMT_P8:
        return 8;

    // 16-bit
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
        return 16;

    // 24-bit? (it calls it 32-bit in app code)
    case D3DFMT_R8G8B8:

    // 32-bit
    case D3DFMT_X8R8G8B8:
    case D3DFMT_A8R8G8B8:
        return 32;

    // index
    case D3DFMT_INDEX16:

    // depth-stencil
    case D3DFMT_D16:
    case D3DFMT_D15S1:
        return 16;

    case D3DFMT_D32:
    case D3DFMT_D24X8:
    case D3DFMT_D24S8:
    case D3DFMT_D24X4S4:
        return 32;

    case D3DFMT_R3G3B2:
        return 8;
    case D3DFMT_A8R3G3B2:
    case D3DFMT_X4R4G4B4:
    case D3DFMT_A8P8:
    case D3DFMT_A8L8:
        return 16;
    case D3DFMT_A4L4:
        return 8;
    case D3DFMT_V8U8: // Bumpmap
    case D3DFMT_L6V5U5: // Bumpmap
        return 16;
    case D3DFMT_X8L8V8U8: // Bumpmap
        return 32;

    case D3DFMT_DXT1:
        return 4;
    case D3DFMT_DXT2:
    case D3DFMT_DXT3:
    case D3DFMT_DXT4:
    case D3DFMT_DXT5:
        return 8;
    }
}

D3D_RESULT IDirect3DDevice8::CreateImageSurface(D3D_U32 width, D3D_U32 height, D3DFORMAT format,
                                                IDirect3DSurface8** result)
{
    // We have claimed we only support RGBA8 (which is actually accurate for WGPU's API!), so assert that
    // here. Fonts etc. were hard-coded to use RGBA4, for example.
    assert(format == D3DFMT_A8R8G8B8);
    auto texture = wgpu::Texture::create(device, WgpuTextureFormat::Bgra8Unorm, width, height, 1,
                                         WGPU_TEXTURE_USAGE_TEXTURE_BINDING |
                                         WGPU_TEXTURE_USAGE_COPY_SRC |
                                         WGPU_TEXTURE_USAGE_COPY_DST);
    auto bpp = FormatBitsPerPixel(format);
    if (bpp == 0)
        return D3DERR_INVALIDCALL;
    *result = IDirect3DSurface8::Create(std::move(texture), 0, width, height, format, bpp).Detach();
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::CreateTexture(D3D_U32 width, D3D_U32 height, D3D_U32 levels, D3D_U32 usage,
                                           D3DFORMAT format, D3DPOOL pool, IDirect3DTexture8** result)
{
    // We have claimed we only support RGBA8 (which is actually accurate for WGPU's API!), so assert that
    // here. Fonts etc. were hard-coded to use RGBA4, for example.
    // We can restore BCn support later, I guess? (But if they have uncompressed textures that are original
    // quality I don't know why we would want to use them)
    assert(format == D3DFMT_A8R8G8B8);

    WgpuTextureFormat wgpu_format;
    switch (format)
    {
    // Normalize color formats to Bgra8Unorm (matching D3DFMT_A8R8G8B8), as WGPU doesn't support <32-bit colors.
    // They will need to be transformed on upload.
    default: wgpu_format = WgpuTextureFormat::Bgra8Unorm;
        break;
    // forward block compressed formats to the appropriate WGPU format.
    // Note that DXT2 and DXT4 are just premultiplied alpha versions of DXT3 and DXT5,
    // so they use the asme BC2 and BC3 formats.
    case D3DFMT_DXT1: wgpu_format = WgpuTextureFormat::Bc1RgbaUnorm;
        break;
    case D3DFMT_DXT2: wgpu_format = WgpuTextureFormat::Bc2RgbaUnorm;
        break;
    case D3DFMT_DXT3: wgpu_format = WgpuTextureFormat::Bc2RgbaUnorm;
        break;
    case D3DFMT_DXT4: wgpu_format = WgpuTextureFormat::Bc3RgbaUnorm;
        break;
    case D3DFMT_DXT5: wgpu_format = WgpuTextureFormat::Bc3RgbaUnorm;
        break;
    }
    if (levels == 0)
    {
        auto width_levels = 32 - std::countl_zero(width);
        auto height_levels = 32 - std::countl_zero(height);
        levels = min(width_levels, height_levels);
    }
    auto texture = wgpu::Texture::create(device, wgpu_format, width, height, levels,
                                         WGPU_TEXTURE_USAGE_TEXTURE_BINDING |
                                         WGPU_TEXTURE_USAGE_COPY_SRC |
                                         WGPU_TEXTURE_USAGE_COPY_DST);
    auto bind_group = wgpu::BindGroup::create_texture(device, texture);
    auto bpp = FormatBitsPerPixel(format);
    if (bpp == 0)
        return D3DERR_INVALIDCALL;
    *result = IDirect3DTexture8::Create(std::move(texture), std::move(bind_group), width, height, format, bpp, levels).
        Detach();
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::UpdateTexture(IDirect3DBaseTexture8*, IDirect3DBaseTexture8*)
{
    return D3D_OK;
}

#ifdef D3D_ADDITIONAL_SWAP_CHAIN
D3D_RESULT IDirect3DDevice8::CreateAdditionalSwapChain(
    D3DPRESENT_PARAMETERS* present_parameters,
    IDirect3DSwapChain8** result)
{
    D3D_U32 width = present_parameters->BackBufferWidth;
    D3D_U32 height = present_parameters->BackBufferHeight;
    D3DFORMAT format = present_parameters->BackBufferFormat;
    auto surface = IDirect3DSurface8::Create(width, height, format);
    *result = IDirect3DSwapChain8::Create(std::move(surface)).Detach();
    return D3D_OK;
}
#endif

#ifdef D3D_BUFFER_ACCESS
D3D_RESULT IDirect3DDevice8::GetFrontBuffer(IDirect3DSurface8*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::GetBackBuffer(D3D_U32 index, D3DBACKBUFFERTYPE type, IDirect3DSurface8** result)
{
    return swap_chain->GetBackBuffer(index, type, result);
}
#endif

D3D_RESULT IDirect3DDevice8::GetDepthStencilSurface(IDirect3DSurface8** result)
{
    *result = set_depth_stencil_target;
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::BeginScene()
{
    device.submit(commands_copy);
    // todo: use SetRenderTarget() values? (not sure if that must be before BeginScene)
    commands.begin_render_pass(surface, depth_buffer.ptr.get(), nullptr);
    // commands.set_bind_group(0, static_bind_group, 0); // offset must be provided
    commands.set_bind_group(1, white_texture_bind_group);
    commands.set_bind_group(2, white_texture_bind_group);
    commands.set_vertex_buffer(1, const_vertex_buffer);
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::EndScene()
{
    device.submit(commands_copy);
    device.submit(commands);
    state_writes = 0;
    uniform_state_dirty = true;
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::GetRenderTarget(IDirect3DSurface8** result)
{
    *result = set_color_target;
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetRenderTarget(
    IDirect3DSurface8* color_target,
    IDirect3DSurface8* depth_stencil_target)
{
    this->set_color_target = color_target;
    this->set_depth_stencil_target = depth_stencil_target;
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetGammaRamp(D3D_U32, const D3DGAMMARAMP*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::Present(void*, void*, void*, void*)
{
    surface.present();
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetViewport(const D3DVIEWPORT8*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetLight(D3D_U32 index, const D3DLIGHT8* value)
{
    if (index >= 4)
        return D3DERR_INVALIDCALL;
    uniform_state.lights[index] = *value;
    uniform_state_dirty = true;
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::LightEnable(D3D_U32 index, D3D_BOOL value)
{
    if (value)
        uniform_state.light_enable_bits |= 1 << index;
    else
        uniform_state.light_enable_bits &= ~(1 << index);
    uniform_state_dirty = true;
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetTexture(D3D_U32 stage, IDirect3DBaseTexture8* texture)
{
    textures[stage] = texture;
    // bind group 0 is reserved for global uniforms
    if (texture)
    {
        commands.set_bind_group(stage + 1, texture->bind_group);
    }
    else
    {
        commands.set_bind_group(stage + 1, white_texture_bind_group);
    }
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetTextureStageState(D3D_U32 stage, D3DTEXTURESTAGESTATETYPE state, D3D_U32 value)
{
    auto& tss = uniform_state.texture_state[stage];
    switch (state)
    {
    case D3DTSS_COLOROP: tss.color_op = value;
        break;
    case D3DTSS_COLORARG1: tss.color_arg1 = value;
        break;
    case D3DTSS_COLORARG2: tss.color_arg2 = value;
        break;
    case D3DTSS_ALPHAOP: tss.alpha_op = value;
        break;
    case D3DTSS_ALPHAARG1: tss.alpha_arg1 = value;
        break;
    case D3DTSS_ALPHAARG2: tss.alpha_arg2 = value;
        break;
    case D3DTSS_TEXTURETRANSFORMFLAGS: tss.ttff = value;
        break;
    case D3DTSS_TEXCOORDINDEX: tss.texcoordindex = value;
        break;
    default: return D3D_OK;
    }
    uniform_state_dirty = true;
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetMaterial(const D3DMATERIAL8* value)
{
    uniform_state.material = *value;
    uniform_state_dirty = true;
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::GetTransform(D3DTRANSFORMSTATETYPE type, D3DMATRIX* result)
{
    if (type >= D3DTS_COUNT)
        return D3DERR_INVALIDCALL;
    *result = uniform_state.ts[type];
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetTransform(D3DTRANSFORMSTATETYPE type, const D3DMATRIX* value)
{
    if (type >= D3DTS_COUNT)
        return D3DERR_INVALIDCALL;
    uniform_state.ts[type] = *value;
    uniform_state_dirty = true;
    return D3D_OK;
}

WgpuBlendFactor d3d_to_wgpu_blend_factor(D3D_U32 f)
{
    switch (f)
    {
    default:
        assert(0 && "invalid blend factor");
        return WgpuBlendFactor::Zero;
    case D3DBLEND_ZERO:
        return WgpuBlendFactor::Zero;
    case D3DBLEND_ONE:
        return WgpuBlendFactor::One;
    case D3DBLEND_SRCCOLOR:
        return WgpuBlendFactor::SrcColor;
    case D3DBLEND_INVSRCCOLOR:
        return WgpuBlendFactor::OneMinusSrcColor;
    case D3DBLEND_SRCALPHA:
        return WgpuBlendFactor::SrcAlpha;
    case D3DBLEND_INVSRCALPHA:
        return WgpuBlendFactor::OneMinusSrcAlpha;
    }
}

WgpuCompare d3d_to_wgpu_compare(D3D_U32 f)
{
    switch (f)
    {
    default:
        assert(0 && "invalid compare function");
        return WgpuCompare::Always;
    case D3DCMP_NEVER:
        return WgpuCompare::Never;
    case D3DCMP_LESS:
        return WgpuCompare::Less;
    case D3DCMP_EQUAL:
        return WgpuCompare::Equal;
    case D3DCMP_LESSEQUAL:
        return WgpuCompare::LessEqual;
    case D3DCMP_GREATER:
        return WgpuCompare::Greater;
    case D3DCMP_NOTEQUAL:
        return WgpuCompare::NotEqual;
    case D3DCMP_GREATEREQUAL:
        return WgpuCompare::GreaterEqual;
    case D3DCMP_ALWAYS:
        return WgpuCompare::Always;
    }
}

D3D_RESULT IDirect3DDevice8::SetRenderState(D3DRENDERSTATETYPE type, D3D_U32 value)
{
    if (type >= D3DRS_COUNT)
        return D3DERR_INVALIDCALL;

    // Set blend and depth buffer state in pipeline state (which will add a new pipeline object to the pipeline cache
    // on the next draw) as it can't be emulated in the shader (at least well)
    // Everything else goes to the uniform state, which will be uploaded on the next draw.
    switch (type)
    {
    case D3DRS_ALPHABLENDENABLE:
        pipeline_state.alpha_blend_enable = value;
        pipeline_state_dirty = true;
        return D3D_OK;
    case D3DRS_SRCBLEND:
        pipeline_state.src_blend = d3d_to_wgpu_blend_factor(value);
        pipeline_state_dirty = true;
        return D3D_OK;
    case D3DRS_DESTBLEND:
        pipeline_state.dest_blend = d3d_to_wgpu_blend_factor(value);
        pipeline_state_dirty = true;
        return D3D_OK;
    case D3DRS_ZWRITEENABLE:
        pipeline_state.z_write_enable = value;
        pipeline_state_dirty = true;
        return D3D_OK;
    case D3DRS_ZBIAS:
        pipeline_state.z_bias = value;
        pipeline_state_dirty = true;
        return D3D_OK;
    case D3DRS_ZFUNC:
        pipeline_state.z_func = d3d_to_wgpu_compare(value);
        pipeline_state_dirty = true;
        return D3D_OK;

    case D3DRS_ALPHATESTENABLE:
        uniform_state.rs.alpha_test_enable = value;
        break;
    case D3DRS_ALPHAREF:
        uniform_state.rs.alpha_test_ref = value / 255.0f;
        break;
    case D3DRS_ALPHAFUNC:
        uniform_state.rs.alpha_test_func = value;
        break;

    case D3DRS_LIGHTING:
        uniform_state.rs.lighting_enable = value;
        break;
    case D3DRS_AMBIENT:
        {
            // value is D3DCOLOR in ARGB format
            uniform_state.rs.ambient_color.r = (value >> 16) / 255.0f;
            uniform_state.rs.ambient_color.g = ((value >> 8) & 0xff) / 255.0f;
            uniform_state.rs.ambient_color.b = (value & 0xff) / 255.0f;
            uniform_state.rs.ambient_color.a = ((value >> 24) & 0xff) / 255.0f;
            break;
        }
    case D3DRS_SPECULARENABLE:
        uniform_state.rs.specular_enable = value;
        break;
    case D3DRS_AMBIENTMATERIALSOURCE:
        uniform_state.rs.ambient_source = value;
        break;
    case D3DRS_DIFFUSEMATERIALSOURCE:
        uniform_state.rs.diffuse_source = value;
        break;
    case D3DRS_SPECULARMATERIALSOURCE:
        uniform_state.rs.specular_source = value;
        break;
    case D3DRS_EMISSIVEMATERIALSOURCE:
        uniform_state.rs.emissive_source = value;
        break;
    default:
        return D3D_OK; // ignore
    }
    uniform_state_dirty = true;
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetStreamSource(D3D_U32 index, IDirect3DVertexBuffer8* vertex_buffer, D3D_U32)
{
    this->vertex_buffer = vertex_buffer;
    commands.set_vertex_buffer(index, vertex_buffer->buffer);
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetIndices(IDirect3DIndexBuffer8* index_buffer, D3D_U32 base_vertex_index)
{
    this->base_vertex_index = base_vertex_index;
    this->index_buffer = index_buffer;
    commands.set_index_buffer(index_buffer->buffer, index_buffer->format, 0);
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetVertexShader(D3D_U32 fvf)
{
    pipeline_state.fvf = fvf;
    pipeline_state_dirty = true;
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::Clear(
    D3D_U32,
    const D3DRECT*,
    D3D_U32,
    D3DCOLOR color,
    D3D_F32 z,
    D3D_U32 stencil)
{
    // todo
    // I think clearing works differently in WGPU inside of a render pass...
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::CopyRects(
    IDirect3DSurface8* source_surface,
    const RECT* source_rect_array,
    D3D_U32 source_rect_len,
    IDirect3DSurface8* dest_surface,
    const POINT* dest_offset_array)
{
    // Only used with null source_rect_array and dest_offset_array, or
    // from SurfaceClass::Copy() with a single variable source and dest.
    assert(source_rect_len <= 1);
    WgpuTexelCopyOffset source_offset{
        .level = source_surface->texture_level,
    };
    WgpuTexelCopyOffset dest_offset{
        .level = dest_surface->texture_level,
    };
    WgpuSize size{};
    if (source_rect_array)
    {
        source_offset.x = source_rect_array->left;
        source_offset.y = source_rect_array->top;
        size.width = source_rect_array->right - source_rect_array->left;
        size.height = source_rect_array->bottom - source_rect_array->top;
    }
    if (dest_offset_array)
    {
        dest_offset.x = dest_offset_array->x;
        dest_offset.y = dest_offset_array->y;
    }
    // note: only valid outside of a render pass, so we need to submit the commands
    // on a separate command encoder. This just means we don't get to share the same
    // texture object between multiple draws with different data, which is a terrible
    // idea anyway.
    commands_copy.copy_texture_to_texture(
        dest_surface->texture,
        dest_offset,
        source_surface->texture,
        source_offset,
        source_rect_array ? &size : nullptr);
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::DrawIndexedPrimitive(
    D3DPRIMITIVETYPE primitive_type, // only D3DPT_TRIANGLELIST seems to be used
    D3D_U32 min_vertex_index,
    D3D_U32 num_vertices, // ???
    D3D_U32 index_first,
    D3D_U32 polygon_count)
{
    if (uniform_state_dirty)
    {
        if (state_writes == max_states_before_flush)
        {
            // Uniform state buffer is full, need to flush pending draws that are
            // offset into it.
            device.submit(commands_copy);
            device.submit(commands);
            state_writes = 0;
            // now need to restore all the state in the render pass, see BeginScene,
            // various Set*() methods
            // fixme: this will clear the depth buffer currently ...
            commands.begin_render_pass(surface, depth_buffer.ptr.get(), nullptr);
            commands.set_bind_group(1, textures[0] ? textures[0]->bind_group : white_texture_bind_group);
            commands.set_bind_group(2, textures[1] ? textures[1]->bind_group : white_texture_bind_group);
            pipeline_state_dirty = true;
            if (vertex_buffer) commands.set_vertex_buffer(0, vertex_buffer->buffer);
            commands.set_vertex_buffer(1, const_vertex_buffer);
            if (index_buffer) commands.set_index_buffer(index_buffer->buffer, index_buffer->format);
        }

        auto offset = state_writes * uniform_state_size_aligned;
        state_writes += 1;
        uniform_state_dirty = false;
        state_buffer.write(offset, (uint8_t*)&uniform_state, sizeof(uniform_state));
        commands.set_bind_group(0, static_bind_group, offset);
    }
    if (pipeline_state_dirty)
    {
        for (pipeline_cache_index = 0;
             pipeline_cache_index < pipeline_cache.size();
             pipeline_cache_index++)
        {
            auto& entry = pipeline_cache[pipeline_cache_index];
            if (entry.state == pipeline_state)
            {
                break;
            }
        }
        if (pipeline_cache_index == pipeline_cache.size())
        {
            pipeline_cache.push_back({
                .state = pipeline_state,
                .pipeline = create_pipeline_for_state(device, shader_module, pipeline_state),
            });
        }
        commands.set_pipeline(pipeline_cache[pipeline_cache_index].pipeline);
        pipeline_state_dirty = false;
    }

    commands.draw_indexed(WgpuDrawIndexed{
        .index_first = index_first,
        .index_count = polygon_count * 3,
        .base_vertex = (int32_t)base_vertex_index,
    });
    return D3D_OK;
}

#ifdef D3D_ADDITIONAL_SWAP_CHAIN
Ptr<SwapChain> SwapChain::Create(Ptr<Surface> surface)
{
    return Ptr(new SwapChain(std::move(surface)));
}

D3D_RESULT SwapChain::GetBackBuffer(D3D_U32, D3DBACKBUFFERTYPE, IDirect3DSurface8** result)
{
    *result = back_buffer.add_ref();
    return D3D_OK;
}
#endif

D3D_RESULT IDirect3DSurface8::GetDesc(D3DSURFACE_DESC* result)
{
    *result = desc;
    return D3D_OK;
}

D3D_RESULT IDirect3DSurface8::LockRect(D3DLOCKED_RECT* lock, RECT* rect, D3D_U32 flags)
{
    // I assume DXTn formats don't support offsets?
    auto bits = data.data();
    if (rect) bits += rect->left * bpp / 8 + rect->top * pitch;
    lock->pBits = bits;
    lock->Pitch = pitch;
    return D3D_OK;
}

D3D_RESULT IDirect3DSurface8::UnlockRect()
{
    texture.write(texture_level, data.data(), data.size());
    return D3D_OK;
}

D3D_RESULT IDirect3DBaseTexture8::LockRect(
    D3D_U32 level,
    D3DLOCKED_RECT* lock,
    RECT* rect,
    D3D_U32 flags)
{
    if (level >= levels.size())
        return D3DERR_INVALIDCALL;
    return levels[level]->LockRect(lock, rect, flags);
}

D3D_RESULT IDirect3DBaseTexture8::UnlockRect(D3D_U32 level)
{
    if (level >= levels.size())
        return D3DERR_INVALIDCALL;
    return levels[level]->UnlockRect();
}

D3D_U32 IDirect3DBaseTexture8::GetLevelCount()
{
    return levels.size();
}

D3D_RESULT IDirect3DBaseTexture8::GetLevelDesc(D3D_U32 level, D3DSURFACE_DESC*)
{
    if (level >= levels.size())
        return D3DERR_INVALIDCALL;
    return D3D_OK;
}

D3D_RESULT IDirect3DBaseTexture8::GetSurfaceLevel(D3D_U32 level, IDirect3DSurface8** surface)
{
    if (level >= levels.size())
        return D3DERR_INVALIDCALL;
    return levels[level].CopyTo(surface);
}

D3D_U32 IDirect3DBaseTexture8::GetPriority()
{
    return D3D_OK;
}

D3D_U32 IDirect3DBaseTexture8::SetPriority(D3D_U32)
{
    return D3D_OK;
}

IDirect3DTexture8::IDirect3DTexture8(
    wgpu::Texture texture_,
    wgpu::BindGroup bind_group,
    D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp,
    D3D_U32 levels)
    : IDirect3DBaseTexture8(std::move(texture_), std::move(bind_group))
{
    for (D3D_U32 level = 0; level < levels; ++level)
    {
        if (!width) width = 1;
        if (!height) height = 1;
        this->levels.push_back(IDirect3DSurface8::Create(texture.clone(), level, width, height, format, bpp));
        width >>= 1;
        height >>= 1;
    }
}

D3D_RESULT IDirect3DResource8::Lock(
    D3D_U32 lock_offset,
    D3D_U32 lock_size,
    D3D_U8** result,
    D3D_U32 flags)
{
    // todo: move to Queue::write_buffer_with() which matches Lock()
    // but we need to handle aligning to COPY_BUFFER_ALIGNMENT (4) too...
    auto lock_end = lock_offset + lock_size;
    lock_end = (lock_end + 3) & ~3;
    if (local_data.size() < lock_end)
        local_data.resize(lock_end);
    this->lock_start = lock_offset & ~3;
    this->lock_end = lock_end;
    *result = local_data.data() + lock_offset;
    return D3D_OK;
}

D3D_RESULT IDirect3DResource8::Unlock()
{
    // just write the whole buffer for now, it should still be up to date
    // from the last Lock() call, unless there's a GPU-side copy, I guess?
    buffer.write(lock_start, local_data.data() + lock_start, lock_end - lock_start);
    lock_start = lock_end = 0;
    return D3D_OK;
}
