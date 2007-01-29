///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// MS3D Loader
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
// Parts of this code are based on code written for rct3tool (Copyright 2005
// Jonathan Wilson) by DragonsIOA.
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "MS3DLoader.h"
#include "MS3DFile.h"

cMS3DLoader::cMS3DLoader(const char *filename): c3DLoader(filename) {
    CMS3DFile *ms3df = new CMS3DFile();
    if (!ms3df->LoadFromFile(filename))
        return;
    for (int m = 0; m < ms3df->GetNumGroups(); m++) {
        c3DMesh cmesh;

        ms3d_group_t *group;
        ms3df->GetGroupAt(m, &group);

        cmesh.m_name = group->name;
        cmesh.m_flag = C3DMESH_VALID;

        VERTEX tv;
        unsigned long i, j;

        if (group->numtriangles == 0) {
            cmesh.m_flag = C3DMESH_INVALID;
        } else
            for (i = 0; i < group->numtriangles * 3; i += 3) {
                ms3d_vertex_t *vertex;
                ms3d_triangle_t *face;

                UINT index = i / 3;
                BOOL add;

                word i_triangle = group->triangleIndices[index];
                if (i_triangle >= ms3df->GetNumTriangles()) {
                    cmesh.m_flag = C3DMESH_INVALID;
                    continue;
                }
                ms3df->GetTriangleAt(i_triangle, &face);

                word i_vertex = face->vertexIndices[0];
                if (i_vertex >= ms3df->GetNumVertices()) {
                    cmesh.m_flag = C3DMESH_INVALID;
                    continue;
                }
                ms3df->GetVertexAt(i_vertex, &vertex);
                tv.position.x = vertex->vertex[0];
                tv.position.y = vertex->vertex[1];
                tv.position.z = vertex->vertex[2];
                tv.color = 0xffffffff;
                tv.normal.x = face->vertexNormals[0][0];
                tv.normal.y = face->vertexNormals[0][1];
                tv.normal.z = face->vertexNormals[0][2];
                tv.tu = face->s[0];
                tv.tv = 1.0-face->t[0];

                // now see if we have already added this point
                add = TRUE;
                for (j = 0; j < (long) cmesh.m_vertices.size(); ++j) {
                    VERTEX *pv = &cmesh.m_vertices[j];
                    if (memcmp(pv, &tv, sizeof(VERTEX)) == 0) {
                        // we have a match so exit
                        add = FALSE;
                        break;
                    }
                }
                if (add) {
                    cmesh.m_vertices.push_back(tv);
                }
                // j should have our real index value now as well
                cmesh.m_indices.push_back(j);

                // now for the second one
                i_vertex = face->vertexIndices[1];
                if (i_vertex >= ms3df->GetNumVertices()) {
                    cmesh.m_flag = C3DMESH_INVALID;
                    continue;
                }
                ms3df->GetVertexAt(i_vertex, &vertex);
                tv.position.x = vertex->vertex[0];
                tv.position.y = vertex->vertex[1];
                tv.position.z = vertex->vertex[2];
                tv.color = 0xffffffff;
                tv.normal.x = face->vertexNormals[1][0];
                tv.normal.y = face->vertexNormals[1][1];
                tv.normal.z = face->vertexNormals[1][2];
                tv.tu = face->s[1];
                tv.tv = 1.0-face->t[1];

                // now see if we have already added this point
                add = TRUE;
                for (j = 0; j < (long) cmesh.m_vertices.size(); ++j) {
                    VERTEX *pv = &cmesh.m_vertices[j];
                    if (memcmp(pv, &tv, sizeof(VERTEX)) == 0) {
                        // we have a match so exit
                        add = FALSE;
                        break;
                    }
                }
                if (add) {
                    cmesh.m_vertices.push_back(tv);
                }
                // j should have our real index value now as well
                cmesh.m_indices.push_back(j);

                // now for the third one
                i_vertex = face->vertexIndices[2];
                if (i_vertex >= ms3df->GetNumVertices()) {
                    cmesh.m_flag = C3DMESH_INVALID;
                    continue;
                }
                ms3df->GetVertexAt(i_vertex, &vertex);
                tv.position.x = vertex->vertex[0];
                tv.position.y = vertex->vertex[1];
                tv.position.z = vertex->vertex[2];
                tv.color = 0xffffffff;
                tv.normal.x = face->vertexNormals[2][0];
                tv.normal.y = face->vertexNormals[2][1];
                tv.normal.z = face->vertexNormals[2][2];
                tv.tu = face->s[2];
                tv.tv = 1.0-face->t[2];

                // now see if we have already added this point
                add = TRUE;
                for (j = 0; j < (long) cmesh.m_vertices.size(); ++j) {
                    VERTEX *pv = &cmesh.m_vertices[j];
                    if (memcmp(pv, &tv, sizeof(VERTEX)) == 0) {
                        // we have a match so exit
                        add = FALSE;
                        break;
                    }
                }
                if (add) {
                    cmesh.m_vertices.push_back(tv);
                }
                // j should have our real index value now as well
                cmesh.m_indices.push_back(j);
            }
        m_meshes.push_back(cmesh);
    }

    for (int m = 0; m < ms3df->GetNumVertices(); m++) {
        ms3d_vertex_t *vertex;
        ms3df->GetVertexAt(m, &vertex);
        if (vertex->referenceCount == 0) {
            c3DMesh cmesh;
            VERTEX tv;
            cmesh.m_flag = C3DMESH_INVALID;
            char *tmp = new char[256];
            snprintf(tmp, 255, "Vertex at <%.4f,%.4f,%.4f>", vertex->vertex[0], vertex->vertex[1], vertex->vertex[2]);
            cmesh.m_name = tmp;
            delete[] tmp;

            tv.position.x = vertex->vertex[0];
            tv.position.y = vertex->vertex[1];
            tv.position.z = vertex->vertex[2];
            tv.color = 0xffffffff;
            tv.normal.x = 0.0;
            tv.normal.y = 0.0;
            tv.normal.z = 0.0;
            tv.tu = 0.0;
            tv.tv = 0.0;
            cmesh.m_vertices.push_back(tv);
            m_meshes.push_back(cmesh);
        }
    }
}
