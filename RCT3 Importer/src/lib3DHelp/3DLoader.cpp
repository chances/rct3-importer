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

c3DLoader* DoLoadFile(const char *filename) {
    c3DLoader *res;

    res = new cMS3DLoader(filename);
    if (res) {
        if (!res->Failed())
            return res;
        else
            delete res;
    }

    res = new cASE3DLoader(filename);
    if (res) {
        if (!res->Failed())
            return res;
        else
            delete res;
    }

    return NULL;
}

#ifndef STD_ONLY
#include <wx/filename.h>

class c3DLoaderCacheEntry: public wxObject {
private:
    c3DLoader* m_object;
    wxFileName m_file;
    wxDateTime m_mtime;
public:
    c3DLoaderCacheEntry(wxString filename) {
        m_object = DoLoadFile(filename.fn_str());
        m_file = filename;
        if (m_object)
            m_mtime = m_file.GetModificationTime();
    }
    virtual ~c3DLoaderCacheEntry() {
        if (m_object)
            delete m_object;
    }
    c3DLoader* Get() {
        if (m_object) {
            wxDateTime lastmod;
            if (!m_file.IsFileReadable()) {
                // File poof?
                wxLogWarning(_("Model Cache Warning: File '%s' could not be read. Using cached copy."), m_file.GetFullPath().fn_str());
                return m_object;
            }
            if (!m_file.GetTimes(NULL, &lastmod, NULL)) {
                // Huh?
                wxLogWarning(_("Model Cache Warning: Modification time of file '%s' could not be read. Using cached copy."), m_file.GetFullPath().fn_str());
                return m_object;
            }
            if (m_mtime != lastmod) {
                wxString lfile = m_file.GetFullPath();
                c3DLoader* reload = DoLoadFile(lfile.fn_str());
                if (reload) {
                    delete m_object;
                    m_object = reload;
                    if (m_object)
                        m_mtime = lastmod;
                } else {
                    // New file kaput
                    wxLogWarning(_("Model Cache Warning: File '%s' was modified and is now broken. Using cached copy."), m_file.GetFullPath().fn_str());
                }
            }
            return m_object;
        } else {
            // Try again
            m_object = DoLoadFile(m_file.GetFullPath().fn_str());
            if (m_object)
                m_mtime = m_file.GetModificationTime();
            return m_object;
        }
    }
    bool Valid() {
        return m_object;
    }
};

c3DLoaderCache c3DLoader::g_cache;
#endif

bool c3DLoader::FetchObject(unsigned int index, unsigned long *vertexcount, VERTEX **vertices, unsigned long *index_count, unsigned long **indices, D3DVECTOR *bbox_min, D3DVECTOR *bbox_max, const D3DMATRIX *transform, D3DVECTOR *fudge_normal) {
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
        if (fudge_normal) {
            (*vertices)[i].normal = *fudge_normal;
        }
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

bool c3DLoader::FetchAsAnimObject(unsigned int index, unsigned long bone, unsigned long unk, unsigned long *vertexcount, VERTEX2 **vertices, unsigned long *index_count, unsigned short **indices, D3DVECTOR *bbox_min, D3DVECTOR *bbox_max, const D3DMATRIX *transform, D3DVECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;
    *vertexcount = m_meshes[index].m_vertices.size();
    *index_count = m_meshes[index].m_indices.size();
    *vertices = new VERTEX2[*vertexcount];
    D3DMATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < *vertexcount; i++) {
        if (transform)
            (*vertices)[i] = vertex2vertex2(matrixApply(m_meshes[index].m_vertices[i], *transform, normaltransform), bone, unk);
        else
            (*vertices)[i] = vertex2vertex2(m_meshes[index].m_vertices[i], bone, unk);
        if (fudge_normal) {
            (*vertices)[i].normal = *fudge_normal;
        }
        boundsContain(&((*vertices)[i]).position, bbox_min, bbox_max);
    }
    *indices = new unsigned short[*index_count];
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < *index_count; i+=3) {
        (*indices)[i] = m_meshes[index].m_indices[i+((do_mirror)?1:0)];
        (*indices)[i+1] = m_meshes[index].m_indices[i+((do_mirror)?0:1)];
        (*indices)[i+2] = m_meshes[index].m_indices[i+2];
    }
    return true;
}

c3DLoader *c3DLoader::LoadFile(const char *filename) {
#ifdef STD_ONLY
    return DoLoadFile(filename);
#else
    wxString fn = filename;
    c3DLoaderCache::iterator it = g_cache.find(fn);
    if (it != g_cache.end()) {
        c3DLoader *ret = it->second->Get();
        if (ret)
            return new c3DLoader(ret);
        else
            return NULL;
    } else {
        c3DLoaderCacheEntry* nw = new c3DLoaderCacheEntry(fn);
        if (nw->Valid()) {
            g_cache[fn] = nw;
            return new c3DLoader(nw->Get());
        } else {
            delete nw;
            return NULL;
        }
    }
#endif
}

#define FLATTEN_OFFSET 0.05
#define MAKE_OFFSET(a, b) \
    a ## _rim.x = bbox_ ## a.x b FLATTEN_OFFSET; \
    a ## _rim.y = bbox_ ## a.y b FLATTEN_OFFSET; \
    a ## _rim.z = bbox_ ## a.z b FLATTEN_OFFSET;

// Tangens of 30 degrees
#define ANGLE_LIMIT 0.5773502
template <class T>
void flattenNormal(T& n) {
    D3DVECTOR temp = n.normal;
    if (n.normal.x != 0.0) {
        float x_ratio = n.normal.y / n.normal.x;
        if (!((x_ratio > ANGLE_LIMIT) || (x_ratio < -ANGLE_LIMIT))) {
            temp.y = 0.0;
        }
    }
    if (n.normal.y != 0.0) {
        float y_ratio = n.normal.x / n.normal.y;
        if (!((y_ratio > ANGLE_LIMIT) || (y_ratio < -ANGLE_LIMIT))) {
            temp.x = 0.0;
        }
        y_ratio = n.normal.z / n.normal.y;
        if (!((y_ratio > ANGLE_LIMIT) || (y_ratio < -ANGLE_LIMIT))) {
            temp.z = 0.0;
        }
    }
    n.normal = vectorNormalize(temp);
};


int c3DLoader::FlattenNormals(const unsigned long vertexcount, VERTEX *vertices, const D3DVECTOR& bbox_min, const D3DVECTOR& bbox_max) {
    D3DVECTOR min_rim, max_rim;
    MAKE_OFFSET(min, +)
    MAKE_OFFSET(max, -)

    for (int i = 0; i < vertexcount; ++i) {
        if ((vertices[i].position.x < min_rim.x) || (vertices[i].position.x > max_rim.x) || (vertices[i].position.y < min_rim.y) || (vertices[i].position.y > max_rim.y) || (vertices[i].position.z < min_rim.z) || (vertices[i].position.z > max_rim.z)) {
            flattenNormal(vertices[i]);
        }
    }
}

int c3DLoader::FlattenNormals(const unsigned long vertexcount, VERTEX2 *vertices, const D3DVECTOR& bbox_min, const D3DVECTOR& bbox_max) {
    D3DVECTOR min_rim, max_rim;
    MAKE_OFFSET(min, +)
    MAKE_OFFSET(max, -)

    for (int i = 0; i < vertexcount; ++i) {
        if ((vertices[i].position.x < min_rim.x) || (vertices[i].position.x > max_rim.x) || (vertices[i].position.y < min_rim.y) || (vertices[i].position.y > max_rim.y) || (vertices[i].position.z < min_rim.z) || (vertices[i].position.z > max_rim.z)) {
            flattenNormal(vertices[i]);
        }
    }
}

#ifndef STD_ONLY
void c3DLoader::ClearCache() {
    for (c3DLoaderCache::iterator it = g_cache.begin(); it != g_cache.end(); ++it)
        delete it->second;
    g_cache.clear();
}
#endif
