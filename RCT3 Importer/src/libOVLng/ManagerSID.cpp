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

#include "ManagerSID.h"

#include "pretty.h"

#include "ManagerCommon.h"
#include "ManagerGSI.h"
#include "ManagerSND.h"
#include "ManagerSVD.h"
#include "ManagerTXT.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlSIDManager::NAME = "SceneryItem";
const char* ovlSIDManager::TAG = "sid";

void cSidSquareUnknowns::Fill(r3::SceneryItemData* i) const {
    i->flags2 = flags;
    /*
    for (unsigned long x = 0; x<32; ++x) {
        if (flag[x])
            i->flags2 += (1 << x);
    }
    */
    i->unk2 = min_height;
    i->unk3 = max_height;

    int height = GetHeight();
    if (height) {
        for(int h = 0; h < (height/32); ++h)
            i->unk4[h] = 0xffffffff;
        if (height % 32)
            i->unk4[height / 32] = (1 << (height % 32)) - 1;
    } else
        i->unk4 = NULL;
    i->supports = supports;
}

/** @brief GetHeightSize
  *
  * @todo: document this function
  */
int cSidSquareUnknowns::GetHeightSize() const {
    int height = GetHeight();
    return ((height / 32) + (height % 32)?1:0) * 4;
}


void cSidImporterUnknowns::Fill(r3::SceneryItem_V* i) {
    i->flags = flags;
    /*
    for (unsigned long x = 0; x<32; ++x) {
        if (flag[x])
            i->flags += (1 << x);
    }
    */
    i->unk4 = unk1;
    i->unk17 = unk2;
}

void cSidUnknowns::Fill(r3::SceneryItem_V* i) {
    i->unk27 = unk27;
    i->unk28 = unk28;
    i->unk34 = unk34;
    i->unk35 = unk35;
    i->unk36 = unk36;
    i->unk37 = unk37;
    i->unk38 = unk38;
    i->unk39 = unk39;
    i->unk40 = unk40;
    i->unk41 = unk41;
    i->unk44 = unk44;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cSidFlatride::Fill(r3::SceneryItem_V* i) {
    i->individual_animation_anr_name_count = individual_animations.size();
    if (!individual_animations.size())
        i->individual_animation_anr_names = NULL;

    i->anr_age_alternatives = chunked_anr_unk1;
    i->anr_animation_chunks = chunked_anr_animation_chunks;
    i->anr_alternate_run_animations = chunked_anr_unk2;
    i->anr_animation_cycles_per_circuit = chunked_anr_unk3;
}

void cSidDefaultColours::Fill(r3::SceneryItem_V* i) {
    i->default_col1 = defaultcol[0];
    i->default_col2 = defaultcol[1];
    i->default_col3 = defaultcol[2];
}

void cSidPosition::Fill(r3::SceneryItem_V* i) {
    i->position_type = positioningtype;
    i->squares_x = xsquares;
    i->squares_z = zsquares;
    i->position_x = xpos;
    i->position_y = ypos;
    i->position_z = zpos;
    i->size_x = xsize;
    i->size_y = ysize;
    i->size_z = zsize;
}

void cSidUI::Fill(r3::SceneryItem_V* i) {
    i->type = type;
    i->cost = cost;
    i->removal_cost = removal_cost;
}

/** @brief GetCommonSize
  *
  * @todo: document this function
  */
unsigned long cSidSoundScript::GetCommonSize() const {
    switch (command) {
        case 0:
        case 1:
        case 2:
            return 8;
        case 3:
        case 4:
            return 16;
        default:
            BOOST_THROW_EXCEPTION(EOvl("Unknown sound script command"));
    }
}

/** @brief FillExtra
  *
  * @todo: document this function
  */
void cSidSound::FillExtra(r3::SceneryExtraSound& ses) const {
    ses.unk1 = extra_unkf[0];
    ses.unk2 = extra_unkf[1];
    ses.unk3 = extra_unk;
}

void cSidExtra::Fill(r3::SceneryItem_V* i) {
    i->structure_version = version;
    if (version == 1)
        Fill(reinterpret_cast<r3::SceneryItem_S*>(i));
    else if (version == 2)
        Fill(reinterpret_cast<r3::SceneryItem_W*>(i));
}

void cSidExtra::Fill(r3::SceneryItem_S* e) {
    e->s.unk2 = unk2;
    e->s.addon_pack = AddonPack;
    e->s.generic_addon = GenericAddon;
}

void cSidExtra::Fill(r3::SceneryItem_W* e) {
    Fill(reinterpret_cast<r3::SceneryItem_S*>(e));
    e->w.unkf = unkf;
    e->w.billboard_aspect = billboardaspect;
}

void cSid::Fill(r3::SceneryItem_V* i) {
    ui.Fill(i);
    position.Fill(i);
    colours.Fill(i);
    importerunknowns.Fill(i);
    stallunknowns.Fill(i);
    unknowns.Fill(i);
    flatride.Fill(i);
    extra.Fill(i);

    for (unsigned long x = 0; x < position.xsquares * position.zsquares; ++x) {
        if (squareunknowns.size() < position.xsquares * position.zsquares) {
            if (squareunknowns.size() == 0) {
                cSidSquareUnknowns uk;
                uk.Fill(&i->data[x]);
            } else {
                squareunknowns[0].Fill(&i->data[x]);
            }
        } else {
            squareunknowns[x].Fill(&i->data[x]);
        }
    }

    i->sound_count = sounds.size();

    // Unsupported stuff
    i->track_struct_count = 0;
    i->track_structs = NULL;
}

void ovlSIDManager::AddSID(const cSid& sid) {
    Check("ovlSIDManager::AddSID");
    if (sid.svds.size() == 0)
        BOOST_THROW_EXCEPTION(EOvl("ovlSIDManager::AddSID called without svds"));
    if (sid.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSIDManager::AddSID called without name"));
    if (m_sids.find(sid.name) != m_sids.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlSIDManager::AddSID: Item with name '"+sid.name+"' already exists"));
    if (sid.ovlpath == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSIDManager::AddSID called without ovlpath"));
    if (sid.ui.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlSIDManager::AddSID called without menu name"));
    if ((sid.squareunknowns.size()>1) && (sid.squareunknowns.size() != (sid.position.xsquares * sid.position.zsquares)))
        BOOST_THROW_EXCEPTION(EOvl("ovlSIDManager::AddSID called with illegal numer of square unknowns"));
    if (sid.extra.version > 2)
        BOOST_THROW_EXCEPTION(EOvl("ovlSIDManager::AddSID called without illegal structure version"));

    m_sids[sid.name] = sid;

    // Sizes
    // Main struct
    if (sid.extra.version == 1)
        m_size += sizeof(SceneryItem_S);
    else if (sid.extra.version == 2)
        m_size += sizeof(SceneryItem_W);
    else
        m_size += sizeof(SceneryItem_V);
    // SVD pointers
    m_size += sid.svds.size() * 4;
    // Data
    m_commonsize += sid.position.xsquares * sid.position.zsquares * sizeof(SceneryItemData);
    // Unknown 8
    if (sid.squareunknowns.size()) {
        if (sid.squareunknowns.size()>1) {
            for (unsigned long x = 0; x < sid.position.xsquares * sid.position.zsquares; ++x) {
                if (sid.squareunknowns[x].GetHeight()) {
                    m_commonsize += sid.squareunknowns[x].GetHeightSize();
                }
            }
        } else {
            if (sid.squareunknowns[0].GetHeight()) {
                m_commonsize += sid.position.xsquares * sid.position.zsquares * sid.squareunknowns[0].GetHeightSize();
            }
        }
    }
    // Params
    m_commonsize += sid.parameters.size() * sizeof(SceneryParams);
    // flat rides
    m_commonsize += sid.flatride.individual_animations.size() * 4; // char pointers
    // Sound
    m_size += sid.sounds.size() * sizeof(ScenerySound);
    if (sid.extra.version) {
        m_commonsize += sid.sounds.size() * sizeof(SceneryExtraSound);
    }
    foreach(const cSidSound& sound, sid.sounds) {
        m_size += sound.sounds.size() * 4;
        foreach(const string& sname, sound.sounds) {
            GetLSRManager()->AddSymRef(OVLT_UNIQUE);
            GetStringTable()->AddSymbolString(sname, ovlSNDManager::TAG);
        }
        m_commonsize += sound.animationscripts.size() * 4;
        foreach(const vector<cSidSoundScript>& scrvec, sound.animationscripts)
            foreach(const cSidSoundScript& scr, scrvec)
                m_commonsize += scr.GetCommonSize();
    }

    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(sid.name.c_str(), ovlSIDManager::TAG);
    GetStringTable()->AddString(sid.ovlpath.c_str());

    GetLSRManager()->AddSymRef(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(sid.ui.name.c_str(), ovlTXTManager::TAG);
    if (sid.ui.icon != "") {
        // Tracksections have no icon
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(sid.ui.icon.c_str(), ovlGSIManager::TAG);
    }
    GetStringTable()->AddString(sid.position.supports.c_str()); // Assign even if empty
    if ((sid.ui.group != "") && (sid.ui.groupicon != "")) {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(sid.ui.group.c_str(), ovlTXTManager::TAG);
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(sid.ui.groupicon.c_str(), ovlGSIManager::TAG);
    }
    for (vector<string>::const_iterator it = sid.svds.begin(); it != sid.svds.end(); ++it) {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(it->c_str(), ovlSVDManager::TAG);
    }
    for (vector<cSidParam>::const_iterator it = sid.parameters.begin(); it != sid.parameters.end(); ++it) {
        GetStringTable()->AddString(it->name.c_str());
        GetStringTable()->AddString(it->param.c_str());
    }
    foreach(const string& caranim, sid.flatride.individual_animations) {
        STRINGLIST_ADD(caranim, true);
    }

}

void ovlSIDManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlSIDManager::Make()");
    Check("ovlSIDManager::Make");

    m_blobs["0"] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    m_blobs["1"] = cOvlMemBlob(OVLT_COMMON, 2, m_commonsize);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs["0"].data;
    unsigned char* c_commondata = m_blobs["1"].data;

    for (map<string, cSid>::iterator it = m_sids.begin(); it != m_sids.end(); ++it) {
        // Assign structs
        SceneryItem_V* c_sid = reinterpret_cast<SceneryItem_V*>(c_data);

        // Assign extra if necessary
        if (it->second.extra.version == 1) {
            c_data += sizeof(SceneryItem_S);
        } else if (it->second.extra.version == 2) {
            c_data += sizeof(SceneryItem_W);
        } else {
            c_data += sizeof(SceneryItem_V);
        }

        // Assign svd pointers
        c_sid->svds_ref = reinterpret_cast<SceneryItemVisual_V**>(c_data);
        c_data += it->second.svds.size() * 4;
        memset(c_sid->svds_ref, 0, it->second.svds.size() * 4); // Set 0, symbolref targets
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->svds_ref));

        // Assign common
        c_sid->data = reinterpret_cast<SceneryItemData*>(c_commondata);
        c_commondata += it->second.position.xsquares * it->second.position.zsquares * sizeof(SceneryItemData);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->data));
        if (it->second.squareunknowns.size()) {
            for (unsigned long x = 0; x < it->second.position.xsquares * it->second.position.zsquares; ++x) {
                if (it->second.squareunknowns.size() >= (it->second.position.xsquares * it->second.position.zsquares)) {
                    if (it->second.squareunknowns[x].GetHeight()) {
                        c_sid->data[x].unk4 = reinterpret_cast<uint32_t*>(c_commondata);
                        c_commondata += it->second.squareunknowns[x].GetHeightSize();
                        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->data[x].unk4));
                    }
                } else {
                    if (it->second.squareunknowns[0].GetHeight()) {
                        c_sid->data[x].unk4 = reinterpret_cast<uint32_t*>(c_commondata);
                        c_commondata += it->second.squareunknowns[0].GetHeightSize();
                        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->data[x].unk4));
                    }
                }
            }
        }
        if (it->second.parameters.size()) {
            c_sid->params = reinterpret_cast<SceneryParams*>(c_commondata);
            c_commondata += it->second.parameters.size() * sizeof(SceneryParams);
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->params));
            c_sid->param_count = it->second.parameters.size();
        } else {
            c_sid->params = NULL;
            c_sid->param_count = 0;
        }
        if (it->second.flatride.individual_animations.size()) {
            c_sid->individual_animation_anr_names = reinterpret_cast<char**>(c_commondata);
            c_commondata += it->second.flatride.individual_animations.size() * 4;
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->individual_animation_anr_names));
        }

        it->second.Fill(c_sid);
        // 'Simple' strings
        c_sid->supports = GetStringTable()->FindString(it->second.position.supports.c_str());
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->supports));
        c_sid->ovl_path = GetStringTable()->FindString(it->second.ovlpath.c_str());
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->ovl_path));
        for (unsigned long c = 0; c < it->second.parameters.size(); ++c) {
            c_sid->params[c].type = GetStringTable()->FindString(it->second.parameters[c].name.c_str());
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->params[c].type));
            c_sid->params[c].params = GetStringTable()->FindString(it->second.parameters[c].param.c_str());
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->params[c].params));
        }
        for(unsigned long c = 0; c < it->second.flatride.individual_animations.size(); ++c) {
            STRINGLIST_ASSIGN(c_sid->individual_animation_anr_names[c], it->second.flatride.individual_animations[c], true, GetStringTable(), GetRelocationManager());
        }

        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->first.c_str(), TAG), reinterpret_cast<unsigned long*>(c_sid));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_sid), false, c_symbol);

        GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.ui.name.c_str(), ovlTXTManager::TAG),
                             reinterpret_cast<unsigned long*>(&c_sid->name_ref));
        if (it->second.ui.icon != "") {
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.ui.icon.c_str(), ovlGSIManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_sid->icon_ref));
        }
        if ((it->second.ui.group != "") && (it->second.ui.groupicon != "")) {
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.ui.group.c_str(), ovlTXTManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_sid->group_name_ref));
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.ui.groupicon.c_str(), ovlGSIManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_sid->group_icon_ref));
        }
        c_sid->svd_count = it->second.svds.size();
        for (unsigned long c = 0; c < it->second.svds.size(); ++c) {
            GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(it->second.svds[c].c_str(), ovlSVDManager::TAG),
                                 reinterpret_cast<unsigned long*>(&c_sid->svds_ref[c]));
        }

        // Sounds
        if (it->second.sounds.size()) {
            RELOC_ARRAY(it->second.sounds.size(), c_sid->sounds, ScenerySound, c_data);
            if (it->second.extra.version) {
                RELOC_ARRAY(it->second.sounds.size(), reinterpret_cast<SceneryItem_S*>(c_sid)->s.sounds_extra, SceneryExtraSound, c_commondata);
            }
            for(int s = 0; s < it->second.sounds.size(); ++s) {
                c_sid->sounds[s].sound_count = it->second.sounds[s].sounds.size();
                c_sid->sounds[s].sound_script_count = it->second.sounds[s].animationscripts.size();
                if (it->second.extra.version) {
                    it->second.sounds[s].FillExtra(reinterpret_cast<SceneryItem_S*>(c_sid)->s.sounds_extra[s]);
                }
                RELOC_ARRAY(it->second.sounds[s].sounds.size(), c_sid->sounds[s].sound_refs, Sound*, c_data);
                RELOC_ARRAY(it->second.sounds[s].animationscripts.size(), c_sid->sounds[s].sound_scripts, SoundScript*, c_commondata);
                for(int ss = 0; ss < it->second.sounds[s].sounds.size(); ++ss) {
                    SYMREF_MAKE(OVLT_UNIQUE, it->second.sounds[s].sounds[ss], ovlSNDManager::TAG, &c_sid->sounds[s].sound_refs[ss], true);
                }
                for(int an = 0; an < it->second.sounds[s].animationscripts.size(); ++an) {
                    if (it->second.sounds[s].animationscripts[an].size()) {
                        c_sid->sounds[s].sound_scripts[an] = reinterpret_cast<SoundScript*>(c_commondata);
                        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_sid->sounds[s].sound_scripts[an]));
                        int index = 0;
                        uint32_t* ints = reinterpret_cast<uint32_t*>(c_commondata);
                        float_t* floats = reinterpret_cast<float_t*>(c_commondata);
                        foreach(const cSidSoundScript& scr, it->second.sounds[s].animationscripts[an]) {
                            floats[index++] = scr.time;
                            ints[index++] = scr.command;
                            if (scr.command >= 3) {
                                floats[index++] = scr.parameter[0];
                                floats[index++] = scr.parameter[1];
                            }
                        }
                        c_commondata += index * 4;
                    }
                }
            }
        } else {
            c_sid->sounds = NULL;
            if (it->second.extra.version) {
                reinterpret_cast<SceneryItem_S*>(c_sid)->s.sounds_extra = NULL;
            }
        }

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
    }

}
