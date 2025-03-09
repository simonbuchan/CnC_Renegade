#include "d3d8.h"

D3D_U32 IDirect3DUnknown8::AddRef()
{
    return 1;
}

D3D_U32 IDirect3DUnknown8::Release()
{
    return 1;
}

D3D_U32 IDirect3D8::GetAdapterCount()
{
    return 0;
}

D3D_U32 IDirect3D8::GetAdapterModeCount(D3D_U32)
{
    return 0;
}

D3D_RESULT IDirect3D8::EnumAdapterModes(D3D_U32, D3D_U32, D3DDISPLAYMODE*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3D8::GetAdapterDisplayMode(D3D_U32, D3DDISPLAYMODE*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3D8::GetAdapterIdentifier(D3D_U32, D3DENUM, D3DADAPTER_IDENTIFIER8*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3D8::GetDeviceCaps(D3D_U32, D3DDEVTYPE, D3DCAPS8*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3D8::CheckDeviceFormat(D3D_U32, D3DDEVTYPE, D3DFORMAT, D3D_U32, D3DRESOURCETYPE, D3DFORMAT)
{
    return D3D_OK;
}

D3D_RESULT IDirect3D8::CheckDepthStencilMatch(D3D_U32, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, D3DFORMAT)
{
    return D3D_OK;
}

D3D_RESULT IDirect3D8::CreateDevice(int, D3DDEVTYPE, HWND, D3D_U32, D3DPRESENT_PARAMETERS*, IDirect3DDevice8**)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::GetDeviceCaps(D3DCAPS8*)
{
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

D3D_RESULT IDirect3DDevice8::CreateVertexBuffer(D3D_U32, D3D_U32, D3D_U32, D3D_U32, IDirect3DVertexBuffer8**)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::CreateIndexBuffer(D3D_U32, D3D_U32, D3D_U32, D3D_U32, IDirect3DIndexBuffer8**)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::CreateImageSurface(D3D_U32, D3D_U32, D3DFORMAT, IDirect3DSurface8**)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::UpdateTexture(IDirect3DBaseTexture8*, IDirect3DBaseTexture8*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS*, IDirect3DSwapChain8**)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::GetFrontBuffer(IDirect3DSurface8*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::GetBackBuffer(D3D_U32, D3DBACKBUFFERTYPE, IDirect3DSurface8**)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::GetDepthStencilSurface(IDirect3DSurface8**)
{
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

D3D_RESULT IDirect3DDevice8::GetRenderTarget(IDirect3DSurface8**)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetRenderTarget(IDirect3DSurface8*, IDirect3DSurface8*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetGammaRamp(D3D_U32, const D3DGAMMARAMP*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::Present(void*, void*, void*, void*)
{
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

D3D_RESULT IDirect3DDevice8::SetTexture(D3D_U32 stage, IDirect3DBaseTexture8*)
{
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

D3D_RESULT IDirect3DDevice8::GetTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetTransform(D3DTRANSFORMSTATETYPE, const D3DMATRIX*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetRenderState(D3DRENDERSTATETYPE, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetStreamSource(D3D_U32, IDirect3DVertexBuffer8*, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetIndices(IDirect3DIndexBuffer8*, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::SetVertexShader(D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::Clear(D3D_U32, const D3DRECT*, D3D_U32, D3DCOLOR, D3D_F32, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::CopyRects(IDirect3DSurface8*, const RECT*, D3D_U32, IDirect3DSurface8*, const POINT*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DDevice8::DrawIndexedPrimitive(D3DPRIMITIVETYPE, D3D_U32, D3D_U32, D3D_U32, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DSwapChain8::GetBackBuffer(D3D_U32, D3DBACKBUFFERTYPE, IDirect3DSurface8**)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DSurface8::GetDesc(D3DSURFACE_DESC*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DSurface8::LockRect(D3DLOCKED_RECT*, RECT*, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DSurface8::UnlockRect()
{
    return D3D_OK;
}

D3D_RESULT IDirect3DBaseTexture8::LockRect(D3D_U32, D3DLOCKED_RECT*, RECT*, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DBaseTexture8::UnlockRect(D3D_U32)
{
    return D3D_OK;
}

D3D_U32 IDirect3DBaseTexture8::GetLevelCount()
{
    return D3D_OK;
}

D3D_RESULT IDirect3DBaseTexture8::GetLevelDesc(D3D_U32, D3DSURFACE_DESC*)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DBaseTexture8::GetSurfaceLevel(D3D_U32, IDirect3DSurface8**)
{
    return D3D_OK;
}

D3D_U32 IDirect3DBaseTexture8::GetPriority()
{
    return D3D_OK;
}

D3D_U32 IDirect3DBaseTexture8::SetPriority(D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DVertexBuffer8::Lock(D3D_U32, D3D_U32, D3D_U8**, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DVertexBuffer8::Unlock()
{
    return D3D_OK;
}

D3D_RESULT IDirect3DIndexBuffer8::Lock(D3D_U32, D3D_U32, D3D_U8**, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT IDirect3DIndexBuffer8::Unlock()
{
    return D3D_OK;
}
