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
#include "ManagerMAM.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlSVDManager::NAME = "SceneryItemVisual";
const char* ovlSVDManager::TAG = "svd";

void cSceneryItemVisualLOD::Fill(r3::SceneryItemVisualLOD* lod) {
    lod->type = meshtype;
    lod->shs_ref = NULL;
    lod->unk2 = unk2;
    lod->bsh_ref = NULL;
    lod->unk4 = unk4;
    lod->ftx_ref = NULL;
    lod->txs_ref = NULL;
    lod->unk7 = unk7;
    lod->unk8 = unk8;
    lod->unk9 = unk9;
    lod->unk10 = unk10;
    lod->unk11 = unk11;
    lod->unk12 = unk12;
    lod->distance = distance;
    lod->animation_count = animations.size();
    lod->unk14 = unk14;
}

void cSceneryItemVisual::Fill(r3::SceneryItemVisual_V* siv) {
    siv->sivflags = sivflags;
    siv->sway = sway;
    siv->brightness = brightness;
    siv->unk4 = unk4;
    siv->scale = scale;
    siv->lod_count = lods.size();
    siv->unk6 = unk6;
    siv->unk7 = unk7;
    siv->unk8 = unk8;
    siv->unk9 = unk9;
    siv->unk10 = unk10;
    siv->unk11 = unk11;
    for (unsigned long i = 0; i < lods.size(); ++i) {
        lods[i].Fill(siv->lods[i]);
    }
    if (sivflags & r3::Constants::SVD::Flags::Soaked) {
        reinterpret_cast<r3::SceneryItemVisual_S*>(siv)->s.proxy_ref = NULL;
    } else if (sivflags & r3::Constants::SVD::Flags::Wild) {
        reinterpret_cast<r3::SceneryItemVisual_W*>(siv)->s.proxy_ref = NULL;
        reinterpret_cast<r3::SceneryItemVisual_W*>(siv)->w.unk13 = unk13;
    }
}

void ovlSVDManager::AddSVD(const cSceneryItemVisual& item) {
    DUMP_LOG("Trace: ovlSVDManager::AddSVD(%s)", UNISTR(item.name.c_str()));
    Check("ovlSVDManager::AddSVD");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSVDManager::AddModel called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlSVDManager::AddSVD: Item with name '"+item.name+"' already exists"));

    m_items[item.name] = item;

    // SceneryItemVisual
    if (item.sivflags & r3::Constants::SVD::Flags::Soaked) {
        m_size += sizeof(SceneryItemVisual_S);
    } else if (item.sivflags & r3::Constants::SVD::Flags::Wild) {
        m_size += sizeof(SceneryItemVisual_W);
    } else {
        m_size += sizeof(SceneryItemVisual_V);
    }
	
	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.name, ovlSVDManager::TAG);	
	
	// ManifoldMesh Proxy
    if (item.sivflags & r3::Constants::SVD::Flags::Soaked_or_Wild) {
		if (item.proxy_ref != "") {
			loader.reserveSymbolReference(item.proxy_ref, ovlMAMManager::TAG, false);
			//fprintf(stderr, "\n\n\nHELLO\n\n\n");
		}
	}
    // SceneryItemVisualLOD pointers & structures
    m_size += item.lods.size() * (sizeof(SceneryItemVisualLOD*) + sizeof(SceneryItemVisualLOD));
	
	foreach(const cSceneryItemVisualLOD& lod, item.lods) {
        GetStringTable()->AddString(lod.name);
		
		loader.reserveSymbolReference(lod.staticshape, ovlSHSManager::TAG, false);
		loader.reserveSymbolReference(lod.boneshape, ovlBSHManager::TAG, false);
		loader.reserveSymbolReference(lod.fts, ovlFTXManager::TAG, false);
		loader.reserveSymbolReference(lod.txs, ovlTXSManager::TAG, false);
		
        // Animation array sizes, two pointers per animation
        m_size += lod.animations.size() * 8;
        // References
		foreach(const string& anim, lod.animations)
			loader.reserveSymbolReference(anim, ovlBANManager::TAG);
	}

/*
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
*/
}

void ovlSVDManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlSVDManager::Make()");
    Check("ovlSVDManager::Make");

    ovlOVLManager::Make(info);
    unsigned char* c_data = m_data;

    for (map<string, cSceneryItemVisual>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        // Data Transfer, SceneryItemVisual
        SceneryItemVisual_V* c_svd = reinterpret_cast<SceneryItemVisual_V*>(c_data);
        if (it->second.sivflags & r3::Constants::SVD::Flags::Soaked) {
            c_data += sizeof(SceneryItemVisual_S);
        } else if (it->second.sivflags & r3::Constants::SVD::Flags::Wild) {
            c_data += sizeof(SceneryItemVisual_W);
        } else {
            c_data += sizeof(SceneryItemVisual_V);
        }

        // Assign space for LOD pointers
        c_svd->lods = reinterpret_cast<SceneryItemVisualLOD**>(c_data);
        c_data += it->second.lods.size() * sizeof(SceneryItemVisualLOD*);
        GetRelocationManager()->AddRelocation((unsigned long *)&c_svd->lods);
        DUMP_RELOCATION("ovlSVDManager::Make, LodMeshes pointers", c_svd->lods);

        // Symbol and Loader
		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, it->first, ovlSVDManager::TAG, c_svd);

        for (unsigned long s = 0; s < it->second.lods.size(); ++s) {
            // Data Transfer, LOD
            c_svd->lods[s] = reinterpret_cast<SceneryItemVisualLOD*>(c_data);
            c_data += sizeof(SceneryItemVisualLOD);
            GetRelocationManager()->AddRelocation((unsigned long*)&c_svd->lods[s]);
            DUMP_RELOCATION("ovlSVDManager::Make, LodMesh", c_svd->lods[s]);

            // LodName
            c_svd->lods[s]->lod_name = GetStringTable()->FindString(it->second.lods[s].name.c_str());
            GetRelocationManager()->AddRelocation((unsigned long*)&c_svd->lods[s]->lod_name);
            DUMP_RELOCATION_STR("ovlSVDManager::Make, LodName", c_svd->lods[s]->lod_name);

            // StaticShape
			loader.assignSymbolReference(it->second.lods[s].staticshape, ovlSHSManager::TAG, &c_svd->lods[s]->shs_ref, false);

            // BoneShape
			loader.assignSymbolReference(it->second.lods[s].boneshape, ovlBSHManager::TAG, &c_svd->lods[s]->bsh_ref, false);

            // Billboard
			loader.assignSymbolReference(it->second.lods[s].fts, ovlFTXManager::TAG, &c_svd->lods[s]->ftx_ref, false);
			loader.assignSymbolReference(it->second.lods[s].txs, ovlTXSManager::TAG, &c_svd->lods[s]->txs_ref, false);

            // Animations
            if (it->second.lods[s].animations.size()) {
                // Allocate Space
                c_svd->lods[s]->animations_ref = reinterpret_cast<BoneAnim***>(c_data);
                c_data += it->second.lods[s].animations.size() * sizeof(BoneAnim**);
                GetRelocationManager()->AddRelocation((unsigned long *)&c_svd->lods[s]->animations_ref);
                DUMP_RELOCATION("ovlSVDManager::Make, AnimationArray pointers", c_svd->lods[s]->animations_ref);

                for (unsigned long a = 0; a < it->second.lods[s].animations.size(); ++a) {
                    // Allocate space for animation pointer
                    c_svd->lods[s]->animations_ref[a] = reinterpret_cast<BoneAnim**>(c_data);
                    c_data += sizeof(BoneAnim*);
                    *c_svd->lods[s]->animations_ref[a] = NULL;
                    GetRelocationManager()->AddRelocation((unsigned long *)&c_svd->lods[s]->animations_ref[a]);
                    DUMP_RELOCATION("ovlSVDManager::Make, AnimationArray", c_svd->lods[s]->animations_ref[a]);

                    // Symbol reference for ban name
					loader.assignSymbolReference(it->second.lods[s].animations[a], ovlBANManager::TAG, c_svd->lods[s]->animations_ref[a], false);
                }
            }

        }

        it->second.Fill(c_svd);
        if (it->second.sivflags & r3::Constants::SVD::Flags::Soaked_or_Wild) {
			if (it->second.proxy_ref != "")
				loader.assignSymbolReference(it->second.proxy_ref, ovlMAMManager::TAG, &reinterpret_cast<SceneryItemVisual_S*>(c_svd)->s.proxy_ref, false);
		}
		/*
        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), Tag()), reinterpret_cast<unsigned long*>(c_svd));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_svd), false, c_symbol);

        for (unsigned long s = 0; s < it->second.lods.size(); ++s) {
            // Data Transfer, LOD
            c_svd->lods[s] = reinterpret_cast<SceneryItemVisualLOD*>(c_data);
            c_data += sizeof(SceneryItemVisualLOD);
            GetRelocationManager()->AddRelocation((unsigned long*)&c_svd->lods[s]);
            DUMP_RELOCATION("ovlSVDManager::Make, LodMesh", c_svd->lods[s]);

            // LodName
            c_svd->lods[s]->lod_name = GetStringTable()->FindString(it->second.lods[s].name.c_str());
            GetRelocationManager()->AddRelocation((unsigned long*)&c_svd->lods[s]->lod_name);
            DUMP_RELOCATION_STR("ovlSVDManager::Make, LodName", c_svd->lods[s]->lod_name);

            // StaticShape
            if (it->second.lods[s].staticshape != "") {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.lods[s].staticshape.c_str(), ovlSHSManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_svd->lods[s]->shs_ref));
            }

            // BoneShape
            if (it->second.lods[s].boneshape != "") {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.lods[s].boneshape.c_str(), ovlBSHManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_svd->lods[s]->bsh_ref));
            }

            // Billboard
            if (it->second.lods[s].fts != "") {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.lods[s].fts.c_str(), ovlFTXManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_svd->lods[s]->ftx_ref));
            }
            if (it->second.lods[s].txs != "") {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.lods[s].txs.c_str(), ovlTXSManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_svd->lods[s]->txs_ref));
            }

            // Animations
            if (it->second.lods[s].animations.size()) {
                // Allocate Space
                c_svd->lods[s]->animations_ref = reinterpret_cast<BoneAnim***>(c_data);
                c_data += it->second.lods[s].animations.size() * sizeof(BoneAnim**);
                GetRelocationManager()->AddRelocation((unsigned long *)&c_svd->lods[s]->animations_ref);
                DUMP_RELOCATION("ovlSVDManager::Make, AnimationArray pointers", c_svd->lods[s]->animations_ref);

                for (unsigned long a = 0; a < it->second.lods[s].animations.size(); ++a) {
                    // Allocate space for animation pointer
                    c_svd->lods[s]->animations_ref[a] = reinterpret_cast<BoneAnim**>(c_data);
                    c_data += sizeof(BoneAnim*);
                    *c_svd->lods[s]->animations_ref[a] = NULL;
                    GetRelocationManager()->AddRelocation((unsigned long *)&c_svd->lods[s]->animations_ref[a]);
                    DUMP_RELOCATION("ovlSVDManager::Make, AnimationArray", c_svd->lods[s]->animations_ref[a]);

                    // Symbol reference for ban name
                    GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.lods[s].animations[a].c_str(), ovlBANManager::TAG),
                                         reinterpret_cast<unsigned long*>(c_svd->lods[s]->animations_ref[a]));
                }
            }

        }

        it->second.Fill(c_svd);

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
		*/
    }
}
