///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SHS structures
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


#include "ManagerSHS.h"

#include "ManagerFTX.h"
#include "OVLDebug.h"
#include "OVLException.h"

const char* ovlSHSManager::NAME = "StaticShape";
const char* ovlSHSManager::TAG = "shs";

ovlSHSManager::~ovlSHSManager() {
    for (unsigned long i = 0; i < m_modellist.size(); ++i) {
        for (unsigned long s = 0; s < m_modellist[i]->MeshCount; ++s) {
            delete[] m_modellist[i]->sh[s]->Vertexes;
            delete[] m_modellist[i]->sh[s]->Triangles;
            delete m_modellist[i]->sh[s];
        }
        delete[] m_modellist[i]->sh;
        delete[] m_modellist[i]->EffectPosition;
        for (unsigned long s = 0; s < m_modellist[i]->EffectCount; ++s) {
            delete[] m_modellist[i]->EffectName[s];
        }
        delete[] m_modellist[i]->EffectName;
        delete m_modellist[i];
    }
}

void ovlSHSManager::AddModel(const char* name, unsigned long meshes, unsigned long effects) {
    Check("ovlSHSManager::AddModel");
    if (m_meshcount)
        throw EOvl("ovlSHSManager::AddModel called but last model misses meshes");
    if (m_effectcount)
        throw EOvl("ovlSHSManager::AddModel called but last model misses effect points");

    StaticShape1* c_shs = new StaticShape1;
    c_shs->TotalIndexCount = 0;
    c_shs->TotalVertexCount = 0;
    c_shs->MeshCount2 = meshes;
    c_shs->MeshCount = meshes;
    c_shs->sh = new StaticShape2*[c_shs->MeshCount];
    c_shs->EffectCount = effects;
    if (c_shs->EffectCount) {
        c_shs->EffectPosition = new D3DMATRIX[c_shs->EffectCount];
        c_shs->EffectName = new char*[c_shs->EffectCount];
    } else {
        c_shs->EffectPosition = NULL;
        c_shs->EffectName = NULL;
    }

    // StaticShape1
    m_size += sizeof(StaticShape1);
    // StaticShape2 pointers
    m_size += c_shs->MeshCount * 4;
    // Effect positions
    m_size += c_shs->EffectCount * sizeof(D3DMATRIX);
    // Effect name pointers
    m_size += c_shs->EffectCount * 4;

    m_modellist.push_back(c_shs);
    m_lsrman->AddLoader();
    m_lsrman->AddSymbol();
    m_modelnames.push_back(string(name));
    m_stable->AddSymbolString(name, Tag());

    m_cmodel = c_shs;
    m_nmesh = 0;
    m_meshcount = c_shs->MeshCount;
    m_neffect = 0;
    m_effectcount = c_shs->EffectCount;
}

void ovlSHSManager::SetBoundingBox(const D3DVECTOR& bbox1, const D3DVECTOR& bbox2) {
    Check("ovlSHSManager::SetBoundingBox");
    if (!m_cmodel)
        throw EOvl("ovlSHSManager::SetBoundingBox called but no model pointer");

    m_cmodel->BoundingBox1 = bbox1;
    m_cmodel->BoundingBox2 = bbox2;
}

void ovlSHSManager::AddEffectPoint(const char* name, const D3DMATRIX& matrix) {
    Check("ovlSHSManager::AddEffectPoint");
    if (!m_effectcount)
        throw EOvl("ovlSHSManager::AddEffectPoint called but no effect points left");
    if (!m_cmodel)
        throw EOvl("ovlSHSManager::AddEffectPoint called but no model pointer");

    unsigned long namelength = strlen(name)+1;
    m_cmodel->EffectPosition[m_neffect] = matrix;
    m_cmodel->EffectName[m_neffect] = new char[namelength];
    strcpy(m_cmodel->EffectName[m_neffect], name);

    // Add padded name size
    while (namelength % 4 != 0) {
        namelength++;
    }
    m_size += namelength;

    m_neffect++;
    m_effectcount--;
}

void ovlSHSManager::AddMesh(const char* ftx, const char* txs, unsigned long place, unsigned long flags, unsigned long sides,
                            unsigned long vertexcount, VERTEX* vertices, unsigned long indexcount, unsigned long* indices) {
    Check("ovlSHSManager::AddMesh");
    if (!m_meshcount)
        throw EOvl("ovlSHSManager::AddMesh called but no meshes left");
    if (!m_cmodel)
        throw EOvl("ovlSHSManager::AddMesh called but no model pointer");

    StaticShape2* shs2 = new StaticShape2;
    m_cmodel->sh[m_nmesh] = shs2;
    m_ftxmap[shs2] = string(ftx);
    m_txsmap[shs2] = string(txs);
    shs2->unk1 = 0xFFFFFFFF;
    shs2->fts = NULL;
    shs2->TextureData = NULL;
    shs2->PlaceTexturing = place;
    shs2->textureflags = flags;
    shs2->sides = sides;
    shs2->VertexCount = vertexcount;
    shs2->IndexCount = indexcount;
    shs2->Vertexes = new VERTEX[shs2->VertexCount];
    memcpy(shs2->Vertexes, vertices, shs2->VertexCount * sizeof(VERTEX));
    shs2->Triangles = new unsigned long[shs2->IndexCount];
    memcpy(shs2->Triangles, indices, shs2->IndexCount * sizeof(unsigned long));

    m_cmodel->TotalVertexCount += shs2->VertexCount;
    m_cmodel->TotalIndexCount += shs2->IndexCount;

    // StaticShape2
    m_size += sizeof(StaticShape2);
    // Vertices
    m_size += shs2->VertexCount * sizeof(VERTEX);
    // Indices
    m_size += shs2->IndexCount * sizeof(unsigned long);

    m_stable->AddSymbolString(ftx, ovlFTXManager::TAG);
    m_lsrman->AddSymRef();
    m_stable->AddSymbolString(txs, ovlTXSManager::TAG);
    m_lsrman->AddSymRef();

    m_nmesh++;
    m_meshcount--;
}

unsigned char* ovlSHSManager::Make() {
    Check("ovlSHSManager::Make");
    if (m_meshcount)
        throw EOvl("ovlSHSManager::Make called but there are unassigned meshes left");
    if (m_effectcount)
        throw EOvl("ovlSHSManager::Make called but there are unassigned effect points left");

    ovlOVLManager::Make();
    unsigned char* c_data = m_data;

    for (unsigned long i = 0; i < m_modellist.size(); ++i) {
        // Data transfer, StaticShape1
        StaticShape1* c_model = reinterpret_cast<StaticShape1*>(c_data);
        c_data += sizeof(StaticShape1);
        memcpy(c_model, m_modellist[i], sizeof(StaticShape1));

        // Assign space for StaticShape2 pointers
        c_model->sh = reinterpret_cast<StaticShape2**>(c_data);
        c_data += c_model->MeshCount * 4;
        m_relman->AddRelocation((unsigned long *)&c_model->sh);
        DUMP_RELOCATION("ovlSHSManager::Make, StaticShape2 pointers", c_model->sh);

        // Symbol and Loader
        SymbolStruct* c_symbol = m_lsrman->MakeSymbol(m_stable->FindSymbolString(m_modelnames[i].c_str(), Tag()), reinterpret_cast<unsigned long*>(c_model), true);
        m_lsrman->OpenLoader(TAG, reinterpret_cast<unsigned long*>(c_model), false, c_symbol);

        for (unsigned long s = 0; s < m_modellist[i]->MeshCount; ++s) {
            // Data transfer, StaticShape2
            StaticShape2* c_mesh = reinterpret_cast<StaticShape2*>(c_data);
            c_data += sizeof(StaticShape2);
            memcpy(c_mesh, m_modellist[i]->sh[s], sizeof(StaticShape2));
            c_model->sh[s] = c_mesh;
            m_relman->AddRelocation((unsigned long *)&c_model->sh[s]);
            DUMP_RELOCATION("ovlSHSManager::Make, StaticShape2", c_model->sh[s]);

            // Data transfer, vertices
            c_mesh->Vertexes = reinterpret_cast<VERTEX*>(c_data);
            c_data += c_mesh->VertexCount * sizeof(VERTEX);
            memcpy(c_mesh->Vertexes, m_modellist[i]->sh[s]->Vertexes, c_mesh->VertexCount * sizeof(VERTEX));
            m_relman->AddRelocation((unsigned long *)&c_mesh->Vertexes);
            DUMP_RELOCATION("ovlSHSManager::Make, Vertexes", c_mesh->Vertexes);

            // Data transfer, indices
            c_mesh->Triangles = reinterpret_cast<unsigned long*>(c_data);
            c_data += c_mesh->IndexCount * sizeof(unsigned long);
            memcpy(c_mesh->Triangles, m_modellist[i]->sh[s]->Triangles, c_mesh->IndexCount * sizeof(unsigned long));
            m_relman->AddRelocation((unsigned long *)&c_mesh->Triangles);
            DUMP_RELOCATION("ovlSHSManager::Make, Triangles", c_mesh->Triangles);

            // Symbol references
            m_lsrman->MakeSymRef(m_stable->FindSymbolString(m_ftxmap[m_modellist[i]->sh[s]].c_str(), ovlFTXManager::TAG), reinterpret_cast<unsigned long*>(&c_mesh->fts));
            m_lsrman->MakeSymRef(m_stable->FindSymbolString(m_txsmap[m_modellist[i]->sh[s]].c_str(), ovlTXSManager::TAG), reinterpret_cast<unsigned long*>(&c_mesh->TextureData));
        }

        if (c_model->EffectCount) {
            // Data transfer, matrices
            c_model->EffectPosition = reinterpret_cast<D3DMATRIX*>(c_data);
            c_data += c_model->EffectCount * sizeof(D3DMATRIX);
            memcpy(c_model->EffectPosition, m_modellist[i]->EffectPosition, c_model->EffectCount * sizeof(D3DMATRIX));
            m_relman->AddRelocation((unsigned long *)&c_model->EffectPosition);
            DUMP_RELOCATION("ovlSHSManager::Make, Effect Positions", c_model->EffectPosition);

            // Assign space for name pointers
            c_model->EffectName = reinterpret_cast<char**>(c_data);
            c_data += c_model->EffectCount * sizeof(char*);
            m_relman->AddRelocation((unsigned long *)&c_model->EffectName);
            DUMP_RELOCATION("ovlSHSManager::Make, Effect Names", c_model->EffectName);

            for (unsigned long s = 0; s < c_model->EffectCount; ++s) {
                // Data transfer, padded effect name
                c_model->EffectName[s] = reinterpret_cast<char*>(c_data);
                strcpy(reinterpret_cast<char*>(c_data), m_modellist[i]->EffectName[s]);
                c_data += strlen(m_modellist[i]->EffectName[s]) + 1;
                m_relman->AddRelocation((unsigned long *)&c_model->EffectName[s]);
                DUMP_RELOCATION("ovlSHSManager::Make, Effect Name", c_model->EffectName[s]);

                // Padding
                unsigned long x = strlen(m_modellist[i]->EffectName[s]) + 1;
                while (x % 4 != 0) {
                    *c_data = 0;
                    c_data++;
                    x++;
                }
            }

        }

        m_lsrman->CloseLoader();
    }

    return m_data;
}

