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

#include <wx/log.h>

#include "matrix.h"

#include "ManagerBSH.h"
#include "ManagerSHS.h"

#include "ASE3DLoader.h"
#include "MS3DLoader.h"
//#include "AN8Loaderr.h"
#include "XML3DLoader.h"

#include "confhelp.h"
#include "lib3Dconfig.h"
#include "pretty.h"

using namespace pretty;
using namespace r3;
using namespace std;

c3DLoader* DoLoadFile(const wxChar *filename) {
    c3DLoader *res = NULL;

    try {
        res = new cXML3DLoader(filename);
        return res;
    } catch (E3DLoaderNotMyBeer) {
        delete res;
    }

    try {
        res = new cMS3DLoader(filename);
        return res;
    } catch (E3DLoaderNotMyBeer) {
        delete res;
    }

//    res = new cAN8Loader(filename);
//    if (res) {
//        if (!res->Failed())
//            return res;
//        else
//            delete res;
//    }

    try {
        res = new cASE3DLoader(filename);
        return res;
    } catch (E3DLoaderNotMyBeer) {
        delete res;
    }

    return NULL;
}

c3DLoaderCacheEntry::c3DLoaderCacheEntry(wxString filename) {
    //m_object = boost::shared_ptr<c3DLoader>(DoLoadFile(filename.c_str()));
    m_file = filename;
	LoadFile();
    //if (m_object.get())
    //    m_mtime = m_file.GetModificationTime();
}

void c3DLoaderCacheEntry::LoadFile() {
	m_exception.reset();
	m_object.reset();
	if (m_file.IsFileReadable()) {
		m_mtime = m_file.GetModificationTime();
		try {
			m_object.reset(DoLoadFile(m_file.GetFullPath().c_str()));
		} catch (E3DLoader& e) {
			m_exception.reset(new E3DLoader(e));
		}
	} else {
		m_mtime = wxDateTime();
	}
}

boost::shared_ptr<c3DLoader> c3DLoaderCacheEntry::Get() {
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
			try {
				boost::shared_ptr<c3DLoader> reload(DoLoadFile(lfile.c_str()));
				if (reload) {
					m_object = reload;
                    m_mtime = lastmod;
				} else {
					throw E3DLoader(_("File was not recognizead as model file any more")) << wxe_file(lfile);
				}
            } catch (E3DLoader& e) {
                // New file kaput
                wxLogWarning(_("Model Cache Warning: File '%s' was modified and is now broken. Using cached copy.\n\n%s"), m_file.GetFullPath().fn_str(), e.formatLong().c_str());
            }
        }
        return m_object;
    } else {
		// First check if reloading would be for naught
		if (m_exception) {
			if (m_mtime.IsValid()) {
				if (m_file.IsFileReadable()) {
					wxDateTime lastmod;
					if (m_file.GetTimes(NULL, &lastmod, NULL)) {
						if (lastmod == m_mtime) {
							// Noting changed
							throw E3DLoader(*m_exception);
						}
					}
				}
			}
		}
        // Try again
		LoadFile();
		if (m_exception) {
			throw E3DLoader(*m_exception);
		}
        return m_object;
    }
}

vector<SplineNode> c3DSpline::GetFixed(const c3DLoaderOrientation ori) const {
    if (ori == ORIENTATION_LEFT_YUP)
        return m_nodes;

    vector<SplineNode> ret;
    for (vector<SplineNode>::const_iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) {
        SplineNode sp = *it;
        doFixOrientation(sp.pos, ori);
        doFixOrientation(sp.cp1, ori);
        doFixOrientation(sp.cp2, ori);
        ret.push_back(sp);
    }
    return ret;
}

/*
E3DLoader::E3DLoader(const wxString& message) {
    m_message = message;
}

const char* E3DLoader::what() const throw() {
    return m_message.mb_str(wxConvLocal);
}

const wxString& E3DLoader::wxwhat() const throw() {
    return m_message;
}
*/

c3DLoaderCache c3DLoader::g_cache;

VERTEX c3DLoader::getObjectVertex(unsigned int mesh, unsigned int vertex) {
    VERTEX res;
    memset(&res, 0, sizeof(res));
    if (mesh<m_meshes.size()) {
        c3DMesh& m = m_meshes[m_meshId[mesh]];
        if (vertex<m.m_vertices.size()) {
            res = vertex22vertex(m.m_vertices[vertex]);
        }
    }
    return res;
};

VERTEX2 c3DLoader::getObjectVertex2(unsigned int mesh, unsigned int vertex) {
    VERTEX2 res;
    memset(&res, 0, sizeof(res));
    vertex2init(res);
    if (mesh<m_meshes.size()) {
        c3DMesh& m = m_meshes[m_meshId[mesh]];
        if (vertex<m.m_vertices.size()) {
            res = m.m_vertices[vertex];
        }
    }
    return res;
};

const set<wxString>& c3DLoader::getObjectBones(const wxString& mesh) const {
    map<wxString, c3DMesh>::const_iterator it = m_meshes.find(mesh);
    if (it == m_meshes.end())
        throw E3DLoader(wxString::Format(_("Unknown mesh '%s' used (c3DLoader::GetObjectBones)"), mesh.c_str()));
    return it->second.m_bones;
}

/*
bool c3DLoader::FetchObject(unsigned int index, unsigned long *vertexcount, VERTEX **vertices, unsigned long *index_count, unsigned long **indices, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;
    c3DMesh& m = m_meshes[m_meshId[index]];
    *vertexcount = m.m_vertices.size();
    *index_count = m.m_indices.size();
    *vertices = new VERTEX[*vertexcount];
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < *vertexcount; i++) {
        if (transform)
            (*vertices)[i] = vertex22vertex(matrixApply(m.m_vertices[i], *transform, normaltransform));
        else
            (*vertices)[i] = vertex22vertex(m.m_vertices[i]);
        if (fudge_normal) {
            (*vertices)[i].normal = *fudge_normal;
        }
        boundsContain(&((*vertices)[i]).position, bbox_min, bbox_max);
    }
    *indices = new unsigned long[*index_count];
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < *index_count; i+=3) {
        (*indices)[i] = m.m_indices[i+((do_mirror)?1:0)];
        (*indices)[i+1] = m.m_indices[i+((do_mirror)?0:1)];
        (*indices)[i+2] = m.m_indices[i+2];
    }
    return true;
}
*/
bool c3DLoader::fetchObject(unsigned int index, cStaticShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;

    c3DMesh& m = m_meshes[m_meshId[index]];
    int vertex_offset = sh->vertices.size();
    int index_offest = sh->indices.size();
    sh->vertices.resize(vertex_offset + m.m_vertices.size());
    sh->indices.resize(index_offest + m.m_indices.size());
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < static_cast<int>(m.m_vertices.size()); i++) {
        if (transform)
            sh->vertices[vertex_offset + i] = vertex22vertex(matrixApply(m.m_vertices[i], *transform, normaltransform));
        else
            sh->vertices[vertex_offset + i] = vertex22vertex(m.m_vertices[i]);
        if (fudge_normal) {
            sh->vertices[vertex_offset + i].normal = *fudge_normal;
        }
        boundsContain(&(sh->vertices[vertex_offset + i]).position, bbox_min, bbox_max);
    }
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < static_cast<int>(m.m_indices.size()); i+=3) {
        sh->indices[index_offest + i] = vertex_offset + m.m_indices[i+((do_mirror)?1:0)];
        sh->indices[index_offest + i+1] = vertex_offset + m.m_indices[i+((do_mirror)?0:1)];
        sh->indices[index_offest + i+2] = vertex_offset + m.m_indices[i+2];
    }
    return true;
}
/*
bool c3DLoader::FetchAsAnimObject(unsigned int index, char bone, unsigned long *vertexcount, VERTEX2 **vertices, unsigned long *index_count, unsigned short **indices, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;
    c3DMesh& m = m_meshes[m_meshId[index]];
    *vertexcount = m.m_vertices.size();
    *index_count = m.m_indices.size();
    *vertices = new VERTEX2[*vertexcount];
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < *vertexcount; i++) {
        if (transform)
            (*vertices)[i] = vertex2castrate(matrixApply(m.m_vertices[i], *transform, normaltransform), bone);
        else
            (*vertices)[i] = vertex2castrate(m.m_vertices[i], bone);
        if (fudge_normal) {
            (*vertices)[i].normal = *fudge_normal;
        }
        boundsContain(&((*vertices)[i]).position, bbox_min, bbox_max);
    }
    *indices = new unsigned short[*index_count];
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < *index_count; i+=3) {
        (*indices)[i] = m.m_indices[i+((do_mirror)?1:0)];
        (*indices)[i+1] = m.m_indices[i+((do_mirror)?0:1)];
        (*indices)[i+2] = m.m_indices[i+2];
    }
    return true;
}
*/
bool c3DLoader::fetchObject(unsigned int index, char bone, cBoneShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;

    c3DMesh& m = m_meshes[m_meshId[index]];
    int vertex_offset = sh->vertices.size();
    int index_offest = sh->indices.size();
    sh->vertices.resize(vertex_offset + m.m_vertices.size());
    sh->indices.resize(index_offest + m.m_indices.size());
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < static_cast<int>(m.m_vertices.size()); i++) {
        if (transform)
            sh->vertices[vertex_offset + i] = vertex2castrate(matrixApply(m.m_vertices[i], *transform, normaltransform), bone);
        else
            sh->vertices[vertex_offset + i] = vertex2castrate(m.m_vertices[i], bone);
        if (fudge_normal) {
            sh->vertices[vertex_offset + i].normal = *fudge_normal;
        }
        boundsContain(&(sh->vertices[vertex_offset + i]).position, bbox_min, bbox_max);
    }
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < static_cast<int>(m.m_indices.size()); i+=3) {
        sh->indices[index_offest + i] = vertex_offset + m.m_indices[i+((do_mirror)?1:0)];
        sh->indices[index_offest + i+1] = vertex_offset + m.m_indices[i+((do_mirror)?0:1)];
        sh->indices[index_offest + i+2] = vertex_offset + m.m_indices[i+2];
    }
    return true;
}

bool c3DLoader::fetchObject(unsigned int index, cBoneShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;

    c3DMesh& m = m_meshes[m_meshId[index]];
    int vertex_offset = sh->vertices.size();
    int index_offest = sh->indices.size();
    sh->vertices.resize(vertex_offset + m.m_vertices.size());
    sh->indices.resize(index_offest + m.m_indices.size());
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < static_cast<int>(m.m_vertices.size()); i++) {
        if (transform)
            sh->vertices[vertex_offset + i] = matrixApply(m.m_vertices[i], *transform, normaltransform);
        else
            sh->vertices[vertex_offset + i] = m.m_vertices[i];
        if (fudge_normal) {
            sh->vertices[vertex_offset + i].normal = *fudge_normal;
        }
        boundsContain(&(sh->vertices[vertex_offset + i]).position, bbox_min, bbox_max);
    }
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < static_cast<int>(m.m_indices.size()); i+=3) {
        sh->indices[index_offest + i] = vertex_offset + m.m_indices[i+((do_mirror)?1:0)];
        sh->indices[index_offest + i+1] = vertex_offset + m.m_indices[i+((do_mirror)?0:1)];
        sh->indices[index_offest + i+2] = vertex_offset + m.m_indices[i+2];
    }
    return true;
}

/** @brief FetchObject
  *
  * @todo: document this function
  */
bool c3DLoader::fetchObject(unsigned int index, vector<wxString>& bonenames, cBoneShape2* sh, r3::VECTOR *bbox_min, r3::VECTOR *bbox_max, const r3::MATRIX *transform, r3::VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;

    c3DMesh& m = m_meshes[m_meshId[index]];
    int vertex_offset = sh->vertices.size();
    int index_offest = sh->indices.size();
    sh->vertices.resize(vertex_offset + m.m_vertices.size());
    sh->indices.resize(index_offest + m.m_indices.size());
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < static_cast<int>(m.m_vertices.size()); i++) {
        if (transform)
            sh->vertices[vertex_offset + i] = matrixApply(m.m_vertices[i], *transform, normaltransform);
        else
            sh->vertices[vertex_offset + i] = m.m_vertices[i];
        if (fudge_normal) {
            sh->vertices[vertex_offset + i].normal = *fudge_normal;
        }
        for (int b = 0; b < 4; ++b) {
            if (sh->vertices[vertex_offset + i].boneweight[b] > 0) {
                int8_t id = sh->vertices[vertex_offset + i].bone[b];
                if (id == -1) {
                    sh->vertices[vertex_offset + i].bone[b] = 0;
                    continue;
                }
                wxString bn = m_boneId[id];
                vector<wxString>::iterator it = find_in(bonenames, bn);
                if (it != bonenames.end()) {
                    sh->vertices[vertex_offset + i].bone[b] = it - bonenames.begin();
                } else {
                    sh->vertices[vertex_offset + i].bone[b] = 0;
                }
            }
        }
        boundsContain(&(sh->vertices[vertex_offset + i]).position, bbox_min, bbox_max);
    }
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < static_cast<int>(m.m_indices.size()); i+=3) {
        sh->indices[index_offest + i] = vertex_offset + m.m_indices[i+((do_mirror)?1:0)];
        sh->indices[index_offest + i+1] = vertex_offset + m.m_indices[i+((do_mirror)?0:1)];
        sh->indices[index_offest + i+2] = vertex_offset + m.m_indices[i+2];
    }
    return true;
}


boost::shared_ptr<c3DLoader> c3DLoader::LoadFile(const wxChar *filename) {
#ifdef STD_ONLY
    return boost::shared_ptr<c3DLoader>(DoLoadFile(filename));
#else
    wxString fn = filename;
    c3DLoaderCache::iterator it = g_cache.find(fn);
    if (it != g_cache.end()) {
        return it->second->Get();
    } else {
        boost::shared_ptr<c3DLoaderCacheEntry> nw(new c3DLoaderCacheEntry(fn));
        if (nw->Recognized()) {
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

    for (unsigned long i = 0; i < vertexcount; ++i) {
        if ((vertices[i].position.x < min_rim.x) || (vertices[i].position.x > max_rim.x) || (vertices[i].position.y < min_rim.y) || (vertices[i].position.y > max_rim.y) || (vertices[i].position.z < min_rim.z) || (vertices[i].position.z > max_rim.z)) {
            flattenNormal(vertices[i]);
        }
    }
}

void c3DLoader::FlattenNormals(const unsigned long vertexcount, VERTEX2 *vertices, const VECTOR& bbox_min, const VECTOR& bbox_max) {
    VECTOR min_rim, max_rim;
    MAKE_OFFSET(min, +)
    MAKE_OFFSET(max, -)

    for (unsigned long i = 0; i < vertexcount; ++i) {
        if ((vertices[i].position.x < min_rim.x) || (vertices[i].position.x > max_rim.x) || (vertices[i].position.y < min_rim.y) || (vertices[i].position.y > max_rim.y) || (vertices[i].position.z < min_rim.z) || (vertices[i].position.z > max_rim.z)) {
            flattenNormal(vertices[i]);
        }
    }
}

void c3DLoader::FlattenNormals(cStaticShape2* sh, const VECTOR& bbox_min, const VECTOR& bbox_max) {
    VECTOR min_rim, max_rim;
    MAKE_OFFSET(min, +)
    MAKE_OFFSET(max, -)

    for (unsigned long i = 0; i < sh->vertices.size(); ++i) {
        if ((sh->vertices[i].position.x < min_rim.x) || (sh->vertices[i].position.x > max_rim.x) || (sh->vertices[i].position.y < min_rim.y) || (sh->vertices[i].position.y > max_rim.y) || (sh->vertices[i].position.z < min_rim.z) || (sh->vertices[i].position.z > max_rim.z)) {
            flattenNormal(sh->vertices[i]);
        }
    }
}

void c3DLoader::FlattenNormals(cBoneShape2* sh, const VECTOR& bbox_min, const VECTOR& bbox_max) {
    VECTOR min_rim, max_rim;
    MAKE_OFFSET(min, +)
    MAKE_OFFSET(max, -)

    for (unsigned long i = 0; i < sh->vertices.size(); ++i) {
        if ((sh->vertices[i].position.x < min_rim.x) || (sh->vertices[i].position.x > max_rim.x) || (sh->vertices[i].position.y < min_rim.y) || (sh->vertices[i].position.y > max_rim.y) || (sh->vertices[i].position.z < min_rim.z) || (sh->vertices[i].position.z > max_rim.z)) {
            flattenNormal(sh->vertices[i]);
        }
    }
}

void c3DLoader::calculateBonePos1() {
    foreach(c3DBone::pair& bone, m_bones) {
        if (bone.second.m_parent.IsEmpty()) {
            bone.second.m_pos[0] = bone.second.m_pos[1];
        } else {
            bone.second.m_pos[0] = matrixMultiply(bone.second.m_pos[1], matrixInverse(m_bones[bone.second.m_parent].m_pos[1]));
        }
    }

}

/** @brief makeDefaultGroup
  *
  * @todo: document this function
  */
void c3DLoader::makeDefaultGroup(const wxString& name) {
    // only make a default group if there are meshes
    if (m_meshes.size()) {
        c3DGroup gr;
        gr.m_name = name.IsEmpty()?"Default":name;
        foreach(const c3DMesh::pair& me, m_meshes)
            gr.m_meshes.insert(me.first);
        foreach(const c3DBone::pair& bn, m_bones)
            gr.m_bones.insert(bn.first);
        foreach(const c3DAnimation::pair& an, m_animations)
            gr.m_animations.push_back(an.first);
        m_groups[gr.m_name] = gr;
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
