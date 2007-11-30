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


#include "ManagerBSH.h"

#include "ManagerFTX.h"
#include "OVLDebug.h"
#include "OVLException.h"

const char* ovlBSHManager::NAME = "BoneShape";
const char* ovlBSHManager::TAG = "bsh";

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

    // BoneShape1
    m_size += sizeof(BoneShape1);
    // BoneShape2 pointers
    m_size += c_bsh->MeshCount * sizeof(BoneShape2*);
    // BoneStructs
    m_size += c_bsh->BoneCount * sizeof(BoneStruct);
    // Bone positions
    m_size += 2 * c_bsh->BoneCount * sizeof(D3DMATRIX);

    m_modellist.push_back(c_bsh);
    m_lsrman->AddLoader();
    m_lsrman->AddSymbol();
    m_modelnames.push_back(string(name));
    m_stable->AddSymbolString(name, Tag());

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
    m_stable->AddString(name);

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
    m_stable->AddString("Scene Root");

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
    m_size += bsh2->VertexCount * sizeof(VERTEX2);
    // Indices
    m_size += bsh2->IndexCount * sizeof(unsigned short);

    m_stable->AddSymbolString(ftx, ovlFTXManager::TAG);
    m_lsrman->AddSymRef();
    m_stable->AddSymbolString(txs, ovlTXSManager::TAG);
    m_lsrman->AddSymRef();

    m_nmesh++;
    m_meshcount--;
}

unsigned char* ovlBSHManager::Make() {
    DUMP_LOG("Trace: ovlBSHManager::Make()");
    Check("ovlBSHManager::Make");
    if (m_meshcount)
        throw EOvl("ovlBSHManager::Make called but there are unassigned meshes left");
    if (m_bonecount)
        throw EOvl("ovlBSHManager::Make called but there are unassigned bones left");

    ovlOVLManager::Make();
    unsigned char* c_data = m_data;

    for (unsigned long i = 0; i < m_modellist.size(); ++i) {
        // Data transfer, BoneShape1
        BoneShape1* c_model = reinterpret_cast<BoneShape1*>(c_data);
        c_data += sizeof(BoneShape1);
        memcpy(c_model, m_modellist[i], sizeof(BoneShape1));

        // Assign space for BoneShape2 pointers
        c_model->sh = reinterpret_cast<BoneShape2**>(c_data);
        c_data += c_model->MeshCount * 4;
        m_relman->AddRelocation((unsigned long *)&c_model->sh);
        DUMP_RELOCATION("ovlBSHManager::Make, BoneShape2 pointers", c_model->sh);

        // Symbol and Loader
        SymbolStruct* c_symbol = m_lsrman->MakeSymbol(m_stable->FindSymbolString(m_modelnames[i].c_str(), Tag()), reinterpret_cast<unsigned long*>(c_model), true);
        m_lsrman->OpenLoader(TAG, reinterpret_cast<unsigned long*>(c_model), false, c_symbol);

        for (unsigned long s = 0; s < m_modellist[i]->MeshCount; ++s) {
            // Data transfer, StaticShape2
            BoneShape2* c_mesh = reinterpret_cast<BoneShape2*>(c_data);
            c_data += sizeof(BoneShape2);
            memcpy(c_mesh, m_modellist[i]->sh[s], sizeof(BoneShape2));
            c_model->sh[s] = c_mesh;
            m_relman->AddRelocation((unsigned long *)&c_model->sh[s]);
            DUMP_RELOCATION("ovlBSHManager::Make, StaticShape2", c_model->sh[s]);

            // Data transfer, vertices
            c_mesh->Vertexes = reinterpret_cast<VERTEX2*>(c_data);
            c_data += c_mesh->VertexCount * sizeof(VERTEX2);
            memcpy(c_mesh->Vertexes, m_modellist[i]->sh[s]->Vertexes, c_mesh->VertexCount * sizeof(VERTEX2));
            m_relman->AddRelocation((unsigned long *)&c_mesh->Vertexes);
            DUMP_RELOCATION("ovlBSHManager::Make, Vertexes", c_mesh->Vertexes);

            // Data transfer, indices
            c_mesh->Triangles = reinterpret_cast<unsigned short*>(c_data);
            c_data += c_mesh->IndexCount * sizeof(unsigned short);
            memcpy(c_mesh->Triangles, m_modellist[i]->sh[s]->Triangles, c_mesh->IndexCount * sizeof(unsigned long));
            m_relman->AddRelocation((unsigned long *)&c_mesh->Triangles);
            DUMP_RELOCATION("ovlBSHManager::Make, Triangles", c_mesh->Triangles);

            // Symbol references
            m_lsrman->MakeSymRef(m_stable->FindSymbolString(m_ftxmap[m_modellist[i]->sh[s]].c_str(), ovlFTXManager::TAG), reinterpret_cast<unsigned long*>(&c_mesh->fts));
            m_lsrman->MakeSymRef(m_stable->FindSymbolString(m_txsmap[m_modellist[i]->sh[s]].c_str(), ovlTXSManager::TAG), reinterpret_cast<unsigned long*>(&c_mesh->TextureData));
        }

        // Data transfer, BonesStructs
        c_model->Bones = reinterpret_cast<BoneStruct*>(c_data);
        c_data += c_model->BoneCount * sizeof(BoneStruct);
        m_relman->AddRelocation((unsigned long *)&c_model->Bones);
        DUMP_RELOCATION("ovlBSHManager::Make, Bones", c_model->Bones);
        for (unsigned long s = 0; s < c_model->BoneCount; ++s) {
            // Find string on the table
            c_model->Bones[s].BoneName = m_stable->FindString(m_modellist[i]->Bones[s].BoneName);
            m_relman->AddRelocation((unsigned long*)&c_model->Bones[s].BoneName);
            DUMP_RELOCATION_STR("ovlBSHManager::Make, BoneName", c_model->Bones[s].BoneName);

            c_model->Bones[s].ParentBoneNumber = m_modellist[i]->Bones[s].ParentBoneNumber;
        }

        // Data transfer, matrices
        c_model->BonePositions1 = reinterpret_cast<D3DMATRIX*>(c_data);
        c_data += c_model->BoneCount * sizeof(D3DMATRIX);
        memcpy(c_model->BonePositions1, m_modellist[i]->BonePositions1, c_model->BoneCount * sizeof(D3DMATRIX));
        m_relman->AddRelocation((unsigned long *)&c_model->BonePositions1);
        DUMP_RELOCATION("ovlBSHManager::Make, BonePositions1", c_model->BonePositions1);

        c_model->BonePositions2 = reinterpret_cast<D3DMATRIX*>(c_data);
        c_data += c_model->BoneCount * sizeof(D3DMATRIX);
        memcpy(c_model->BonePositions2, m_modellist[i]->BonePositions2, c_model->BoneCount * sizeof(D3DMATRIX));
        m_relman->AddRelocation((unsigned long *)&c_model->BonePositions2);
        DUMP_RELOCATION("ovlBSHManager::Make, BonePositions2", c_model->BonePositions2);

        m_lsrman->CloseLoader();
    }

    return m_data;
}

