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

#include <vector>
#include "matrix.h"
#include <math.h>
#include <float.h>
#include <string.h>

using namespace r3;

float Deg2Rad(const float deg) {
    return deg * M_PI / 180.0;
}

float Rad2Deg(const float rad) {
    return rad * 180.0 / M_PI;
}

VECTOR vectorInvert(const VECTOR& v) {
    VECTOR r;
    r.x = -v.x;
    r.y = -v.y;
    r.z = -v.z;
    return r;
}

VECTOR vectorNormalize(const float x, const float y, const float z) {
    float norm = sqrt(pow(x, 2)+pow(y, 2)+pow(z, 2));
    VECTOR r;
    r.x = x/norm;
    r.y = y/norm;
    r.z = z/norm;
    return r;
}

/** @brief txyzEulerToAxis
  *
  * @todo: document this function
  */
txyz txyzEulerToAxis(const txyz& euler) {
    float sx = sin(euler.x/2.0);
    float sy = sin(euler.y/2.0);
    float sz = sin(euler.z/2.0);
    float cx = cos(euler.x/2.0);
    float cy = cos(euler.y/2.0);
    float cz = cos(euler.z/2.0);

    // Calculate quaternion
    float qw = cx*cy*cz - sx*sy*sz;
    if (fabs(qw) > 0.99999)
        return txyzMake(euler.Time, 0, 0, 0);
    float qx = sx*cy*cz + cx*sy*sz;
    float qy = cx*sy*cz - sx*cy*sz;
    float qz = sx*sy*cz + cx*cy*sz;

    // axis angle
    float aa = 2* acos(qw);

    return txyzMake(euler.Time, qx*aa/sqrtf(1-qw*qw), qy*aa/sqrtf(1-qw*qw), qz*aa/sqrtf(1-qw*qw));
}

/** @brief vertex2boneFix
  *
  * @todo: document this function
  */
void vertex2boneFix(r3::VERTEX2& v) {
    int used = 0;
    int weight = 0;
    for (int i = 0; i < 4; ++i) {
        if (v.boneweight[i]) {
            used++;
            weight += v.boneweight[i];
        }
    }
    if ((weight == 0xFF) || (!used))
        return;

    int rest = 0xFF - weight;
    if (used == 4) {
        if ((rest > 4) || (rest < -4)) {
            for (int i = 0; i < 4; ++i) {
                v.boneweight[i] = static_cast<float>(v.boneweight[i]) * 255.0 / static_cast<float>(weight);
            }
            vertex2boneFix(v);
        } else {
            if (rest > 0) {
                int b = 0;
                while (rest) {
                    if (v.boneweight[b]<255) {
                        v.boneweight[b]++;
                        rest--;
                    }
                    b++;
                    if (b == used)
                        b = 0;
                }
            } else {
                int b = 0;
                while (rest) {
                    if (v.boneweight[b]) {
                        v.boneweight[b]--;
                        rest++;
                    }
                    b++;
                    if (b == used)
                        b = 0;
                }
            }
        }
    } else {
        if (rest > used) {
            v.bone[used] = -1;
            v.boneweight[used] = rest;
        } else if (rest > 0) {
            int b = 0;
            while (rest) {
                if (v.boneweight[b]<255) {
                    v.boneweight[b]++;
                    rest--;
                }
                b++;
                if (b == used)
                    b = 0;
            }
        } else {
            int b = 0;
            while (rest) {
                if (v.boneweight[b]) {
                    v.boneweight[b]--;
                    rest++;
                }
                b++;
                if (b == used)
                    b = 0;
            }
        }
    }
}


void matrixSetUnity(MATRIX& m) {
    memset(&m, 0, sizeof(m));
    m._11 = 1.0;
    m._22 = 1.0;
    m._33 = 1.0;
    m._44 = 1.0;
}

MATRIX matrixGetFixOrientation(const c3DLoaderOrientation orient) {
    switch (orient) {
        case ORIENTATION_RIGHT_XUP: { // Whatever...            | s-x m-y a-z
            MATRIX m;
            memset(&m, 0, sizeof(m));
            m._12 = 1.0;                //  0.0  1.0  0.0  0.0
            m._23 = -1.0;               //  0.0  0.0 -1.0  0.0
            m._31 = 1.0;                //  1.0  0.0  0.0  0.0
            m._44 = 1.0;                //  0.0  0.0  0.0  1.0
            return m;
        }
        case ORIENTATION_RIGHT_YUP: // MirrorZ                  | s-y m-z a-x
            return matrixGetScale(1.0, 1.0, -1.0);
        case ORIENTATION_RIGHT_ZUP: { // MirrorZ, -90 X, -90 Y  | s-z m-x a-y
            MATRIX m;
            memset(&m, 0, sizeof(m));
            m._13 = -1.0;               //  0.0  0.0 -1.0  0.0
            m._21 = 1.0;                //  1.0  0.0  0.0  0.0
            m._32 = 1.0;                //  0.0  1.0  0.0  0.0
            m._44 = 1.0;                //  0.0  0.0  0.0  1.0
            return m;
        }
        case ORIENTATION_LEFT_XUP: { // Whatever...              | s-x m--y a-z
            MATRIX m;
            memset(&m, 0, sizeof(m));
            m._12 = 1.0;                //  0.0  1.0  0.0  0.0
            m._23 = 1.0;                //  0.0  0.0  1.0  0.0
            m._31 = 1.0;                //  1.0  0.0  0.0  0.0
            m._44 = 1.0;                //  0.0  0.0  0.0  1.0
            return m;
        }
        // case ORIENTATION_LEFT_YUP: // Fall through to default | s-y m--z a-x
        case ORIENTATION_LEFT_ZUP: { // Whatever...              | s-z m--x a-y
            MATRIX m;
            memset(&m, 0, sizeof(m));
            m._13 = 1.0;                //  0.0  0.0  1.0  0.0
            m._21 = 1.0;                //  1.0  0.0  0.0  0.0
            m._32 = 1.0;                //  0.0  1.0  0.0  0.0
            m._44 = 1.0;                //  0.0  0.0  0.0  1.0
            return m;
        }
        default:
            return matrixGetUnity();
    }
}

void txyzFixOrientation(txyz& src, const c3DLoaderOrientation& orient, bool rotate) {
    switch (orient) {
        case ORIENTATION_RIGHT_XUP: {
            float t = -src.Y * (rotate?-1.0:1.0);
            src.Y = src.X * (rotate?-1.0:1.0);
            src.X = src.Z * (rotate?-1.0:1.0);
            src.Z = t;
            break;
        }
        case ORIENTATION_RIGHT_YUP: {
            src.X = src.X * (rotate?-1.0:1.0);
            src.Y = src.Y * (rotate?-1.0:1.0);
            src.Z = -src.Z * (rotate?-1.0:1.0);
            break;
        }
        case ORIENTATION_RIGHT_ZUP: {
            float t = -src.X * (rotate?-1.0:1.0);
            src.X = src.Y * (rotate?-1.0:1.0);
            src.Y = src.Z * (rotate?-1.0:1.0);
            src.Z = t;
            break;
        }
        case ORIENTATION_LEFT_XUP: {
            float t = src.X * (rotate?-1.0:1.0);
            src.X = src.Z * (rotate?-1.0:1.0);
            src.Z = src.Y * (rotate?-1.0:1.0);
            src.Y = t;
            break;
        }
        // case ORIENTATION_LEFT_YUP: // Fall through to default
        case ORIENTATION_LEFT_ZUP: {
            float t = src.X;
            src.X = src.Y;
            src.Y = src.Z;
            src.Z = t;
            break;
        }
        default:
            break;
    }
}

MATRIX matrixGetTranslation(const float x, const float y, const float z) {
    MATRIX m = matrixGetUnity();
    m._41 = x;
    m._42 = y;
    m._43 = z;
    return m;
}

MATRIX matrixGetTranslation(const VECTOR& v) {
    return matrixGetTranslation(v.x, v.y, v.z);
}

MATRIX matrixGetScale(const float x, const float y, const float z) {
    MATRIX m = matrixGetUnity();
    m._11 = x;
    m._22 = y;
    m._33 = z;
    return m;
}

MATRIX matrixGetScale(const VECTOR& v) {
    return matrixGetScale(v.x, v.y, v.z);
}

MATRIX matrixGetRotationX(const float rad) {
    MATRIX m = matrixGetUnity();
    m._22 = cos(rad);
    m._23 = sin(rad);
    m._32 = -sin(rad);
    m._33 = cos(rad);
    return m;
}
MATRIX matrixGetRotationY(const float rad) {
    MATRIX m = matrixGetUnity();
    m._11 = cos(rad);
    m._31 = sin(rad);
    m._13 = -sin(rad);
    m._33 = cos(rad);
    return m;
}
MATRIX matrixGetRotationZ(const float rad) {
    MATRIX m = matrixGetUnity();
    m._11 = cos(rad);
    m._12 = sin(rad);
    m._21 = -sin(rad);
    m._22 = cos(rad);
    return m;
}


float matrixCalcDeterminant(const MATRIX *m) {
    return (m->_11*m->_22*m->_33*m->_44)
           -(m->_11*m->_22*m->_34*m->_43)
           +(m->_11*m->_23*m->_34*m->_42)
           -(m->_11*m->_23*m->_32*m->_44)
           +(m->_11*m->_24*m->_32*m->_43)
           -(m->_11*m->_24*m->_33*m->_42)
           -(m->_12*m->_23*m->_34*m->_41)
           +(m->_12*m->_23*m->_31*m->_44)
           -(m->_12*m->_24*m->_31*m->_43)
           +(m->_12*m->_24*m->_33*m->_41)
           -(m->_12*m->_21*m->_33*m->_44)
           +(m->_12*m->_21*m->_34*m->_43)
           +(m->_13*m->_24*m->_31*m->_42)
           -(m->_13*m->_24*m->_32*m->_41)
           +(m->_13*m->_21*m->_32*m->_44)
           -(m->_13*m->_21*m->_34*m->_42)
           +(m->_13*m->_22*m->_34*m->_41)
           -(m->_13*m->_22*m->_31*m->_44)
           -(m->_14*m->_21*m->_32*m->_43)
           +(m->_14*m->_21*m->_33*m->_42)
           -(m->_14*m->_22*m->_33*m->_41)
           +(m->_14*m->_22*m->_31*m->_43)
           -(m->_14*m->_23*m->_31*m->_42)
           +(m->_14*m->_23*m->_32*m->_41);
}

MATRIX matrixMultiply(const MATRIX& m1, const MATRIX& m2) {
    MATRIX m;
    m._11 = (m1._11 * m2._11) + (m1._12 * m2._21) + (m1._13 * m2._31) + (m1._14 * m2._41);
    m._12 = (m1._11 * m2._12) + (m1._12 * m2._22) + (m1._13 * m2._32) + (m1._14 * m2._42);
    m._13 = (m1._11 * m2._13) + (m1._12 * m2._23) + (m1._13 * m2._33) + (m1._14 * m2._43);
    m._14 = (m1._11 * m2._14) + (m1._12 * m2._24) + (m1._13 * m2._34) + (m1._14 * m2._44);

    m._21 = (m1._21 * m2._11) + (m1._22 * m2._21) + (m1._23 * m2._31) + (m1._24 * m2._41);
    m._22 = (m1._21 * m2._12) + (m1._22 * m2._22) + (m1._23 * m2._32) + (m1._24 * m2._42);
    m._23 = (m1._21 * m2._13) + (m1._22 * m2._23) + (m1._23 * m2._33) + (m1._24 * m2._43);
    m._24 = (m1._21 * m2._14) + (m1._22 * m2._24) + (m1._23 * m2._34) + (m1._24 * m2._44);

    m._31 = (m1._31 * m2._11) + (m1._32 * m2._21) + (m1._33 * m2._31) + (m1._34 * m2._41);
    m._32 = (m1._31 * m2._12) + (m1._32 * m2._22) + (m1._33 * m2._32) + (m1._34 * m2._42);
    m._33 = (m1._31 * m2._13) + (m1._32 * m2._23) + (m1._33 * m2._33) + (m1._34 * m2._43);
    m._34 = (m1._31 * m2._14) + (m1._32 * m2._24) + (m1._33 * m2._34) + (m1._34 * m2._44);

    m._41 = (m1._41 * m2._11) + (m1._42 * m2._21) + (m1._43 * m2._31) + (m1._44 * m2._41);
    m._42 = (m1._41 * m2._12) + (m1._42 * m2._22) + (m1._43 * m2._32) + (m1._44 * m2._42);
    m._43 = (m1._41 * m2._13) + (m1._42 * m2._23) + (m1._43 * m2._33) + (m1._44 * m2._43);
    m._44 = (m1._41 * m2._14) + (m1._42 * m2._24) + (m1._43 * m2._34) + (m1._44 * m2._44);
    return m;
}

MATRIX& matrixMultiplyIP(MATRIX& m1, const MATRIX& m2) {
    m1 = matrixMultiply(m1, m2);
    return m1;
}

MATRIX matrixMultiply(const MATRIX *ms, const unsigned int count) {
    MATRIX m;
    switch (count) {
    case 0:
        m = matrixGetUnity();
        break;
    case 1:
        m = *ms;
        break;
    case 2:
        m = matrixMultiply(ms[0], ms[1]);
        break;
    default: {
        m = *ms;
        for (unsigned int i = 1; i < count; i++)
            matrixMultiplyIP(m, ms[i]);
    }
    break;
    }
    return m;
}

MATRIX *matrixMultiplyIP(MATRIX *ms, const unsigned int count) {
    switch (count) {
    case 0:
    case 1:
        break;
    case 2:
        matrixMultiplyIP(ms[0], ms[1]);
        break;
    default: {
        for (unsigned int i = 1; i < count; i++)
            matrixMultiplyIP(*ms, ms[i]);
    }
    break;
    }
    return ms;
}

MATRIX matrixMultiply(const std::vector<MATRIX>& ms) {
    MATRIX m;
    switch (ms.size()) {
    case 0:
        m = matrixGetUnity();
        break;
    case 1:
        m = ms[0];
        break;
    case 2:
        m = matrixMultiply(ms[0], ms[1]);
        break;
    default: {
        m = ms[0];
        for (unsigned int i = 1; i < ms.size(); i++)
            matrixMultiplyIP(m, ms[i]);
    }
    break;
    }
    return m;
}

MATRIX matrixMultiply(const MATRIX& m, const float f) {
    MATRIX r;
    for (int i = 0; i<4; i++)
        for (int j = 0; j<4; j++)
            r.m[i][j] = m.m[i][j] * f;
    return r;
}

MATRIX matrixTranspose(const MATRIX& m) {
    MATRIX r;
    r._11 = m._11;
    r._12 = m._21;
    r._13 = m._31;
    r._14 = m._41;
    r._21 = m._12;
    r._22 = m._22;
    r._23 = m._32;
    r._24 = m._42;
    r._31 = m._13;
    r._32 = m._23;
    r._33 = m._33;
    r._34 = m._43;
    r._41 = m._14;
    r._42 = m._24;
    r._43 = m._34;
    r._44 = m._44;
    return r;
}

MATRIX matrixInverse(const MATRIX& m) {
    float tmp[12]; /* temp array for pairs */
    MATRIX src = matrixTranspose(m); /* array of transpose source matrix */
    float det; /* determinant */
    MATRIX r;

    /* calculate pairs for first 8 elements (cofactors) */
    tmp[0] = src._33 * src._44;
    tmp[1] = src._34 * src._43;
    tmp[2] = src._32 * src._44;
    tmp[3] = src._34 * src._42;
    tmp[4] = src._32 * src._43;
    tmp[5] = src._33 * src._42;
    tmp[6] = src._31 * src._44;
    tmp[7] = src._34 * src._41;
    tmp[8] = src._31 * src._43;
    tmp[9] = src._33 * src._41;
    tmp[10] = src._31 * src._42;
    tmp[11] = src._32 * src._41;
    /* calculate first 8 elements (cofactors) */
    r._11 = tmp[0]*src._22 + tmp[3]*src._23 + tmp[4]*src._24;
    r._11 -= tmp[1]*src._22 + tmp[2]*src._23 + tmp[5]*src._24;
    r._12 = tmp[1]*src._21 + tmp[6]*src._23 + tmp[9]*src._24;
    r._12 -= tmp[0]*src._21 + tmp[7]*src._23 + tmp[8]*src._24;
    r._13 = tmp[2]*src._21 + tmp[7]*src._22 + tmp[10]*src._24;
    r._13 -= tmp[3]*src._21 + tmp[6]*src._22 + tmp[11]*src._24;
    r._14 = tmp[5]*src._21 + tmp[8]*src._22 + tmp[11]*src._23;
    r._14 -= tmp[4]*src._21 + tmp[9]*src._22 + tmp[10]*src._23;
    r._21 = tmp[1]*src._12 + tmp[2]*src._13 + tmp[5]*src._14;
    r._21 -= tmp[0]*src._12 + tmp[3]*src._13 + tmp[4]*src._14;
    r._22 = tmp[0]*src._11 + tmp[7]*src._13 + tmp[8]*src._14;
    r._22 -= tmp[1]*src._11 + tmp[6]*src._13 + tmp[9]*src._14;
    r._23 = tmp[3]*src._11 + tmp[6]*src._12 + tmp[11]*src._14;
    r._23 -= tmp[2]*src._11 + tmp[7]*src._12 + tmp[10]*src._14;
    r._24 = tmp[4]*src._11 + tmp[9]*src._12 + tmp[10]*src._13;
    r._24 -= tmp[5]*src._11 + tmp[8]*src._12 + tmp[11]*src._13;
    /* calculate pairs for second 8 elements (cofactors) */
    tmp[0] = src._13*src._24;
    tmp[1] = src._14*src._23;
    tmp[2] = src._12*src._24;
    tmp[3] = src._14*src._22;
    tmp[4] = src._12*src._23;
    tmp[5] = src._13*src._22;
    tmp[6] = src._11*src._24;
    tmp[7] = src._14*src._21;
    tmp[8] = src._11*src._23;
    tmp[9] = src._13*src._21;
    tmp[10] = src._11*src._22;
    tmp[11] = src._12*src._21;
    /* calculate second 8 elements (cofactors) */
    r._31 = tmp[0]*src._42 + tmp[3]*src._43 + tmp[4]*src._44;
    r._31 -= tmp[1]*src._42 + tmp[2]*src._43 + tmp[5]*src._44;
    r._32 = tmp[1]*src._41 + tmp[6]*src._43 + tmp[9]*src._44;
    r._32 -= tmp[0]*src._41 + tmp[7]*src._43 + tmp[8]*src._44;
    r._33 = tmp[2]*src._41 + tmp[7]*src._42 + tmp[10]*src._44;
    r._33 -= tmp[3]*src._41 + tmp[6]*src._42 + tmp[11]*src._44;
    r._34 = tmp[5]*src._41 + tmp[8]*src._42 + tmp[11]*src._43;
    r._34 -= tmp[4]*src._41 + tmp[9]*src._42 + tmp[10]*src._43;
    r._41 = tmp[2]*src._33 + tmp[5]*src._34 + tmp[1]*src._32;
    r._41 -= tmp[4]*src._34 + tmp[0]*src._32 + tmp[3]*src._33;
    r._42 = tmp[8]*src._34 + tmp[0]*src._31 + tmp[7]*src._33;
    r._42 -= tmp[6]*src._33 + tmp[9]*src._34 + tmp[1]*src._31;
    r._43 = tmp[6]*src._32 + tmp[11]*src._34 + tmp[3]*src._31;
    r._43 -= tmp[10]*src._34 + tmp[2]*src._31 + tmp[7]*src._32;
    r._44 = tmp[10]*src._33 + tmp[4]*src._31 + tmp[9]*src._32;
    r._44 -= tmp[8]*src._32 + tmp[11]*src._33 + tmp[5]*src._31;
    /* calculate determinant */
    det=src._11*r._11+src._12*r._12+src._13*r._13+src._14*r._14;
    /* calculate matrix inverse */
    det = 1/det;
    return matrixMultiply(r, det);
}

VECTOR matrixApply(const VECTOR& v, const MATRIX& m) {
    VECTOR r;
    r.x = v.x * m._11 + v.y * m._21 + v.z * m._31 + m._41;
    r.y = v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42;
    r.z = v.x * m._13 + v.y * m._23 + v.z * m._33 + m._43;
    return r;
}

VECTOR *matrixApplyIP(VECTOR *v, const MATRIX& m) {
    VECTOR r = matrixApply(*v, m);
    *v = r;
    return v;
}

VERTEX matrixApply(const VERTEX& v, const MATRIX& m, const MATRIX& mnormal) {
    VERTEX r = v;
    r.position.x = v.position.x * m._11 + v.position.y * m._21 + v.position.z * m._31 + m._41;
    r.position.y = v.position.x * m._12 + v.position.y * m._22 + v.position.z * m._32 + m._42;
    r.position.z = v.position.x * m._13 + v.position.y * m._23 + v.position.z * m._33 + m._43;
    r.normal = vectorNormalize(v.normal.x * mnormal._11 + v.normal.y * mnormal._21 + v.normal.z * mnormal._31,
                               v.normal.x * mnormal._12 + v.normal.y * mnormal._22 + v.normal.z * mnormal._32,
                               v.normal.x * mnormal._13 + v.normal.y * mnormal._23 + v.normal.z * mnormal._33);
    return r;
}

VERTEX *matrixApplyIP(VERTEX *v, const MATRIX& m, const MATRIX& mnormal) {
    VERTEX r = matrixApply(*v, m, mnormal);
    *v = r;
    return v;
}

VERTEX2 matrixApply(const VERTEX2& v, const MATRIX& m, const MATRIX& mnormal) {
    VERTEX2 r = v;
    r.position.x = v.position.x * m._11 + v.position.y * m._21 + v.position.z * m._31 + m._41;
    r.position.y = v.position.x * m._12 + v.position.y * m._22 + v.position.z * m._32 + m._42;
    r.position.z = v.position.x * m._13 + v.position.y * m._23 + v.position.z * m._33 + m._43;
    r.normal = vectorNormalize(v.normal.x * mnormal._11 + v.normal.y * mnormal._21 + v.normal.z * mnormal._31,
                               v.normal.x * mnormal._12 + v.normal.y * mnormal._22 + v.normal.z * mnormal._32,
                               v.normal.x * mnormal._13 + v.normal.y * mnormal._23 + v.normal.z * mnormal._33);
    return r;
}

VERTEX2 *matrixApplyIP(VERTEX2 *v, const MATRIX& m, const MATRIX& mnormal) {
    VERTEX2 r = matrixApply(*v, m, mnormal);
    *v = r;
    return v;
}

bool matrixIsEqual(const MATRIX& m1, const MATRIX& m2) {
    return (m1._11 == m2._11) && (m1._12 == m2._12) && (m1._13 == m2._13) && (m1._14 == m2._14) &&
           (m1._21 == m2._21) && (m1._22 == m2._22) && (m1._23 == m2._23) && (m1._24 == m2._24) &&
           (m1._31 == m2._31) && (m1._32 == m2._32) && (m1._33 == m2._33) && (m1._34 == m2._34) &&
           (m1._41 == m2._41) && (m1._42 == m2._42) && (m1._43 == m2._43) && (m1._44 == m2._44);
}

MATRIX matrixThereAndBackAgain(const std::vector<MATRIX>& stack, const MATRIX& transform, const MATRIX& undodamage) {
    std::vector<MATRIX> tempstack = stack;
    // to correctely apply the model transformation matrix to effect points we have to
    // transform in-game objects into modeler-space first by applying the inverse
    // of the model transformation matrix
    // (now replaced by the undoDamage matrix to allow separation of the coordinate
    // system fix from other model transformations)
    tempstack.insert(tempstack.begin(), undodamage);
    // At the end of the stack, transform back
    tempstack.push_back(transform);
    return matrixMultiply(tempstack);
}

void boundsInit(VECTOR *bbox_min, VECTOR *bbox_max) {
    bbox_min->x = FLT_MAX;
    bbox_min->y = FLT_MAX;
    bbox_min->z = FLT_MAX;
    bbox_max->x = FLT_MIN;
    bbox_max->y = FLT_MIN;
    bbox_max->z = FLT_MIN;
}

void boundsContain(const VECTOR *v, VECTOR *bbox_min, VECTOR *bbox_max) {
    if (v->x < bbox_min->x)
        bbox_min->x = v->x;
    if (v->y < bbox_min->y)
        bbox_min->y = v->y;
    if (v->z < bbox_min->z)
        bbox_min->z = v->z;
    if (v->x > bbox_max->x)
        bbox_max->x = v->x;
    if (v->y > bbox_max->y)
        bbox_max->y = v->y;
    if (v->z > bbox_max->z)
        bbox_max->z = v->z;
}

void boundsContain(const VECTOR *inner_min, const VECTOR *inner_max, VECTOR *bbox_min, VECTOR *bbox_max) {
    if (inner_min->x < bbox_min->x)
        bbox_min->x = inner_min->x;
    if (inner_min->y < bbox_min->y)
        bbox_min->y = inner_min->y;
    if (inner_min->z < bbox_min->z)
        bbox_min->z = inner_min->z;
    if (inner_max->x > bbox_max->x)
        bbox_max->x = inner_max->x;
    if (inner_max->y > bbox_max->y)
        bbox_max->y = inner_max->y;
    if (inner_max->z > bbox_max->z)
        bbox_max->z = inner_max->z;
}
