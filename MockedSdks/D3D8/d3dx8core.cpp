#include "d3dx8core.h"

D3D_RESULT D3DXGetErrorStringA(D3D_RESULT, char*, UINT)
{
    return D3D_OK;
}

D3D_RESULT D3DXCreateTexture(IDirect3DDevice8*, D3D_U32, D3D_U32, D3D_U32, D3D_U32, D3DFORMAT, D3DPOOL,
    IDirect3DTexture8**)
{
    return D3D_OK;
}

D3D_RESULT D3DXCreateTextureFromFileExA(IDirect3DDevice8*, const char*, D3D_U32, D3D_U32, D3D_U32, D3D_U32, D3DFORMAT,
    D3DPOOL, D3D_U32, D3D_U32, D3D_U32, void*, void*, IDirect3DTexture8**)
{
    return D3D_OK;
}

D3D_RESULT D3DXLoadSurfaceFromSurface(IDirect3DSurface8*, const PALETTEENTRY*, const RECT*, IDirect3DSurface8*,
    const PALETTEENTRY*, const RECT*, D3D_U32, D3D_U32)
{
    return D3D_OK;
}

D3D_RESULT D3DXFilterTexture(IDirect3DTexture8*, const PALETTEENTRY*, D3D_U32, D3D_U32)
{
    return D3D_OK;
}

D3D_U32 D3DXGetFVFVertexSize(D3D_U32)
{
    return 0;
}
