#pragma once

#include <ddraw.h>
#include <wtypes.h>

// Redefine used types so we know what types are used too.
// Eventually will need to replace uses of the Windows.h in app code.
typedef BOOL D3D_BOOL;

typedef BYTE D3D_U8;
typedef WORD D3D_U16;
typedef DWORD D3D_U32;
typedef FLOAT D3D_F32;

typedef HRESULT D3D_RESULT;

typedef RECT D3DRECT;

struct D3DVECTOR
{
    D3D_F32 x, y, z;
};

enum
{
    D3D_OK = 0,
    D3DERR_INVALIDCALL,
    D3DERR_NOTAVAILABLE,
    D3DERR_OUTOFVIDEOMEMORY,
    D3DERR_DEVICELOST,
    D3DERR_DEVICENOTRESET,
    D3DERR_CONFLICTINGTEXTUREFILTER,
    D3DERR_CONFLICTINGTEXTUREPALETTE,
    D3DERR_TOOMANYOPERATIONS,
    D3DERR_UNSUPPORTEDALPHAARG,
    D3DERR_UNSUPPORTEDALPHAOPERATION,
    D3DERR_UNSUPPORTEDCOLORARG,
    D3DERR_UNSUPPORTEDCOLOROPERATION,
    D3DERR_UNSUPPORTEDFACTORVALUE,
    D3DERR_UNSUPPORTEDTEXTUREFILTER,
    D3DERR_WRONGTEXTUREFORMAT,
};

enum
{
    D3DDP_MAXTEXCOORD = 8
};

enum D3DDEVTYPE
{
    D3DDEVTYPE_HAL,
};

// LARGE_INTEGER in Windows.h
struct D3DVERSION
{
    D3D_U32 LowPart;;
    D3D_U32 HighPart;
};

struct D3DMATRIX
{
    D3D_F32 m[4][4];
};

enum D3DPOOL
{
    D3DPOOL_DEFAULT,
    D3DPOOL_MANAGED,
    D3DPOOL_SYSTEMMEM,
};

#define D3DCOLOR_DEFINED
struct D3DCOLOR
{
    D3D_U8 r, g, b, a;

    D3DCOLOR(D3D_U32 value = 0)
        : r(value >> 16), g(value >> 8), b(value), a(value >> 24)
    {
    }

    D3DCOLOR(D3D_U8 r, D3D_U8 g, D3D_U8 b, D3D_U8 a = 0xFF)
        : r(r), g(g), b(b), a(a)
    {
    }

    D3DCOLOR(D3D_F32 r, D3D_F32 g, D3D_F32 b, D3D_F32 a = 1.0)
        : r(D3D_U8(r * 255.0)),
          g(D3D_U8(g * 255.0)),
          b(D3D_U8(b * 255.0)),
          a(D3D_U8(a * 255.0))
    {
    }

    operator D3D_U32() const
    {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }
};

enum D3DFORMAT
{
    D3DFMT_UNKNOWN,

    // 8-bit
    D3DFMT_L8,
    D3DFMT_A8,
    D3DFMT_P8,

    // 16-bit
    D3DFMT_R5G6B5,
    D3DFMT_X1R5G5B5,
    D3DFMT_A1R5G5B5,
    D3DFMT_A4R4G4B4,

    // 24-bit? (it calls it 32-bit in app code)
    D3DFMT_R8G8B8,

    // 32-bit
    D3DFMT_X8R8G8B8,
    D3DFMT_A8R8G8B8,

    // index
    D3DFMT_INDEX16,

    // depth-stencil
    D3DFMT_D16,
    D3DFMT_D15S1,
    D3DFMT_D32,
    D3DFMT_D24X8,
    D3DFMT_D24S8,
    D3DFMT_D24X4S4,

    D3DFMT_R3G3B2,
    D3DFMT_A8R3G3B2,
    D3DFMT_X4R4G4B4,
    D3DFMT_A8P8,
    D3DFMT_A8L8,
    D3DFMT_A4L4,
    D3DFMT_V8U8, // Bumpmap
    D3DFMT_L6V5U5, // Bumpmap
    D3DFMT_X8L8V8U8, // Bumpmap

    D3DFMT_DXT1 = FOURCC_DXT1,
    D3DFMT_DXT2 = FOURCC_DXT2,
    D3DFMT_DXT3 = FOURCC_DXT3,
    D3DFMT_DXT4 = FOURCC_DXT4,
    D3DFMT_DXT5 = FOURCC_DXT5,
};

enum D3DCULL
{
    D3DCULL_NONE,
    D3DCULL_CW,
    D3DCULL_CCW,
};

enum D3DBLEND
{
    D3DBLEND_ZERO,
    D3DBLEND_ONE,
    D3DBLEND_SRCCOLOR,
    D3DBLEND_INVSRCCOLOR,
    D3DBLEND_SRCALPHA,
    D3DBLEND_INVSRCALPHA,
};

enum D3DLOCK
{
    D3DLOCK_READONLY = 1 << 0,
    D3DLOCK_DISCARD = 1 << 1,
    D3DLOCK_NOOVERWRITE = 1 << 2,
    D3DLOCK_NOSYSLOCK = 1 << 3,
};

enum D3DUSAGE
{
    D3DUSAGE_SOFTWAREPROCESSING,
    D3DUSAGE_DYNAMIC,
    D3DUSAGE_WRITEONLY,
    D3DUSAGE_RENDERTARGET,
    D3DUSAGE_DEPTHSTENCIL,
    D3DUSAGE_NPATCHES,
};

enum D3DRESOURCETYPE
{
    D3DRTYPE_SURFACE,
    D3DRTYPE_TEXTURE,
};

enum D3DBACKBUFFERTYPE { D3DBACKBUFFER_TYPE_MONO };

struct D3DDISPLAYMODE
{
    D3D_U32 Width;
    D3D_U32 Height;
    D3DFORMAT Format;
};

// states
enum D3DRENDERSTATETYPE
{
    // input processing
    D3DRS_FILLMODE,
    D3DRS_CULLMODE,
    D3DRS_SOFTWAREVERTEXPROCESSING,
    D3DRS_PATCHSEGMENTS,

    // material
    D3DRS_COLORVERTEX,
    D3DRS_SPECULARENABLE,
    D3DRS_AMBIENTMATERIALSOURCE,
    D3DRS_DIFFUSEMATERIALSOURCE,
    D3DRS_SPECULARMATERIALSOURCE,
    D3DRS_EMISSIVEMATERIALSOURCE,

    // blending
    D3DRS_ALPHATESTENABLE,
    D3DRS_ALPHAREF,
    D3DRS_ALPHAFUNC,
    D3DRS_ALPHABLENDENABLE,
    D3DRS_SRCBLEND,
    D3DRS_DESTBLEND,

    // z-buffer
    D3DRS_ZWRITEENABLE,
    D3DRS_ZBIAS,
    D3DRS_ZFUNC,

    // lighting
    D3DRS_LIGHTING,
    D3DRS_AMBIENT,

    // fog
    D3DRS_FOGENABLE,
    D3DRS_FOGCOLOR,
    D3DRS_FOGSTART,
    D3DRS_FOGEND,
    D3DRS_RANGEFOGENABLE,
    D3DRS_FOGTABLEMODE,
    D3DRS_FOGVERTEXMODE,
};

enum D3DCMPFUNC
{
    D3DCMP_LESSEQUAL,
    D3DCMP_GREATEREQUAL,
};

enum D3DMATERIALSOURCE
{
    D3DMCS_MATERIAL,
    D3DMCS_COLOR1,
    D3DMCS_COLOR2,
};

enum D3DFILLMODE
{
    D3DFILL_POINT,
    D3DFILL_WIREFRAME,
    D3DFILL_SOLID,
};

enum D3DFOGMODE
{
    D3DFOG_NONE,
    D3DFOG_LINEAR,
};

enum D3DTRANSFORMSTATETYPE
{
    D3DTS_WORLD,
    D3DTS_VIEW,
    D3DTS_PROJECTION,
    D3DTS_TEXTURE0,
};

enum D3DTEXTURESTAGESTATETYPE
{
    D3DTSS_MINFILTER,
    D3DTSS_MAGFILTER,
    D3DTSS_MIPFILTER,
    D3DTSS_ADDRESSU,
    D3DTSS_ADDRESSV,
    D3DTSS_MAXANISOTROPY,
    D3DTSS_COLOROP,
    D3DTSS_COLORARG1,
    D3DTSS_COLORARG2,
    D3DTSS_ALPHAOP,
    D3DTSS_ALPHAARG1,
    D3DTSS_ALPHAARG2,
    D3DTSS_TEXCOORDINDEX,
    D3DTSS_TEXTURETRANSFORMFLAGS,
    D3DTSS_BUMPENVLSCALE,
    D3DTSS_BUMPENVLOFFSET,
    D3DTSS_BUMPENVMAT00,
    D3DTSS_BUMPENVMAT01,
    D3DTSS_BUMPENVMAT10,
    D3DTSS_BUMPENVMAT11,
};

enum D3DTEXCOORDINDEX
{
    D3DTSS_TCI_CAMERASPACEPOSITION,
    D3DTSS_TCI_CAMERASPACENORMAL,
    D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR,
    D3DTSS_TCI_PASSTHRU,
};

enum D3DTEXTUREFILTERTYPE
{
    D3DTEXF_NONE,
    D3DTEXF_POINT,
    D3DTEXF_LINEAR,
    D3DTEXF_ANISOTROPIC,
};

enum D3DTEXTURETRANSFORMFLAGS
{
    D3DTTFF_DISABLE = 1 << 0,
    D3DTTFF_PROJECTED = 1 << 1,
    D3DTTFF_COUNT2 = 1 << 2,
    D3DTTFF_COUNT3 = 1 << 3,
};

enum D3DTADRESS
{
    D3DTADDRESS_WRAP,
    D3DTADDRESS_CLAMP,
};

enum D3DFVF
{
    D3DFVF_XYZ = 1 << 0,
    D3DFVF_XYZB4 = 1 << 1,
    D3DFVF_DIFFUSE = 1 << 2,
    D3DFVF_SPECULAR = 1 << 3,
    D3DFVF_NORMAL = 1 << 4,

    D3DFVF_TEX1 = 1 << 16,
    D3DFVF_TEX2 = 1 << 17,
    // only 2 actually used
    D3DFVF_TEX3 = 1 << 18,
    D3DFVF_TEX4 = 1 << 19,
    D3DFVF_TEX5 = 1 << 20,
    D3DFVF_TEX6 = 1 << 21,
    D3DFVF_TEX7 = 1 << 22,
    D3DFVF_TEX8 = 1 << 23,

    D3DFVF_LASTBETA_UBYTE4 = 1 << 24,

    D3DFVF_TEXTUREFORMAT1 = 1 << 25,
    D3DFVF_TEXTUREFORMAT2 = 1 << 26,
    D3DFVF_TEXTUREFORMAT3 = 1 << 27,
    D3DFVF_TEXTUREFORMAT4 = 1 << 28,
};

#define D3DFVF_TEXCOORDSIZEN(CoordIndex)
#define D3DFVF_TEXCOORDSIZE1(CoordIndex) (D3DFVF_TEXTUREFORMAT1 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE2(CoordIndex) (D3DFVF_TEXTUREFORMAT2)
#define D3DFVF_TEXCOORDSIZE3(CoordIndex) (D3DFVF_TEXTUREFORMAT3 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE4(CoordIndex) (D3DFVF_TEXTUREFORMAT4 << (CoordIndex*2 + 16))

enum D3DPRIMITIVETYPE
{
    D3DPT_TRIANGLELIST,
    D3DPT_TRIANGLESTRIP,
    D3DPT_TRIANGLEFAN,
};

enum D3DTEXTUREOP
{
    // ops
    D3DTOP_DISABLE,
    D3DTOP_ADD,
    D3DTOP_SUBTRACT,
    D3DTOP_MODULATE,
    D3DTOP_DOTPRODUCT3,
    D3DTOP_ADDSMOOTH,

    // input
    D3DTOP_SELECTARG1,
    D3DTOP_SELECTARG2,
    D3DTOP_BUMPENVMAP,
    D3DTOP_BUMPENVMAPLUMINANCE,
    D3DTOP_BLENDTEXTUREALPHA,
    D3DTOP_BLENDCURRENTALPHA,
};

enum D3DTEXTUREARG
{
    D3DTA_TEXTURE,
    D3DTA_DIFFUSE,
    D3DTA_CURRENT,
};

struct D3DVIEWPORT8
{
    D3D_U32 X;
    D3D_U32 Y;
    D3D_U32 Width;
    D3D_U32 Height;
    D3D_F32 MinZ;
    D3D_F32 MaxZ;
};

enum D3DLIGHTTYPE
{
    D3DLIGHT_POINT,
    D3DLIGHT_DIRECTIONAL,
    D3DLIGHT_SPOT,
};

struct D3DLIGHT8
{
    D3DLIGHTTYPE Type;
    D3DVECTOR Position;
    D3DVECTOR Direction;
    D3DCOLOR Diffuse;
    D3DCOLOR Specular;
    D3DCOLOR Ambient;
    D3D_F32 Range;
    D3D_F32 Falloff;
    D3D_F32 Attenuation0;
    D3D_F32 Attenuation1;
    D3D_F32 Attenuation2;
    D3D_F32 Theta;
    D3D_F32 Phi;
};

struct D3DMATERIAL8
{
    D3DCOLOR Ambient;
    D3DCOLOR Diffuse;
    D3DCOLOR Specular;
    D3DCOLOR Emissive;
    D3D_F32 Power;
};

struct D3DLOCKED_RECT
{
    void* pBits;
    D3D_U32 Pitch;
};
