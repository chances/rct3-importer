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

#ifndef _3DLOADER_H_INCLUDED
#define _3DLOADER_H_INCLUDED

#include "3Dstring.h"

#include <vector>

#include "vertex.h"

#include "3DLoaderTypes.h"

class cBoneShape2;
class cStaticShape2;

class c3DMesh {
public:
    std::vector<VERTEX> m_vertices;
    std::vector<unsigned long> m_indices;
    STRING3D m_name;
    unsigned long m_flag;

    c3DMesh():m_name("") {};
};

#ifndef STD_ONLY
#include <wx/hashmap.h>
class c3DLoaderCacheEntry;
WX_DECLARE_STRING_HASH_MAP(c3DLoaderCacheEntry*, c3DLoaderCache);
#endif
class c3DLoader {
#ifndef STD_ONLY
friend class c3DLoaderCacheEntry;
#endif
protected:
    STRING3D m_filename;
    STRING3D m_name;
    // Having no meshes signifies an invalid file
    std::vector<c3DMesh> m_meshes;
    // Warnings should only be added by a loader implementation if it
    // recognized the file is of it's type but is otherwise invalid
    STRINGLIST3D m_warnings;
    // no m_meshes and no m_warnigs signal that the loader did not recognize the file.
#ifndef STD_ONLY
    static c3DLoaderCache g_cache;
#endif
public:
    c3DLoader(const char *filename) : m_meshes(0) {
        m_filename = filename;
        m_name = STRING3D_EMPTY;
    };
    c3DLoader(c3DLoader* original) {
        if (original) {
            m_filename = original->m_filename;
            m_name = original->m_name;
            m_meshes = original->m_meshes;
            m_warnings = original->m_warnings;
        }
    };

    virtual ~c3DLoader() {};

    virtual int GetObjectCount() {
        return m_meshes.size();
    };
    virtual bool Failed() {
        return ((m_meshes.size()==0) && (m_warnings.size()==0));
    };
    virtual STRINGLIST3D GetWarnings() const {return m_warnings;};
    virtual STRING3D GetObjectName(unsigned int index) {
        return (index>=m_meshes.size())?"":m_meshes[index].m_name;
    };
    virtual int GetObjectVertexCount(unsigned int index) {
        return (index>=m_meshes.size())?0:m_meshes[index].m_vertices.size();
    };
    virtual int GetObjectIndexCount(unsigned int index) {
        return (index>=m_meshes.size())?0:m_meshes[index].m_indices.size();
    };
    virtual bool IsObjectValid(unsigned int index) {
        return (index>=m_meshes.size())?false:m_meshes[index].m_flag;
    };
    virtual VERTEX GetObjectVertex(unsigned int mesh, unsigned int vertex) {
        VERTEX res;
        memset(&res, 0, sizeof(res));
        if (mesh<m_meshes.size()) {
            if (vertex<m_meshes[mesh].m_vertices.size()) {
                res = m_meshes[mesh].m_vertices[vertex];
            }
        }
        return res;
    };
    virtual bool FetchObject(unsigned int index, unsigned long *vertexcount, VERTEX **vertices, unsigned long *index_count, unsigned long **indices, D3DVECTOR *bbox_min, D3DVECTOR *bbox_max, const D3DMATRIX *transform, D3DVECTOR *fudge_normal = NULL);
    virtual bool FetchAsAnimObject(unsigned int index, unsigned long bone, unsigned long unk, unsigned long *vertexcount, VERTEX2 **vertices, unsigned long *index_count, unsigned short **indices, D3DVECTOR *bbox_min, D3DVECTOR *bbox_max, const D3DMATRIX *transform, D3DVECTOR *fudge_normal = NULL);
    virtual bool FetchObject(unsigned int index, cStaticShape2* sh, D3DVECTOR *bbox_min, D3DVECTOR *bbox_max, const D3DMATRIX *transform, D3DVECTOR *fudge_normal = NULL);
    virtual bool FetchObject(unsigned int index, unsigned long bone, unsigned long unk, cBoneShape2* sh, D3DVECTOR *bbox_min, D3DVECTOR *bbox_max, const D3DMATRIX *transform, D3DVECTOR *fudge_normal = NULL);

    virtual int GetType() {return C3DLOADER_GENERIC;};
    virtual STRING3D GetName() {return m_name;};
    virtual c3DLoaderOrientation GetOrientation() {return ORIENTATION_UNKNOWN;};

    static int FlattenNormals(const unsigned long vertexcount, VERTEX *vertices, const D3DVECTOR& bbox_min, const D3DVECTOR& bbox_max);
    static int FlattenNormals(const unsigned long vertexcount, VERTEX2 *vertices, const D3DVECTOR& bbox_min, const D3DVECTOR& bbox_max);
    static int FlattenNormals(cStaticShape2* sh, const D3DVECTOR& bbox_min, const D3DVECTOR& bbox_max);
    static int FlattenNormals(cBoneShape2* sh, const D3DVECTOR& bbox_min, const D3DVECTOR& bbox_max);
    static c3DLoader *LoadFile(const char *filename);
#ifndef STD_ONLY
    static void ClearCache();
#endif
};

#endif // 3DLOADER_H_INCLUDED
