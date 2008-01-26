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
#include "boneanim.h"
#include "3DLoaderTypes.h"

float Deg2Rad(float deg);
float Rad2Deg(float rad);

VECTOR vectorInvert(const VECTOR& v);
VECTOR vectorNormalize(const float x, const float y, const float z);
inline VECTOR vectorNormalize(const VECTOR& v) {
    return vectorNormalize(v.x, v.y, v.z);
}
inline VECTOR vectorMake(const float x, const float y, const float z) {
    VECTOR v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}
inline VERTEX2 vertex2vertex2(const VERTEX& v, unsigned char bone) {
    VERTEX2 ret;
    ret.position = v.position;
    ret.normal = v.normal;
    *reinterpret_cast<unsigned long*>(&ret.bone) = 0;
    *reinterpret_cast<unsigned long*>(&ret.boneweight) = 0;
    ret.bone[0] = bone;
    ret.boneweight[0] = 0xff;
    ret.color = v.color;
    ret.tu = v.tu;
    ret.tv = v.tv;
    return ret;
}

inline VERTEX2 vertex2castrate(const VERTEX2& v, unsigned char bone) {
    VERTEX2 ret;
    ret.position = v.position;
    ret.normal = v.normal;
    *reinterpret_cast<unsigned long*>(&ret.bone) = 0;
    *reinterpret_cast<unsigned long*>(&ret.boneweight) = 0;
    ret.bone[0] = bone;
    ret.boneweight[0] = 0xff;
    ret.color = v.color;
    ret.tu = v.tu;
    ret.tv = v.tv;
    return ret;
}

inline void vertex2init(VERTEX2& v) {
    *reinterpret_cast<unsigned long*>(&v.bone) = 0;
    *reinterpret_cast<unsigned long*>(&v.boneweight) = 0;
    v.bone[0] = -1;
    v.boneweight[0] = 0xff;
    v.color = 0xffffffff;
}

inline VERTEX vertex22vertex(const VERTEX2& v) {
    VERTEX ret;
    ret.position = v.position;
    ret.normal = v.normal;
    ret.color = v.color;
    ret.tu = v.tu;
    ret.tv = v.tv;
    return ret;
}

MATRIX matrixGetUnity();
MATRIX matrixGetFixOrientation(const c3DLoaderOrientation orient = ORIENTATION_RIGHT_ZUP);
void txyzFixOrientation(txyz& src, const c3DLoaderOrientation& orient);
MATRIX matrixGetTranslation(const float x, const float y, const float z);
MATRIX matrixGetTranslation(const VECTOR& v);
MATRIX matrixGetScale(const float x, const float y, const float z);
MATRIX matrixGetScale(const VECTOR& v);
// Rotations are in a right-handed coordinate system
MATRIX matrixGetRotationX(const float rad);
MATRIX matrixGetRotationY(const float rad);
MATRIX matrixGetRotationZ(const float rad);

// Note: the IP (In Place) functions return the result in the first parameter or the first
//       member of the array. They also return a pointer to this result.
float matrixCalcDeterminant(const MATRIX *m);
MATRIX matrixMultiply(const MATRIX *m1, const MATRIX *m2);
inline MATRIX matrixMultiply(const MATRIX& m1, const MATRIX& m2) {
    return matrixMultiply(&m1, &m2);
}
MATRIX *matrixMultiplyIP(MATRIX *m1, const MATRIX *m2);
// These take an array and multiply it up in order.
MATRIX matrixMultiply(const MATRIX *ms, const unsigned int count);
MATRIX *matrixMultiplyIP(MATRIX *ms, const unsigned int count);
MATRIX matrixMultiply(const std::vector<MATRIX>& ms);
MATRIX matrixMultiply(const MATRIX& m, const float f);

MATRIX matrixTranspose(const MATRIX& m);
MATRIX matrixInverse(const MATRIX& m);
#define matrixNormalTransform(m) matrixTranspose(matrixInverse( m ))

VECTOR matrixApply(const VECTOR& v, const MATRIX& m);
VECTOR *matrixApplyIP(VECTOR *v, const MATRIX& m);
VERTEX matrixApply(const VERTEX& v, const MATRIX& m, const MATRIX& mnormal);
VERTEX *matrixApplyIP(VERTEX *v, const MATRIX& m, const MATRIX& mnormal);
VERTEX2 matrixApply(const VERTEX2& v, const MATRIX& m, const MATRIX& mnormal);
VERTEX2 *matrixApplyIP(VERTEX2 *v, const MATRIX& m, const MATRIX& mnormal);

template <class T>
inline void doFixOrientation(T& v, const c3DLoaderOrientation& orient) {
    matrixApplyIP(&v, matrixGetFixOrientation(orient));
}
template <>
inline void doFixOrientation(txyz& v, const c3DLoaderOrientation& orient) {
    txyzFixOrientation(v, orient);
}
/*
template <class T>
inline T max(const T& a, const T& b) {
    return (a>b)?a:b;
}
*/

bool matrixIsEqual(const MATRIX& m1, const MATRIX& m2);

MATRIX matrixThereAndBackAgain(const std::vector<MATRIX>& stack, const MATRIX& transform, const MATRIX& undodamage);

void boundsInit(VECTOR *bbox_min, VECTOR *bbox_max);
void boundsContain(const VECTOR *v, VECTOR *bbox_min, VECTOR *bbox_max);
void boundsContain(const VECTOR *inner_min, const VECTOR *inner_max, VECTOR *bbox_min, VECTOR *bbox_max);

#endif // OVLMATRIX_H_INCLUDED
