#include <atomic>
#include <vector>

#include "d3d8.h"

template <typename T>
struct Ptr
{
    Ptr() : ptr(nullptr)
    {
    }

    explicit Ptr(T* ptr) : ptr(ptr)
    {
    }

    Ptr(Ptr&& other) noexcept : ptr(other.move())
    {
    }

    ~Ptr()
    {
        if (ptr)
            ptr->Release();
    }

    Ptr& operator=(Ptr&& other) noexcept
    {
        if (ptr)
            ptr->Release();
        ptr = other.move();
        return *this;
    }

    T* add_ref()
    {
        if (ptr)
            ptr->AddRef();
        return ptr;
    }

    T* move()
    {
        auto result = ptr;
        ptr = nullptr;
        return result;
    }

    T* operator->() { return ptr; }
    T& operator*() { return *ptr; }

    T* ptr;
};

// offer both a 32 and 16 bit format so it can find either.
static const D3DDISPLAYMODE DummyModes[] = {
    { .Width = 800, .Height = 600, .Format = D3DFMT_X8R8G8B8 },
    { .Width = 800, .Height = 600, .Format = D3DFMT_R5G6B5 },
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

struct Adapter;
struct Device;
struct SwapChain;
struct Surface;
struct Texture;
struct VertexBuffer;
struct IndexBuffer;

template <typename I>
struct Unknown : I
{
    D3D_U32 AddRef() override { return ++count; }

    D3D_U32 Release() override
    {
        if (--count)
            return count;
        delete this;
        return 0;
    }

protected:
    Unknown() : count(1)
    {
    }

private:
    std::atomic_uint32_t count;
};

struct Adapter final : Unknown<IDirect3D8>
{
    static Ptr<Adapter> Create();

    D3D_U32 GetAdapterCount() override;
    D3D_U32 GetAdapterModeCount(D3D_U32) override;
    D3D_RESULT EnumAdapterModes(D3D_U32, D3D_U32, D3DDISPLAYMODE* mode) override;
    D3D_RESULT GetAdapterDisplayMode(D3D_U32, D3DDISPLAYMODE* mode) override;
    D3D_RESULT GetAdapterIdentifier(D3D_U32, D3DENUM, D3DADAPTER_IDENTIFIER8* id) override;
    D3D_RESULT GetDeviceCaps(D3D_U32, D3DDEVTYPE, D3DCAPS8* caps) override;
    D3D_RESULT CheckDeviceFormat(D3D_U32, D3DDEVTYPE, D3DFORMAT, D3D_U32, D3DRESOURCETYPE, D3DFORMAT) override;
    D3D_RESULT CheckDepthStencilMatch(D3D_U32, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, D3DFORMAT) override;
    D3D_RESULT CreateDevice(int, D3DDEVTYPE, HWND, D3D_U32, D3DPRESENT_PARAMETERS*, IDirect3DDevice8** device) override;
};

struct Device final : Unknown<IDirect3DDevice8>
{
    static Ptr<Device> Create();
    D3D_RESULT GetDeviceCaps(D3DCAPS8*) override;
    D3D_RESULT GetDisplayMode(D3DDISPLAYMODE*) override;
    D3D_RESULT ValidateDevice(D3D_U32*) override;
    D3D_RESULT Reset(const D3DPRESENT_PARAMETERS*) override;
    D3D_RESULT TestCooperativeLevel() override;
    D3D_RESULT ResourceManagerDiscardBytes(D3D_U32) override;
    D3D_U32 GetAvailableTextureMem() override;
    D3D_RESULT CreateVertexBuffer(D3D_U32, D3D_U32, D3D_U32, D3D_U32, IDirect3DVertexBuffer8**) override;
    D3D_RESULT CreateIndexBuffer(D3D_U32, D3D_U32, D3D_U32, D3D_U32, IDirect3DIndexBuffer8**) override;
    D3D_RESULT CreateImageSurface(D3D_U32, D3D_U32, D3DFORMAT, IDirect3DSurface8**) override;
    D3D_RESULT CreateTexture(D3D_U32, D3D_U32, D3D_U32, D3D_U32, D3DFORMAT, D3DPOOL, IDirect3DTexture8**) override;
    D3D_RESULT UpdateTexture(IDirect3DBaseTexture8*, IDirect3DBaseTexture8*) override;
    D3D_RESULT CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS*, IDirect3DSwapChain8**) override;
    D3D_RESULT GetFrontBuffer(IDirect3DSurface8*) override;
    D3D_RESULT GetBackBuffer(D3D_U32, D3DBACKBUFFERTYPE, IDirect3DSurface8**) override;
    D3D_RESULT GetDepthStencilSurface(IDirect3DSurface8**) override;
    D3D_RESULT BeginScene() override;
    D3D_RESULT EndScene() override;
    D3D_RESULT GetRenderTarget(IDirect3DSurface8**) override;
    D3D_RESULT SetRenderTarget(IDirect3DSurface8*, IDirect3DSurface8*) override;
    D3D_RESULT SetGammaRamp(D3D_U32, const D3DGAMMARAMP*) override;
    D3D_RESULT Present(void*, void*, void*, void*) override;
    D3D_RESULT SetViewport(const D3DVIEWPORT8*) override;
    D3D_RESULT SetLight(D3D_U32 index, const D3DLIGHT8*) override;
    D3D_RESULT LightEnable(D3D_U32 index, D3D_BOOL) override;
    D3D_RESULT SetTexture(D3D_U32 stage, IDirect3DBaseTexture8*) override;
    D3D_RESULT SetTextureStageState(D3D_U32 stage, D3DTEXTURESTAGESTATETYPE state, D3D_U32 value) override;
    D3D_RESULT SetMaterial(const D3DMATERIAL8*) override;
    D3D_RESULT GetTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX*) override;
    D3D_RESULT SetTransform(D3DTRANSFORMSTATETYPE, const D3DMATRIX*) override;
    D3D_RESULT SetRenderState(D3DRENDERSTATETYPE, D3D_U32) override;
    D3D_RESULT SetStreamSource(D3D_U32, IDirect3DVertexBuffer8*, D3D_U32) override;
    D3D_RESULT SetIndices(IDirect3DIndexBuffer8*, D3D_U32) override;
    D3D_RESULT SetVertexShader(D3D_U32) override;
    D3D_RESULT Clear(D3D_U32, const D3DRECT*, D3D_U32, D3DCOLOR, D3D_F32, D3D_U32) override;
    D3D_RESULT CopyRects(IDirect3DSurface8*, const RECT*, D3D_U32, IDirect3DSurface8*, const POINT*) override;
    D3D_RESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE, D3D_U32, D3D_U32, D3D_U32, D3D_U32) override;

protected:
    Device();

    Ptr<SwapChain> swap_chain;
};

struct SwapChain final : Unknown<IDirect3DSwapChain8>
{
    static Ptr<SwapChain> Create() { return Ptr(new SwapChain()); }

    D3D_RESULT GetBackBuffer(D3D_U32, D3DBACKBUFFERTYPE, IDirect3DSurface8**) override;

protected:
    Ptr<Surface> back_buffer;
};

struct Surface : Unknown<IDirect3DSurface8>
{
    static Ptr<Surface> Create(D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp);

    D3D_RESULT GetDesc(D3DSURFACE_DESC*) override;
    D3D_RESULT LockRect(D3DLOCKED_RECT*, RECT*, D3D_U32) override;
    D3D_RESULT UnlockRect() override;

protected:
    explicit Surface(D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp);

    std::vector<D3D_U8> data;
    D3DSURFACE_DESC desc;
    D3D_U32 bpp;
    D3D_U32 pitch;
};

struct Texture final : Unknown<IDirect3DTexture8>
{
    static Ptr<Texture> Create(
        D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp, D3D_U32 levels
    );

    D3D_RESULT LockRect(D3D_U32 level, D3DLOCKED_RECT* lock, RECT* rect, D3D_U32 flags) override;
    D3D_RESULT UnlockRect(D3D_U32) override;
    D3D_U32 GetLevelCount() override;
    D3D_RESULT GetLevelDesc(D3D_U32, D3DSURFACE_DESC*) override;
    D3D_RESULT GetSurfaceLevel(D3D_U32, IDirect3DSurface8**) override;
    D3D_U32 GetPriority() override;
    D3D_U32 SetPriority(D3D_U32) override;

protected:
    explicit Texture(D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp, D3D_U32 levels);

    std::vector<Ptr<Surface>> levels;
};

struct VertexBuffer final : Unknown<IDirect3DVertexBuffer8>
{
    static Ptr<VertexBuffer> Create(D3D_U32 length)
    {
        return Ptr(new VertexBuffer(length));
    }

    D3D_RESULT Lock(D3D_U32, D3D_U32, D3D_U8**, D3D_U32) override;
    D3D_RESULT Unlock() override;

private:
    explicit VertexBuffer(D3D_U32 length) : data(length)
    {
    }

    std::vector<D3D_U8> data;
};

struct IndexBuffer final : Unknown<IDirect3DIndexBuffer8>
{
    static Ptr<IndexBuffer> Create(D3D_U32 length);

    D3D_RESULT Lock(D3D_U32, D3D_U32, D3D_U8**, D3D_U32) override;
    D3D_RESULT Unlock() override;

private:
    explicit IndexBuffer(D3D_U32 length);

    std::vector<D3D_U8> data;
};

IDirect3D8* Direct3DCreate8(int)
{
    return Adapter::Create().move();
}

Ptr<Adapter> Adapter::Create()
{
    return Ptr(new Adapter());
}

D3D_U32 Adapter::GetAdapterCount()
{
    return 1;
}

D3D_U32 Adapter::GetAdapterModeCount(D3D_U32)
{
    return ARRAYSIZE(DummyModes);
}

D3D_RESULT Adapter::EnumAdapterModes(D3D_U32 adapter, D3D_U32 index, D3DDISPLAYMODE* mode)
{
    if (adapter != 0)
        return D3DERR_INVALIDCALL;
    if (index >= ARRAYSIZE(DummyModes))
        return D3DERR_INVALIDCALL;
    *mode = DummyModes[index];
    return D3D_OK;
}

D3D_RESULT Adapter::GetAdapterDisplayMode(D3D_U32 adapter, D3DDISPLAYMODE* mode)
{
    if (adapter != 0)
        return D3DERR_INVALIDCALL;
    *mode = DummyModes[0];
    return D3D_OK;
}

D3D_RESULT Adapter::GetAdapterIdentifier(D3D_U32, D3DENUM, D3DADAPTER_IDENTIFIER8* id)
{
    *id = {
        .Driver = "MockDriver",
        .Description = "MockDevice",
    };
    return D3D_OK;
}

D3D_RESULT Adapter::GetDeviceCaps(D3D_U32, D3DDEVTYPE, D3DCAPS8* caps)
{
    *caps = MockCaps;
    return D3D_OK;
}

D3D_RESULT Adapter::CheckDeviceFormat(D3D_U32, D3DDEVTYPE, D3DFORMAT, D3D_U32, D3DRESOURCETYPE, D3DFORMAT)
{
    return D3D_OK;
}

D3D_RESULT Adapter::CheckDepthStencilMatch(D3D_U32, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, D3DFORMAT)
{
    return D3D_OK;
}

D3D_RESULT Adapter::CreateDevice(
    int,
    D3DDEVTYPE,
    HWND,
    D3D_U32,
    D3DPRESENT_PARAMETERS*,
    IDirect3DDevice8** device)
{
    *device = Device::Create().move();
    return D3D_OK;
}

Ptr<Device> Device::Create()
{
    return Ptr(new Device());
}

D3D_RESULT Device::GetDeviceCaps(D3DCAPS8* caps)
{
    *caps = MockCaps;
    return D3D_OK;
}

D3D_RESULT Device::ValidateDevice(D3D_U32*)
{
    return D3D_OK;
}

D3D_RESULT Device::Reset(const D3DPRESENT_PARAMETERS*)
{
    return D3D_OK;
}

D3D_RESULT Device::GetDisplayMode(D3DDISPLAYMODE*)
{
    return D3D_OK;
}

D3D_RESULT Device::TestCooperativeLevel()
{
    return D3D_OK;
}

D3D_RESULT Device::ResourceManagerDiscardBytes(D3D_U32)
{
    return D3D_OK;
}

D3D_U32 Device::GetAvailableTextureMem()
{
    return 0x1000000;
}

D3D_RESULT Device::CreateVertexBuffer(D3D_U32 length, D3D_U32 usage, D3D_U32 fvf, D3D_U32 pool,
                                      IDirect3DVertexBuffer8** result)
{
    *result = VertexBuffer::Create(length).move();
    return D3D_OK;
}

D3D_RESULT Device::CreateIndexBuffer(D3D_U32 length, D3D_U32 usage, D3D_U32 format, D3D_U32 pool,
                                     IDirect3DIndexBuffer8** result)
{
    *result = IndexBuffer::Create(length).move();
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

D3D_RESULT Device::CreateImageSurface(D3D_U32 width, D3D_U32 height, D3DFORMAT format, IDirect3DSurface8** result)
{
    auto bpp = FormatBitsPerPixel(format);
    if (bpp == 0)
        return D3DERR_INVALIDCALL;
    *result = Surface::Create(width, height, format, bpp).move();
    return D3D_OK;
}

D3D_RESULT Device::CreateTexture(D3D_U32 width, D3D_U32 height, D3D_U32 levels, D3D_U32 usage,
                                 D3DFORMAT format, D3DPOOL pool, IDirect3DTexture8** result)
{
    if (levels == 0)
    {
        auto width_levels = 32 - std::countl_zero(width);
        auto height_levels = 32 - std::countl_zero(height);
        levels = min(width_levels, height_levels);
    }
    auto bpp = FormatBitsPerPixel(format);
    if (bpp == 0)
        return D3DERR_INVALIDCALL;
    *result = Texture::Create(width, height, format, bpp, levels).move();
    return D3D_OK;
}

D3D_RESULT Device::UpdateTexture(IDirect3DBaseTexture8*, IDirect3DBaseTexture8*)
{
    return D3D_OK;
}

D3D_RESULT Device::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS*, IDirect3DSwapChain8** result)
{
    *result = SwapChain::Create().move();
    return D3D_OK;
}

D3D_RESULT Device::GetFrontBuffer(IDirect3DSurface8*)
{
    return D3D_OK;
}

D3D_RESULT Device::GetBackBuffer(D3D_U32 index, D3DBACKBUFFERTYPE type, IDirect3DSurface8** result)
{
    return swap_chain->GetBackBuffer(index, type, result);
}

D3D_RESULT Device::GetDepthStencilSurface(IDirect3DSurface8**)
{
    return D3D_OK;
}

D3D_RESULT Device::BeginScene()
{
    return D3D_OK;
}

D3D_RESULT Device::EndScene()
{
    return D3D_OK;
}

D3D_RESULT Device::GetRenderTarget(IDirect3DSurface8** result)
{
    *result = nullptr;
    return D3D_OK;
}

D3D_RESULT Device::SetRenderTarget(IDirect3DSurface8*, IDirect3DSurface8*)
{
    return D3D_OK;
}

D3D_RESULT Device::SetGammaRamp(D3D_U32, const D3DGAMMARAMP*)
{
    return D3D_OK;
}

D3D_RESULT Device::Present(void*, void*, void*, void*)
{
    return D3D_OK;
}

D3D_RESULT Device::SetViewport(const D3DVIEWPORT8*)
{
    return D3D_OK;
}

D3D_RESULT Device::SetLight(D3D_U32 index, const D3DLIGHT8*)
{
    return D3D_OK;
}

D3D_RESULT Device::LightEnable(D3D_U32 index, D3D_BOOL)
{
    return D3D_OK;
}

D3D_RESULT Device::SetTexture(D3D_U32 stage, IDirect3DBaseTexture8*)
{
    return D3D_OK;
}

D3D_RESULT Device::SetTextureStageState(D3D_U32 stage, D3DTEXTURESTAGESTATETYPE state, D3D_U32 value)
{
    return D3D_OK;
}

D3D_RESULT Device::SetMaterial(const D3DMATERIAL8*)
{
    return D3D_OK;
}

D3D_RESULT Device::GetTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX*)
{
    return D3D_OK;
}

D3D_RESULT Device::SetTransform(D3DTRANSFORMSTATETYPE, const D3DMATRIX*)
{
    return D3D_OK;
}

D3D_RESULT Device::SetRenderState(D3DRENDERSTATETYPE, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT Device::SetStreamSource(D3D_U32, IDirect3DVertexBuffer8*, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT Device::SetIndices(IDirect3DIndexBuffer8*, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT Device::SetVertexShader(D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT Device::Clear(D3D_U32, const D3DRECT*, D3D_U32, D3DCOLOR, D3D_F32, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT Device::CopyRects(IDirect3DSurface8*, const RECT*, D3D_U32, IDirect3DSurface8*, const POINT*)
{
    return D3D_OK;
}

D3D_RESULT Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE, D3D_U32, D3D_U32, D3D_U32, D3D_U32)
{
    return D3D_OK;
}

Device::Device()
    : swap_chain(new SwapChain())
{
}

D3D_RESULT SwapChain::GetBackBuffer(D3D_U32, D3DBACKBUFFERTYPE, IDirect3DSurface8** result)
{
    *result = back_buffer.add_ref();
    return D3D_OK;
}

Ptr<Surface> Surface::Create(D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp)
{
    return Ptr(new Surface(width, height, format, bpp));
}

D3D_RESULT Surface::GetDesc(D3DSURFACE_DESC* result)
{
    *result = desc;
    return D3D_OK;
}

Surface::Surface(D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp)
    : data(width * height * bpp / 8),
      bpp(bpp),
      desc({
          .Size = data.size(),
          .Width = width,
          .Height = height,
          .Format = format,
      }),
      pitch(width * bpp / 8)
{
}

D3D_RESULT Surface::LockRect(D3DLOCKED_RECT* lock, RECT* rect, D3D_U32 flags)
{
    // I assume DXTn formats don't support offsets?
    auto bits = data.data();
    if (rect) bits += rect->left * bpp / 8 + rect->top * pitch;
    lock->pBits = bits;
    lock->Pitch = pitch;
    return D3D_OK;
}

D3D_RESULT Surface::UnlockRect()
{
    return D3D_OK;
}

Ptr<Texture> Texture::Create(D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp, D3D_U32 levels)
{
    return Ptr(new Texture(width, height, format, bpp, levels));
}

D3D_RESULT Texture::LockRect(
    D3D_U32 level,
    D3DLOCKED_RECT* lock,
    RECT* rect,
    D3D_U32 flags)
{
    if (level >= levels.size())
        return D3DERR_INVALIDCALL;
    return levels[level]->LockRect(lock, rect, flags);
}

D3D_RESULT Texture::UnlockRect(D3D_U32 level)
{
    if (level >= levels.size())
        return D3DERR_INVALIDCALL;
    return levels[level]->UnlockRect();
}

D3D_U32 Texture::GetLevelCount()
{
    return levels.size();
}

D3D_RESULT Texture::GetLevelDesc(D3D_U32 level, D3DSURFACE_DESC*)
{
    if (level >= levels.size())
        return D3DERR_INVALIDCALL;
    return D3D_OK;
}

D3D_RESULT Texture::GetSurfaceLevel(D3D_U32 level, IDirect3DSurface8** surface)
{
    if (level >= levels.size())
        return D3DERR_INVALIDCALL;
    *surface = levels[level].add_ref();
    return D3D_OK;
}

D3D_U32 Texture::GetPriority()
{
    return D3D_OK;
}

D3D_U32 Texture::SetPriority(D3D_U32)
{
    return D3D_OK;
}

Texture::Texture(D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp, D3D_U32 levels)
{
    for (D3D_U32 i = 0; i < levels; ++i)
    {
        if (!width) width = 1;
        if (!height) height = 1;
        this->levels.push_back(Surface::Create(width, height, format, bpp));
        width >>= 1;
        height >>= 1;
    }
}

D3D_RESULT VertexBuffer::Lock(D3D_U32 offset, D3D_U32 size, D3D_U8** result, D3D_U32 flags)
{
    *result = data.data() + offset;
    return D3D_OK;
}

D3D_RESULT VertexBuffer::Unlock()
{
    return D3D_OK;
}

Ptr<IndexBuffer> IndexBuffer::Create(D3D_U32 length)
{
    return Ptr(new IndexBuffer(length));
}

D3D_RESULT IndexBuffer::Lock(D3D_U32 offset, D3D_U32 size, D3D_U8** result, D3D_U32 flags)
{
    *result = data.data() + offset;
    return D3D_OK;
}

D3D_RESULT IndexBuffer::Unlock()
{
    return D3D_OK;
}

IndexBuffer::IndexBuffer(D3D_U32 length): data(length)
{
}
