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

#include "wx_pch.h"

#include "vertex.h"
#include <string>
#include <vector>

#include "3DLoaderTypes.h"

class c3DMesh {
public:
    std::vector<VERTEX> m_vertices;
    std::vector<unsigned long> m_indices;
    wxString m_name;
    unsigned long m_flag;

    c3DMesh():m_name("") {};
};

class c3DLoader {
protected:
    wxString m_filename;
    wxString m_name;
    std::vector<c3DMesh> m_meshes;
public:
    c3DLoader(const char *filename) : m_meshes(0) {
        m_filename = filename;
        m_name = wxT("");
    };

    virtual ~c3DLoader() {};

    virtual int GetObjectCount() {
        return m_meshes.size();
    };
    virtual bool Ok() {
        return m_meshes.size()>0;
    };
    virtual wxString GetObjectName(unsigned int index) {
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
    virtual bool FetchObject(unsigned int index, unsigned long *vertexcount, VERTEX **vertices, unsigned long *index_count, unsigned long **indices, D3DVECTOR *bbox_min, D3DVECTOR *bbox_max, const D3DMATRIX *transform);

    virtual int GetType() {return C3DLOADER_GENERIC;};
    virtual wxString GetName() {return m_name;};
    virtual c3DLoaderOrientation GetOrientation() {return ORIENTATION_UNKNOWN;};

    static c3DLoader *LoadFile(const char *filename);
};

#endif // 3DLOADER_H_INCLUDED
