///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SVD structures
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

#include "ManagerSVD.h"

#include "ManagerCommon.h"
#include "ManagerBAN.h"
#include "ManagerBSH.h"
#include "ManagerSHS.h"
#include "ManagerFTX.h"
#include "OVLException.h"

using namespace r3;

const char* ovlSVDManager::NAME = "SceneryItemVisual";
const char* ovlSVDManager::TAG = "svd";

void ovlSVDManager::AddSVD(const cSceneryItemVisual& item) {
    DUMP_LOG("Trace: ovlSVDManager::AddSVD(%s)", UNISTR(item.name.c_str()));
    Check("ovlSVDManager::AddSVD");
    if (item.name == "")
        throw EOvl("ovlSVDManager::AddModel called without name");
    if (m_items.find(item.name) != m_items.end())
        throw EOvl("ovlSVDManager::AddSVD: Item with name '"+item.name+"' already exists");

    m_items[item.name] = item;

    // SceneryItemVisual
    m_size += sizeof(SceneryItemVisual);
    // SceneryItemVisualLOD pointers & structures
    m_size += item.lods.size() * (sizeof(SceneryItemVisualLOD*) + sizeof(SceneryItemVisualLOD));

    // walk the lods
    for (vector<cSceneryItemVisualLOD>::const_iterator it = item.lods.begin(); it != item.lods.end(); ++it) {
        GetStringTable()->AddString(it->name.c_str());

        // StaticShape
        if (it->staticshape != "") {
            GetStringTable()->AddSymbolString(it->staticshape.c_str(), ovlSHSManager::TAG);
            GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        }

        // BoneShape
        if (it->boneshape != "") {
            GetStringTable()->AddSymbolString(it->boneshape.c_str(), ovlBSHManager::TAG);
            GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        }

        // Billboard
        if (it->fts != "") {
            GetStringTable()->AddSymbolString(it->fts.c_str(), ovlFTXManager::TAG);
            GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        }
        if (it->txs != "") {
            GetStringTable()->AddSymbolString(it->txs.c_str(), ovlTXSManager::TAG);
            GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        }

        // Animation array sizes, two pointers per animation
        m_size += it->animations.size() * 8;
        // References
        for (unsigned long i = 0; i < it->animations.size(); ++i) {
            GetStringTable()->AddSymbolString(it->animations[i].c_str(), ovlBANManager::TAG);
            GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        }
    }

    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlSVDManager::TAG);
}

void ovlSVDManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlSVDManager::Make()");
    Check("ovlSVDManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    for (map<string, cSceneryItemVisual>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        // Data Transfer, SceneryItemVisual
        SceneryItemVisual* c_svd = reinterpret_cast<SceneryItemVisual*>(c_data);
        c_data += sizeof(SceneryItemVisual);

        // Assign space for LOD pointers
        c_svd->LODMeshes = reinterpret_cast<SceneryItemVisualLOD**>(c_data);
        c_data += it->second.lods.size() * sizeof(SceneryItemVisualLOD*);
        GetRelocationManager()->AddRelocation((unsigned long *)&c_svd->LODMeshes);
        DUMP_RELOCATION("ovlSVDManager::Make, LodMeshes pointers", c_svd->LODMeshes);

        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), Tag()), reinterpret_cast<unsigned long*>(c_svd));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_svd), false, c_symbol);

        for (unsigned long s = 0; s < it->second.lods.size(); ++s) {
            // Data Transfer, LOD
            c_svd->LODMeshes[s] = reinterpret_cast<SceneryItemVisualLOD*>(c_data);
            c_data += sizeof(SceneryItemVisualLOD);
            GetRelocationManager()->AddRelocation((unsigned long*)&c_svd->LODMeshes[s]);
            DUMP_RELOCATION("ovlSVDManager::Make, LodMesh", c_svd->LODMeshes[s]);

            // LodName
            c_svd->LODMeshes[s]->LODName = GetStringTable()->FindString(it->second.lods[s].name.c_str());
            GetRelocationManager()->AddRelocation((unsigned long*)&c_svd->LODMeshes[s]->LODName);
            DUMP_RELOCATION_STR("ovlSVDManager::Make, LodName", c_svd->LODMeshes[s]->LODName);

            // StaticShape
            if (it->second.lods[s].staticshape != "") {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.lods[s].staticshape.c_str(), ovlSHSManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_svd->LODMeshes[s]->ss));
            }

            // BoneShape
            if (it->second.lods[s].boneshape != "") {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.lods[s].boneshape.c_str(), ovlBSHManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_svd->LODMeshes[s]->bs));
            }

            // Billboard
            if (it->second.lods[s].fts != "") {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.lods[s].fts.c_str(), ovlFTXManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_svd->LODMeshes[s]->fts));
            }
            if (it->second.lods[s].txs != "") {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.lods[s].txs.c_str(), ovlTXSManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_svd->LODMeshes[s]->TextureData));
            }

            // Animations
            if (it->second.lods[s].animations.size()) {
                // Allocate Space
                c_svd->LODMeshes[s]->AnimationArray = reinterpret_cast<BoneAnim***>(c_data);
                c_data += it->second.lods[s].animations.size() * sizeof(BoneAnim**);
                GetRelocationManager()->AddRelocation((unsigned long *)&c_svd->LODMeshes[s]->AnimationArray);
                DUMP_RELOCATION("ovlSVDManager::Make, AnimationArray pointers", c_svd->LODMeshes[s]->AnimationArray);

                for (unsigned long a = 0; a < it->second.lods[s].animations.size(); ++a) {
                    // Allocate space for animation pointer
                    c_svd->LODMeshes[s]->AnimationArray[a] = reinterpret_cast<BoneAnim**>(c_data);
                    c_data += sizeof(BoneAnim*);
                    *c_svd->LODMeshes[s]->AnimationArray[a] = NULL;
                    GetRelocationManager()->AddRelocation((unsigned long *)&c_svd->LODMeshes[s]->AnimationArray[a]);
                    DUMP_RELOCATION("ovlSVDManager::Make, AnimationArray", c_svd->LODMeshes[s]->AnimationArray[a]);

                    // Symbol reference for ban name
                    GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.lods[s].animations[a].c_str(), ovlBANManager::TAG),
                                         reinterpret_cast<unsigned long*>(c_svd->LODMeshes[s]->AnimationArray[a]));
                }
            }

        }

        it->second.Fill(c_svd);

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }
}

/*
ovlSVDManager::~ovlSVDManager() {
    for (unsigned long i = 0; i < m_svdlist.size(); ++i) {
        for (unsigned long s = 0; s < m_svdlist[i]->LODCount; ++s) {
            delete[] m_svdlist[i]->LODMeshes[s]->LODName;
            delete m_svdlist[i]->LODMeshes[s];
        }
        delete[] m_svdlist[i]->LODMeshes;
        delete m_svdlist[i];
    }
}

void ovlSVDManager::AddSVD(const char* name, unsigned long lods, unsigned long flags, float sway, float brightness, float scale) {
    DUMP_LOG("Trace: ovlSVDManager::AddSVD(name:'%s', lods:%ld, flags:%ld, sway:%f, blightness:%f, scale:%f)", UNISTR(name), lods, flags, sway, brightness, scale);
    Check("ovlSVDManager::AddSVD");
    if (m_lodcount)
        throw EOvl("ovlSVDManager::AddSVD called but last svd misses lods");
    if (m_animcount)
        throw EOvl("ovlSVDManager::AddSVD called but last lod misses animations");

    SceneryItemVisual* c_svd = new SceneryItemVisual;
    c_svd->sivflags = flags;
    c_svd->sway = sway;
    c_svd->brightness = brightness;
    c_svd->unk4 = 1.0;
    c_svd->scale = scale;
    c_svd->LODCount = lods;
    c_svd->LODMeshes = new SceneryItemVisualLOD*[c_svd->LODCount];
    c_svd->unk6 = 0;
    c_svd->unk7 = 0;
    c_svd->unk8 = 0;
    c_svd->unk9 = 0;
    c_svd->unk10 = 0;
    c_svd->unk11 = 0;

    // SceneryItemVisual
    m_size += sizeof(SceneryItemVisual);
    // SceneryItemVisualLOD pointers
    m_size += c_svd->LODCount * sizeof(SceneryItemVisualLOD*);

    m_svdlist.push_back(c_svd);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    m_svdnames.push_back(string(name));
    GetStringTable()->AddSymbolString(name, Tag());

    m_csvd = c_svd;
    m_nlod = 0;
    m_lodcount = c_svd->LODCount;
    m_animcount = 0;
}

void ovlSVDManager::SetSVDUnknowns(float unk4, unsigned long unk6, unsigned long unk7, unsigned long unk8, unsigned long unk9, unsigned long unk10, unsigned long unk11) {
    DUMP_LOG("Trace: ovlSVDManager::SetSVDUnknowns(%f %ld %ld %ld %ld %ld %ld)", unk4, unk6, unk7, unk8, unk9, unk10, unk11);
    Check("ovlSVDManager::SetSVDUnknowns");
    if (!m_csvd)
        throw EOvl("ovlSVDManager::SetSVDUnknowns called but there is no svd pointer");

    m_csvd->unk4 = unk4;
    m_csvd->unk6 = unk6;
    m_csvd->unk7 = unk7;
    m_csvd->unk8 = unk8;
    m_csvd->unk9 = unk9;
    m_csvd->unk10 = unk10;
    m_csvd->unk11 = unk11;
}

void ovlSVDManager::AddStaticLOD(const char* name, const char* modelname, float distance, unsigned long unk2, unsigned long unk4, unsigned long unk14) {
    DUMP_LOG("Trace: ovlSVDManager::AddStaticLOD(name:'%s', modelname:'%s', distance:%f, unk2:%ld, Unk4:%ld, unk14:%ld)", UNISTR(name), UNISTR(modelname), distance, unk2, unk4, unk14);
    Check("ovlSVDManager::AddStaticLOD");
    if (!m_lodcount)
        throw EOvl("ovlSVDManager::AddStaticLOD called but there are no lods left");
    if (m_animcount)
        throw EOvl("ovlSVDManager::AddStaticLOD called but last lod misses animations");
    if (!m_csvd)
        throw EOvl("ovlSVDManager::AddStaticLOD called but there is no svd pointer");

    SceneryItemVisualLOD* c_lod = new SceneryItemVisualLOD;
    m_csvd->LODMeshes[m_nlod] = c_lod;

    c_lod->MeshType = SVDLOD_MESHTYPE_STATIC;
    c_lod->LODName = new char[strlen(name)+1];
    strcpy(c_lod->LODName, name);
    c_lod->StaticShape = 0;
    c_lod->unk2 = unk2;
    c_lod->BoneShape = 0;
    c_lod->unk4 = unk4;
    c_lod->fts = 0;
    c_lod->TextureData = 0;
    c_lod->unk7 = 1.0;
    c_lod->unk8 = 1.0;
    c_lod->unk9 = 0;
    c_lod->unk10 = 1.0;
    c_lod->unk11 = 0;
    c_lod->unk12 = 1.0;
    c_lod->distance = distance;
    c_lod->AnimationCount = 0;
    c_lod->unk14 = unk14;
    c_lod->AnimationArray = 0;

    // SceneryItemVisualLOD
    m_size += sizeof(SceneryItemVisualLOD);

    GetStringTable()->AddString(name);
    m_modelmap[c_lod] = string(modelname);
    GetStringTable()->AddSymbolString(modelname, ovlSHSManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);

    m_nlod++;
    m_lodcount--;
}

void ovlSVDManager::OpenAnimatedLOD(const char* name, const char* modelname, unsigned long animations, float distance, unsigned long unk2, unsigned long unk4, unsigned long unk14) {
    DUMP_LOG("Trace: ovlSVDManager::OpenAnimatedLOD(name:'%s', modelname:'%s', animations:%ld, distance:%f, unk2:%ld, Unk4:%ld, unk14:%ld)", UNISTR(name), UNISTR(modelname), animations, distance, unk2, unk4, unk14);
    Check("ovlSVDManager::OpenAnimatedLOD");
    if (!m_lodcount)
        throw EOvl("ovlSVDManager::OpenAnimatedLOD called but there are no lods left");
    if (m_animcount)
        throw EOvl("ovlSVDManager::OpenAnimatedLOD called but last lod misses animations");
    if (!m_csvd)
        throw EOvl("ovlSVDManager::OpenAnimatedLOD called but there is no svd pointer");

    SceneryItemVisualLOD* c_lod = new SceneryItemVisualLOD;
    m_csvd->LODMeshes[m_nlod] = c_lod;

    c_lod->MeshType = SVDLOD_MESHTYPE_ANIMATED;
    c_lod->LODName = new char[strlen(name)+1];
    strcpy(c_lod->LODName, name);
    c_lod->StaticShape = 0;
    c_lod->unk2 = unk2;
    c_lod->BoneShape = 0;
    c_lod->unk4 = unk4;
    c_lod->fts = 0;
    c_lod->TextureData = 0;
    c_lod->unk7 = 1.0;
    c_lod->unk8 = 1.0;
    c_lod->unk9 = 0;
    c_lod->unk10 = 1.0;
    c_lod->unk11 = 0;
    c_lod->unk12 = 1.0;
    c_lod->distance = distance;
    c_lod->AnimationCount = animations;
    c_lod->unk14 = unk14;
    c_lod->AnimationArray = NULL; // No need to allocate a list of useless pointers here

    // SceneryItemVisualLOD
    m_size += sizeof(SceneryItemVisualLOD);

    // Pointers
    m_size += c_lod->AnimationCount * sizeof(BoneAnim**);

    GetStringTable()->AddString(name);
    m_modelmap[c_lod] = string(modelname);
    GetStringTable()->AddSymbolString(modelname, ovlBSHManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);

    m_animcount = c_lod->AnimationCount;
}

void ovlSVDManager::CloseAnimatedLOD() {
    DUMP_LOG("Trace: ovlSVDManager::CloseAnimatedLOD()");
    Check("ovlSVDManager::CloseAnimatedLOD");
    if (m_animcount)
        throw EOvl("ovlSVDManager::CloseAnimatedLOD called but last lod misses animations");

    m_nlod++;
    m_lodcount--;
}

void ovlSVDManager::AddAnimation(const char* name) {
    DUMP_LOG("Trace: ovlSVDManager::AddAnimation(name:'%s')", UNISTR(name));
    Check("ovlSVDManager::AddAnimation");
    if (!m_animcount)
        throw EOvl("ovlSVDManager::AddAnimation called but no animations left");
    if (!m_csvd)
        throw EOvl("ovlSVDManager::AddAnimation called but there is no svd pointer");

    // No direct assignment

    // Pointer
    m_size += 4;

    m_animationmap[m_csvd->LODMeshes[m_nlod]].push_back(string(name));
    GetStringTable()->AddSymbolString(name, ovlBANManager::TAG);
    GetLSRManager()->AddSymRef(OVLT_UNIQUE);

    m_animcount--;
}

void ovlSVDManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlSVDManager::Make()");
    Check("ovlSVDManager::Make");
    if (m_lodcount)
        throw EOvl("ovlSVDManager::Make called but last svd misses lods");
    if (m_animcount)
        throw EOvl("ovlSVDManager::Make called but last lod misses animations");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    for (unsigned long i = 0; i < m_svdlist.size(); ++i) {
        // Data Transfer, SceneryItemVisual
        SceneryItemVisual* c_svd = reinterpret_cast<SceneryItemVisual*>(c_data);
        c_data += sizeof(SceneryItemVisual);
        memcpy(c_svd, m_svdlist[i], sizeof(SceneryItemVisual));

        // Assign space for LOD pointers
        c_svd->LODMeshes = reinterpret_cast<SceneryItemVisualLOD**>(c_data);
        c_data += c_svd->LODCount * sizeof(SceneryItemVisualLOD*);
        GetRelocationManager()->AddRelocation((unsigned long *)&c_svd->LODMeshes);
        DUMP_RELOCATION("ovlSVDManager::Make, LodMeshes pointers", c_svd->LODMeshes);

        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_svdnames[i].c_str(), Tag()), reinterpret_cast<unsigned long*>(c_svd));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_svd), false, c_symbol);

        for (unsigned long s = 0; s < m_svdlist[i]->LODCount; ++s) {
            // Data Transfer, LOD
            c_svd->LODMeshes[s] = reinterpret_cast<SceneryItemVisualLOD*>(c_data);
            c_data += sizeof(SceneryItemVisualLOD);
            memcpy(c_svd->LODMeshes[s], m_svdlist[i]->LODMeshes[s], sizeof(SceneryItemVisualLOD));
            GetRelocationManager()->AddRelocation((unsigned long*)&c_svd->LODMeshes[s]);
            DUMP_RELOCATION("ovlSVDManager::Make, LodMesh", c_svd->LODMeshes[s]);

            // LodName
            c_svd->LODMeshes[s]->LODName = GetStringTable()->FindString(m_svdlist[i]->LODMeshes[s]->LODName);
            GetRelocationManager()->AddRelocation((unsigned long*)&c_svd->LODMeshes[s]->LODName);
            DUMP_RELOCATION_STR("ovlSVDManager::Make, LodName", c_svd->LODMeshes[s]->LODName);

            // LOD type specific stuff
            switch (c_svd->LODMeshes[s]->MeshType) {
                case SVDLOD_MESHTYPE_STATIC: {
                        // Symbol reference for shs name
                        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_modelmap[m_svdlist[i]->LODMeshes[s]].c_str(), ovlSHSManager::TAG),
                                             reinterpret_cast<unsigned long*>(&c_svd->LODMeshes[s]->StaticShape));
                    }
                    break;
                case SVDLOD_MESHTYPE_ANIMATED: {
                        // Symbol reference for bsh name
                        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_modelmap[m_svdlist[i]->LODMeshes[s]].c_str(), ovlBSHManager::TAG),
                                             reinterpret_cast<unsigned long*>(&c_svd->LODMeshes[s]->BoneShape));

                        // Animations
                        if (c_svd->LODMeshes[s]->AnimationCount) {
                            // Allocate space for the pointers
                            c_svd->LODMeshes[s]->AnimationArray = reinterpret_cast<BoneAnim***>(c_data);
                            c_data += c_svd->LODMeshes[s]->AnimationCount * sizeof(BoneAnim**);
                            GetRelocationManager()->AddRelocation((unsigned long *)&c_svd->LODMeshes[s]->AnimationArray);
                            DUMP_RELOCATION("ovlSVDManager::Make, AnimationArray pointers", c_svd->LODMeshes[s]->AnimationArray);

                            for (unsigned long a = 0; a < m_svdlist[i]->LODMeshes[s]->AnimationCount; ++a) {
                                // Allocate space for animation pointer
                                c_svd->LODMeshes[s]->AnimationArray[a] = reinterpret_cast<BoneAnim**>(c_data);
                                c_data += sizeof(BoneAnim*);
                                GetRelocationManager()->AddRelocation((unsigned long *)&c_svd->LODMeshes[s]->AnimationArray[a]);
                                DUMP_RELOCATION("ovlSVDManager::Make, AnimationArray", c_svd->LODMeshes[s]->AnimationArray[a]);

                                // Symbol reference for ban name
                                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(m_animationmap[m_svdlist[i]->LODMeshes[s]][a].c_str(), ovlBANManager::TAG),
                                                     reinterpret_cast<unsigned long*>(c_svd->LODMeshes[s]->AnimationArray[a]));
                            }
                        }
                    }
                    break;
            }
        }

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }
}
*/
