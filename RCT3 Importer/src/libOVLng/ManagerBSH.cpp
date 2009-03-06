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

#include <algorithm>

#include "ManagerBSH.h"

#include "ManagerFTX.h"
#include "OVLException.h"

using namespace r3;
using namespace std;


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


void cBoneShape2::Fill(BoneShapeMesh* bs2, uint32_t* vert, uint32_t* ind) {
    bs2->support_type = support;
    bs2->ftx_ref = NULL;
    bs2->txs_ref = NULL;
    bs2->transparency = placetexturing;
    bs2->texture_flags = textureflags;
    bs2->sides = sides;
    bs2->vertex_count = vertices.size();
    if (vert)
        *vert += vertices.size();
#ifndef GLASS_OLD
    if ((algo_x == cTriangleSortAlgorithm::NONE) || (algo_y == cTriangleSortAlgorithm::NONE) || (algo_z == cTriangleSortAlgorithm::NONE)) {
        algo_x = cTriangleSortAlgorithm::NONE;
        algo_y = cTriangleSortAlgorithm::NONE;
        algo_z = cTriangleSortAlgorithm::NONE;
    }

    if ((algo_x == cTriangleSortAlgorithm::NONE) && (placetexturing != 0))
        bs2->index_count = indices.size() / 3;
    else
#endif
    bs2->index_count = indices.size();
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
        bs2->vertexes[i] = vertices[i];
    }
#ifndef GLASS_OLD
    if ((placetexturing != 0) && (algo_x != cTriangleSortAlgorithm::NONE)) {
        vector<Triangle2> temptri(indices.size()/3);
        memcpy(&temptri[0], &indices[0], indices.size() * sizeof(unsigned short));
        sort(temptri.begin(), temptri.end(), cTriangle2Sorter(&vertices, cTriangleSortAlgorithm(algo_x, cTriangleSortAlgorithm::BY_X)));
        for(unsigned long i = 0; i < temptri.size(); ++i) {
            bs2->indices[i*3] = temptri[i].a;
            bs2->indices[(i*3)+1] = temptri[i].b;
            bs2->indices[(i*3)+2] = temptri[i].c;
        }
        sort(temptri.begin(), temptri.end(), cTriangle2Sorter(&vertices, cTriangleSortAlgorithm(algo_y, cTriangleSortAlgorithm::BY_Y)));
        for(unsigned long i = 0; i < temptri.size(); ++i) {
            bs2->indices[(i*3)+indices.size()] = temptri[i].a;
            bs2->indices[(i*3)+indices.size()+1] = temptri[i].b;
            bs2->indices[(i*3)+indices.size()+2] = temptri[i].c;
        }
        sort(temptri.begin(), temptri.end(), cTriangle2Sorter(&vertices, cTriangleSortAlgorithm(algo_z, cTriangleSortAlgorithm::BY_Z)));
        for(unsigned long i = 0; i < temptri.size(); ++i) {
            bs2->indices[(i*3)+(indices.size()*2)] = temptri[i].a;
            bs2->indices[(i*3)+(indices.size()*2)+1] = temptri[i].b;
            bs2->indices[(i*3)+(indices.size()*2)+2] = temptri[i].c;
        }
    } else {
        for(unsigned long i = 0; i < indices.size(); ++i) {
            bs2->indices[i] = indices[i];
        }
    }
#else
    for(unsigned long i = 0; i < indices.size(); ++i) {
        bs2->Triangles[i] = indices[i];
    }
#endif
}

void cBoneShape1::Fill(r3::BoneShape* bs1) {
    bs1->bounding_box_min = bbox1;
    bs1->bounding_box_max = bbox2;
    bs1->total_vertex_count = 0;
    bs1->total_index_count = 0;
    bs1->mesh_count2 = 0;
    bs1->mesh_count = meshes.size();
    for(unsigned long i = 0; i < meshes.size(); ++i) {
        if (meshes[i].support == r3::Constants::Mesh::SupportType::None)
            bs1->mesh_count2++;
        meshes[i].Fill(bs1->sh[i], &bs1->total_vertex_count, &bs1->total_index_count);
    }
    bs1->bone_count = bones.size();
    for(unsigned long i = 0; i < bones.size(); ++i) {
        bones[i].Fill(&bs1->bones[i]);
        bones[i].Fill1(&bs1->bone_positions1[i]);
        bones[i].Fill2(&bs1->bone_positions2[i]);
    }
}

void ovlBSHManager::AddModel(const cBoneShape1& item) {
    DUMP_LOG("Trace: ovlBSHManager::AddModel(%s)", UNISTR(item.name.c_str()));
    Check("ovlBSHManager::AddModel");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlBSHManager::AddModel called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlBSHManager::AddModel: Item with name '"+item.name+"' already exists"));

    m_items[item.name] = item;

    // One common blob per model
    m_blobs[item.name] = cOvlMemBlob(OVLT_COMMON, 4, 0);

    // BoneShape1
    m_size += sizeof(BoneShape);
    // BoneShape2 pointers
    m_size += item.meshes.size() * 4;

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
        m_size += sizeof(BoneShapeMesh);
        // Vertices
        m_blobs[item.name].size += it->vertices.size() * sizeof(VERTEX2);
        // Indices
        m_blobs[item.name].size += it->indices.size() * sizeof(unsigned short);
#ifndef GLASS_OLD
        if ((it->IsAlgoNone()) && (it->placetexturing != 0)) {
            if (it->indices.size() % 3)
                BOOST_THROW_EXCEPTION(EOvl("ovlBSHManager::AddModel: mesh has triangle sort algorithm none but indices aren't divisible by 3"));
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

        BoneShape* c_model = reinterpret_cast<BoneShape*>(c_data);
        c_data += sizeof(BoneShape);

        // Assign space for BoneShape2 pointers
        c_model->sh = reinterpret_cast<BoneShapeMesh**>(c_data);
        c_data += it->second.meshes.size() * 4;
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh));
        DUMP_RELOCATION("ovlBSHManager::Make, BoneShape2 pointers", c_model->sh);

        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), Tag()), reinterpret_cast<unsigned long*>(c_model));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_model), false, c_symbol);

        unsigned long s = 0;
        for (vector<cBoneShape2>::iterator itb = it->second.meshes.begin(); itb != it->second.meshes.end(); ++itb) {
            c_model->sh[s] = reinterpret_cast<BoneShapeMesh*>(c_data);
            c_data += sizeof(BoneShapeMesh);
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh[s]));
            DUMP_RELOCATION("ovlBSHManager::Make, BoneShape2", c_model->sh[s]);

            c_model->sh[s]->vertexes = reinterpret_cast<VERTEX2*>(c_commondata);
            c_commondata += itb->vertices.size() * sizeof(VERTEX2);
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh[s]->vertexes));
            DUMP_RELOCATION("ovlBSHManager::Make, Vertexes", c_model->sh[s]->vertexes);

            c_model->sh[s]->indices = reinterpret_cast<unsigned short*>(c_commondata);
            c_commondata += itb->indices.size() * sizeof(unsigned short);
#ifndef GLASS_OLD
            if ((itb->placetexturing != 0) && (!itb->IsAlgoNone())) {
                c_commondata += 2 * itb->indices.size() * sizeof(unsigned short);
            }
#endif
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->sh[s]->indices));
            DUMP_RELOCATION("ovlBSHManager::Make, Triangles", c_model->sh[s]->indices);

            // Symbol references
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(itb->fts.c_str(), ovlFTXManager::TAG), reinterpret_cast<unsigned long*>(&c_model->sh[s]->ftx_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(itb->texturestyle.c_str(), ovlTXSManager::TAG), reinterpret_cast<unsigned long*>(&c_model->sh[s]->txs_ref));

            s++;
        }

        c_model->bones = reinterpret_cast<BoneStruct*>(c_commondata);
        c_commondata += it->second.bones.size() * sizeof(BoneStruct);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->bones));
        DUMP_RELOCATION("ovlBSHManager::Make, Bones", c_model->bones);

        c_model->bone_positions1 = reinterpret_cast<MATRIX*>(c_commondata);
        c_commondata += it->second.bones.size() * sizeof(MATRIX);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->bone_positions1));
        DUMP_RELOCATION("ovlBSHManager::Make, BonePositions1", c_model->bone_positions1);

        c_model->bone_positions2 = reinterpret_cast<MATRIX*>(c_commondata);
        c_commondata += it->second.bones.size() * sizeof(MATRIX);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->bone_positions2));
        DUMP_RELOCATION("ovlBSHManager::Make, BonePositions2", c_model->bone_positions2);

        s = 0;
        for (vector<cBoneStruct>::iterator itb = it->second.bones.begin(); itb != it->second.bones.end(); ++itb) {
            c_model->bones[s].bone_name = GetStringTable()->FindString(itb->name.c_str());
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_model->bones[s].bone_name));
            DUMP_RELOCATION("ovlBSHManager::Make, BoneName", c_model->bones[s].bone_name);
            s++;
        }

        it->second.Fill(c_model);
        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }

}
