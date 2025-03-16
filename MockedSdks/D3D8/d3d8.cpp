#include "d3d8.h"

inline D3DMATRIX D3DMATRIX::IDENTITY = {
    {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    },
};


// offer both a 32 and 16 bit format so it can find either.
static const D3DDISPLAYMODE DummyModes[] = {
    {.Width = 800, .Height = 600, .Format = D3DFMT_X8R8G8B8},
    {.Width = 800, .Height = 600, .Format = D3DFMT_R5G6B5},
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
    D3DTEXOPCAPS_BUMPENVMAP |
    D3DTEXOPCAPS_BUMPENVMAPLUMINANCE |
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
    Position = 0,
    Blend = 1,
    Normal = 2,
    Tex1 = 3,
    Tex2 = 4,
    Diffuse = 5,
    Specular = 6,
};

const std::string_view WGSL = R"(
// struct VertexInput {
//     @location(0) position: vec4f, // FVF_XYZ
//     @location(1) blend: f32,      // FVF_XYZB4 | FVF_LASTBETA_UBYTE4
//     @location(2) normal: vec3f,   // FVF_NORMAL
//     @location(3) tex1: vec2f,     // FVF_TEX1
//     @location(4) tex2: vec2f,     // FVF_TEX2
//     @location(5) diffuse: u32,  // FVF_DIFFUSE
//     @location(6) specular: u32, // FVF_SPECULAR
// }

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec4f,
}

@vertex
fn vs_xyz(
    @builtin(vertex_index) index: u32,
    @location(0) position: vec3f,
) -> VertexOutput {
    var output: VertexOutput;
    output.position = vec4f(position, 1.0);
    output.color = vec4f(1.0, 0.0, 1.0, 1.0);
    return output;
}

@vertex
fn vs_xyz_diffuse(
    @builtin(vertex_index) index: u32,
    @location(0) position: vec3f,
    @location(5) diffuse: u32,
) -> VertexOutput {
    var output: VertexOutput;
    output.position = vec4f(position, 1.0);
    output.color = unpack4x8unorm(diffuse);
    return output;
}

@vertex
fn vs_xyz_uv(
    @builtin(vertex_index) index: u32,
    @location(0) position: vec3f,
    @location(3) tex1: vec2f,
) -> VertexOutput {
    var output: VertexOutput;
    output.position = vec4f(position, 1.0);
    output.color = vec4f(tex1, 0.0, 1.0);
    return output;
}

@vertex
fn vs_xyz_diffuse_uv(
    @builtin(vertex_index) index: u32,
    @location(0) position: vec3f,
    @location(3) tex1: vec2f,
    @location(5) diffuse: u32,
) -> VertexOutput {
    var output: VertexOutput;
    output.position = vec4f(position, 1.0);
    output.color = vec4f(tex1, 0.0, 1.0) * unpack4x8unorm(diffuse);
    return output;
}

@fragment
fn fs_todo(input: VertexOutput) -> @location(0) vec4<f32> {
    return input.color;
}
)";

wgpu::Pipeline create_pipeline_for_fvf(
    wgpu::Device& device,
    wgpu::ShaderModule& shader_module,
    std::uint32_t fvf)
{
    // build a new pipeline to match FVF
    WgpuShaderDesc vertex_shader;
    if (fvf & D3DFVF_TEXCOUNT_MASK)
    {
        if (fvf & D3DFVF_DIFFUSE)
            vertex_shader = shader_module.desc("vs_xyz_diffuse_uv");
        else
            vertex_shader = shader_module.desc("vs_xyz_uv");
    }
    else
    {
        if (fvf & D3DFVF_DIFFUSE)
            vertex_shader = shader_module.desc("vs_xyz_diffuse");
        else
            vertex_shader = shader_module.desc("vs_xyz");
    }
    auto fragment_shader = shader_module.desc("fs_todo");

    auto size = 0U;

    // This must match just d3d8fvf.cpp's FVFInfoClass
    // Should be able to port to that later
    std::vector<WgpuVertexBufferLayout> attrs;
    if (fvf & D3DFVF_XYZ)
    {
        attrs.push_back({
            .shader_location = (uint32_t)WgslLocation::Position,
            .format = WgpuVertexFormat::Float32x3,
            .offset = size,
        });
        size += 3 * sizeof(float);
    }
    if (fvf & D3DFVF_XYZB4)
    {
        attrs.push_back({
            .shader_location = (uint32_t)WgslLocation::Blend,
            .format = WgpuVertexFormat::Float32x4,
            .offset = size,
        });
        size += sizeof(float) * 4;
    }
    if (fvf & D3DFVF_NORMAL)
    {
        attrs.push_back({
            .shader_location = (uint32_t)WgslLocation::Normal,
            .format = WgpuVertexFormat::Float32x3,
            .offset = size,
        });
        size += 3 * sizeof(float);
    }
    if (fvf & D3DFVF_DIFFUSE)
    {
        attrs.push_back({
            .shader_location = (uint32_t)WgslLocation::Diffuse,
            .format = WgpuVertexFormat::Uint8x4,
            .offset = size,
        });
        size += 4;
    }
    if (fvf & D3DFVF_SPECULAR)
    {
        attrs.push_back({
            .shader_location = (uint32_t)WgslLocation::Specular,
            .format = WgpuVertexFormat::Uint8x4,
            .offset = size,
        });
        size += 4;
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
        if ((fvf >> 16) & 3) __debugbreak();
        attrs.push_back({
            .shader_location = (uint32_t)WgslLocation::Tex1,
            .format = WgpuVertexFormat::Float32x2,
            .offset = size,
        });
        size += 2 * sizeof(float);
    }
    if (tex_count > 1)
    {
        if ((fvf >> 18) & 3) __debugbreak();
        attrs.push_back({
            .shader_location = (uint32_t)WgslLocation::Tex2,
            .format = WgpuVertexFormat::Float32x2,
            .offset = size,
        });
        size += 2 * sizeof(float);
    }

    return device.create_pipeline({
        .stride = size,
        .layout_ptr = attrs.data(),
        .layout_len = attrs.size(),
        .vertex_shader = &vertex_shader,
        .fragment_shader = &fragment_shader,
    });
}

IDirect3DDevice8::IDirect3DDevice8(wgpu::Device device_, wgpu::Surface surface)
    : device(std::move(device_)),
      surface(std::move(surface)),
      commands(device.create_commands()),
      shader_module(device.create_shader_module(WGSL))

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
    if (format != D3DFMT_A8R8G8B8)
        return D3DERR_WRONGTEXTUREFORMAT;
    return D3D_OK;
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
    auto wgpu_device = wgpu.create_device(index);
    auto wgpu_surface = wgpu.create_surface(hwnd);

    D3D_U32 width = present_params->BackBufferWidth;
    D3D_U32 height = present_params->BackBufferHeight;
    D3DFORMAT format = present_params->BackBufferFormat;

    if (!wgpu_surface.configure(wgpu_device.ptr.get(), width, height))
        return D3DERR_WRONGTEXTUREFORMAT;

    *result = IDirect3DDevice8::Create(std::move(wgpu_device), std::move(wgpu_surface)).Detach();
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
    auto vertex_buffer = device.create_buffer(length, WGPU_BUFFER_USAGE_VERTEX);
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

    auto index_buffer = device.create_buffer(length, WGPU_BUFFER_USAGE_INDEX);
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
    auto bpp = FormatBitsPerPixel(format);
    if (bpp == 0)
        return D3DERR_INVALIDCALL;
    *result = IDirect3DSurface8::Create(0, nullptr, width, height, format, bpp).Detach();
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::CreateTexture(D3D_U32 width, D3D_U32 height, D3D_U32 levels, D3D_U32 usage,
                                           D3DFORMAT format, D3DPOOL pool, IDirect3DTexture8** result)
{
    WgpuTextureFormat wgpu_format;
    switch (format)
    {
    // Normalize color formats to Rgba8Unorm, as WGPU doesn't support <32-bit colors.
    // They will need to be transformed on upload.
    default: wgpu_format = WgpuTextureFormat::Rgba8Unorm;
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
    auto texture = device.create_texture(wgpu_format, width, height, levels,
                                         WGPU_TEXTURE_USAGE_TEXTURE_BINDING | WGPU_TEXTURE_USAGE_COPY_DST);
    auto bpp = FormatBitsPerPixel(format);
    if (bpp == 0)
        return D3DERR_INVALIDCALL;
    *result = IDirect3DTexture8::Create(std::move(texture), width, height, format, bpp, levels).Detach();
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
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::EndScene()
{
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
    device.submit(commands);
    surface.present();
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetViewport(const D3DVIEWPORT8*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetLight(D3D_U32 index, const D3DLIGHT8*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::LightEnable(D3D_U32 index, D3D_BOOL)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetTexture(D3D_U32 stage, IDirect3DBaseTexture8* texture)
{
    // bind group 0 is reserved for global uniforms
    commands.set_bind_group_to_texture(stage + 1, texture->texture);
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetTextureStageState(D3D_U32 stage, D3DTEXTURESTAGESTATETYPE state, D3D_U32 value)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetMaterial(const D3DMATERIAL8*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::GetTransform(D3DTRANSFORMSTATETYPE type, D3DMATRIX* result)
{
    *result = matrices[type];
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetTransform(D3DTRANSFORMSTATETYPE type, const D3DMATRIX* value)
{
    matrices[type] = *value;
    // todo: update uniforms
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetRenderState(D3DRENDERSTATETYPE, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetStreamSource(D3D_U32 index, IDirect3DVertexBuffer8* vertex_buffer, D3D_U32)
{
    commands.set_vertex_buffer(index, vertex_buffer->buffer);
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetIndices(IDirect3DIndexBuffer8* index_buffer, D3D_U32 base_vertex_index)
{
    this->base_vertex_index = base_vertex_index;
    commands.set_index_buffer(index_buffer->buffer, index_buffer->format, 0);
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetVertexShader(D3D_U32 fvf)
{
    for (auto& pipeline : pipelines)
    {
        if (pipeline.fvf == fvf)
        {
            commands.set_pipeline(pipeline.pipeline);
            return D3D_OK;
        }
    }
    auto pipeline = create_pipeline_for_fvf(device, shader_module, fvf);
    commands.set_pipeline(pipeline);
    pipelines.push_back({
        .fvf = fvf,
        .pipeline = std::move(pipeline),
    });

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
    commands.begin_render_pass(surface, nullptr);
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::CopyRects(IDirect3DSurface8*, const RECT*, D3D_U32, IDirect3DSurface8*, const POINT*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::DrawIndexedPrimitive(
    D3DPRIMITIVETYPE primitive_type, // only D3DPT_TRIANGLELIST seems to be used
    D3D_U32 min_vertex_index,
    D3D_U32 num_vertices, // ???
    D3D_U32 index_first,
    D3D_U32 polygon_count)
{
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

CComPtr<IDirect3DSurface8> IDirect3DSurface8::Create(
    D3D_U32 texture_level,
    IDirect3DBaseTexture8* texture,
    D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp)
{
    return new IDirect3DSurface8(texture_level, texture, width, height, format, bpp);
}

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
    // todo: crashes due to format size mismatch
    // if (texture)
    //     texture->texture.write(texture_level, data.data(), data.size());
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
    wgpu::Texture texture,
    D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp,
    D3D_U32 levels)
    : IDirect3DBaseTexture8(std::move(texture))
{
    for (D3D_U32 i = 0; i < levels; ++i)
    {
        if (!width) width = 1;
        if (!height) height = 1;
        this->levels.push_back(IDirect3DSurface8::Create(i, this, width, height, format, bpp));
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
