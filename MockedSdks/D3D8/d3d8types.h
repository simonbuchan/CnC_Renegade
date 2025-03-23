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

struct alignas(16) D3DVECTOR
{
    D3D_F32 x, y, z;
};

struct alignas(16) D3DVECTOR4
{
    D3D_F32 x, y, z, w;
};

enum
{
    D3D_OK = 0,
    // errors must have high bits set for SUCCEEDED() / FAILED() to work.
    D3DERR_INVALIDCALL = 0x80000000,
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
    static D3DMATRIX IDENTITY;

    D3DVECTOR4 x;
    D3DVECTOR4 y;
    D3DVECTOR4 z;
    D3DVECTOR4 w;

    D3DMATRIX() { *this = IDENTITY; }

    D3DMATRIX(
        D3DVECTOR4 x,
        D3DVECTOR4 y,
        D3DVECTOR4 z,
        D3DVECTOR4 w
    ) : x(x), y(y), z(z), w(w)
    {
    }
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
    union
    {
        D3D_U32 value;
        struct { D3D_U8 b, g, r, a; };
    };

    D3DCOLOR(D3D_U32 value = 0)
        : value(value)
        // : r(value >> 16), g(value >> 8), b(value), a(value >> 24)
    {
    }

    // D3DCOLOR(D3D_U8 r, D3D_U8 g, D3D_U8 b, D3D_U8 a = 0xFF)
    //     : r(r), g(g), b(b), a(a)
    // {
    // }
    //
    D3DCOLOR(D3D_F32 r, D3D_F32 g, D3D_F32 b, D3D_F32 a = 1.0)
        : r(D3D_U8(r * 255.0)),
          g(D3D_U8(g * 255.0)),
          b(D3D_U8(b * 255.0)),
          a(D3D_U8(a * 255.0))
    {
    }

    operator D3D_U32() const
    {
        return value;
        // return (a << 24) | (r << 16) | (g << 8) | b;
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

    D3DRS_COUNT,
};

enum D3DCMPFUNC
{
    D3DCMP_LESSEQUAL,
    D3DCMP_GREATEREQUAL,
};

enum D3DMATERIALSOURCE
{
    D3DMCS_MATERIAL = 0,
    D3DMCS_COLOR1 = 1, // diffuse vertex color
    D3DMCS_COLOR2 = 2, // specular vertex color
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
    D3DTS_TEXTURE1,

    D3DTS_COUNT,
};

enum D3DTEXTURESTAGESTATETYPE
{
    D3DTSS_COLOROP = 1, // D3DTEXTUREOP
    D3DTSS_COLORARG1 = 2, // D3DTA_*
    D3DTSS_COLORARG2 = 3, // D3DTA_*
    D3DTSS_ALPHAOP = 4,
    D3DTSS_ALPHAARG1 = 5,
    D3DTSS_ALPHAARG2 = 6,
    D3DTSS_BUMPENVMAT00 = 7,
    D3DTSS_BUMPENVMAT01 = 8,
    D3DTSS_BUMPENVMAT10 = 9,
    D3DTSS_BUMPENVMAT11 = 10,
    D3DTSS_TEXCOORDINDEX = 11, // D3DTSS_TCI_*
    D3DTSS_ADDRESSU = 13,
    D3DTSS_ADDRESSV = 14,
    D3DTSS_MAGFILTER = 16,
    D3DTSS_MINFILTER = 17,
    D3DTSS_MIPFILTER = 18,
    D3DTSS_MAXANISOTROPY = 21,
    D3DTSS_BUMPENVLSCALE = 22,
    D3DTSS_BUMPENVLOFFSET = 23,
    D3DTSS_TEXTURETRANSFORMFLAGS = 24, // D3DTTFF__
};

enum D3DTEXCOORDINDEX
{
    // uv = in.uv
    D3DTSS_TCI_PASSTHRU = 0,
    // uv = in.normal * camera
    D3DTSS_TCI_CAMERASPACENORMAL = 0x0001'0000,
    // uv = in.position * camera
    D3DTSS_TCI_CAMERASPACEPOSITION = 0x0002'0000,
    // uv = reflect(-in.position * camera, in.normal * camera)
    D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR = 0x0003'0000,
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
    D3DTTFF_DISABLE = 0,
    D3DTTFF_COUNT2 = 2,
    D3DTTFF_COUNT3 = 3,
    D3DTTFF_PROJECTED = 256,
};

enum D3DTADRESS
{
    D3DTADDRESS_WRAP,
    D3DTADDRESS_CLAMP,
};

// matched in case it's read from a file like D3DFMT from .dds files
enum D3DFVF
{
    D3DFVF_XYZ = 0x002, // location: vec2f
    D3DFVF_B4 = 0x004, // blend: vec4f / vec3f + u32
    D3DFVF_XYZB4 = D3DFVF_XYZ | D3DFVF_B4,

    D3DFVF_NORMAL = 0x010, // normal: vec3f
    D3DFVF_DIFFUSE = 0x040, // color: u32
    D3DFVF_SPECULAR = 0x080, // color: u32

    D3DFVF_TEXCOUNT_MASK = 0xF00,
    D3DFVF_TEXCOUNT_SHIFT = 8,
    // number of texture coordinates
    D3DFVF_TEX0 = 0x000,
    D3DFVF_TEX1 = 0x100,
    D3DFVF_TEX2 = 0x200,
    // only 2 actually used
    D3DFVF_TEX3 = 0x300,
    D3DFVF_TEX4 = 0x400,
    D3DFVF_TEX5 = 0x500,
    D3DFVF_TEX6 = 0x600,
    D3DFVF_TEX7 = 0x700,
    D3DFVF_TEX8 = 0x800,

    D3DFVF_LASTBETA_UBYTE4 = 0x1000,
};

#define D3DFVF_TEXCOORDSIZE1(CoordIndex) (3 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE2(CoordIndex) (0)
#define D3DFVF_TEXCOORDSIZE3(CoordIndex) (1 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE4(CoordIndex) (2 << (CoordIndex*2 + 16))

enum D3DPRIMITIVETYPE
{
    D3DPT_TRIANGLELIST,
    D3DPT_TRIANGLESTRIP,
    D3DPT_TRIANGLEFAN,
};

enum D3DTEXTUREOP
{
    // Control
    D3DTOP_DISABLE = 1,
    D3DTOP_SELECTARG1 = 2,
    D3DTOP_SELECTARG2 = 3,

    // Modulate
    D3DTOP_MODULATE = 4, // arg1 * arg2

    // Add
    D3DTOP_ADD = 7, // arg1 + arg2
    D3DTOP_SUBTRACT = 10, // arg1 - arg2
    D3DTOP_ADDSMOOTH = 11, // arg1 + (1 - arg1) * arg2

    // Linear alpha blend: mix(arg1, arg2, alpha)
    D3DTOP_BLENDTEXTUREALPHA = 12,
    // Linear alpha blend with pre-multiplied arg1:
    // arg1 + arg2 * (1 - arg1.a)
    D3DTOP_BLENDCURRENTALPHA = 16,

    // bump
    D3DTOP_BUMPENVMAP = 22,
    D3DTOP_BUMPENVMAPLUMINANCE = 23,

    // vec4(dot(arg1, arg2))
    D3DTOP_DOTPRODUCT3 = 24,
};

enum D3DTEXTUREARG
{
    D3DTA_DIFFUSE = 0,
    D3DTA_CURRENT = 1,
    D3DTA_TEXTURE = 2,
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
    D3DLIGHT_POINT  = 1,
    D3DLIGHT_SPOT = 2,
    D3DLIGHT_DIRECTIONAL = 3,
};

struct D3DLIGHT8
{
    D3DLIGHTTYPE Type;
    D3DCOLOR Diffuse;
    D3DCOLOR Specular;
    D3DCOLOR Ambient;
    D3DVECTOR Position;
    D3DVECTOR Direction;
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
    D3DCOLOR Diffuse;
    D3DCOLOR Ambient;
    D3DCOLOR Specular;
    D3DCOLOR Emissive;
    D3D_F32 Power;
};

struct D3DLOCKED_RECT
{
    void* pBits;
    D3D_U32 Pitch;
};
