#pragma once

#include <d3d8types.h>

enum D3DCAPS2
{
    D3DCAPS2_FULLSCREENGAMMA = 1 << 0,
};

enum D3DDEVCAPS
{
    D3DDEVCAPS_HWTRANSFORMANDLIGHT = 1 << 0,
    D3DDEVCAPS_NPATCHES = 1 << 1,
};

enum D3DPRASTERCAPS
{
    D3DPRASTERCAPS_FOGRANGE = 1 << 0,
    D3DPRASTERCAPS_ZBIAS = 1 << 1,
};

enum D3DTEXOPCAPS
{
    D3DTEXOPCAPS_BUMPENVMAP = 1 << 0,
    D3DTEXOPCAPS_BUMPENVMAPLUMINANCE = 1 << 1,
    D3DTEXOPCAPS_ADD = 1 << 2,
    D3DTEXOPCAPS_SUBTRACT = 1 << 3,
    D3DTEXOPCAPS_MODULATE = 1 << 4,
    D3DTEXOPCAPS_DOTPRODUCT3 = 1 << 5,
    D3DTEXOPCAPS_SELECTARG1 = 1 << 6,
    D3DTEXOPCAPS_ADDSMOOTH = 1 << 7,
    D3DTEXOPCAPS_BLENDTEXTUREALPHA = 1 << 8,
    D3DTEXOPCAPS_BLENDCURRENTALPHA = 1 << 9,
};


enum D3DPTFILTERCAPS
{
    D3DPTFILTERCAPS_MAGFLINEAR = 1 << 0,
    D3DPTFILTERCAPS_MINFANISOTROPIC = 1 << 1,
    D3DPTFILTERCAPS_MAGFANISOTROPIC = 1 << 2,
    D3DPTFILTERCAPS_MINFLINEAR = 1 << 3,
    D3DPTFILTERCAPS_MIPFLINEAR = 1 << 4,
};

struct D3DCAPS8
{
    D3D_U32 AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3D_U32 MaxTextureWidth;
    D3D_U32 MaxTextureHeight;
    D3D_U32 MaxSimultaneousTextures;
    D3D_U32 DevCaps;
    D3D_U32 RasterCaps;
    D3D_U32 TextureOpCaps;
    D3D_U32 TextureFilterCaps;
    D3D_U32 Caps2;
    D3D_U32 VertexShaderVersion;
    D3D_U32 PixelShaderVersion;

    bool Is_Valid_Display_Format(D3D_U32, D3D_U32, D3DFORMAT) const
    {
        return true;
    }
};
