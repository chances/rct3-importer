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

#include "ManagerBSH.h"
#include "ManagerSHS.h"

#include "ASE3DLoader.h"
#include "MS3DLoader.h"
//#include "AN8Loaderr.h"

c3DLoader* DoLoadFile(const wxChar *filename) {
    c3DLoader *res;

    res = new cMS3DLoader(filename);
    if (res) {
        if (!res->Failed())
            return res;
        else
            delete res;
    }

//    res = new cAN8Loader(filename);
//    if (res) {
//        if (!res->Failed())
//            return res;
//        else
//            delete res;
//    }

    res = new cASE3DLoader(filename);
    if (res) {
        if (!res->Failed())
            return res;
        else
            delete res;
    }

    return NULL;
}

c3DLoaderCacheEntry::c3DLoaderCacheEntry(wxString filename) {
    m_object = counted_ptr<c3DLoader>(DoLoadFile(filename.fn_str()));
    m_file = filename;
    if (m_object.get())
        m_mtime = m_file.GetModificationTime();
}

counted_ptr<c3DLoader>& c3DLoaderCacheEntry::Get() {
    if (m_object.get()) {
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
            counted_ptr<c3DLoader> reload(DoLoadFile(lfile.fn_str()));
            if (reload.get()) {
                m_object = reload;
                if (m_object.get())
                    m_mtime = lastmod;
            } else {
                // New file kaput
                wxLogWarning(_("Model Cache Warning: File '%s' was modified and is now broken. Using cached copy."), m_file.GetFullPath().fn_str());
            }
        }
        return m_object;
    } else {
        // Try again
        m_object = counted_ptr<c3DLoader>(DoLoadFile(m_file.GetFullPath().fn_str()));
        if (m_object.get())
            m_mtime = m_file.GetModificationTime();
        return m_object;
    }
}

E3DLoader::E3DLoader(const wxString& message) {
    m_message = message;
}

const char* E3DLoader::what() const throw() {
    return m_message.mb_str(wxConvLocal);
}

const wxString& E3DLoader::wxwhat() const throw() {
    return m_message;
}


c3DLoaderCache c3DLoader::g_cache;

VERTEX c3DLoader::GetObjectVertex(unsigned int mesh, unsigned int vertex) {
    VERTEX res;
    memset(&res, 0, sizeof(res));
    if (mesh<m_meshes.size()) {
        if (vertex<m_meshes[mesh].m_vertices.size()) {
            res = vertex22vertex(m_meshes[mesh].m_vertices[vertex]);
        }
    }
    return res;
};

bool c3DLoader::FetchObject(unsigned int index, unsigned long *vertexcount, VERTEX **vertices, unsigned long *index_count, unsigned long **indices, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;
    *vertexcount = m_meshes[index].m_vertices.size();
    *index_count = m_meshes[index].m_indices.size();
    *vertices = new VERTEX[*vertexcount];
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < *vertexcount; i++) {
        if (transform)
            (*vertices)[i] = vertex22vertex(matrixApply(m_meshes[index].m_vertices[i], *transform, normaltransform));
        else
            (*vertices)[i] = vertex22vertex(m_meshes[index].m_vertices[i]);
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

bool c3DLoader::FetchObject(unsigned int index, cStaticShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;

    sh->vertices.resize(m_meshes[index].m_vertices.size());
    sh->indices.resize(m_meshes[index].m_indices.size());
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < m_meshes[index].m_vertices.size(); i++) {
        if (transform)
            sh->vertices[i] = vertex22vertex(matrixApply(m_meshes[index].m_vertices[i], *transform, normaltransform));
        else
            sh->vertices[i] = vertex22vertex(m_meshes[index].m_vertices[i]);
        if (fudge_normal) {
            sh->vertices[i].normal = *fudge_normal;
        }
        boundsContain(&(sh->vertices[i]).position, bbox_min, bbox_max);
    }
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < m_meshes[index].m_indices.size(); i+=3) {
        sh->indices[i] = m_meshes[index].m_indices[i+((do_mirror)?1:0)];
        sh->indices[i+1] = m_meshes[index].m_indices[i+((do_mirror)?0:1)];
        sh->indices[i+2] = m_meshes[index].m_indices[i+2];
    }
    return true;
}

bool c3DLoader::FetchAsAnimObject(unsigned int index, char bone, unsigned long *vertexcount, VERTEX2 **vertices, unsigned long *index_count, unsigned short **indices, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;
    *vertexcount = m_meshes[index].m_vertices.size();
    *index_count = m_meshes[index].m_indices.size();
    *vertices = new VERTEX2[*vertexcount];
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < *vertexcount; i++) {
        if (transform)
            (*vertices)[i] = vertex2castrate(matrixApply(m_meshes[index].m_vertices[i], *transform, normaltransform), bone);
        else
            (*vertices)[i] = vertex2castrate(m_meshes[index].m_vertices[i], bone);
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

bool c3DLoader::FetchObject(unsigned int index, char bone, cBoneShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;

    sh->vertices.resize(m_meshes[index].m_vertices.size());
    sh->indices.resize(m_meshes[index].m_indices.size());
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < m_meshes[index].m_vertices.size(); i++) {
        if (transform)
            sh->vertices[i] = vertex2castrate(matrixApply(m_meshes[index].m_vertices[i], *transform, normaltransform), bone);
        else
            sh->vertices[i] = vertex2castrate(m_meshes[index].m_vertices[i], bone);
        if (fudge_normal) {
            sh->vertices[i].normal = *fudge_normal;
        }
        boundsContain(&(sh->vertices[i]).position, bbox_min, bbox_max);
    }
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < m_meshes[index].m_indices.size(); i+=3) {
        sh->indices[i] = m_meshes[index].m_indices[i+((do_mirror)?1:0)];
        sh->indices[i+1] = m_meshes[index].m_indices[i+((do_mirror)?0:1)];
        sh->indices[i+2] = m_meshes[index].m_indices[i+2];
    }
    return true;
}

bool c3DLoader::FetchObject(unsigned int index, cBoneShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;

    sh->vertices.resize(m_meshes[index].m_vertices.size());
    sh->indices.resize(m_meshes[index].m_indices.size());
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < m_meshes[index].m_vertices.size(); i++) {
        if (transform)
            sh->vertices[i] = matrixApply(m_meshes[index].m_vertices[i], *transform, normaltransform);
        else
            sh->vertices[i] = m_meshes[index].m_vertices[i];
        if (fudge_normal) {
            sh->vertices[i].normal = *fudge_normal;
        }
        boundsContain(&(sh->vertices[i]).position, bbox_min, bbox_max);
    }
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < m_meshes[index].m_indices.size(); i+=3) {
        sh->indices[i] = m_meshes[index].m_indices[i+((do_mirror)?1:0)];
        sh->indices[i+1] = m_meshes[index].m_indices[i+((do_mirror)?0:1)];
        sh->indices[i+2] = m_meshes[index].m_indices[i+2];
    }
    return true;
}

counted_ptr<c3DLoader>& c3DLoader::LoadFile(const wxChar *filename) {
#ifdef STD_ONLY
    return DoLoadFile(filename);
#else
    wxString fn = filename;
    c3DLoaderCache::iterator it = g_cache.find(fn);
    if (it != g_cache.end()) {
        return it->second->Get();
    } else {
        counted_ptr<c3DLoaderCacheEntry> nw(new c3DLoaderCacheEntry(fn));
        if (nw->Valid()) {
            g_cache[fn] = nw;
            return nw->Get();
        } else {
            return nw->Get();
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
    VECTOR temp = n.normal;
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


void c3DLoader::FlattenNormals(const unsigned long vertexcount, VERTEX *vertices, const VECTOR& bbox_min, const VECTOR& bbox_max) {
    VECTOR min_rim, max_rim;
    MAKE_OFFSET(min, +)
    MAKE_OFFSET(max, -)

    for (int i = 0; i < vertexcount; ++i) {
        if ((vertices[i].position.x < min_rim.x) || (vertices[i].position.x > max_rim.x) || (vertices[i].position.y < min_rim.y) || (vertices[i].position.y > max_rim.y) || (vertices[i].position.z < min_rim.z) || (vertices[i].position.z > max_rim.z)) {
            flattenNormal(vertices[i]);
        }
    }
}

void c3DLoader::FlattenNormals(const unsigned long vertexcount, VERTEX2 *vertices, const VECTOR& bbox_min, const VECTOR& bbox_max) {
    VECTOR min_rim, max_rim;
    MAKE_OFFSET(min, +)
    MAKE_OFFSET(max, -)

    for (int i = 0; i < vertexcount; ++i) {
        if ((vertices[i].position.x < min_rim.x) || (vertices[i].position.x > max_rim.x) || (vertices[i].position.y < min_rim.y) || (vertices[i].position.y > max_rim.y) || (vertices[i].position.z < min_rim.z) || (vertices[i].position.z > max_rim.z)) {
            flattenNormal(vertices[i]);
        }
    }
}

void c3DLoader::FlattenNormals(cStaticShape2* sh, const VECTOR& bbox_min, const VECTOR& bbox_max) {
    VECTOR min_rim, max_rim;
    MAKE_OFFSET(min, +)
    MAKE_OFFSET(max, -)

    for (int i = 0; i < sh->vertices.size(); ++i) {
        if ((sh->vertices[i].position.x < min_rim.x) || (sh->vertices[i].position.x > max_rim.x) || (sh->vertices[i].position.y < min_rim.y) || (sh->vertices[i].position.y > max_rim.y) || (sh->vertices[i].position.z < min_rim.z) || (sh->vertices[i].position.z > max_rim.z)) {
            flattenNormal(sh->vertices[i]);
        }
    }
}

void c3DLoader::FlattenNormals(cBoneShape2* sh, const VECTOR& bbox_min, const VECTOR& bbox_max) {
    VECTOR min_rim, max_rim;
    MAKE_OFFSET(min, +)
    MAKE_OFFSET(max, -)

    for (int i = 0; i < sh->vertices.size(); ++i) {
        if ((sh->vertices[i].position.x < min_rim.x) || (sh->vertices[i].position.x > max_rim.x) || (sh->vertices[i].position.y < min_rim.y) || (sh->vertices[i].position.y > max_rim.y) || (sh->vertices[i].position.z < min_rim.z) || (sh->vertices[i].position.z > max_rim.z)) {
            flattenNormal(sh->vertices[i]);
        }
    }
}

#ifndef STD_ONLY
void c3DLoader::ClearCache() {
    /*
    for (c3DLoaderCache::iterator it = g_cache.begin(); it != g_cache.end(); ++it)
        delete it->second;
    */
    g_cache.clear();
}
#endif
