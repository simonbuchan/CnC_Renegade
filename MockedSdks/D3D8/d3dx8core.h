#pragma once

#include <d3d8.h>

enum { D3DX_DEFAULT = ~0 };

enum D3DX_FILTER
{
    D3DX_FILTER_NONE,
    D3DX_FILTER_TRIANGLE,
    D3DX_FILTER_BOX,
};

D3D_RESULT D3DXGetErrorStringA(D3D_RESULT, char*, UINT);
D3D_RESULT D3DXCreateTexture(
    IDirect3DDevice8*,
    D3D_U32,
    D3D_U32,
    D3D_U32,
    D3D_U32,
    D3DFORMAT,
    D3DPOOL,
    IDirect3DTexture8**);
D3D_RESULT D3DXCreateTextureFromFileExA(
    IDirect3DDevice8*,
    const char*,
    D3D_U32,
    D3D_U32,
    D3D_U32,
    D3D_U32,
    D3DFORMAT,
    D3DPOOL,
    D3D_U32,
    D3D_U32,
    D3D_U32,
    void*,
    void*,
    IDirect3DTexture8**);
D3D_RESULT D3DXLoadSurfaceFromSurface(
    IDirect3DSurface8*,
    const PALETTEENTRY*,
    const RECT*,
    IDirect3DSurface8*,
    const PALETTEENTRY*,
    const RECT*,
    D3D_U32,
    D3D_U32);
D3D_RESULT D3DXFilterTexture(
    IDirect3DTexture8*,
    const PALETTEENTRY*,
    D3D_U32,
    D3D_U32);
// D3D_U32 D3DXGetFVFVertexSize(D3D_U32);
