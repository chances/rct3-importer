///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Loader base class
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

#include "3DLoader.h"
#include "matrix.h"

#include "ASE3DLoader.h"
#include "MS3DLoader.h"

bool c3DLoader::FetchObject(unsigned int index, unsigned long *vertexcount, VERTEX **vertices, unsigned long *index_count, unsigned long **indices, D3DVECTOR *bbox_min, D3DVECTOR *bbox_max, const D3DMATRIX *transform) {
    int i;
    if (m_meshes.size() <= 0)
        return false;
    *vertexcount = m_meshes[index].m_vertices.size();
    *index_count = m_meshes[index].m_indices.size();
    *vertices = new VERTEX[*vertexcount];
    D3DMATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < *vertexcount; i++) {
        if (transform)
            (*vertices)[i] = matrixApply(m_meshes[index].m_vertices[i], *transform, normaltransform);
        else
            (*vertices)[i] = m_meshes[index].m_vertices[i];
        boundsContain(&((*vertices)[i]).position, bbox_min, bbox_max);
    }
    *indices = new unsigned long[*index_count];
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < *index_count; i+=3) {
        (*indices)[i] = m_meshes[index].m_indices[i+((do_mirror)?1:0)];
        (*indices)[i+1] = m_meshes[index].m_indices[i+((do_mirror)?0:1)];
        (*indices)[i+2] = m_meshes[index].m_indices[i+2];
    }
    return true;
}

c3DLoader *c3DLoader::LoadFile(const char *filename) {
    c3DLoader *res;

    res = new cMS3DLoader(filename);
    if (res) {
        if (res->Ok())
            return res;
        else
            delete res;
    }

    res = new cASE3DLoader(filename);
    if (res) {
        if (res->Ok())
            return res;
        else
            delete res;
    }

    return NULL;
}
