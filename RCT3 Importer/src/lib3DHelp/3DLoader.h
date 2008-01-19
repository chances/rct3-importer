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

#include <wx/filename.h>
#include <wx/hashmap.h>

#include <vector>

#include "vertex.h"
#include "counted_ptr.h"

#include "3DLoaderTypes.h"

#define USE_3DLOADER_FILESYSTEM

class cBoneShape2;
class cStaticShape2;

class c3DMesh {
public:
    std::vector<VERTEX2> m_vertices;
    std::vector<unsigned long> m_indices;
    wxString m_name;
    unsigned long m_flag;

    c3DMesh():m_name(wxT("")) {};
};

class c3DLoader;
class c3DLoaderCacheEntry: public wxObject {
private:
    counted_ptr<c3DLoader> m_object;
    wxFileName m_file;
    wxDateTime m_mtime;
public:
    c3DLoaderCacheEntry(wxString filename);
    counted_ptr<c3DLoader>& Get();
    bool Valid() {
        return m_object.get();
    }
};

WX_DECLARE_STRING_HASH_MAP(counted_ptr<c3DLoaderCacheEntry>, c3DLoaderCache);

class E3DLoader: public std::exception {
protected:
    wxString m_message;
public:
    E3DLoader(const wxString& message);
    virtual const char* what() const throw();
    virtual const wxString& wxwhat() const throw();
    ~E3DLoader() throw() {};
};

class c3DLoader {
friend class c3DLoaderCacheEntry;
protected:
    wxString m_filename;
    wxString m_name;
    // Having no meshes signifies an invalid file
    std::vector<c3DMesh> m_meshes;
    wxArrayString m_bones;
    // Warnings should only be added by a loader implementation if it
    // recognized the file is of it's type but is otherwise invalid
    wxArrayString m_warnings;
    // no m_meshes and no m_warnings signal that the loader did not recognize the file.
    static c3DLoaderCache g_cache;
public:
    c3DLoader(const wxChar *filename) : m_meshes(0) {
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
    virtual const wxArrayString& GetWarnings() const {return m_warnings;};
    virtual const wxString& GetObjectName(unsigned int index) {
        if (index>=m_meshes.size())
            throw E3DLoader(wxT("c3DLoader::GetObjectName called with illegal index"));
        return m_meshes[index].m_name;
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
    virtual VERTEX GetObjectVertex(unsigned int mesh, unsigned int vertex);
    virtual bool FetchObject(unsigned int index, unsigned long *vertexcount, VERTEX **vertices, unsigned long *index_count, unsigned long **indices, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal = NULL);
    virtual bool FetchAsAnimObject(unsigned int index, char bone, unsigned long *vertexcount, VERTEX2 **vertices, unsigned long *index_count, unsigned short **indices, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal = NULL);
    virtual bool FetchObject(unsigned int index, cStaticShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal = NULL);
    virtual bool FetchObject(unsigned int index, char bone, cBoneShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal = NULL);
    virtual bool FetchObject(unsigned int index, cBoneShape2* sh, VECTOR *bbox_min, VECTOR *bbox_max, const MATRIX *transform, VECTOR *fudge_normal = NULL);

    virtual int GetType() {return C3DLOADER_GENERIC;};
    virtual STRING3D GetName() {return m_name;};
    virtual c3DLoaderOrientation GetOrientation() {return ORIENTATION_UNKNOWN;};

    static void FlattenNormals(const unsigned long vertexcount, VERTEX *vertices, const VECTOR& bbox_min, const VECTOR& bbox_max);
    static void FlattenNormals(const unsigned long vertexcount, VERTEX2 *vertices, const VECTOR& bbox_min, const VECTOR& bbox_max);
    static void FlattenNormals(cStaticShape2* sh, const VECTOR& bbox_min, const VECTOR& bbox_max);
    static void FlattenNormals(cBoneShape2* sh, const VECTOR& bbox_min, const VECTOR& bbox_max);
    static counted_ptr<c3DLoader>& LoadFile(const wxChar *filename);
    static void ClearCache();
};

#endif // 3DLOADER_H_INCLUDED
