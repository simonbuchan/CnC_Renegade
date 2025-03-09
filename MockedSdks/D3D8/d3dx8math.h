#pragma once

#include <d3d8types.h>

struct D3DXVECTOR3
{
    float x, y, z;

    float& operator[](int index)
    {
        switch (index)
        {
        default:
        case 0: return x;
        case 1: return y;
        case 2: return z;
        }
    }
};

struct D3DXVECTOR4
{
    float x, y, z, w;

    float& operator[](int index)
    {
        switch (index)
        {
        default:
        case 0: return x;
        case 1: return y;
        case 2: return z;
        case 3: return w;
        }
    }
};

struct D3DXMATRIX
{
    D3DMATRIX value;
    D3DXMATRIX(D3DMATRIX value) : value(value) {}
    D3DXMATRIX operator*(const D3DXMATRIX& other) const;
};

void D3DXMatrixTranspose(
    D3DXMATRIX*,
    const D3DXMATRIX*);

void D3DXVec3Transform(
    D3DXVECTOR4*,
    const D3DXVECTOR3*,
    const D3DXMATRIX*);