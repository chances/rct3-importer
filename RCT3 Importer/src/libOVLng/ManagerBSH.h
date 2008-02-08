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
#include "ManagerCommon.h"
#include "ManagerOVL.h"

using namespace std;

class cBoneStruct {
public:
    string name;
	unsigned long parentbonenumber;
	MATRIX pos1;
	MATRIX pos2;

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
    void Fill1(MATRIX* m) {
        *m = pos1;
    }
    void Fill2(MATRIX* m) {
        *m = pos2;
    }
};

class cBoneShape2 {
public:
    long support;
	string fts; //is 0 in disk files
	string texturestyle; //is 0 in disk files
	unsigned long placetexturing; //0 = dont see texturing on things when being placed, 1 = see texturing on things when being placed
	unsigned long textureflags; //always 0
	unsigned long sides; //seen values of 3 or 1 for this, 3 is more common
	vector<VERTEX2> vertices;
	vector<unsigned short> indices;

#ifndef GLASS_OLD
	cTriangleSortAlgorithm::Algorithm algo_x;
	cTriangleSortAlgorithm::Algorithm algo_y;
	cTriangleSortAlgorithm::Algorithm algo_z;
#endif

	cBoneShape2() {
	    placetexturing = 0;
	    textureflags = 0;
	    sides = 3;
	    support = SharedShape::Supports::None;
#ifndef GLASS_OLD
	    algo_x = cTriangleSortAlgorithm::DEFAULT;
	    algo_y = cTriangleSortAlgorithm::DEFAULT;
	    algo_z = cTriangleSortAlgorithm::DEFAULT;
#endif
	}
	void Fill(BoneShape2* bs2, unsigned long* vert, unsigned long* ind);
	bool IsAlgoNone() const {
	    return ((algo_x == cTriangleSortAlgorithm::NONE) || (algo_y == cTriangleSortAlgorithm::NONE) || (algo_z == cTriangleSortAlgorithm::NONE));
	}
};

class cBoneShape1 {
public:
    string name;
    VECTOR bbox1;
    VECTOR bbox2;
	vector<cBoneShape2> meshes;
    vector<cBoneStruct> bones;

	cBoneShape1() {};
	void Fill(BoneShape1* bs1) {
	    bs1->BoundingBox1 = bbox1;
	    bs1->BoundingBox2 = bbox2;
	    bs1->TotalVertexCount = 0;
	    bs1->TotalIndexCount = 0;
	    bs1->MeshCount2 = 0;
	    bs1->MeshCount = meshes.size();
	    for(unsigned long i = 0; i < meshes.size(); ++i) {
	        if (meshes[i].support == SharedShape::Supports::None)
                bs1->MeshCount2++;
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
