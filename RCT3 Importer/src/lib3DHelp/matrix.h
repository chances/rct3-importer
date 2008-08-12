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
#include <math.h>

#include "vertex.h"
#include "boneanim.h"
#include "3DLoaderTypes.h"

float Deg2Rad(float deg);
float Rad2Deg(float rad);

r3::VECTOR vectorInvert(const r3::VECTOR& v);
r3::VECTOR vectorNormalize(const float x, const float y, const float z);
inline r3::VECTOR vectorNormalize(const r3::VECTOR& v) {
    return vectorNormalize(v.x, v.y, v.z);
}
inline void vectorMake(r3::VECTOR& v, const float x, const float y, const float z) {
    v.x = x;
    v.y = y;
    v.z = z;
}
inline r3::VECTOR vectorMake(const float x, const float y, const float z) {
    r3::VECTOR v;
    vectorMake(v, x, y, z);
    return v;
}
inline void txyzMake(r3::txyz& v, const float t, const float x, const float y, const float z) {
    v.Time = t;
    v.x = x;
    v.y = y;
    v.z = z;
}
inline r3::txyz txyzMake(const float t, const float x, const float y, const float z) {
    r3::txyz v;
    txyzMake(v, t, x, y, z);
    return v;
}

r3::txyz txyzEulerToAxis(const r3::txyz& euler);

inline r3::VERTEX2 vertex2vertex2(const r3::VERTEX& v, unsigned char bone) {
    r3::VERTEX2 ret;
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

inline r3::VERTEX2 vertex2castrate(const r3::VERTEX2& v, unsigned char bone) {
    r3::VERTEX2 ret;
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

inline void vertex2init(r3::VERTEX2& v) {
    *reinterpret_cast<unsigned long*>(&v.bone) = 0;
    *reinterpret_cast<unsigned long*>(&v.boneweight) = 0;
    v.bone[0] = -1;
    v.boneweight[0] = 0xff;
    v.color = 0xffffffff;
}

inline r3::VERTEX vertex22vertex(const r3::VERTEX2& v) {
    r3::VERTEX ret;
    ret.position = v.position;
    ret.normal = v.normal;
    ret.color = v.color;
    ret.tu = v.tu;
    ret.tv = v.tv;
    return ret;
}

void matrixSetUnity(r3::MATRIX& m);
inline r3::MATRIX matrixGetUnity() {
    r3::MATRIX m;
    matrixSetUnity(m);
    return m;
}
r3::MATRIX matrixGetFixOrientation(const c3DLoaderOrientation orient = ORIENTATION_RIGHT_ZUP);
void txyzFixOrientation(r3::txyz& src, const c3DLoaderOrientation& orient, bool rotate = false);
r3::MATRIX matrixGetTranslation(const float x, const float y, const float z);
r3::MATRIX matrixGetTranslation(const r3::VECTOR& v);
r3::MATRIX matrixGetScale(const float x, const float y, const float z);
r3::MATRIX matrixGetScale(const r3::VECTOR& v);
// Rotations are in a right-handed coordinate system
r3::MATRIX matrixGetRotationX(const float rad);
r3::MATRIX matrixGetRotationY(const float rad);
r3::MATRIX matrixGetRotationZ(const float rad);

inline void matrixExtractTranslation(const r3::MATRIX& m, r3::VECTOR& t) {
    vectorMake(t, m._41, m._42, m._43);
}
inline r3::VECTOR matrixExtractTranslation(const r3::MATRIX& m) {
    return vectorMake(m._41, m._42, m._43);
}

// Note: the IP (In Place) functions return the result in the first parameter or the first
//       member of the array. They also return a pointer to this result.
float matrixCalcDeterminant(const r3::MATRIX *m);
r3::MATRIX matrixMultiply(const r3::MATRIX& m1, const r3::MATRIX& m2);
r3::MATRIX& matrixMultiplyIP(r3::MATRIX& m1, const r3::MATRIX& m2);
// These take an array and multiply it up in order.
r3::MATRIX matrixMultiply(const r3::MATRIX *ms, const unsigned int count);
r3::MATRIX *matrixMultiplyIP(r3::MATRIX *ms, const unsigned int count);
r3::MATRIX matrixMultiply(const std::vector<r3::MATRIX>& ms);
r3::MATRIX matrixMultiply(const r3::MATRIX& m, const float f);

r3::MATRIX matrixTranspose(const r3::MATRIX& m);
r3::MATRIX matrixInverse(const r3::MATRIX& m);
#define matrixNormalTransform(m) matrixTranspose(matrixInverse( m ))

r3::VECTOR matrixApply(const r3::VECTOR& v, const r3::MATRIX& m);
r3::VECTOR *matrixApplyIP(r3::VECTOR *v, const r3::MATRIX& m);
r3::VERTEX matrixApply(const r3::VERTEX& v, const r3::MATRIX& m, const r3::MATRIX& mnormal);
r3::VERTEX *matrixApplyIP(r3::VERTEX *v, const r3::MATRIX& m, const r3::MATRIX& mnormal);
r3::VERTEX2 matrixApply(const r3::VERTEX2& v, const r3::MATRIX& m, const r3::MATRIX& mnormal);
r3::VERTEX2 *matrixApplyIP(r3::VERTEX2 *v, const r3::MATRIX& m, const r3::MATRIX& mnormal);

inline r3::MATRIX matrixGetRotation(const float x, const float y, const float z) {
    r3::MATRIX m = matrixGetRotationX(x);
    matrixMultiplyIP(m, matrixGetRotationY(y));
    matrixMultiplyIP(m, matrixGetRotationZ(z));
    return m;
}


inline void matrixExtractRotation(const r3::MATRIX& m, r3::VECTOR& r, bool alternate = false) {
    float s = alternate?-1:1;
    r.x = atan2(-m.m[1][2]*s, m.m[2][2]*s);
    r.y = asin(m.m[0][2]*s)+(alternate?M_PI:0);
    r.z = atan2(-m.m[0][1]*s, m.m[0][0]*s);
}

inline void matrixExtractAxisRotation(const r3::MATRIX& m, float& x, float& y, float& z) {
    float w = -acos((m.m[0][0] + m.m[1][1] + m.m[2][2] - 1.0)/2.0);
    float s = sqrt(
                   ((m.m[2][1] - m.m[1][2])*(m.m[2][1] - m.m[1][2])) +
                   ((m.m[0][2] - m.m[2][0])*(m.m[0][2] - m.m[2][0])) +
                   ((m.m[1][0] - m.m[0][1])*(m.m[1][0] - m.m[0][1]))
                  );
    if (s != 0) {
        x = w*(m.m[2][1] - m.m[1][2])/s;
        y = w*(m.m[0][2] - m.m[2][0])/s;
        z = w*(m.m[1][0] - m.m[0][1])/s;
    } else {
        x = 0;
        y = 0;
        z = 0;
    }
}

inline void matrixExtractAxisRotation(const r3::MATRIX& m, r3::VECTOR& r) {
    matrixExtractAxisRotation(m, r.x, r.y, r.z);
}

template <class T>
inline void doFixOrientation(T& v, const c3DLoaderOrientation& orient) {
    matrixApplyIP(&v, matrixGetFixOrientation(orient));
}
template <>
inline void doFixOrientation(r3::txyz& v, const c3DLoaderOrientation& orient) {
    txyzFixOrientation(v, orient);
}
/*
template <class T>
inline T max(const T& a, const T& b) {
    return (a>b)?a:b;
}
*/

bool matrixIsEqual(const r3::MATRIX& m1, const r3::MATRIX& m2);

r3::MATRIX matrixThereAndBackAgain(const std::vector<r3::MATRIX>& stack, const r3::MATRIX& transform, const r3::MATRIX& undodamage);

void boundsInit(r3::VECTOR *bbox_min, r3::VECTOR *bbox_max);
void boundsContain(const r3::VECTOR *v, r3::VECTOR *bbox_min, r3::VECTOR *bbox_max);
void boundsContain(const r3::VECTOR *inner_min, const r3::VECTOR *inner_max, r3::VECTOR *bbox_min, r3::VECTOR *bbox_max);

#endif // OVLMATRIX_H_INCLUDED
