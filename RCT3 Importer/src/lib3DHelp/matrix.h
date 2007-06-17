///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Matrix Utility functions
// Copyright (C) 2006 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#ifndef OVLMATRIX_H_INCLUDED
#define OVLMATRIX_H_INCLUDED

#include <vector>
#include "vertex.h"
#include "3DLoaderTypes.h"

float Deg2Rad(float deg);

D3DVECTOR vectorInvert(const D3DVECTOR v);
D3DVECTOR vectorNormalize(const float x, const float y, const float z);
inline D3DVECTOR vectorNormalize(const D3DVECTOR v) {
    return vectorNormalize(v.x, v.y, v.z);
}
inline VERTEX2 vertex2vertex2(VERTEX v, unsigned long bone, unsigned long unk) {
    VERTEX2 ret;
    ret.position = v.position;
    ret.normal = v.normal;
    ret.Bone = bone;
    ret.unk = unk;
    ret.color = v.color;
    ret.tu = v.tu;
    ret.tv = v.tv;
    return ret;
}

D3DMATRIX matrixGetUnity();
D3DMATRIX matrixGetFixOrientation(c3DLoaderOrientation orient = ORIENTATION_RIGHT_ZUP);
D3DMATRIX matrixGetTranslation(float x, float y, float z);
D3DMATRIX matrixGetTranslation(D3DVECTOR v);
D3DMATRIX matrixGetScale(float x, float y, float z);
D3DMATRIX matrixGetScale(D3DVECTOR v);
// Rotations are in a right-handed coordinate system
D3DMATRIX matrixGetRotationX(float rad);
D3DMATRIX matrixGetRotationY(float rad);
D3DMATRIX matrixGetRotationZ(float rad);

// Note: the IP (In Place) functions return the result in the first parameter or the first
//       member of the array. They also return a pointer to this result.
float matrixCalcDeterminant(const D3DMATRIX *m);
D3DMATRIX matrixMultiply(const D3DMATRIX *m1, const D3DMATRIX *m2);
D3DMATRIX *matrixMultiplyIP(D3DMATRIX *m1, const D3DMATRIX *m2);
// These take an array and multiply it up in order.
D3DMATRIX matrixMultiply(D3DMATRIX *ms, unsigned int count);
D3DMATRIX *matrixMultiplyIP(D3DMATRIX *ms, unsigned int count);
D3DMATRIX matrixMultiply(const std::vector<D3DMATRIX> ms);
D3DMATRIX matrixMultiply(const D3DMATRIX m, float f);

D3DMATRIX matrixTranspose(const D3DMATRIX m);
D3DMATRIX matrixInverse(const D3DMATRIX m);
#define matrixNormalTransform(m) matrixTranspose(matrixInverse( m ))

VERTEX matrixApply(const VERTEX v, const D3DMATRIX m, const D3DMATRIX mnormal);
VERTEX *matrixApplyIP(VERTEX *v, const D3DMATRIX m, const D3DMATRIX mnormal);
VERTEX2 matrixApply(VERTEX2 v, const D3DMATRIX m, const D3DMATRIX mnormal);
VERTEX2 *matrixApplyIP(VERTEX2 *v, const D3DMATRIX m, const D3DMATRIX mnormal);

bool matrixIsEqual(const D3DMATRIX m1, const D3DMATRIX m2);

void boundsInit(D3DVECTOR *bbox_min, D3DVECTOR *bbox_max);
void boundsContain(const D3DVECTOR *v, D3DVECTOR *bbox_min, D3DVECTOR *bbox_max);
void boundsContain(const D3DVECTOR *inner_min, const D3DVECTOR *inner_max, D3DVECTOR *bbox_min, D3DVECTOR *bbox_max);

#endif // OVLMATRIX_H_INCLUDED
