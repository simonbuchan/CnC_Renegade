/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dx8fvf.h"
#include "wwstring.h"
#include <D3dx8core.h>

FVFInfoClass::FVFInfoClass(unsigned FVF_) 
	:
	FVF(FVF_),
	fvf_size(0)
{
	location_offset = fvf_size;
    if (FVF & D3DFVF_XYZ) fvf_size += 12; // vec3f
	blend_offset = fvf_size;
    if (FVF & D3DFVF_B4) fvf_size += 16; // vec3f + u32
	normal_offset = fvf_size;
    if (FVF & D3DFVF_NORMAL) fvf_size += 12; // vec3f
	specular_offset = fvf_size;
    if (FVF & D3DFVF_SPECULAR) fvf_size += 4; // u32
	diffuse_offset = fvf_size;
    if (FVF & D3DFVF_DIFFUSE) fvf_size += 4; // u32
	auto texcoord_count = (FVF&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT;
	for (auto i = 0; i < texcoord_count; i++)
	{
		texcoord_offset[i] = fvf_size;

		auto shift = i * 2 + 16;
		auto n = (FVF >> shift) & 3;
		// n != coord count:
		// #define D3DFVF_TEXCOORDSIZE1(CoordIndex) (3 << (CoordIndex*2 + 16))
		// #define D3DFVF_TEXCOORDSIZE2(CoordIndex) (0)
		// #define D3DFVF_TEXCOORDSIZE3(CoordIndex) (1 << (CoordIndex*2 + 16))
		// #define D3DFVF_TEXCOORDSIZE4(CoordIndex) (2 << (CoordIndex*2 + 16))
		switch (n)
		{
			case 0: fvf_size += 8; break; // vec2f
			case 1: fvf_size += 12; break; // vec3f
			case 2: fvf_size += 16; break; // vec4f
			case 3: fvf_size += 4; break; // f32
		}
	}
	for (auto i = texcoord_count; i < D3DDP_MAXTEXCOORD; i++)
	{
		texcoord_offset[i] = fvf_size;
	}
}

void FVFInfoClass::Get_FVF_Name(StringClass& fvfname) const
{
	switch (Get_FVF()) {
	case DX8_FVF_XYZ: fvfname="D3DFVF_XYZ"; break;
	case DX8_FVF_XYZN: fvfname="D3DFVF_XYZ|D3DFVF_NORMAL"; break;
	case DX8_FVF_XYZNUV1: fvfname="D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1"; break;
	case DX8_FVF_XYZNUV2: fvfname="D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2"; break;
	case DX8_FVF_XYZNDUV1: fvfname="D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_DIFFUSE"; break;
#ifdef CLEANBULD_NAMES
	case DX8_FVF_XYZNDUV2: fvfname="D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2|D3DFVF_DIFFUSE"; break;
	case DX8_FVF_XYZDUV1: fvfname="D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE"; break;
	case DX8_FVF_XYZDUV2: fvfname="D3DFVF_XYZ|D3DFVF_TEX2|D3DFVF_DIFFUSE"; break;
	case DX8_FVF_XYZUV1: fvfname="D3DFVF_XYZ|D3DFVF_TEX1"; break;
	case DX8_FVF_XYZUV2: fvfname="D3DFVF_XYZ|D3DFVF_TEX2"; break;
#endif
	default: fvfname="Unknown!";
	}
}
