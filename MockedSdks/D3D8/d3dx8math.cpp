#include "d3dx8math.h"

D3DXMATRIX D3DXMATRIX::operator*(const D3DXMATRIX& other) const
{
    return *this;
}

void D3DXMatrixTranspose(D3DXMATRIX*, const D3DXMATRIX*)
{
}

void D3DXVec3Transform(D3DXVECTOR4*, const D3DXVECTOR3*, const D3DXMATRIX*)
{
}
