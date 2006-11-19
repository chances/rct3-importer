///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// ASE Loader
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

#include "ASE3DLoader.h"
#include "libASE.h"

cASE3DLoader::cASE3DLoader(const char *filename): c3DLoader(filename) {
    ASE_Scene *scene = NULL;

    {
        // Test whether it's an ASE
        FILE *fp = fopen(filename, "r");
        if (!fp)
            return;
        char test[20];
        test[19] = 0;
        fread(&test, 1, 19, fp);
        fclose(fp);
        if (stricmp(test, "*3DSMAX_ASCIIEXPORT") != 0)
            return;
    }



    try {
        scene = ASE_loadFilename(const_cast<char *> (filename));
    } catch (...) {
        try {
            ASE_freeScene(scene);
        } catch (...) {}
        return;
    }
    if (scene) {
        for (int m = 0; m < scene->objectCount; m++) {
            c3DMesh cmesh;
            cmesh.m_name = scene->objs[m].name;
            cmesh.m_flag = C3DMESH_VALID;

            ASE_Mesh *mesh = &scene->objs[m].mesh;
            VERTEX tv;
            unsigned long i, j;

            if (mesh->faceCount == 0) {
                cmesh.m_flag = C3DMESH_INVALID;
                for (i = 0; i < (unsigned long) mesh->vertexCount; i++) {
                    tv.position.x = mesh->vertices[i].x;
                    tv.position.y = mesh->vertices[i].y;
                    tv.position.z = mesh->vertices[i].z;
                    tv.color = 0xffffffff;
                    tv.normal.x = mesh->vertex_normals[i].x;
                    tv.normal.y = mesh->vertex_normals[i].y;
                    tv.normal.z = mesh->vertex_normals[i].z;
                    tv.tu = 0.0;
                    tv.tv = 0.0;
                    cmesh.m_vertices.push_back(tv);
                }
            } else
                for (i = 0; i < (unsigned long) mesh->faceCount * 3; i += 3) {
                    UINT index = i / 3;
                    UINT tvs, tts;
                    BOOL add;

                    tvs = mesh->faces[index].vertex[0];
                    tts = mesh->faces[index].texture_coordinates[0];

                    tv.position.x = mesh->vertices[tvs].x;
                    tv.position.y = mesh->vertices[tvs].y;
                    tv.position.z = mesh->vertices[tvs].z;
                    tv.color = 0xffffffff;
                    tv.normal.x = mesh->vertex_normals[tvs].x;
                    tv.normal.y = mesh->vertex_normals[tvs].y;
                    tv.normal.z = mesh->vertex_normals[tvs].z;
                    if (tts>=(UINT) mesh->textureCoordinateCount) {
                        cmesh.m_flag = C3DMESH_INVALID;
                        tv.tu = 0.0;
                        tv.tv = 0.0;
                    } else {
                        tv.tu = mesh->texture_coordinates[tts].x;
                        tv.tv = mesh->texture_coordinates[tts].y;
                    }

                    // now see if we have already added this point
                    add = TRUE;
                    for (j = 0; j < (unsigned long) cmesh.m_vertices.size(); ++j) {
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
                    tvs = mesh->faces[index].vertex[1];
                    tts = mesh->faces[index].texture_coordinates[1];

                    tv.position.x = mesh->vertices[tvs].x;
                    tv.position.y = mesh->vertices[tvs].y;
                    tv.position.z = mesh->vertices[tvs].z;
                    tv.color = 0xffffffff;
                    tv.normal.x = mesh->vertex_normals[tvs].x;
                    tv.normal.y = mesh->vertex_normals[tvs].y;
                    tv.normal.z = mesh->vertex_normals[tvs].z;
                    if (tts>=(UINT) mesh->textureCoordinateCount) {
                        cmesh.m_flag = C3DMESH_INVALID;
                        tv.tu = 0.0;
                        tv.tv = 0.0;
                    } else {
                        tv.tu = mesh->texture_coordinates[tts].x;
                        tv.tv = mesh->texture_coordinates[tts].y;
                    }

                    // now see if we have already added this point
                    add = TRUE;
                    for (j = 0; j < (unsigned long) cmesh.m_vertices.size(); ++j) {
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
                    tvs = mesh->faces[index].vertex[2];
                    tts = mesh->faces[index].texture_coordinates[2];

                    tv.position.x = mesh->vertices[tvs].x;
                    tv.position.y = mesh->vertices[tvs].y;
                    tv.position.z = mesh->vertices[tvs].z;
                    tv.color = 0xffffffff;
                    tv.normal.x = mesh->vertex_normals[tvs].x;
                    tv.normal.y = mesh->vertex_normals[tvs].y;
                    tv.normal.z = mesh->vertex_normals[tvs].z;
                    if (tts>=(UINT) mesh->textureCoordinateCount) {
                        cmesh.m_flag = C3DMESH_INVALID;
                        tv.tu = 0.0;
                        tv.tv = 0.0;
                    } else {
                        tv.tu = mesh->texture_coordinates[tts].x;
                        tv.tv = mesh->texture_coordinates[tts].y;
                    }

                    // now see if we have already added this point
                    add = TRUE;
                    for (j = 0; j < (unsigned long) cmesh.m_vertices.size(); ++j) {
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
        ASE_freeScene(scene);
    }
}
