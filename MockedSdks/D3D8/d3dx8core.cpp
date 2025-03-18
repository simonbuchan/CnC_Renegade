#include "d3dx8core.h"

D3D_RESULT D3DXGetErrorStringA(D3D_RESULT, char*, UINT)
{
    return D3D_OK;
}

D3D_RESULT D3DXCreateTexture(
    IDirect3DDevice8* device,
    D3D_U32 width,
    D3D_U32 height,
    D3D_U32 mip_level_count,
    D3D_U32 usage,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DTexture8** texture)
{
    return device->CreateTexture(
        width,
        height,
        mip_level_count,
        usage,
        format,
        pool,
        texture);
}

D3D_RESULT D3DXCreateTextureFromFileExA(
    IDirect3DDevice8* device,
    const char* filename,
    D3D_U32 width,
    D3D_U32 height,
    D3D_U32 mip_level_count,
    D3D_U32 usage,
    D3DFORMAT format,
    D3DPOOL pool,
    D3D_U32 filter_min,
    D3D_U32 filter_max,
    D3D_U32 color_key,
    void* src_info,
    void* palette,
    IDirect3DTexture8** texture)
{
    auto data = image::RgbaData::load(filename);
    auto hr = device->CreateTexture(data->width, data->height, mip_level_count, usage, D3DFMT_A8R8G8B8, pool, texture);
    if (FAILED(hr)) return hr;
    (*texture)->texture.write(0, data->data_ptr, data->data_len);
    return hr;
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

// D3D_U32 D3DXGetFVFVertexSize(D3D_U32 fvf)
// {
//     D3D_U32 result = 0;
//     if (fvf & D3DFVF_XYZ) result += 12; // vec3f
//     if (fvf & D3DFVF_B4) result += 16; // vec3f + u32
//     if (fvf & D3DFVF_NORMAL) result += 12; // vec3f
//     if (fvf & D3DFVF_SPECULAR) result += 4; // u32
//     if (fvf & D3DFVF_DIFFUSE) result += 4; // u32
//
//     if (fvf & D3DFVF_TEX1) result += 8; // vec2f
//     if (fvf & D3DFVF_TEX2) result += 8; // vec2f
//     return result;
// }
