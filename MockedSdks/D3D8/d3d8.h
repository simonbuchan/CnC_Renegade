// Mock of the D3D8 API
// Could actually use this, but it's handy to be able to
// see what's actually used.

#pragma once

#include <atomic>
#include <vector>

#include <atlbase.h>
#include <d3d8types.h>
#include <d3d8caps.h>

#include "render_crate.hpp"

enum { D3D_SDK_VERSION };

enum { D3DADAPTER_DEFAULT };

enum D3DENUM
{
    D3DENUM_NO_WHQL_LEVEL,
};

enum D3DSWAPEFFECT
{
    D3DSWAPEFFECT_DISCARD,
    D3DSWAPEFFECT_FLIP,
    D3DSWAPEFFECT_COPY_VSYNC,
};

enum D3DMULTISAMPLE
{
    D3DMULTISAMPLE_NONE,
};

struct D3DADAPTER_IDENTIFIER8
{
    unsigned VendorId;
    unsigned DeviceId;
    unsigned SubSysId;
    unsigned Revision;
    unsigned DriverId;
    char Driver[256];
    D3DVERSION DriverVersion;
    UUID DeviceIdentifier;
    char Description[256];
};

enum D3DCREATE_FLAGS
{
    D3DCREATE_MULTITHREADED,
    D3DCREATE_SOFTWARE_VERTEXPROCESSING,
    D3DCREATE_MIXED_VERTEXPROCESSING,
    D3DCREATE_FPU_PRESERVE,
};

enum D3DPRESENT_FLAGS
{
    D3DPRESENTFLAG_LOCKABLE_BACKBUFFER,
};

enum D3DPRESENT_INTERVAL
{
    D3DPRESENT_INTERVAL_DEFAULT,
    D3DPRESENT_INTERVAL_IMMEDIATE,
    D3DPRESENT_INTERVAL_ONE,
    D3DPRESENT_INTERVAL_TWO,
    D3DPRESENT_INTERVAL_THREE,
};

enum D3DPRESENT_RATE
{
    D3DPRESENT_RATE_DEFAULT,
};

struct D3DPRESENT_PARAMETERS
{
    D3D_U32 Flags;

    D3D_U32 BackBufferWidth;
    D3D_U32 BackBufferHeight;
    D3D_U32 BackBufferCount;
    D3DFORMAT BackBufferFormat;

    D3DMULTISAMPLE MultiSampleType;
    D3DSWAPEFFECT SwapEffect;
    HWND hDeviceWindow;
    D3D_BOOL Windowed;

    D3D_BOOL EnableAutoDepthStencil;
    D3DFORMAT AutoDepthStencilFormat;

    D3DPRESENT_INTERVAL FullScreen_PresentationInterval;
    D3DPRESENT_RATE FullScreen_RefreshRateInHz;
};

struct D3DSURFACE_DESC
{
    D3D_U32 Size;
    D3D_U32 Width;
    D3D_U32 Height;
    D3DFORMAT Format;
};

enum D3DCLEAR_FLAGS
{
    D3DCLEAR_TARGET,
    D3DCLEAR_ZBUFFER,
    D3DCLEAR_STENCIL,
};

enum D3DGAMMARAMP_FLAGS
{
    D3DSGR_NO_CALIBRATION,
    D3DSGR_CALIBRATE,
};

struct D3DGAMMARAMP
{
    D3D_U16 red[256];
    D3D_U16 green[256];
    D3D_U16 blue[256];
};

// interfaces
struct IDirect3D8;
struct IDirect3DDevice8;
struct IDirect3DSwapChain8;
struct IDirect3DSurface8;
struct IDirect3DBaseTexture8;
struct IDirect3DTexture8;
struct IDirect3DVertexBuffer8;
struct IDirect3DIndexBuffer8;

IDirect3D8* Direct3DCreate8(int);

typedef IDirect3DSurface8* LPDIRECT3DSURFACE8;

struct IDirect3DUnknown8
{
    D3D_U32 AddRef();
    D3D_U32 Release();

protected:
    IDirect3DUnknown8() = default;
    virtual ~IDirect3DUnknown8() = default;

    // initialize to 0 because CComPtr AddRefs on construction
    std::atomic_uint32_t count = 0;
};

struct IDirect3D8 : IDirect3DUnknown8
{
    wgpu::Instance wgpu;

    D3D_U32 GetAdapterCount();
    D3D_U32 GetAdapterModeCount(D3D_U32);
    D3D_RESULT EnumAdapterModes(D3D_U32, D3D_U32, D3DDISPLAYMODE*);
    D3D_RESULT GetAdapterDisplayMode(D3D_U32, D3DDISPLAYMODE*);
    D3D_RESULT GetAdapterIdentifier(D3D_U32, D3DENUM, D3DADAPTER_IDENTIFIER8*);
    D3D_RESULT GetDeviceCaps(D3D_U32, D3DDEVTYPE, D3DCAPS8*);
    D3D_RESULT CheckDeviceFormat(D3D_U32, D3DDEVTYPE, D3DFORMAT, D3D_U32, D3DRESOURCETYPE, D3DFORMAT);
    D3D_RESULT CheckDepthStencilMatch(D3D_U32, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, D3DFORMAT);
    D3D_RESULT CreateDevice(
        D3D_U32,
        D3DDEVTYPE,
        HWND,
        D3D_U32,
        D3DPRESENT_PARAMETERS*,
        IDirect3DDevice8**);
};

struct IDirect3DDevice8 : IDirect3DUnknown8
{
    static CComPtr<IDirect3DDevice8> Create(wgpu::Device device, wgpu::Surface surface)
    {
        return new IDirect3DDevice8(std::move(device), std::move(surface));
    }

    wgpu::Device device;
    wgpu::Surface surface; // wgpu Surface ~= IDirect3DSwapChain8
    wgpu::Commands commands;

private:
    IDirect3DDevice8(wgpu::Device device, wgpu::Surface surface);

    // disable rendering when set, they are only used for shadows
    IDirect3DSurface8* set_color_target = nullptr;
    IDirect3DSurface8* set_depth_stencil_target = nullptr;

public:
    // general management
    D3D_RESULT GetDeviceCaps(D3DCAPS8*);
    D3D_RESULT GetDisplayMode(D3DDISPLAYMODE*);
    D3D_RESULT ValidateDevice(D3D_U32*);
    D3D_RESULT Reset(const D3DPRESENT_PARAMETERS*);
    D3D_RESULT TestCooperativeLevel();
    D3D_RESULT ResourceManagerDiscardBytes(D3D_U32);
    D3D_U32 GetAvailableTextureMem();

    // resources
    D3D_RESULT CreateVertexBuffer(D3D_U32, D3D_U32, D3D_U32, D3D_U32, IDirect3DVertexBuffer8**);
    D3D_RESULT CreateIndexBuffer(D3D_U32, D3D_U32, D3D_U32, D3D_U32, IDirect3DIndexBuffer8**);
    D3D_RESULT CreateImageSurface(D3D_U32, D3D_U32, D3DFORMAT, IDirect3DSurface8**);
    D3D_RESULT CreateTexture(D3D_U32, D3D_U32, D3D_U32, D3D_U32, D3DFORMAT, D3DPOOL, IDirect3DTexture8**);
    D3D_RESULT UpdateTexture(IDirect3DBaseTexture8*, IDirect3DBaseTexture8*);
#ifdef D3D_ADDITIONAL_SWAP_CHAIN
    D3D_RESULT CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS*, IDirect3DSwapChain8**);
#endif

    // presenting
#ifdef D3D_BUFFER_ACCESS
    D3D_RESULT GetFrontBuffer(IDirect3DSurface8*);
    D3D_RESULT GetBackBuffer(D3D_U32, D3DBACKBUFFERTYPE, IDirect3DSurface8**);
#endif
    D3D_RESULT GetDepthStencilSurface(IDirect3DSurface8**);
    D3D_RESULT BeginScene();
    D3D_RESULT EndScene();
    D3D_RESULT GetRenderTarget(IDirect3DSurface8**);
    D3D_RESULT SetRenderTarget(IDirect3DSurface8*, IDirect3DSurface8*);
    D3D_RESULT SetGammaRamp(D3D_U32, const D3DGAMMARAMP*);
    // Only used with NULLs
    D3D_RESULT Present(void*, void*, void*, void*);

    // render state
    D3D_RESULT SetViewport(const D3DVIEWPORT8*);
    D3D_RESULT SetLight(D3D_U32 index, const D3DLIGHT8*);
    D3D_RESULT LightEnable(D3D_U32 index, D3D_BOOL);
    D3D_RESULT SetTexture(D3D_U32 stage, IDirect3DBaseTexture8*);
    D3D_RESULT SetTextureStageState(D3D_U32 stage, D3DTEXTURESTAGESTATETYPE state, D3D_U32 value);
    D3D_RESULT SetMaterial(const D3DMATERIAL8*);
    D3D_RESULT GetTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX*);
    D3D_RESULT SetTransform(D3DTRANSFORMSTATETYPE, const D3DMATRIX*);
    D3D_RESULT SetRenderState(D3DRENDERSTATETYPE, D3D_U32);
    D3D_RESULT SetStreamSource(D3D_U32, IDirect3DVertexBuffer8*, D3D_U32);
    D3D_RESULT SetIndices(IDirect3DIndexBuffer8*, D3D_U32);
    D3D_RESULT SetVertexShader(D3D_U32);

    // draw
    D3D_RESULT Clear(D3D_U32, const D3DRECT*, D3D_U32, D3DCOLOR, D3D_F32, D3D_U32);
    D3D_RESULT CopyRects(IDirect3DSurface8*, const RECT*, D3D_U32, IDirect3DSurface8*, const POINT*);
    D3D_RESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE, D3D_U32, D3D_U32, D3D_U32, D3D_U32);
};

#ifdef D3D_ADDITIONAL_SWAP_CHAIN
struct IDirect3DSwapChain8 : IDirect3DUnknown8
{
    D3D_RESULT GetBackBuffer(D3D_U32, D3DBACKBUFFERTYPE, IDirect3DSurface8**);
};
#endif

struct IDirect3DSurface8 : IDirect3DUnknown8
{
    static CComPtr<IDirect3DSurface8> Create(D3D_U32 width, D3D_U32 height, D3DFORMAT format);
    static CComPtr<IDirect3DSurface8> Create(D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp);

    D3D_RESULT GetDesc(D3DSURFACE_DESC*);
    D3D_RESULT LockRect(D3DLOCKED_RECT*, RECT*, D3D_U32);
    D3D_RESULT UnlockRect();

private:
    IDirect3DSurface8(D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp);

    std::vector<D3D_U8> data;
    D3DSURFACE_DESC desc;
    D3D_U32 bpp;
    D3D_U32 pitch;
};

struct IDirect3DBaseTexture8 : IDirect3DUnknown8
{
    D3D_RESULT LockRect(D3D_U32, D3DLOCKED_RECT*, RECT*, D3D_U32);
    D3D_RESULT UnlockRect(D3D_U32);
    D3D_U32 GetLevelCount();
    D3D_RESULT GetLevelDesc(D3D_U32, D3DSURFACE_DESC*);
    D3D_RESULT GetSurfaceLevel(D3D_U32, IDirect3DSurface8**);
    D3D_U32 GetPriority();
    D3D_U32 SetPriority(D3D_U32);

protected:
    std::vector<CComPtr<IDirect3DSurface8>> levels;
};

struct IDirect3DTexture8 : IDirect3DBaseTexture8
{
    static CComPtr<IDirect3DTexture8> Create(
        D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp, D3D_U32 levels
    )
    {
        return new IDirect3DTexture8(width, height, format, bpp, levels);
    }

private:
    explicit IDirect3DTexture8(D3D_U32 width, D3D_U32 height, D3DFORMAT format, D3D_U32 bpp, D3D_U32 levels);
};

struct IDirect3DResource8 : IDirect3DUnknown8
{
    D3D_RESULT Lock(D3D_U32, D3D_U32, D3D_U8**, D3D_U32);
    D3D_RESULT Unlock();

protected:
    explicit IDirect3DResource8(D3D_U32 length) : data(length)
    {
    }

    std::vector<D3D_U8> data;
};

struct IDirect3DVertexBuffer8 : IDirect3DResource8
{
    static CComPtr<IDirect3DVertexBuffer8> Create(D3D_U32 length)
    {
        return new IDirect3DVertexBuffer8(length);
    }

protected:
    explicit IDirect3DVertexBuffer8(D3D_U32 length) : IDirect3DResource8(length)
    {
    }
};

struct IDirect3DIndexBuffer8 : IDirect3DResource8
{
    static CComPtr<IDirect3DIndexBuffer8> Create(D3D_U32 length)
    {
        return new IDirect3DIndexBuffer8(length);
    }

protected:
    explicit IDirect3DIndexBuffer8(D3D_U32 length) : IDirect3DResource8(length)
    {
    }
};
