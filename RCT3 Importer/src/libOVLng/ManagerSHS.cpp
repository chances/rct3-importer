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

#include "OVLDebug.h"

#include "ManagerSHS.h"

#include "ManagerFTX.h"
#include "OVLException.h"

//#include <iostream>
#include <stdexcept>

using namespace r3;
using namespace std;

const char* ovlSHSManager::NAME = "StaticShape";
const char* ovlSHSManager::TAG = "shs";

struct cVertexSorter {
    bool operator() (const VERTEX& lhs, const VERTEX& rhs) const {
        if (lhs.position.x == rhs.position.x)
            return lhs.position.z > rhs.position.z;
        else
            return lhs.position.x > rhs.position.x;
    }
};

struct cVertexComparator {
    VERTEX* c;
    cVertexComparator(VERTEX* t): c(t) {}
    bool operator() (const VERTEX& t) const {
        return memcmp(c, &t, sizeof(VERTEX)) == 0;
    }
};

struct cTriangleSorter {
    vector<VERTEX>* verts;
    cTriangleSortAlgorithm algo;
    cTriangleSorter(vector<VERTEX>* v, const cTriangleSortAlgorithm& a): verts(v), algo(a) {
#ifdef OVLDEBUG
        switch(a.algo) {
            case cTriangleSortAlgorithm::MIN:
                DUMP_LOG("Sorting by min.");
                break;
            case cTriangleSortAlgorithm::MAX:
                DUMP_LOG("Sorting by max.");
                break;
            case cTriangleSortAlgorithm::MEAN:
                DUMP_LOG("Sorting by mean.");
                break;
            case cTriangleSortAlgorithm::MINMAX:
                DUMP_LOG("Sorting by minmax.");
                break;
            case cTriangleSortAlgorithm::MAXMIN:
                DUMP_LOG("Sorting by maxmin.");
                break;
            case cTriangleSortAlgorithm::ANGLE45:
                DUMP_LOG("Sorting by angle45.");
                break;
            default:
                DUMP_LOG("Sort algorithm called with illegal algo");
        }
#endif
    }
    bool operator() (const Triangle& lhs, const Triangle& rhs) const {
        //DUMP_LOG("sort: %ld %ld %ld vs %ld %ld %ld", lhs.a, lhs.b, lhs.c, rhs.a, rhs.b, rhs.c);
        //try {
            return algo(verts->at(lhs.a).position, verts->at(lhs.b).position, verts->at(lhs.c).position,
                        verts->at(rhs.a).position, verts->at(rhs.b).position, verts->at(rhs.c).position);
        //} catch (out_of_range) {
        //    DUMP_LOG("Out of range exception in cTriangleSorter");
        //    return false;
        //}
        /*
        switch (which) {
            case 1: // y
                return algo((*verts)[lhs.a].position, (*verts)[lhs.b].position, (*verts)[lhs.c].position,
                            (*verts)[rhs.a].position, (*verts)[rhs.b].position, (*verts)[rhs.c].position,
                            cTriangleSortAlgorithm::BY_Y);
            case 2: // z
                return algo((*verts)[lhs.a].position, (*verts)[lhs.b].position, (*verts)[lhs.c].position,
                            (*verts)[rhs.a].position, (*verts)[rhs.b].position, (*verts)[rhs.c].position,
                            cTriangleSortAlgorithm::BY_Z);
            default: // x
                return algo((*verts)[lhs.a].position, (*verts)[lhs.b].position, (*verts)[lhs.c].position.x,
                            (*verts)[rhs.a].position, (*verts)[rhs.b].position, (*verts)[rhs.c].position.x);
        }
        */
    }
};


void cStaticShape2::Fill(StaticShapeMesh* ss2, uint32_t* vert, uint32_t* ind) {
    ss2->support_type = support;
    ss2->ftx_ref = NULL;
    ss2->txs_ref = NULL;
    ss2->transparency = placetexturing;
    ss2->texture_flags = textureflags;
    ss2->sides = sides;
    ss2->vertex_count = vertices.size();
    if (vert)
        *vert += vertices.size();
#ifndef GLASS_OLD
    if ((algo_x == cTriangleSortAlgorithm::NONE) || (algo_y == cTriangleSortAlgorithm::NONE) || (algo_z == cTriangleSortAlgorithm::NONE)) {
        algo_x = cTriangleSortAlgorithm::NONE;
        algo_y = cTriangleSortAlgorithm::NONE;
        algo_z = cTriangleSortAlgorithm::NONE;
    }

    if ((algo_x == cTriangleSortAlgorithm::NONE) && (placetexturing != 0))
        ss2->index_count = indices.size() / 3;
    else
#endif
    ss2->index_count = indices.size();
    if (ind)
        *ind += ss2->index_count;
#ifndef GLASS_OLD
    if ((placetexturing != 0) && (algo_x != cTriangleSortAlgorithm::NONE)) {
        // Sort vertices
        vector<VERTEX> sortvertex = vertices;
        sort(sortvertex.begin(), sortvertex.end(), cVertexSorter());

        // Fix indices
        for (vector<unsigned long>::size_type i = 0; i < indices.size(); ++i) {
            vector<VERTEX>::iterator newind = find_if(sortvertex.begin(), sortvertex.end(), cVertexComparator(&vertices[indices[i]]));
            indices[i] = newind - sortvertex.begin();
        }

        // Write back sorted
        vertices = sortvertex;
    }
#endif
    for(unsigned long i = 0; i < vertices.size(); ++i) {
        ss2->vertexes[i] = vertices[i];
    }
#ifndef GLASS_OLD
    if ((placetexturing != 0) && (algo_x != cTriangleSortAlgorithm::NONE)) {
        vector<Triangle> temptri(indices.size()/3);
        memcpy(&temptri[0], &indices[0], indices.size() * sizeof(unsigned long));
        DUMP_LOG("Sorting x. %d %d", temptri.size(), indices.size());

        sort(temptri.begin(), temptri.end(), cTriangleSorter(&vertices, cTriangleSortAlgorithm(algo_x, cTriangleSortAlgorithm::BY_X)));
        for(unsigned long i = 0; i < temptri.size(); ++i) {
//            std::cerr << vertices[temptri[i].a].position.x+vertices[temptri[i].b].position.x+vertices[temptri[i].c].position.x << std::endl;
//            if (i) {
//                if (cTriangleSorter(&vertices, 0)(temptri[i-1], temptri[i]))
//                    std::cerr << "Yoy" << std::endl;
//                if (vertices[temptri[i-1].a].position.x+vertices[temptri[i-1].b].position.x+vertices[temptri[i-1].c].position.x < vertices[temptri[i].a].position.x+vertices[temptri[i].b].position.x+vertices[temptri[i].c].position.x)
//                    std::cerr << "Yay" << std::endl;
//            }
            DUMP_LOG("  Tri Sum %f  Min %f  Max %f", vertices[temptri[i].a].position.x+vertices[temptri[i].b].position.x+vertices[temptri[i].c].position.x,
                        TriMin(vertices[temptri[i].a].position.x,vertices[temptri[i].b].position.x,vertices[temptri[i].c].position.x),
                        TriMax(vertices[temptri[i].a].position.x,vertices[temptri[i].b].position.x,vertices[temptri[i].c].position.x));
            ss2->indices[i*3] = temptri[i].a;
            ss2->indices[(i*3)+1] = temptri[i].b;
            ss2->indices[(i*3)+2] = temptri[i].c;
        }
        DUMP_LOG("Sorting y.");
        sort(temptri.begin(), temptri.end(), cTriangleSorter(&vertices, cTriangleSortAlgorithm(algo_y, cTriangleSortAlgorithm::BY_Y)));
        for(unsigned long i = 0; i < temptri.size(); ++i) {
            DUMP_LOG("  Tri Sum %f  Min %f  Max %f", vertices[temptri[i].a].position.y+vertices[temptri[i].b].position.y+vertices[temptri[i].c].position.y,
                        TriMin(vertices[temptri[i].a].position.y,vertices[temptri[i].b].position.y,vertices[temptri[i].c].position.y),
                        TriMax(vertices[temptri[i].a].position.y,vertices[temptri[i].b].position.y,vertices[temptri[i].c].position.y));
            ss2->indices[(i*3)+indices.size()] = temptri[i].a;
            ss2->indices[(i*3)+indices.size()+1] = temptri[i].b;
            ss2->indices[(i*3)+indices.size()+2] = temptri[i].c;
        }
        DUMP_LOG("Sorting z.");
        sort(temptri.begin(), temptri.end(), cTriangleSorter(&vertices, cTriangleSortAlgorithm(algo_z, cTriangleSortAlgorithm::BY_Z)));
        for(unsigned long i = 0; i < temptri.size(); ++i) {
            DUMP_LOG("  Tri Sum %f  Min %f  Max %f", vertices[temptri[i].a].position.z+vertices[temptri[i].b].position.z+vertices[temptri[i].c].position.z,
                        TriMin(vertices[temptri[i].a].position.z,vertices[temptri[i].b].position.z,vertices[temptri[i].c].position.z),
                        TriMax(vertices[temptri[i].a].position.z,vertices[temptri[i].b].position.z,vertices[temptri[i].c].position.z));
            ss2->indices[(i*3)+(indices.size()*2)] = temptri[i].a;
            ss2->indices[(i*3)+(indices.size()*2)+1] = temptri[i].b;
            ss2->indices[(i*3)+(indices.size()*2)+2] = temptri[i].c;
        }
    } else {
        for(unsigned long i = 0; i < indices.size(); ++i) {
            ss2->indices[i] = indices[i];
        }
    }
#else
    for(unsigned long i = 0; i < indices.size(); ++i) {
        ss2->indices[i] = indices[i];
    }
#endif
}

void cStaticShape1::Fill(r3::StaticShape* ss1) {
    ss1->bounding_box_min = bbox1;
    ss1->bounding_box_max = bbox2;
    ss1->total_vertex_count = 0;
    ss1->total_index_count = 0;
    ss1->mesh_count2 = 0;
    ss1->mesh_count = meshes.size();
    for(unsigned long i = 0; i < meshes.size(); ++i) {
        if (meshes[i].support == r3::Constants::Mesh::SupportType::None)
            ss1->mesh_count2++;
        meshes[i].Fill(ss1->sh[i], &ss1->total_vertex_count, &ss1->total_index_count);
    }
    ss1->effect_count = effects.size();
    for(unsigned long i = 0; i < effects.size(); ++i) {
        effects[i].Fill(&ss1->effect_positions[i]);
    }
}


void ovlSHSManager::AddModel(const cStaticShape1& item) {
    DUMP_LOG("Trace: ovlSHSManager::Make(%s)", UNISTR(item.name.c_str()));
    Check("ovlSHSManager::AddModel");
    if (item.name == "")
        throw EOvl("ovlSHSManager::AddModel called without name");
    if (m_items.find(item.name) != m_items.end())
        throw EOvl("ovlSHSManager::AddModel: Item with name '"+item.name+"' already exists");

    m_items[item.name] = item;

    // One common blob per model
    m_blobs[item.name] = cOvlMemBlob(OVLT_COMMON, 4, 0);

    // StaticShape1
    m_size += sizeof(StaticShape);
    // StaticShape2 pointers
    m_size += item.meshes.size() * 4;

    // Put in common
    // Effect name pointers
    m_blobs[item.name].size += item.effects.size() * sizeof(char*);
    // Effect positions
    m_blobs[item.name].size += item.effects.size() * sizeof(MATRIX);

    // Meshes
    for (vector<cStaticShape2>::const_iterator it = item.meshes.begin(); it != item.meshes.end(); ++it) {
        // StaticShape2
        m_size += sizeof(StaticShapeMesh);
        // Vertices
        m_blobs[item.name].size += it->vertices.size() * sizeof(VERTEX);
        // Indices
        m_blobs[item.name].size += it->indices.size() * sizeof(unsigned long);
#ifndef GLASS_OLD
        if ((it->IsAlgoNone()) && (it->placetexturing != 0)) {
            if (it->indices.size() % 3)
                throw EOvl("ovlSHSManager::AddModel: mesh has triangle sort algorithm none but indices aren't divisible by 3");
        }
        if ((it->placetexturing != 0) && (!it->IsAlgoNone())) {
            m_blobs[item.name].size += 2 * it->indices.size() * sizeof(unsigned long);
        }
#endif

        // Symbol Refs
        GetStringTable()->AddSymbolString(it->fts.c_str(), ovlFTXManager::TAG);
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(it->texturestyle.c_str(), ovlTXSManager::TAG);
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    }

    // Effect names
    for (vector<cEffectStruct>::const_iterator it = item.effects.begin(); it != item.effects.end(); ++it) {
        unsigned long namelength = it->name.length()+1;
        // Add padded name size
        while (namelength % 4 != 0) {
            namelength++;
        }
        //m_size += namelength;
        m_blobs[item.name].size += namelength;
    }

    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlSHSManager::TAG);
}

void ovlSHSManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlSHSManager::Make()");
    Check("ovlSHSManager::Make");

    m_blobs[""] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[""].data;

    for (map<string, cStaticShape1>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        unsigned char* c_commondata = m_blobs[it->first].data;

        StaticShape* c_model = reinterpret_cast<StaticShape*>(c_data);
        c_data += sizeof(StaticShape);

        // Assign space for StaticShape2 pointers
        c_model->sh = reinterpret_cast<StaticShapeMesh**>(c_data);
        c_data += it->second.meshes.size() * 4;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh));
        DUMP_RELOCATION("ovlSHSManager::Make, BoneShape2 pointers", c_model->sh);

        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), Tag()), reinterpret_cast<unsigned long*>(c_model));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_model), false, c_symbol);

        unsigned long s = 0;
        for (vector<cStaticShape2>::iterator itb = it->second.meshes.begin(); itb != it->second.meshes.end(); ++itb) {
            c_model->sh[s] = reinterpret_cast<StaticShapeMesh*>(c_data);
            c_data += sizeof(StaticShapeMesh);
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh[s]));
            DUMP_RELOCATION("ovlSHSManager::Make, StaticShape2", c_model->sh[s]);

            c_model->sh[s]->vertexes = reinterpret_cast<VERTEX*>(c_commondata);
            c_commondata += itb->vertices.size() * sizeof(VERTEX);
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh[s]->vertexes));
            DUMP_RELOCATION("ovlSHSManager::Make, Vertexes", c_model->sh[s]->vertexes);

            c_model->sh[s]->indices = reinterpret_cast<uint32_t*>(c_commondata);
            c_commondata += itb->indices.size() * sizeof(unsigned long);
#ifndef GLASS_OLD
            if ((itb->placetexturing != 0) && (!itb->IsAlgoNone())) {
                c_commondata += 2 * itb->indices.size() * sizeof(unsigned long);
            }
#endif
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh[s]->indices));
            DUMP_RELOCATION("ovlSHSManager::Make, Triangles", c_model->sh[s]->indices);

            // Symbol references
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(itb->fts.c_str(), ovlFTXManager::TAG), reinterpret_cast<unsigned long*>(&c_model->sh[s]->ftx_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(itb->texturestyle.c_str(), ovlTXSManager::TAG), reinterpret_cast<unsigned long*>(&c_model->sh[s]->txs_ref));

            s++;
        }

        if (it->second.effects.size()) {
            c_model->effect_names = reinterpret_cast<char**>(c_commondata);
            c_commondata += it->second.effects.size() * sizeof(char*);
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->effect_names));
            DUMP_RELOCATION("ovlSHSManager::Make, EffectNames", c_model->effect_names);

            c_model->effect_positions = reinterpret_cast<MATRIX*>(c_commondata);
            c_commondata += it->second.effects.size() * sizeof(MATRIX);
            GetRelocationManager()->AddRelocation((unsigned long *)&c_model->effect_positions);
            DUMP_RELOCATION("ovlSHSManager::Make, EffectPositions", c_model->effect_positions);

            s = 0;
            for (vector<cEffectStruct>::iterator itb = it->second.effects.begin(); itb != it->second.effects.end(); ++itb) {
                c_model->effect_names[s] = reinterpret_cast<char*>(c_commondata);
                strcpy(reinterpret_cast<char*>(c_commondata), itb->name.c_str());
                c_commondata += itb->name.length() + 1;
                GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->effect_names[s]));
                DUMP_RELOCATION("ovlSHSManager::Make, Effect Name", c_model->effect_names[s]);

                // Padding
                unsigned long x = itb->name.length() + 1;
                while (x % 4 != 0) {
                    *c_commondata = 0;
                    c_commondata++;
                    x++;
                }
                s++;
            }
        } else {
            c_model->effect_names = NULL;
            c_model->effect_positions = NULL;
        }

        it->second.Fill(c_model);
        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }
}

/*
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

    // One common blob per model
    m_blobs[reinterpret_cast<unsigned char*>(c_shs)] = cOvlMemBlob(OVLT_COMMON, 4, 0);

    // StaticShape1
    m_size += sizeof(StaticShape1);
    // StaticShape2 pointers
    m_size += c_shs->MeshCount * 4;

    // These go to common
    // Effect positions
    //m_size += c_shs->EffectCount * sizeof(D3DMATRIX);
    m_blobs[reinterpret_cast<unsigned char*>(c_shs)].size += c_shs->EffectCount * sizeof(D3DMATRIX);
    // Effect name pointers
    //m_size += c_shs->EffectCount * 4;
    m_blobs[reinterpret_cast<unsigned char*>(c_shs)].size += c_shs->EffectCount * 4;


    m_modellist.push_back(c_shs);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    m_modelnames.push_back(string(name));
    GetStringTable()->AddSymbolString(name, Tag());

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
    //m_size += namelength;
    m_blobs[reinterpret_cast<unsigned char*>(m_cmodel)].size += namelength;

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
    //m_size += shs2->VertexCount * sizeof(VERTEX);
    m_blobs[reinterpret_cast<unsigned char*>(m_cmodel)].size += shs2->VertexCount * sizeof(VERTEX);
    // Indices
    //m_size += shs2->IndexCount * sizeof(unsigned long);
    m_blobs[reinterpret_cast<unsigned char*>(m_cmodel)].size += shs2->IndexCount * sizeof(unsigned long);

    GetStringTable()->AddSymbolString(ftx, ovlFTXManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(txs, ovlTXSManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);

    m_nmesh++;
    m_meshcount--;
}

unsigned char* ovlSHSManager::Make(cOvlInfo* info) {
    Check("ovlSHSManager::Make");
    if (m_meshcount)
        throw EOvl("ovlSHSManager::Make called but there are unassigned meshes left");
    if (m_effectcount)
        throw EOvl("ovlSHSManager::Make called but there are unassigned effect points left");

    m_blobs[0] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[0].data;

    for (unsigned long i = 0; i < m_modellist.size(); ++i) {
        unsigned char* c_commondata = m_blobs[reinterpret_cast<unsigned char*>(m_modellist[i])].data;

        // Data transfer, StaticShape1
        StaticShape1* c_model = reinterpret_cast<StaticShape1*>(c_data);
        c_data += sizeof(StaticShape1);
        memcpy(c_model, m_modellist[i], sizeof(StaticShape1));

        // Assign space for StaticShape2 pointers
        c_model->sh = reinterpret_cast<StaticShape2**>(c_data);
        c_data += c_model->MeshCount * 4;
        GetRelocationManager()->AddRelocation((unsigned long *)&c_model->sh);
        DUMP_RELOCATION("ovlSHSManager::Make, StaticShape2 pointers", c_model->sh);

        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_modelnames[i].c_str(), Tag()), reinterpret_cast<unsigned long*>(c_model));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_model), false, c_symbol);

        for (unsigned long s = 0; s < m_modellist[i]->MeshCount; ++s) {
            // Data transfer, StaticShape2
            StaticShape2* c_mesh = reinterpret_cast<StaticShape2*>(c_data);
            c_data += sizeof(StaticShape2);
            memcpy(c_mesh, m_modellist[i]->sh[s], sizeof(StaticShape2));
            c_model->sh[s] = c_mesh;
            GetRelocationManager()->AddRelocation((unsigned long *)&c_model->sh[s]);
            DUMP_RELOCATION("ovlSHSManager::Make, StaticShape2", c_model->sh[s]);

            // Data transfer, vertices
//            c_mesh->Vertexes = reinterpret_cast<VERTEX*>(c_data);
//            c_data += c_mesh->VertexCount * sizeof(VERTEX);
//            memcpy(c_mesh->Vertexes, m_modellist[i]->sh[s]->Vertexes, c_mesh->VertexCount * sizeof(VERTEX));
//            m_relman->AddRelocation((unsigned long *)&c_mesh->Vertexes);
//            DUMP_RELOCATION("ovlSHSManager::Make, Vertexes", c_mesh->Vertexes);
            c_mesh->Vertexes = reinterpret_cast<VERTEX*>(c_commondata);
            c_commondata += c_mesh->VertexCount * sizeof(VERTEX);
            memcpy(c_mesh->Vertexes, m_modellist[i]->sh[s]->Vertexes, c_mesh->VertexCount * sizeof(VERTEX));
            GetRelocationManager()->AddRelocation((unsigned long *)&c_mesh->Vertexes);
            DUMP_RELOCATION("ovlSHSManager::Make, Vertexes", c_mesh->Vertexes);

            // Data transfer, indices
//            c_mesh->Triangles = reinterpret_cast<unsigned long*>(c_data);
//            c_data += c_mesh->IndexCount * sizeof(unsigned long);
//            memcpy(c_mesh->Triangles, m_modellist[i]->sh[s]->Triangles, c_mesh->IndexCount * sizeof(unsigned long));
//            m_relman->AddRelocation((unsigned long *)&c_mesh->Triangles);
//            DUMP_RELOCATION("ovlSHSManager::Make, Triangles", c_mesh->Triangles);
            c_mesh->Triangles = reinterpret_cast<unsigned long*>(c_commondata);
            c_commondata += c_mesh->IndexCount * sizeof(unsigned long);
            memcpy(c_mesh->Triangles, m_modellist[i]->sh[s]->Triangles, c_mesh->IndexCount * sizeof(unsigned long));
            GetRelocationManager()->AddRelocation((unsigned long *)&c_mesh->Triangles);
            DUMP_RELOCATION("ovlSHSManager::Make, Triangles", c_mesh->Triangles);

            // Symbol references
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_ftxmap[m_modellist[i]->sh[s]].c_str(), ovlFTXManager::TAG), reinterpret_cast<unsigned long*>(&c_mesh->fts));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_txsmap[m_modellist[i]->sh[s]].c_str(), ovlTXSManager::TAG), reinterpret_cast<unsigned long*>(&c_mesh->TextureData));
        }

        if (c_model->EffectCount) {
            // Data transfer, matrices
//            c_model->EffectPosition = reinterpret_cast<D3DMATRIX*>(c_data);
//            c_data += c_model->EffectCount * sizeof(D3DMATRIX);
//            memcpy(c_model->EffectPosition, m_modellist[i]->EffectPosition, c_model->EffectCount * sizeof(D3DMATRIX));
//            m_relman->AddRelocation((unsigned long *)&c_model->EffectPosition);
//            DUMP_RELOCATION("ovlSHSManager::Make, Effect Positions", c_model->EffectPosition);
            c_model->EffectPosition = reinterpret_cast<D3DMATRIX*>(c_commondata);
            c_commondata += c_model->EffectCount * sizeof(D3DMATRIX);
            memcpy(c_model->EffectPosition, m_modellist[i]->EffectPosition, c_model->EffectCount * sizeof(D3DMATRIX));
            GetRelocationManager()->AddRelocation((unsigned long *)&c_model->EffectPosition);
            DUMP_RELOCATION("ovlSHSManager::Make, Effect Positions", c_model->EffectPosition);

            // Assign space for name pointers
//            c_model->EffectName = reinterpret_cast<char**>(c_data);
//            c_data += c_model->EffectCount * sizeof(char*);
//            m_relman->AddRelocation((unsigned long *)&c_model->EffectName);
//            DUMP_RELOCATION("ovlSHSManager::Make, Effect Names", c_model->EffectName);
            c_model->EffectName = reinterpret_cast<char**>(c_commondata);
            c_commondata += c_model->EffectCount * sizeof(char*);
            GetRelocationManager()->AddRelocation((unsigned long *)&c_model->EffectName);
            DUMP_RELOCATION("ovlSHSManager::Make, Effect Names", c_model->EffectName);

            for (unsigned long s = 0; s < c_model->EffectCount; ++s) {
//                // Data transfer, padded effect name
//                c_model->EffectName[s] = reinterpret_cast<char*>(c_data);
//                strcpy(reinterpret_cast<char*>(c_data), m_modellist[i]->EffectName[s]);
//                c_data += strlen(m_modellist[i]->EffectName[s]) + 1;
//                m_relman->AddRelocation((unsigned long *)&c_model->EffectName[s]);
//                DUMP_RELOCATION("ovlSHSManager::Make, Effect Name", c_model->EffectName[s]);
//
//                // Padding
//                unsigned long x = strlen(m_modellist[i]->EffectName[s]) + 1;
//                while (x % 4 != 0) {
//                    *c_data = 0;
//                    c_data++;
//                    x++;
//                }
                // Data transfer, padded effect name
                c_model->EffectName[s] = reinterpret_cast<char*>(c_commondata);
                strcpy(reinterpret_cast<char*>(c_commondata), m_modellist[i]->EffectName[s]);
                c_commondata += strlen(m_modellist[i]->EffectName[s]) + 1;
                GetRelocationManager()->AddRelocation((unsigned long *)&c_model->EffectName[s]);
                DUMP_RELOCATION("ovlSHSManager::Make, Effect Name", c_model->EffectName[s]);

                // Padding
                unsigned long x = strlen(m_modellist[i]->EffectName[s]) + 1;
                while (x % 4 != 0) {
                    *c_commondata = 0;
                    c_commondata++;
                    x++;
                }
            }

        }

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }

    return m_data;
}
*/
