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
#include "XML3DLoader.h"

#include "confhelp.h"
#include "lib3Dconfig.h"
#include "pretty.h"

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
    m_object = boost::shared_ptr<c3DLoader>(DoLoadFile(filename.c_str()));
    m_file = filename;
    if (m_object.get())
        m_mtime = m_file.GetModificationTime();
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
            boost::shared_ptr<c3DLoader> reload(DoLoadFile(lfile.c_str()));
            if (reload) {
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
        m_object.reset(DoLoadFile(m_file.GetFullPath().c_str()));
        if (m_object)
            m_mtime = m_file.GetModificationTime();
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
        c3DMesh& m = m_meshes[m_meshId[mesh]];
        if (vertex<m.m_vertices.size()) {
            res = vertex22vertex(m.m_vertices[vertex]);
        }
    }
    return res;
};

VERTEX2 c3DLoader::GetObjectVertex2(unsigned int mesh, unsigned int vertex) {
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

const set<wxString>& c3DLoader::GetObjectBones(const wxString& mesh) const {
    map<wxString, c3DMesh>::const_iterator it = m_meshes.find(mesh);
    if (it == m_meshes.end())
        throw E3DLoader(wxString::Format(_("Unknown mesh '%s' used (c3DLoader::GetObjectBones)."), mesh.c_str()));
    return it->second.m_bones;
}


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

bool c3DLoader::FetchObject(unsigned int index, cStaticShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;

    c3DMesh& m = m_meshes[m_meshId[index]];
    sh->vertices.resize(m.m_vertices.size());
    sh->indices.resize(m.m_indices.size());
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < m.m_vertices.size(); i++) {
        if (transform)
            sh->vertices[i] = vertex22vertex(matrixApply(m.m_vertices[i], *transform, normaltransform));
        else
            sh->vertices[i] = vertex22vertex(m.m_vertices[i]);
        if (fudge_normal) {
            sh->vertices[i].normal = *fudge_normal;
        }
        boundsContain(&(sh->vertices[i]).position, bbox_min, bbox_max);
    }
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < m.m_indices.size(); i+=3) {
        sh->indices[i] = m.m_indices[i+((do_mirror)?1:0)];
        sh->indices[i+1] = m.m_indices[i+((do_mirror)?0:1)];
        sh->indices[i+2] = m.m_indices[i+2];
    }
    return true;
}

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

bool c3DLoader::FetchObject(unsigned int index, char bone, cBoneShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;

    c3DMesh& m = m_meshes[m_meshId[index]];
    sh->vertices.resize(m.m_vertices.size());
    sh->indices.resize(m.m_indices.size());
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < m.m_vertices.size(); i++) {
        if (transform)
            sh->vertices[i] = vertex2castrate(matrixApply(m.m_vertices[i], *transform, normaltransform), bone);
        else
            sh->vertices[i] = vertex2castrate(m.m_vertices[i], bone);
        if (fudge_normal) {
            sh->vertices[i].normal = *fudge_normal;
        }
        boundsContain(&(sh->vertices[i]).position, bbox_min, bbox_max);
    }
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < m.m_indices.size(); i+=3) {
        sh->indices[i] = m.m_indices[i+((do_mirror)?1:0)];
        sh->indices[i+1] = m.m_indices[i+((do_mirror)?0:1)];
        sh->indices[i+2] = m.m_indices[i+2];
    }
    return true;
}

bool c3DLoader::FetchObject(unsigned int index, cBoneShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal) {
    int i;
    if (m_meshes.size() <= 0)
        return false;

    c3DMesh& m = m_meshes[m_meshId[index]];
    sh->vertices.resize(m.m_vertices.size());
    sh->indices.resize(m.m_indices.size());
    MATRIX normaltransform;
    if (transform)
        normaltransform = matrixNormalTransform(*transform);
    for (i = 0; i < m.m_vertices.size(); i++) {
        if (transform)
            sh->vertices[i] = matrixApply(m.m_vertices[i], *transform, normaltransform);
        else
            sh->vertices[i] = m.m_vertices[i];
        if (fudge_normal) {
            sh->vertices[i].normal = *fudge_normal;
        }
        boundsContain(&(sh->vertices[i]).position, bbox_min, bbox_max);
    }
    bool do_mirror = (transform)?(matrixCalcDeterminant(transform)<0.0):false;
    for (i = 0; i < m.m_indices.size(); i+=3) {
        sh->indices[i] = m.m_indices[i+((do_mirror)?1:0)];
        sh->indices[i+1] = m.m_indices[i+((do_mirror)?0:1)];
        sh->indices[i+2] = m.m_indices[i+2];
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

#if 0
/** @brief splitBones
  *
  * @todo: document this function
  */

#define SPLIT_THRESHOLD 0.00001
struct SplitInfo {
    typedef pair<const wxString, SplitInfo> pair;
    bool x, y, z;
    SplitInfo(): x(false), y(false), z(false) {}
    inline bool operator() () const {
        return ((x?1:0) + (y?1:0) + (z?1:0)) > 1;
    }
};
void c3DLoader::splitBones() {
    if (!READ_RCT3_SPLITBONES())
        return;

    map<wxString, SplitInfo> bones_to_split;
    foreach(c3DAnimation::pair& ani, m_animations) {
        foreach(c3DAnimBone::pair& anibone, ani.second.m_bones) {
            foreach(txyz& fr, anibone.second.m_rotations) {
                if (fabs(fr.X) > SPLIT_THRESHOLD)
                    bones_to_split[anibone.first].x = true;
                if (fabs(fr.Y) > SPLIT_THRESHOLD)
                    bones_to_split[anibone.first].y = true;
                if (fabs(fr.Z) > SPLIT_THRESHOLD)
                    bones_to_split[anibone.first].z = true;
            }
        }
    }
    foreach(SplitInfo::pair& binfo, bones_to_split) {
        if (binfo.second()) {
            c3DBone bone_z, bone_y;
            c3DAnimBone abone_x, abone_y, abone_z;

            c3DBone& bone = GetBone(binfo.first);
            MATRIX m, mbase;
            if (bone.m_parent.IsEmpty()) {
                m = bone.m_pos[1];
                matrixSetUnity(mbase);
            } else {
                m = matrixMultiply(bone.m_pos[1], matrixInverse(GetBone(bone.m_parent).m_pos[1]));
                mbase = GetBone(bone.m_parent).m_pos[1];
            }
            VECTOR tv, rv;
            matrixExtractTranslation(m, tv);
            matrixExtractRotation(m, rv);
            if (binfo.second.z) {
                bone_z.m_name = "z_" + binfo.first;
                bone_z.m_parent = bone.m_parent;
                bone_z.m_type = "Bone";
                bone_z.m_pos[1] = matrixMultiply(matrixMultiply(matrixGetRotationX(rv.z), matrixGetTranslation(tv)), mbase);
            }
            if (binfo.second.y) {
                bone_y.m_name = "y_" + binfo.first;
                bone_y.m_parent = bone_z.m_name.IsEmpty()?bone.m_parent:bone_z.m_name;
                bone_y.m_type = "Bone";
                bone_y.m_pos[1] = matrixMultiply(matrixMultiply(matrixMultiply(matrixGetRotationX(rv.y),matrixGetRotationY(rv.z)), matrixGetTranslation(tv)), mbase);
            }
            if (binfo.second.x) {
                bone.m_parent = bone_y.m_name.IsEmpty()?bone_z.m_name:bone_y.m_name;
                bone.m_pos[1] = matrixMultiply(matrixMultiply(matrixMultiply(matrixMultiply(matrixGetRotationX(rv.x),matrixGetRotationY(rv.y)),matrixGetRotationZ(rv.z)), matrixGetTranslation(tv)), mbase);
                if (binfo.second.z) {
                    m_bones[bone_z.m_name] = bone_z;
                    m_boneId.push_back(bone_z.m_name);
                }
                if (binfo.second.y) {
                    m_bones[bone_y.m_name] = bone_y;
                    m_boneId.push_back(bone_y.m_name);
                }
            } else {
                bone.m_parent = bone_y.m_parent;
                bone.m_pos[1] = bone_y.m_pos[1];
                m_bones[bone_z.m_name] = bone_z;
                m_boneId.push_back(bone_z.m_name);
            }
            foreach (c3DAnimation::pair& ani, m_animations) {
                if (has(ani.second.m_bones, binfo.first)) {
                    c3DAnimBone abone_x, abone_y, abone_z;
                    c3DAnimBone& abone = ani.second.m_bones.find(binfo.first)->second;
                    abone_z.m_name = "z_" + binfo.first;
                    abone_y.m_name = "y_" + binfo.first;
                    foreach(txyz& fr, abone.m_rotations) {
                        if (binfo.second.x)
                            abone_x.m_rotations.push_back(txyzMake(fr.Time, fr.x, 0, 0));
                        if (binfo.second.y)
                            abone_y.m_rotations.push_back(txyzMake(fr.Time, 0, fr.y, 0));
                        if (binfo.second.z)
                            abone_z.m_rotations.push_back(txyzMake(fr.Time, 0, 0, fr.z));
                    }
                    float maxtime = abone.m_rotations[abone.m_rotations.size()-1].Time;
                    if (binfo.second.z) {
                        abone_z.m_translations = abone.m_translations;
                        abone_y.m_translations.push_back(txyzMake(0.0, 0.0, 0.0, 0.0));
                        abone_y.m_translations.push_back(txyzMake(maxtime, 0.0, 0.0, 0.0));
                    } else {
                        abone_y.m_translations = abone.m_translations;
                    }
                    abone.m_translations.clear();
                    abone.m_translations.push_back(txyzMake(0.0, 0.0, 0.0, 0.0));
                    abone.m_translations.push_back(txyzMake(maxtime, 0.0, 0.0, 0.0));
                    if (binfo.second.x) {
                        abone.m_rotations = abone_x.m_rotations;
                        if (binfo.second.z)
                            ani.second.m_bones[abone_z.m_name] = abone_z;
                        if (binfo.second.y)
                            ani.second.m_bones[abone_y.m_name] = abone_y;
                    } else {
                        abone.m_rotations = abone_y.m_rotations;
                        ani.second.m_bones[abone_z.m_name] = abone_z;
                    }
                }
            }
/*
            if (binfo.second.x) {
                bone_x.m_name = "x_" + binfo.first;
                bone_x.m_parent = bone.m_parent;
                bone_x.m_type = "Bone";
                bone_x.m_pos[1] = matrixMultiply(matrixMultiply(matrixGetRotationX(rv.x), matrixGetTranslation(tv)), mbase);
            }
            if (binfo.second.y) {
                bone_y.m_name = "y_" + binfo.first;
                bone_y.m_parent = bone_x.m_name.IsEmpty()?bone.m_parent:bone_x.m_name;
                bone_y.m_type = "Bone";
                bone_y.m_pos[1] = matrixMultiply(matrixMultiply(matrixMultiply(matrixGetRotationX(rv.x),matrixGetRotationY(rv.y)), matrixGetTranslation(tv)), mbase);
            }
            if (binfo.second.z) {
                bone.m_parent = bone_y.m_name.IsEmpty()?bone_x.m_name:bone_y.m_name;
                bone.m_pos[1] = matrixMultiply(matrixMultiply(matrixMultiply(matrixMultiply(matrixGetRotationX(rv.x),matrixGetRotationY(rv.y)),matrixGetRotationZ(rv.z)), matrixGetTranslation(tv)), mbase);
                if (binfo.second.x) {
                    m_bones[bone_x.m_name] = bone_x;
                    m_boneId.push_back(bone_x.m_name);
                }
                if (binfo.second.y) {
                    m_bones[bone_y.m_name] = bone_y;
                    m_boneId.push_back(bone_y.m_name);
                }
            } else {
                bone.m_parent = bone_y.m_parent;
                bone.m_pos[1] = bone_y.m_pos[1];
                m_bones[bone_x.m_name] = bone_x;
                m_boneId.push_back(bone_x.m_name);
            }
            foreach (c3DAnimation::pair& ani, m_animations) {
                if (has(ani.second.m_bones, binfo.first)) {
                    c3DAnimBone abone_x, abone_y, abone_z;
                    c3DAnimBone& abone = ani.second.m_bones.find(binfo.first)->second;
                    abone_x.m_name = "x_" + binfo.first;
                    abone_y.m_name = "y_" + binfo.first;
                    foreach(txyz& fr, abone.m_rotations) {
                        if (binfo.second.x)
                            abone_x.m_rotations.push_back(txyzMake(fr.Time, fr.x, 0, 0));
                        if (binfo.second.y)
                            abone_y.m_rotations.push_back(txyzMake(fr.Time, 0, fr.y, 0));
                        if (binfo.second.z)
                            abone_z.m_rotations.push_back(txyzMake(fr.Time, 0, 0, fr.z));
                    }
                    float maxtime = abone.m_rotations[abone.m_rotations.size()-1].Time;
                    if (binfo.second.x) {
                        abone_x.m_translations = abone.m_translations;
                        abone_y.m_translations.push_back(txyzMake(0.0, 0.0, 0.0, 0.0));
                        abone_y.m_translations.push_back(txyzMake(maxtime, 0.0, 0.0, 0.0));
                    } else {
                        abone_y.m_translations = abone.m_translations;
                    }
                    abone.m_translations.clear();
                    abone.m_translations.push_back(txyzMake(0.0, 0.0, 0.0, 0.0));
                    abone.m_translations.push_back(txyzMake(maxtime, 0.0, 0.0, 0.0));
                    if (binfo.second.z) {
                        abone.m_rotations = abone_z.m_rotations;
                        if (binfo.second.x)
                            ani.second.m_bones[abone_x.m_name] = abone_x;
                        if (binfo.second.y)
                            ani.second.m_bones[abone_y.m_name] = abone_y;
                    } else {
                        abone.m_rotations = abone_y.m_rotations;
                        ani.second.m_bones[abone_x.m_name] = abone_x;
                    }
                }
            }
*/
        }
    }
}
#endif

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
