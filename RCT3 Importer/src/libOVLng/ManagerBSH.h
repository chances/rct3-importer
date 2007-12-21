///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for BSH structures
// Copyright (C) 2007 Tobias Minch
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
// Based on libOVL by Jonathan Wilson
//
///////////////////////////////////////////////////////////////////////////////


#ifndef MANAGERBSH_H_INCLUDED
#define MANAGERBSH_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "boneshape.h"
#include "ManagerOVL.h"

using namespace std;

class cBoneStruct {
public:
    string name;
	unsigned long parentbonenumber;
	D3DMATRIX pos1;
	D3DMATRIX pos2;

    cBoneStruct() {
        parentbonenumber = -1;
    }
    cBoneStruct(bool root) {
        parentbonenumber = -1;
        if (root) {
            name = "Scene Root";
            pos1._11  = 1.0;
            pos1._12  = 0.0;
            pos1._13  = 0.0;
            pos1._14  = 0.0;
            pos1._21  = 0.0;
            pos1._22  = 1.0;
            pos1._23  = 0.0;
            pos1._24  = 0.0;
            pos1._31  = 0.0;
            pos1._32  = 0.0;
            pos1._33  = 1.0;
            pos1._34  = 0.0;
            pos1._41  = 0.0;
            pos1._42  = 0.0;
            pos1._43  = 0.0;
            pos1._44  = 1.0;
            pos2 = pos1;
        }
    }
    void Fill(BoneStruct* bs) {
        bs->ParentBoneNumber = parentbonenumber;
    }
    void Fill1(D3DMATRIX* m) {
        *m = pos1;
    }
    void Fill2(D3DMATRIX* m) {
        *m = pos2;
    }
};

class cBoneShape2 {
public:
	string fts; //is 0 in disk files
	string texturestyle; //is 0 in disk files
	unsigned long placetexturing; //0 = dont see texturing on things when being placed, 1 = see texturing on things when being placed
	unsigned long textureflags; //always 0
	unsigned long sides; //seen values of 3 or 1 for this, 3 is more common
	vector<VERTEX2> vertices;
	vector<unsigned short> indices;

	cBoneShape2() {
	    placetexturing = 1;
	    textureflags = 0;
	    sides = 3;
	}
	void Fill(BoneShape2* bs2, unsigned long* vert, unsigned long* ind) {
	    bs2->unk1 = 0xffffffff;
	    bs2->fts = NULL;
	    bs2->TextureData = NULL;
	    bs2->PlaceTexturing = placetexturing;
	    bs2->textureflags = textureflags;
	    bs2->sides = sides;
	    bs2->VertexCount = vertices.size();
	    if (vert)
            *vert += vertices.size();
	    bs2->IndexCount = indices.size();
	    if (ind)
            *ind += indices.size();
	    for(unsigned long i = 0; i < vertices.size(); ++i) {
	        bs2->Vertexes[i] = vertices[i];
	    }
	    for(unsigned long i = 0; i < indices.size(); ++i) {
	        bs2->Triangles[i] = indices[i];
	    }
	}
};

class cBoneShape1 {
public:
    string name;
    D3DVECTOR bbox1;
    D3DVECTOR bbox2;
	vector<cBoneShape2> meshes;
    vector<cBoneStruct> bones;

	cBoneShape1() {};
	void Fill(BoneShape1* bs1) {
	    bs1->BoundingBox1 = bbox1;
	    bs1->BoundingBox2 = bbox2;
	    bs1->TotalVertexCount = 0;
	    bs1->TotalIndexCount = 0;
	    bs1->MeshCount2 = meshes.size();
	    bs1->MeshCount = meshes.size();
	    for(unsigned long i = 0; i < meshes.size(); ++i) {
            meshes[i].Fill(bs1->sh[i], &bs1->TotalVertexCount, &bs1->TotalIndexCount);
	    }
	    bs1->BoneCount = bones.size();
	    for(unsigned long i = 0; i < bones.size(); ++i) {
            bones[i].Fill(&bs1->Bones[i]);
            bones[i].Fill1(&bs1->BonePositions1[i]);
            bones[i].Fill2(&bs1->BonePositions2[i]);
	    }
	}
};

class ovlBSHManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    map<string, cBoneShape1> m_items;
public:
    ovlBSHManager(): ovlOVLManager() {};

    void AddModel(const cBoneShape1& item);

    virtual void Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};

/*
class ovlBSHManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    vector<BoneShape1*> m_modellist;
    vector<string> m_modelnames;
    map<BoneShape2*, string> m_ftxmap;
    map<BoneShape2*, string> m_txsmap;

    BoneShape1* m_cmodel;
    unsigned long m_nmesh;
    long m_meshcount;
    unsigned long m_nbone;
    long m_bonecount;
    bool m_rootadded;
public:
    ovlBSHManager(): ovlOVLManager() {
        m_cmodel = NULL;
        m_nmesh = 0;
        m_nbone = 0;
        m_meshcount = 0;
        m_bonecount = 0;
        m_rootadded = false;
    };
    virtual ~ovlBSHManager();

    void AddModel(const char* name, unsigned long meshes, unsigned long bones);
    void SetBoundingBox(const D3DVECTOR& bbox1, const D3DVECTOR& bbox2);
    void AddBone(const char* name, unsigned long parent, const D3DMATRIX& pos1, const D3DMATRIX& pos2, bool isroot = false);
    void AddRootBone();
    void AddMesh(const char* ftx, const char* txs, unsigned long place, unsigned long flags, unsigned long sides,
                 unsigned long vertexcount, VERTEX2* vertices, unsigned long indexcount, unsigned short* indices);

    virtual unsigned char* Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};
*/

#endif
