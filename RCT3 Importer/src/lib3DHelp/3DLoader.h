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

#include "3Dstring.h"

#include <boost/shared_ptr.hpp>
#include <wx/filename.h>
#include <wx/hashmap.h>

#include <map>
#include <set>
#include <vector>

#include "pretty.h"

#include "spline.h"
#include "vertex.h"
//#include "counted_ptr.h"

#include "3DLoaderTypes.h"

#define USE_3DLOADER_FILESYSTEM

class cBoneShape2;
class cStaticShape2;

class c3DMesh {
public:
    typedef std::pair<const wxString, c3DMesh> pair;
public:
    std::vector<r3::VERTEX2> m_vertices;
    std::vector<unsigned long> m_indices;
    wxString m_name;
    unsigned long m_flag;
    std::set<wxString> m_bones;
    wxString m_texture;
    wxString m_meshOptions;

    c3DMesh():m_name(wxT("")) {};
};

class c3DBone {
public:
    typedef std::pair<const wxString, c3DBone> pair;
public:
    wxString m_name;
    wxString m_parent;
    wxString m_type;
    r3::MATRIX m_pos[2];

    int m_id;

    //bool m_processed;

    c3DBone(): m_id(-1) {}
};

class c3DAnimBone {
public:
    typedef std::pair<const wxString, c3DAnimBone> pair;
public:
    wxString m_name;
    bool m_axis;
    std::vector<r3::txyz> m_translations;
    std::vector<r3::txyz> m_rotations;

    c3DAnimBone(): m_axis(false) {}
};

class c3DAnimation {
public:
    typedef std::pair<const wxString, c3DAnimation> pair;
public:
    wxString m_name;
    std::map<wxString, c3DAnimBone> m_bones;
};

class c3DGroup {
public:
    typedef std::pair<const wxString, c3DGroup> pair;
public:
    wxString m_name;
    float m_loddistance;
    std::set<wxString> m_meshes;
    std::set<wxString> m_bones;
    std::set<wxString> m_animations;
    bool m_forceanim;

    c3DGroup(): m_loddistance(-1.0), m_forceanim(false) {}
};

class c3DSpline {
public:
    typedef std::pair<const wxString, c3DSpline> pair;
public:
    wxString m_name;
    unsigned long m_cyclic;
    std::vector<r3::SplineNode> m_nodes;

    c3DSpline(): m_cyclic(0) {}
    std::vector<r3::SplineNode> GetFixed(const c3DLoaderOrientation ori) const;
};

class c3DTexture {
public:
    typedef std::pair<const wxString, c3DTexture> pair;
public:
    wxString m_name;
    wxString m_file;
    wxString m_alphafile;
    unsigned long m_recol;
    unsigned long m_alphaType;
    bool m_referenced;

    c3DTexture(): m_recol(0), m_alphaType(0), m_referenced(false) {}
};

class c3DLoader;
class c3DLoaderCacheEntry: public wxObject {
private:
    boost::shared_ptr<c3DLoader> m_object;
    wxFileName m_file;
    wxDateTime m_mtime;
public:
    c3DLoaderCacheEntry(wxString filename);
    boost::shared_ptr<c3DLoader> Get();
    bool Valid() {
        return m_object.get();
    }
};

WX_DECLARE_STRING_HASH_MAP(boost::shared_ptr<c3DLoaderCacheEntry>, c3DLoaderCache);

class E3DLoader: public std::exception {
protected:
    wxString m_message;
public:
    E3DLoader(const wxString& message);
    virtual const char* what() const throw();
    virtual const wxString& wxwhat() const throw();
    ~E3DLoader() throw() {};
};

class E3DLoaderNotMyBeer: public E3DLoader {
public:
    E3DLoaderNotMyBeer(): E3DLoader("Wrong Format") {}
};

class c3DLoader {
friend class c3DLoaderCacheEntry;
protected:
    wxString m_filename;
    wxString m_name;

    // Suggestions for scenery
    wxString m_path;
    bool m_noshadow;

    // Having no meshes signifies an invalid file
/*
    std::vector<c3DMesh> m_meshes;
    std::vector<c3DBone> m_bones;
    std::map<wxString, int> m_bonemap;
*/
    std::map<wxString, c3DMesh> m_meshes;
    std::vector<wxString> m_meshId;
    std::map<wxString, c3DBone> m_bones;
    std::vector<wxString> m_boneId;
    std::map<wxString, c3DAnimation> m_animations;
    std::map<wxString, c3DGroup> m_groups;
    std::map<wxString, c3DSpline> m_splines;
    std::map<wxString, c3DTexture> m_textures;
    // Warnings should only be added by a loader implementation if it
    // recognized the file is of it's type but is otherwise invalid
    wxArrayString m_warnings;
    // no m_meshes and no m_warnings signal that the loader did not recognize the file.
    static c3DLoaderCache g_cache;

    //void splitBones();
    void calculateBonePos1();
    void makeDefaultGroup(const wxString& name = "");
public:
    c3DLoader(const wxChar *filename): m_noshadow(false) {
        m_filename = filename;
        m_name = STRING3D_EMPTY;
    };
    c3DLoader(c3DLoader* original): m_noshadow(false) {
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
    inline const c3DMesh& GetObject(const wxString& mesh) const {
        std::map<wxString, c3DMesh>::const_iterator m = m_meshes.find(mesh);
        if (m != m_meshes.end())
            return m->second;
        else
            throw E3DLoader("Internal Error: c3DLoader::GetObject");
    }
/*
    virtual bool Failed() {
        return ((m_meshes.size()==0) && (m_warnings.size()==0));
    };
*/
    virtual const wxArrayString& GetWarnings() const {return m_warnings;};
    virtual const wxString& GetObjectName(unsigned int index) {
        if (index>=m_meshes.size())
            throw E3DLoader(wxT("c3DLoader::GetObjectName called with illegal index"));
        return m_meshes[m_meshId[index]].m_name;
    };
    virtual int GetObjectVertexCount(unsigned int index) {
        return (index>=m_meshes.size())?0:m_meshes[m_meshId[index]].m_vertices.size();
    };
    virtual int GetObjectIndexCount(unsigned int index) {
        return (index>=m_meshes.size())?0:m_meshes[m_meshId[index]].m_indices.size();
    };
    virtual bool IsObjectValid(unsigned int index) {
        return (index>=m_meshes.size())?false:m_meshes[m_meshId[index]].m_flag;
    };
    virtual r3::VERTEX GetObjectVertex(unsigned int mesh, unsigned int vertex);
    virtual r3::VERTEX2 GetObjectVertex2(unsigned int mesh, unsigned int vertex);
    virtual inline const std::set<wxString>& GetObjectBones(unsigned int mesh) const { return GetObjectBones(m_meshId[mesh]); }
    virtual const std::set<wxString>& GetObjectBones(const wxString& mesh) const;
    virtual int GetBoneCount() const {
        return m_bones.size();
    };
    virtual inline c3DBone& GetBone(unsigned int bone) {
        if (bone >= m_boneId.size())
            throw E3DLoader(_("Bone index out of bounds"));
        return m_bones[m_boneId[bone]];
    }
    virtual inline c3DBone& GetBone(const wxString& bone) {
        if (!has(m_bones, bone))
            throw E3DLoader(wxString::Format(_("Unknown bone '%s'."), bone.c_str()));
        return m_bones[bone];
    }
    virtual inline const std::map<wxString, c3DBone>& GetBones() const {
        return m_bones;
    }
    virtual inline const std::map<wxString, c3DSpline>& GetSplines() const {
        return m_splines;
    }
    virtual inline const std::map<wxString, c3DAnimation>& GetAnimations() const {
        return m_animations;
    }
    virtual inline const std::map<wxString, c3DGroup>& GetGroups() const {
        return m_groups;
    }
    virtual inline const std::map<wxString, c3DTexture>& GetTextures() const {
        return m_textures;
    }

    inline const wxString& getSuggestedPath() const { return m_path; }
    inline bool getSuggestedNoShadow() const { return m_noshadow; }

    virtual bool FetchObject(unsigned int index, unsigned long *vertexcount, r3::VERTEX **vertices, unsigned long *index_count, unsigned long **indices, r3::VECTOR *bbox_min, r3::VECTOR *bbox_max, const r3::MATRIX *transform, r3::VECTOR *fudge_normal = NULL);
    virtual bool FetchAsAnimObject(unsigned int index, char bone, unsigned long *vertexcount, r3::VERTEX2 **vertices, unsigned long *index_count, unsigned short **indices, r3::VECTOR *bbox_min, r3::VECTOR *bbox_max, const r3::MATRIX *transform, r3::VECTOR *fudge_normal = NULL);
    virtual bool FetchObject(unsigned int index, cStaticShape2* sh, r3::VECTOR *bbox_min, r3::VECTOR *bbox_max, const r3::MATRIX *transform, r3::VECTOR *fudge_normal = NULL);
    virtual bool FetchObject(unsigned int index, char bone, cBoneShape2* sh, r3::VECTOR *bbox_min, r3::VECTOR *bbox_max, const r3::MATRIX *transform, r3::VECTOR *fudge_normal = NULL);
    virtual bool FetchObject(unsigned int index, cBoneShape2* sh, r3::VECTOR *bbox_min, r3::VECTOR *bbox_max, const r3::MATRIX *transform, r3::VECTOR *fudge_normal = NULL);

    virtual int GetType() {return C3DLOADER_GENERIC;};
    inline const wxString& GetName() {return m_name;};
    inline const wxString& GetFilename() {return m_filename;};
    virtual c3DLoaderOrientation GetOrientation() {return ORIENTATION_UNKNOWN;};

    static void FlattenNormals(const unsigned long vertexcount, r3::VERTEX *vertices, const r3::VECTOR& bbox_min, const r3::VECTOR& bbox_max);
    static void FlattenNormals(const unsigned long vertexcount, r3::VERTEX2 *vertices, const r3::VECTOR& bbox_min, const r3::VECTOR& bbox_max);
    static void FlattenNormals(cStaticShape2* sh, const r3::VECTOR& bbox_min, const r3::VECTOR& bbox_max);
    static void FlattenNormals(cBoneShape2* sh, const r3::VECTOR& bbox_min, const r3::VECTOR& bbox_max);
    static boost::shared_ptr<c3DLoader> LoadFile(const wxChar *filename);
    static void ClearCache();

    static wxString GetSupportedFilesFilter() {
        return _("Supported 3D Model Files|*.ase;*.ms3d;*.xml;*.modxml|ASE Files (*.ase)|*.ase|Milkshape 3D (*.ms3d)|*.ms3d|Model xml (*.xml, *.modxml)|*.xml;*.modxml");
    }
    static wxString GetSupportedFilesFilterExt() {
        return _("*.ase;*.ms3d;*.xml;*.modxml");
    }
};

#endif // 3DLOADER_H_INCLUDED
