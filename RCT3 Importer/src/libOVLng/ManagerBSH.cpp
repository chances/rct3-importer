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

#include "OVLDebug.h"

#include "ManagerBSH.h"

#include "ManagerFTX.h"
#include "OVLException.h"

const char* ovlBSHManager::NAME = "BoneShape";
const char* ovlBSHManager::TAG = "bsh";

struct cVertex2Sorter {
    bool operator() (const VERTEX2& lhs, const VERTEX2& rhs) const {
        if (lhs.position.x == rhs.position.x)
            return lhs.position.z > rhs.position.z;
        else
            return lhs.position.x > rhs.position.x;
    }
};

struct cVertex2Comparator {
    VERTEX2* c;
    cVertex2Comparator(VERTEX2* t): c(t) {}
    bool operator() (const VERTEX2& t) const {
        return memcmp(c, &t, sizeof(VERTEX2)) == 0;
    }
};

struct cTriangle2Sorter {
    vector<VERTEX2>* verts;
    cTriangleSortAlgorithm algo;
    cTriangle2Sorter(vector<VERTEX2>* v, const cTriangleSortAlgorithm& a): verts(v), algo(a) {}
    bool operator() (const Triangle2& lhs, const Triangle2& rhs) const {
        return algo((*verts)[lhs.a].position, (*verts)[lhs.b].position, (*verts)[lhs.c].position,
                    (*verts)[rhs.a].position, (*verts)[rhs.b].position, (*verts)[rhs.c].position);
    }
};


void cBoneShape2::Fill(BoneShape2* bs2, unsigned long* vert, unsigned long* ind) {
    bs2->unk1 = 0xffffffff;
    bs2->fts = NULL;
    bs2->TextureData = NULL;
    bs2->PlaceTexturing = placetexturing;
    bs2->textureflags = textureflags;
    bs2->sides = sides;
    bs2->VertexCount = vertices.size();
    if (vert)
        *vert += vertices.size();
#ifndef GLASS_OLD
    if ((algo_x == cTriangleSortAlgorithm::NONE) || (algo_y == cTriangleSortAlgorithm::NONE) || (algo_z == cTriangleSortAlgorithm::NONE)) {
        algo_x = cTriangleSortAlgorithm::NONE;
        algo_y = cTriangleSortAlgorithm::NONE;
        algo_z = cTriangleSortAlgorithm::NONE;
    }

    if ((algo_x == cTriangleSortAlgorithm::NONE) && (placetexturing != 0))
        bs2->IndexCount = indices.size() / 3;
    else
#endif
    bs2->IndexCount = indices.size();
    if (ind)
        *ind += indices.size();
#ifndef GLASS_OLD
    if ((placetexturing != 0) && (algo_x != cTriangleSortAlgorithm::NONE)) {
        // Sort vertices
        vector<VERTEX2> sortvertex = vertices;
        sort(sortvertex.begin(), sortvertex.end(), cVertex2Sorter());

        // Fix indices
        for (vector<unsigned long>::size_type i = 0; i < indices.size(); ++i) {
            vector<VERTEX2>::iterator newind = find_if(sortvertex.begin(), sortvertex.end(), cVertex2Comparator(&vertices[indices[i]]));
            indices[i] = newind - sortvertex.begin();
        }

        // Write back sorted
        vertices = sortvertex;
    }
#endif
    for(unsigned long i = 0; i < vertices.size(); ++i) {
        bs2->Vertexes[i] = vertices[i];
    }
#ifndef GLASS_OLD
    if ((placetexturing != 0) && (algo_x != cTriangleSortAlgorithm::NONE)) {
        vector<Triangle2> temptri(indices.size()/3);
        memcpy(&temptri[0], &indices[0], indices.size() * sizeof(unsigned short));
        sort(temptri.begin(), temptri.end(), cTriangle2Sorter(&vertices, cTriangleSortAlgorithm(algo_x, cTriangleSortAlgorithm::BY_X)));
        for(unsigned long i = 0; i < temptri.size(); ++i) {
            bs2->Triangles[i*3] = temptri[i].a;
            bs2->Triangles[(i*3)+1] = temptri[i].b;
            bs2->Triangles[(i*3)+2] = temptri[i].c;
        }
        sort(temptri.begin(), temptri.end(), cTriangle2Sorter(&vertices, cTriangleSortAlgorithm(algo_y, cTriangleSortAlgorithm::BY_Y)));
        for(unsigned long i = 0; i < temptri.size(); ++i) {
            bs2->Triangles[(i*3)+indices.size()] = temptri[i].a;
            bs2->Triangles[(i*3)+indices.size()+1] = temptri[i].b;
            bs2->Triangles[(i*3)+indices.size()+2] = temptri[i].c;
        }
        sort(temptri.begin(), temptri.end(), cTriangle2Sorter(&vertices, cTriangleSortAlgorithm(algo_z, cTriangleSortAlgorithm::BY_Z)));
        for(unsigned long i = 0; i < temptri.size(); ++i) {
            bs2->Triangles[(i*3)+(indices.size()*2)] = temptri[i].a;
            bs2->Triangles[(i*3)+(indices.size()*2)+1] = temptri[i].b;
            bs2->Triangles[(i*3)+(indices.size()*2)+2] = temptri[i].c;
        }
    } else {
        for(unsigned long i = 0; i < indices.size(); ++i) {
            bs2->Triangles[i] = indices[i];
        }
    }
#else
    for(unsigned long i = 0; i < indices.size(); ++i) {
        bs2->Triangles[i] = indices[i];
    }
#endif
}

void ovlBSHManager::AddModel(const cBoneShape1& item) {
    DUMP_LOG("Trace: ovlBSHManager::Make(%s)", UNISTR(item.name.c_str()));
    Check("ovlBSHManager::AddModel");
    if (item.name == "")
        throw EOvl("ovlBSHManager::AddModel called without name");
    if (m_items.find(item.name) != m_items.end())
        throw EOvl("ovlBSHManager::AddModel: Item with name '"+item.name+"' already exists");

    m_items[item.name] = item;

    // One common blob per model
    m_blobs[item.name] = cOvlMemBlob(OVLT_COMMON, 4, 0);

    // BoneShape1
    m_size += sizeof(BoneShape1);
    // BoneShape2 pointers
    m_size += item.meshes.size() * sizeof(BoneShape2*);

    // Put in common
    // BoneStructs
    //m_size += c_bsh->BoneCount * sizeof(BoneStruct);
    m_blobs[item.name].size += item.bones.size() * sizeof(BoneStruct);
    // Bone positions
    //m_size += 2 * c_bsh->BoneCount * sizeof(D3DMATRIX);
    m_blobs[item.name].size += 2 * item.bones.size() * sizeof(MATRIX);

    // Meshes
    for (vector<cBoneShape2>::const_iterator it = item.meshes.begin(); it != item.meshes.end(); ++it) {
        // BoneShape2
        m_size += sizeof(BoneShape2);
        // Vertices
        m_blobs[item.name].size += it->vertices.size() * sizeof(VERTEX2);
        // Indices
        m_blobs[item.name].size += it->indices.size() * sizeof(unsigned short);
#ifndef GLASS_OLD
        if ((it->IsAlgoNone()) && (it->placetexturing != 0)) {
            if (it->indices.size() % 3)
                throw EOvl("ovlSHSManager::AddModel: mesh has triangle sort algorithm none but indices aren't divisible by 3.");
        }
        if ((it->placetexturing != 0) && (!it->IsAlgoNone())) {
            m_blobs[item.name].size += 2 * it->indices.size() * sizeof(unsigned short);
        }
#endif

        // Symbol Refs
        GetStringTable()->AddSymbolString(it->fts.c_str(), ovlFTXManager::TAG);
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(it->texturestyle.c_str(), ovlTXSManager::TAG);
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    }

    // Bone names
    for (vector<cBoneStruct>::const_iterator it = item.bones.begin(); it != item.bones.end(); ++it) {
        GetStringTable()->AddString(it->name.c_str());
    }

    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlBSHManager::TAG);

}

void ovlBSHManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlBSHManager::Make()");
    Check("ovlBSHManager::Make");

    m_blobs[""] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[""].data;

    for (map<string, cBoneShape1>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        unsigned char* c_commondata = m_blobs[it->first].data;

        BoneShape1* c_model = reinterpret_cast<BoneShape1*>(c_data);
        c_data += sizeof(BoneShape1);

        // Assign space for BoneShape2 pointers
        c_model->sh = reinterpret_cast<BoneShape2**>(c_data);
        c_data += it->second.meshes.size() * 4;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh));
        DUMP_RELOCATION("ovlBSHManager::Make, BoneShape2 pointers", c_model->sh);

        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), Tag()), reinterpret_cast<unsigned long*>(c_model));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_model), false, c_symbol);

        unsigned long s = 0;
        for (vector<cBoneShape2>::iterator itb = it->second.meshes.begin(); itb != it->second.meshes.end(); ++itb) {
            c_model->sh[s] = reinterpret_cast<BoneShape2*>(c_data);
            c_data += sizeof(BoneShape2);
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh[s]));
            DUMP_RELOCATION("ovlBSHManager::Make, BoneShape2", c_model->sh[s]);

            c_model->sh[s]->Vertexes = reinterpret_cast<VERTEX2*>(c_commondata);
            c_commondata += itb->vertices.size() * sizeof(VERTEX2);
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh[s]->Vertexes));
            DUMP_RELOCATION("ovlBSHManager::Make, Vertexes", c_model->sh[s]->Vertexes);

            c_model->sh[s]->Triangles = reinterpret_cast<unsigned short*>(c_commondata);
            c_commondata += itb->indices.size() * sizeof(unsigned short);
#ifndef GLASS_OLD
            if ((itb->placetexturing != 0) && (!itb->IsAlgoNone())) {
                c_commondata += 2 * itb->indices.size() * sizeof(unsigned short);
            }
#endif
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh[s]->Triangles));
            DUMP_RELOCATION("ovlBSHManager::Make, Triangles", c_model->sh[s]->Triangles);

            // Symbol references
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(itb->fts.c_str(), ovlFTXManager::TAG), reinterpret_cast<unsigned long*>(&c_model->sh[s]->fts));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(itb->texturestyle.c_str(), ovlTXSManager::TAG), reinterpret_cast<unsigned long*>(&c_model->sh[s]->TextureData));

            s++;
        }

        c_model->Bones = reinterpret_cast<BoneStruct*>(c_commondata);
        c_commondata += it->second.bones.size() * sizeof(BoneStruct);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->Bones));
        DUMP_RELOCATION("ovlBSHManager::Make, Bones", c_model->Bones);

        c_model->BonePositions1 = reinterpret_cast<MATRIX*>(c_commondata);
        c_commondata += it->second.bones.size() * sizeof(MATRIX);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->BonePositions1));
        DUMP_RELOCATION("ovlBSHManager::Make, BonePositions1", c_model->BonePositions1);

        c_model->BonePositions2 = reinterpret_cast<MATRIX*>(c_commondata);
        c_commondata += it->second.bones.size() * sizeof(MATRIX);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->BonePositions2));
        DUMP_RELOCATION("ovlBSHManager::Make, BonePositions2", c_model->BonePositions2);

        s = 0;
        for (vector<cBoneStruct>::iterator itb = it->second.bones.begin(); itb != it->second.bones.end(); ++itb) {
            c_model->Bones[s].BoneName = GetStringTable()->FindString(itb->name.c_str());
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->Bones[s].BoneName));
            DUMP_RELOCATION("ovlBSHManager::Make, BoneName", c_model->Bones[s].BoneName);
            s++;
        }

        it->second.Fill(c_model);
        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }

}

/*
ovlBSHManager::~ovlBSHManager() {
    for (unsigned long i = 0; i < m_modellist.size(); ++i) {
        for (unsigned long s = 0; s < m_modellist[i]->MeshCount; ++s) {
            delete[] m_modellist[i]->sh[s]->Vertexes;
            delete[] m_modellist[i]->sh[s]->Triangles;
            delete m_modellist[i]->sh[s];
        }
        delete[] m_modellist[i]->sh;
        delete[] m_modellist[i]->BonePositions1;
        delete[] m_modellist[i]->BonePositions2;
        for (unsigned long s = 0; s < m_modellist[i]->BoneCount; ++s) {
            delete[] m_modellist[i]->Bones[s].BoneName;
        }
        delete[] m_modellist[i]->Bones;
        delete m_modellist[i];
    }
}

void ovlBSHManager::AddModel(const char* name, unsigned long meshes, unsigned long bones) {
    DUMP_LOG("Trace: ovlBSHManager::AddModel(name:'%s', meshes:%ld, bones:%ld)", name, meshes, bones);
    Check("ovlBSHManager::AddModel");
    if (m_meshcount)
        throw EOvl("ovlBSHManager::AddModel called but last model misses meshes");
    if (m_bonecount)
        throw EOvl("ovlBSHManager::AddModel called but last model misses bones");
    if (!bones)
        throw EOvl("ovlBSHManager::AddModel called with zero bones. At least one bone is necessary for the scene root.");

    BoneShape1* c_bsh = new BoneShape1;
    c_bsh->TotalIndexCount = 0;
    c_bsh->TotalVertexCount = 0;
    c_bsh->MeshCount2 = meshes;
    c_bsh->MeshCount = meshes;
    c_bsh->sh = new BoneShape2*[c_bsh->MeshCount];
    c_bsh->BoneCount = bones;
    c_bsh->Bones = new BoneStruct[c_bsh->BoneCount];
    c_bsh->BonePositions1 = new D3DMATRIX[c_bsh->BoneCount];
    c_bsh->BonePositions2 = new D3DMATRIX[c_bsh->BoneCount];

    // One common blob per model
    m_blobs[reinterpret_cast<unsigned char*>(c_bsh)] = cOvlMemBlob(OVLT_COMMON, 4, 0);

    // BoneShape1
    m_size += sizeof(BoneShape1);
    // BoneShape2 pointers
    m_size += c_bsh->MeshCount * sizeof(BoneShape2*);

    // Put in common
    // BoneStructs
    //m_size += c_bsh->BoneCount * sizeof(BoneStruct);
    m_blobs[reinterpret_cast<unsigned char*>(c_bsh)].size += c_bsh->BoneCount * sizeof(BoneStruct);
    // Bone positions
    //m_size += 2 * c_bsh->BoneCount * sizeof(D3DMATRIX);
    m_blobs[reinterpret_cast<unsigned char*>(c_bsh)].size += 2 * c_bsh->BoneCount * sizeof(D3DMATRIX);

    m_modellist.push_back(c_bsh);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    m_modelnames.push_back(string(name));
    GetStringTable()->AddSymbolString(name, Tag());

    m_cmodel = c_bsh;
    m_nmesh = 0;
    m_meshcount = c_bsh->MeshCount;
    m_nbone = 0;
    m_bonecount = c_bsh->BoneCount;
    m_rootadded = false;
}

void ovlBSHManager::SetBoundingBox(const D3DVECTOR& bbox1, const D3DVECTOR& bbox2) {
    DUMP_LOG("Trace: ovlBSHManager::SetBoundingBox(min:<%f/%f/%f>, max:<%f/%f/%f>)", bbox1.x, bbox1.y, bbox1.z, bbox2.x, bbox2.y, bbox2.z);
    Check("ovlBSHManager::SetBoundingBox");
    if (!m_cmodel)
        throw EOvl("ovlSHSManager::SetBoundingBox called but no model pointer");

    m_cmodel->BoundingBox1 = bbox1;
    m_cmodel->BoundingBox2 = bbox2;
}

void ovlBSHManager::AddBone(const char* name, unsigned long parent, const D3DMATRIX& pos1, const D3DMATRIX& pos2, bool isroot) {
    DUMP_LOG("Trace: ovlBSHManager::AddBone(name:'%s', parent:%ld, (pos1), (pos2), isroot: %d)", name, parent, isroot);
    Check("ovlBSHManager::AddBone");
    if ((!m_rootadded) && (!isroot))
        throw EOvl("ovlBSHManager::AddBone called before adding root bone");
    if (!m_bonecount)
        throw EOvl("ovlBSHManager::AddBone called but no bones left");
    if (!m_cmodel)
        throw EOvl("ovlBSHManager::AddBone called but no model pointer");

    m_cmodel->Bones[m_nbone].BoneName = new char[strlen(name)+1];
    strcpy(m_cmodel->Bones[m_nbone].BoneName, name);
    m_cmodel->Bones[m_nbone].ParentBoneNumber = parent;
    m_cmodel->BonePositions1[m_nbone] = pos1;
    m_cmodel->BonePositions2[m_nbone] = pos2;

    // Bone names are assigned on the stringtable
    GetStringTable()->AddString(name);

    m_nbone++;
    m_bonecount--;
    if (isroot)
        m_rootadded = true;
}

void ovlBSHManager::AddRootBone() {
    DUMP_LOG("Trace: ovlBSHManager::AddRootBone()");
    Check("ovlBSHManager::AddRootBone");
    if (m_rootadded)
        throw EOvl("ovlBSHManager::AddRootBone called after adding root bone");
    if (!m_bonecount)
        throw EOvl("ovlBSHManager::AddRootBone called but no bones left");
    if (!m_cmodel)
        throw EOvl("ovlBSHManager::AddRootBone called but no model pointer");

    m_cmodel->Bones[0].BoneName = new char[strlen("Scene Root")+1];
    strcpy(m_cmodel->Bones[0].BoneName, "Scene Root");
    m_cmodel->Bones[0].ParentBoneNumber = -1;
    m_cmodel->BonePositions1[0]._11  = 1.0;
    m_cmodel->BonePositions1[0]._12  = 0.0;
    m_cmodel->BonePositions1[0]._13  = 0.0;
    m_cmodel->BonePositions1[0]._14  = 0.0;
    m_cmodel->BonePositions1[0]._21  = 0.0;
    m_cmodel->BonePositions1[0]._22  = 1.0;
    m_cmodel->BonePositions1[0]._23  = 0.0;
    m_cmodel->BonePositions1[0]._24  = 0.0;
    m_cmodel->BonePositions1[0]._31  = 0.0;
    m_cmodel->BonePositions1[0]._32  = 0.0;
    m_cmodel->BonePositions1[0]._33  = 1.0;
    m_cmodel->BonePositions1[0]._34  = 0.0;
    m_cmodel->BonePositions1[0]._41  = 0.0;
    m_cmodel->BonePositions1[0]._42  = 0.0;
    m_cmodel->BonePositions1[0]._43  = 0.0;
    m_cmodel->BonePositions1[0]._44  = 1.0;
    m_cmodel->BonePositions2[0] = m_cmodel->BonePositions1[0];

    // Bone names are assigned on the stringtable
    GetStringTable()->AddString("Scene Root");

    m_nbone++;
    m_bonecount--;
    m_rootadded = true;
}

void ovlBSHManager::AddMesh(const char* ftx, const char* txs, unsigned long place, unsigned long flags, unsigned long sides,
                            unsigned long vertexcount, VERTEX2* vertices, unsigned long indexcount, unsigned short* indices) {
    DUMP_LOG("Trace: ovlBSHManager::AddMesh(ftx:'%s', txs:'%s', place:%ld, flags:%ld, sides:%ld, vertexcount:%ld, vertices:%08lx, indexcount:%ld, indices:%08lx)", ftx, txs, place, flags, sides, vertexcount, DPTR(vertices), indexcount, DPTR(indices));
    Check("ovlBSHManager::AddMesh");
    if (!m_meshcount)
        throw EOvl("ovlBSHManager::AddMesh called but no meshes left");
    if (!m_cmodel)
        throw EOvl("ovlBSHManager::AddMesh called but no model pointer");
    if (vertexcount > USHRT_MAX)
        throw EOvl("ovlBSHManager::AddMesh called with too many vertices");

    BoneShape2* bsh2 = new BoneShape2;
    m_cmodel->sh[m_nmesh] = bsh2;
    m_ftxmap[bsh2] = string(ftx);
    m_txsmap[bsh2] = string(txs);
    bsh2->unk1 = 0xFFFFFFFF;
    bsh2->fts = NULL;
    bsh2->TextureData = NULL;
    bsh2->PlaceTexturing = place;
    bsh2->textureflags = flags;
    bsh2->sides = sides;
    bsh2->VertexCount = vertexcount;
    bsh2->IndexCount = indexcount;
    bsh2->Vertexes = new VERTEX2[bsh2->VertexCount];
    memcpy(bsh2->Vertexes, vertices, bsh2->VertexCount * sizeof(VERTEX2));
    bsh2->Triangles = new unsigned short[bsh2->IndexCount];
    memcpy(bsh2->Triangles, indices, bsh2->IndexCount * sizeof(unsigned short));

    m_cmodel->TotalVertexCount += bsh2->VertexCount;
    m_cmodel->TotalIndexCount += bsh2->IndexCount;

    // StaticShape2
    m_size += sizeof(BoneShape2);
    // Vertices
    //m_size += bsh2->VertexCount * sizeof(VERTEX2);
    m_blobs[reinterpret_cast<unsigned char*>(m_cmodel)].size += bsh2->VertexCount * sizeof(VERTEX2);
    // Indices
    //m_size += bsh2->IndexCount * sizeof(unsigned short);
    m_blobs[reinterpret_cast<unsigned char*>(m_cmodel)].size += bsh2->IndexCount * sizeof(unsigned short);

    GetStringTable()->AddSymbolString(ftx, ovlFTXManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(txs, ovlTXSManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);

    m_nmesh++;
    m_meshcount--;
}

unsigned char* ovlBSHManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlBSHManager::Make()");
    Check("ovlBSHManager::Make");
    if (m_meshcount)
        throw EOvl("ovlBSHManager::Make called but there are unassigned meshes left");
    if (m_bonecount)
        throw EOvl("ovlBSHManager::Make called but there are unassigned bones left");

    m_blobs[0] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[0].data;

    for (unsigned long i = 0; i < m_modellist.size(); ++i) {
        unsigned char* c_commondata = m_blobs[reinterpret_cast<unsigned char*>(m_modellist[i])].data;

        // Data transfer, BoneShape1
        BoneShape1* c_model = reinterpret_cast<BoneShape1*>(c_data);
        c_data += sizeof(BoneShape1);
        memcpy(c_model, m_modellist[i], sizeof(BoneShape1));

        // Assign space for BoneShape2 pointers
        c_model->sh = reinterpret_cast<BoneShape2**>(c_data);
        c_data += c_model->MeshCount * 4;
        GetRelocationManager()->AddRelocation((unsigned long *)&c_model->sh);
        DUMP_RELOCATION("ovlBSHManager::Make, BoneShape2 pointers", c_model->sh);

        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_modelnames[i].c_str(), Tag()), reinterpret_cast<unsigned long*>(c_model));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_model), false, c_symbol);

        for (unsigned long s = 0; s < m_modellist[i]->MeshCount; ++s) {
            // Data transfer, StaticShape2
            BoneShape2* c_mesh = reinterpret_cast<BoneShape2*>(c_data);
            c_data += sizeof(BoneShape2);
            memcpy(c_mesh, m_modellist[i]->sh[s], sizeof(BoneShape2));
            c_model->sh[s] = c_mesh;
            GetRelocationManager()->AddRelocation((unsigned long *)&c_model->sh[s]);
            DUMP_RELOCATION("ovlBSHManager::Make, StaticShape2", c_model->sh[s]);

            // Data transfer, vertices
//            c_mesh->Vertexes = reinterpret_cast<VERTEX2*>(c_data);
//            c_data += c_mesh->VertexCount * sizeof(VERTEX2);
//            memcpy(c_mesh->Vertexes, m_modellist[i]->sh[s]->Vertexes, c_mesh->VertexCount * sizeof(VERTEX2));
//            m_relman->AddRelocation((unsigned long *)&c_mesh->Vertexes);
//            DUMP_RELOCATION("ovlBSHManager::Make, Vertexes", c_mesh->Vertexes);
            c_mesh->Vertexes = reinterpret_cast<VERTEX2*>(c_commondata);
            c_commondata += c_mesh->VertexCount * sizeof(VERTEX2);
            memcpy(c_mesh->Vertexes, m_modellist[i]->sh[s]->Vertexes, c_mesh->VertexCount * sizeof(VERTEX2));
            GetRelocationManager()->AddRelocation((unsigned long *)&c_mesh->Vertexes);
            DUMP_RELOCATION("ovlBSHManager::Make, Vertexes", c_mesh->Vertexes);

            // Data transfer, indices
//            c_mesh->Triangles = reinterpret_cast<unsigned short*>(c_data);
//            c_data += c_mesh->IndexCount * sizeof(unsigned short);
//            memcpy(c_mesh->Triangles, m_modellist[i]->sh[s]->Triangles, c_mesh->IndexCount * sizeof(unsigned short));
//            m_relman->AddRelocation((unsigned long *)&c_mesh->Triangles);
//            DUMP_RELOCATION("ovlBSHManager::Make, Triangles", c_mesh->Triangles);
            c_mesh->Triangles = reinterpret_cast<unsigned short*>(c_commondata);
            c_commondata += c_mesh->IndexCount * sizeof(unsigned short);
            memcpy(c_mesh->Triangles, m_modellist[i]->sh[s]->Triangles, c_mesh->IndexCount * sizeof(unsigned short));
            GetRelocationManager()->AddRelocation((unsigned long *)&c_mesh->Triangles);
            DUMP_RELOCATION("ovlBSHManager::Make, Triangles", c_mesh->Triangles);

            // Symbol references
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_ftxmap[m_modellist[i]->sh[s]].c_str(), ovlFTXManager::TAG), reinterpret_cast<unsigned long*>(&c_mesh->fts));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_txsmap[m_modellist[i]->sh[s]].c_str(), ovlTXSManager::TAG), reinterpret_cast<unsigned long*>(&c_mesh->TextureData));
        }

        // Data transfer, BonesStructs
//        c_model->Bones = reinterpret_cast<BoneStruct*>(c_data);
//        c_data += c_model->BoneCount * sizeof(BoneStruct);
//        m_relman->AddRelocation((unsigned long *)&c_model->Bones);
//        DUMP_RELOCATION("ovlBSHManager::Make, Bones", c_model->Bones);
        c_model->Bones = reinterpret_cast<BoneStruct*>(c_commondata);
        c_commondata += c_model->BoneCount * sizeof(BoneStruct);
        GetRelocationManager()->AddRelocation((unsigned long *)&c_model->Bones);
        DUMP_RELOCATION("ovlBSHManager::Make, Bones", c_model->Bones);
        for (unsigned long s = 0; s < c_model->BoneCount; ++s) {
            // Find string on the table
            c_model->Bones[s].BoneName = GetStringTable()->FindString(m_modellist[i]->Bones[s].BoneName);
            GetRelocationManager()->AddRelocation((unsigned long*)&c_model->Bones[s].BoneName);
            DUMP_RELOCATION_STR("ovlBSHManager::Make, BoneName", c_model->Bones[s].BoneName);

            c_model->Bones[s].ParentBoneNumber = m_modellist[i]->Bones[s].ParentBoneNumber;
        }

        // Data transfer, matrices
//        c_model->BonePositions1 = reinterpret_cast<D3DMATRIX*>(c_data);
//        c_data += c_model->BoneCount * sizeof(D3DMATRIX);
//        memcpy(c_model->BonePositions1, m_modellist[i]->BonePositions1, c_model->BoneCount * sizeof(D3DMATRIX));
//        m_relman->AddRelocation((unsigned long *)&c_model->BonePositions1);
//        DUMP_RELOCATION("ovlBSHManager::Make, BonePositions1", c_model->BonePositions1);
        c_model->BonePositions1 = reinterpret_cast<D3DMATRIX*>(c_commondata);
        c_commondata += c_model->BoneCount * sizeof(D3DMATRIX);
        memcpy(c_model->BonePositions1, m_modellist[i]->BonePositions1, c_model->BoneCount * sizeof(D3DMATRIX));
        GetRelocationManager()->AddRelocation((unsigned long *)&c_model->BonePositions1);
        DUMP_RELOCATION("ovlBSHManager::Make, BonePositions1", c_model->BonePositions1);

//        c_model->BonePositions2 = reinterpret_cast<D3DMATRIX*>(c_data);
//        c_data += c_model->BoneCount * sizeof(D3DMATRIX);
//        memcpy(c_model->BonePositions2, m_modellist[i]->BonePositions2, c_model->BoneCount * sizeof(D3DMATRIX));
//        m_relman->AddRelocation((unsigned long *)&c_model->BonePositions2);
//        DUMP_RELOCATION("ovlBSHManager::Make, BonePositions2", c_model->BonePositions2);
        c_model->BonePositions2 = reinterpret_cast<D3DMATRIX*>(c_commondata);
        c_commondata += c_model->BoneCount * sizeof(D3DMATRIX);
        memcpy(c_model->BonePositions2, m_modellist[i]->BonePositions2, c_model->BoneCount * sizeof(D3DMATRIX));
        GetRelocationManager()->AddRelocation((unsigned long *)&c_model->BonePositions2);
        DUMP_RELOCATION("ovlBSHManager::Make, BonePositions2", c_model->BonePositions2);

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }

    return m_data;
}
*/
