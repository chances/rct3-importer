///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for TKS structures
// Copyright (C) 2008 Tobias Minch
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

#include <boost/algorithm/string.hpp>

#include "pretty.h"

#include "ManagerTKS.h"

#include "ManagerSPL.h"
#include "ManagerSID.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlTKSManager::LOADER = "FGDK";
const char* ovlTKSManager::NAME = "TrackSection";
const char* ovlTKSManager::TAG = "tks";

#define DO_ASSIGN(var, subvar)  var.subvar = subvar

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSectionSpeed::Fill(r3::TrackSectionSpeedStruct_SW& tkss) const {
    Fill(tkss.v);
    tkss.s.unk4 = unk04;
    tkss.s.unk5 = unk05;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSectionSpeed::Fill(r3::TrackSectionSpeedStruct_V& tkss) const {
    tkss.unk1 = unk01;
    tkss.unk2 = unk02;
    tkss.unk3 = unk03;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSectionSpeedSplines::Fill(r3::TrackSectionSpeedSplines& tkss) const {
    tkss.speed_selector = speed;
}

/** @brief GetUniqueSize
  *
  * @todo: document this function
  */
unsigned long cTrackSection::GetUniqueSize() const {
    unsigned long size = 0;
    if (version) {
        if (version == 2) {
            size += sizeof(TrackSection_S);
            size += 10 * 4; // Animation
        } else {
            size += sizeof(TrackSection_W);
            size += 11 * 4; // Animation
        }
        size += speeds.size() * sizeof(TrackSectionSpeedStruct_SW);
        size += soaked.paths.size() * 4;
    } else {
        size += sizeof(TrackSection_V);
        size += speeds.size() * sizeof(TrackSectionSpeedStruct_V);
    }
    return size;
}

/** @brief GetCommonSize
  *
  * @todo: document this function
  */
unsigned long cTrackSection::GetCommonSize() const {
    if (!version) {
        return 0;
    }
    unsigned long size = 0;
    size += soaked.ride_station_limits.size() * sizeof(RideStationLimit);
    size += soaked.speeds.size() * sizeof(TrackSectionSpeedSplines);
    size += soaked.groups_is_at_entry.size() * 4;
    size += soaked.groups_is_at_exit.size() * 4;
    size += soaked.groups_must_have_at_entry.size() * 4;
    size += soaked.groups_must_have_at_exit.size() * 4;
    size += soaked.groups_must_not_be_at_entry.size() * 4;
    size += soaked.groups_must_not_be_at_exit.size() * 4;
    return size;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::Fill(r3::TrackSection_S* tks, ovlStringTable* tab, ovlRelocationManager* rel) const {
    tks->s.version = version;
    tks->v.entry_flags |= r3::Constants::TKS::Flags::Extended_Structure;
    tks->v.exit_flags |= r3::Constants::TKS::Flags::Extended_Structure;
    unknowns.Fill(*tks);
    soaked.Fill(*tks, tab, rel);
    tks->v.speed_count = speeds.size();
    for(int i = 0; i < speeds.size(); ++i) {
        speeds[i].Fill(tks->v.speeds_sw[i]);
    }
    if (version == 2) {
        tks->v.anim_count = 10;
        animations.Fill(*tks->v.anim_ex_s);
    } else {
        tks->v.anim_count = 11;
        animations.Fill(*tks->v.anim_ex_w);
        wild.Fill(*reinterpret_cast<TrackSection_W*>(tks), tab, rel);
    }
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::Fill(r3::TrackSection_V* tks, ovlStringTable* tab, ovlRelocationManager* rel) const {
    STRINGLIST_ASSIGN(tks->internalname, internalname, false, tab, rel);
    basic.Fill(*tks, tab, rel);
    splines.Fill(*tks);
    options.Fill(*tks, tab, rel);
    if (!version) {
        animations.Fill(tks->anim);
        unknowns.Fill(*tks);
        tks->entry_flags &= ~r3::Constants::TKS::Flags::Extended_Structure;
        tks->exit_flags &= ~r3::Constants::TKS::Flags::Extended_Structure;
        tks->speed_count = speeds.size();
        for(int i = 0; i < speeds.size(); ++i) {
            speeds[i].Fill(tks->speeds[i]);
        }
    } else {
        Fill(reinterpret_cast<TrackSection_S*>(tks), tab, rel);
    }
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::WildOptions::Fill(r3::TrackSection_W& tva, ovlStringTable* tab, ovlRelocationManager* rel) const {
    DO_ASSIGN(tva.w, splitter_half);
    DO_ASSIGN(tva.w, rotator_type);
    DO_ASSIGN(tva.w, animal_house);
    STRINGLIST_ASSIGN(tva.w.alternate_text_lookup, alternate_text_lookup, false, tab, rel);
    DO_ASSIGN(tva.w, tower_cap01);
    DO_ASSIGN(tva.w, tower_cap02);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::SoakedOptions::Fill(r3::TrackSection_S& tva, ovlStringTable* tab, ovlRelocationManager* rel) const {
    tva.s.ride_station_limit_count = ride_station_limits.size();
    for(int i = 0; i < ride_station_limits.size(); ++i) {
        ride_station_limits[i].Fill(&tva.s.ride_station_limits[i]);
    }
    DO_ASSIGN(tva.s, tower_unkf2);
    DO_ASSIGN(tva.s, aquarium_val);
    STRINGLIST_ASSIGN(tva.s.auto_group, auto_group, false, tab, rel);
    DO_ASSIGN(tva.s, giant_flume_val);
    DO_ASSIGN(tva.s, entry_wide_flag);
    DO_ASSIGN(tva.s, exit_wide_flag);
    tva.s.speed_spline_count = speeds.size();
    for(int i = 0; i < speeds.size(); ++i) {
        speeds[i].Fill(tva.s.speed_splines[i]);
    }
    DO_ASSIGN(tva.s, speed_spline_val);
    DO_ASSIGN(tva.s, slide_end_to_lifthill_val);
    DO_ASSIGN(tva.s, soaked_options);
    tva.s.group_is_at_entry_count = groups_is_at_entry.size();
    for(int i = 0; i < groups_is_at_entry.size(); ++i) {
        STRINGLIST_ASSIGN(tva.s.groups_is_at_entry[i], groups_is_at_entry[i], false, tab, rel);
    }
    tva.s.group_is_at_exit_count = groups_is_at_exit.size();
    for(int i = 0; i < groups_is_at_exit.size(); ++i) {
        STRINGLIST_ASSIGN(tva.s.groups_is_at_exit[i], groups_is_at_exit[i], false, tab, rel);
    }
    tva.s.group_must_have_at_entry_count = groups_must_have_at_entry.size();
    for(int i = 0; i < groups_must_have_at_entry.size(); ++i) {
        STRINGLIST_ASSIGN(tva.s.groups_must_have_at_entry[i], groups_must_have_at_entry[i], false, tab, rel);
    }
    tva.s.group_must_have_at_exit_count = groups_must_have_at_exit.size();
    for(int i = 0; i < groups_must_have_at_exit.size(); ++i) {
        STRINGLIST_ASSIGN(tva.s.groups_must_have_at_exit[i], groups_must_have_at_exit[i], false, tab, rel);
    }
    tva.s.group_must_not_be_at_entry_count = groups_must_not_be_at_entry.size();
    for(int i = 0; i < groups_must_not_be_at_entry.size(); ++i) {
        STRINGLIST_ASSIGN(tva.s.groups_must_not_be_at_entry[i], groups_must_not_be_at_entry[i], false, tab, rel);
    }
    tva.s.group_must_not_be_at_exit_count = groups_must_not_be_at_exit.size();
    for(int i = 0; i < groups_must_not_be_at_exit.size(); ++i) {
        STRINGLIST_ASSIGN(tva.s.groups_must_not_be_at_exit[i], groups_must_not_be_at_exit[i], false, tab, rel);
    }
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::Options::Fill(r3::TrackSection_V& tva, ovlStringTable* tab, ovlRelocationManager* rel) const {
    DO_ASSIGN(tva, tower_ride_base_flag);
    DO_ASSIGN(tva, tower_unkf01);
    DO_ASSIGN(tva, water_splash01);
    DO_ASSIGN(tva, water_splash02);
    DO_ASSIGN(tva, reverser_val);
    DO_ASSIGN(tva, elevator_top_val);
    DO_ASSIGN(tva, rapids01);
    DO_ASSIGN(tva, rapids02);
    DO_ASSIGN(tva, rapids03);
    DO_ASSIGN(tva, whirlpool01);
    DO_ASSIGN(tva, whirlpool02);
    STRINGLIST_ASSIGN(tva.chair_lift_station_end, chair_lift_station_end, false, tab, rel);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::Unknowns::Fill(r3::TrackSection_V& tva) const {
    DO_ASSIGN(tva, unk15);
    DO_ASSIGN(tva, unk16);
    DO_ASSIGN(tva, unk17);
    DO_ASSIGN(tva, unk22);
    DO_ASSIGN(tva, unk23);
    DO_ASSIGN(tva, unk24);
    DO_ASSIGN(tva, unk45);
    DO_ASSIGN(tva, unk47);
    DO_ASSIGN(tva, unk53);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::Unknowns::Fill(r3::TrackSection_S& tva) const {
    Fill(tva.v);
    DO_ASSIGN(tva.s, unk68);
    DO_ASSIGN(tva.s, unk69);
    DO_ASSIGN(tva.s, unk77);
    DO_ASSIGN(tva.s, unk78);
    DO_ASSIGN(tva.s, unk79);
}


/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::Animations::Fill(r3::TrackSectionAnimation_W& tva) const {
    Fill(reinterpret_cast<TrackSectionAnimation_S&>(tva));
    DO_ASSIGN(tva.w, rotating_tower_idle);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::Animations::Fill(r3::TrackSectionAnimation_S& tva) const {
    Fill(tva.v);
    DO_ASSIGN(tva.s, pre_station_leave);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::Animations::Fill(r3::TrackSectionAnimation_V& tva) const {
    DO_ASSIGN(tva, stopped);
    DO_ASSIGN(tva, starting);
    DO_ASSIGN(tva, running);
    DO_ASSIGN(tva, stopping);
    DO_ASSIGN(tva, hold_after_train_stop);
    DO_ASSIGN(tva, hold_loop);
    DO_ASSIGN(tva, hold_before_release);
    DO_ASSIGN(tva, hold_leaving);
    DO_ASSIGN(tva, hold_after_train_left);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::Splines::Fill(r3::TrackSection_V& tva) const {
    if ((water.left != "") && (water.right != ""))
        tva.water_spline_flag = 1;
    else
        tva.water_spline_flag = 0;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackSection::Basic::Fill(r3::TrackSection_V& tva, ovlStringTable* tab, ovlRelocationManager* rel) const {
    tva.entry_curve = entry.curve;
    tva.exit_curve = exit.curve;
    tva.special_curves = special;
    tva.direction = direction;
    tva.entry_flags = entry.flags;
    tva.exit_flags = exit.flags;
    tva.unk18 = entry.unknown;
    tva.entry_slope = entry.slope;
    tva.entry_bank = entry.bank;
    STRINGLIST_ASSIGN(tva.entry_trackgroup, entry.group, false, tab, rel);
    tva.unk25 = exit.unknown;
    tva.exit_slope = exit.slope;
    tva.exit_bank = exit.bank;
    STRINGLIST_ASSIGN(tva.exit_trackgroup, exit.group, false, tab, rel);
}



void ovlTKSManager::AddSection(const cTrackSection& item) {
    Check("ovlTKSManager::AddSection");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlTKSManager::AddSection called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlTKSManager::AddSection: Item with name '"+item.name+"' already exists"));
    if ((item.version == 1) || (item.version > 3))
        BOOST_THROW_EXCEPTION(EOvl("ovlTKSManager::AddSection called with illegal version"));
    if (item.internalname == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlTKSManager::AddSection called without internal name"));
    if (item.sid == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlTKSManager::AddSection called without sid reference"));
    if ((item.splines.car.left == "") || (item.splines.car.right == ""))
        BOOST_THROW_EXCEPTION(EOvl("ovlTKSManager::AddSection called with missing car spline"));
    if ((item.splines.join.left == "") || (item.splines.join.right == ""))
        BOOST_THROW_EXCEPTION(EOvl("ovlTKSManager::AddSection called with missing join spline"));

    m_items[item.name] = item;

    // Base structures
    m_size += item.GetUniqueSize();
    m_commonsize += item.GetCommonSize();

	cLoader& loader = GetLSRManager()->reserveIndexElement(OVLT_UNIQUE, item.name, ovlTKSManager::TAG);

    STRINGLIST_ADD(item.internalname, true);
		
	loader.reserveSymbolReference(item.sid, ovlSIDManager::TAG);
	
	loader.reserveSymbolReference(item.splines.car.left, ovlSPLManager::TAG);
	loader.reserveSymbolReference(item.splines.car.right, ovlSPLManager::TAG);
	loader.reserveSymbolReference(item.splines.join.left, ovlSPLManager::TAG);
	loader.reserveSymbolReference(item.splines.join.right, ovlSPLManager::TAG);
    if ((item.splines.extra.left != "") && (item.splines.extra.right != "")) {
		loader.reserveSymbolReference(item.splines.extra.left, ovlSPLManager::TAG);
		loader.reserveSymbolReference(item.splines.extra.right, ovlSPLManager::TAG);
    }
	
    STRINGLIST_ADD(item.basic.entry.group, false);
    STRINGLIST_ADD(item.basic.exit.group, false);

    STRINGLIST_ADD(item.options.chair_lift_station_end, false);
	
    if (item.version) {
		loader.reserveSymbolReference(item.soaked.loop_path, ovlSPLManager::TAG, false);
        foreach(const string& path, item.soaked.paths)
			loader.reserveSymbolReference(path, ovlSPLManager::TAG);

        STRINGLIST_ADD(item.soaked.auto_group, false);
        foreach(const cTrackSectionSpeedSplines& spl, item.soaked.speeds) {
			loader.reserveSymbolReference(spl.splines.left, ovlSPLManager::TAG);
			loader.reserveSymbolReference(spl.splines.right, ovlSPLManager::TAG);
        }

        foreach(const string& gr, item.soaked.groups_is_at_entry)
            STRINGLIST_ADD(gr, true);
        foreach(const string& gr, item.soaked.groups_is_at_exit)
            STRINGLIST_ADD(gr, true);
        foreach(const string& gr, item.soaked.groups_must_have_at_entry)
            STRINGLIST_ADD(gr, true);
        foreach(const string& gr, item.soaked.groups_must_have_at_exit)
            STRINGLIST_ADD(gr, true);
        foreach(const string& gr, item.soaked.groups_must_not_be_at_entry)
            STRINGLIST_ADD(gr, true);
        foreach(const string& gr, item.soaked.groups_must_not_be_at_exit)
            STRINGLIST_ADD(gr, true);

        if (item.version == 3) {
			loader.reserveSymbolReference(item.wild.splitter_joined_other_ref, ovlTKSManager::TAG, false);
            STRINGLIST_ADD(item.wild.alternate_text_lookup, false);
        }
    }
	/*
    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name, ovlTKSManager::TAG);

    STRINGLIST_ADD(item.internalname, true);

    SYMREF_ADD(OVLT_UNIQUE, item.sid, ovlSIDManager::TAG, true);

    SYMREF_ADD(OVLT_UNIQUE, item.splines.car.left, ovlSPLManager::TAG, true);
    SYMREF_ADD(OVLT_UNIQUE, item.splines.car.right, ovlSPLManager::TAG, true);
    SYMREF_ADD(OVLT_UNIQUE, item.splines.join.left, ovlSPLManager::TAG, true);
    SYMREF_ADD(OVLT_UNIQUE, item.splines.join.right, ovlSPLManager::TAG, true);
    if ((item.splines.extra.left != "") && (item.splines.extra.right != "")) {
        SYMREF_ADD(OVLT_UNIQUE, item.splines.extra.left, ovlSPLManager::TAG, true);
        SYMREF_ADD(OVLT_UNIQUE, item.splines.extra.right, ovlSPLManager::TAG, true);
    }

    STRINGLIST_ADD(item.basic.entry.group, false);
    STRINGLIST_ADD(item.basic.exit.group, false);

    STRINGLIST_ADD(item.options.chair_lift_station_end, false);

    if (item.version) {
        SYMREF_ADD(OVLT_UNIQUE, item.soaked.loop_path, ovlSPLManager::TAG, false);
        foreach(const string& path, item.soaked.paths)
            SYMREF_ADD(OVLT_UNIQUE, path, ovlSPLManager::TAG, true);

        STRINGLIST_ADD(item.soaked.auto_group, false);
        foreach(const cTrackSectionSpeedSplines& spl, item.soaked.speeds) {
            SYMREF_ADD(OVLT_UNIQUE, spl.splines.left, ovlSPLManager::TAG, true);
            SYMREF_ADD(OVLT_UNIQUE, spl.splines.right, ovlSPLManager::TAG, true);
        }

        foreach(const string& gr, item.soaked.groups_is_at_entry)
            STRINGLIST_ADD(gr, true);
        foreach(const string& gr, item.soaked.groups_is_at_exit)
            STRINGLIST_ADD(gr, true);
        foreach(const string& gr, item.soaked.groups_must_have_at_entry)
            STRINGLIST_ADD(gr, true);
        foreach(const string& gr, item.soaked.groups_must_have_at_exit)
            STRINGLIST_ADD(gr, true);
        foreach(const string& gr, item.soaked.groups_must_not_be_at_entry)
            STRINGLIST_ADD(gr, true);
        foreach(const string& gr, item.soaked.groups_must_not_be_at_exit)
            STRINGLIST_ADD(gr, true);

        if (item.version == 3) {
            SYMREF_ADD(OVLT_UNIQUE, item.wild.splitter_joined_other_ref, ovlTKSManager::TAG, false);
            STRINGLIST_ADD(item.wild.alternate_text_lookup, false);
        }
    }
	*/
}

void ovlTKSManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlTKSManager::Make()");
    Check("ovlTKSManager::Make");

    m_blobs["0"] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    m_blobs["1"] = cOvlMemBlob(OVLT_COMMON, 2, m_commonsize);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs["0"].data;
    unsigned char* c_commondata = m_blobs["1"].data;

    foreach(const cTrackSection::mapentry item, m_items) {
        TrackSection_V* c_item = reinterpret_cast<TrackSection_V*>(c_data);
        TrackSection_W* c_item_w = reinterpret_cast<TrackSection_W*>(c_data);
        if (item.second.version) {
            if (item.second.version == 2) {
                c_data += sizeof(TrackSection_S);
            } else {
                c_data += sizeof(TrackSection_W);
            }
        } else {
            c_data += sizeof(TrackSection_V);
        }

        if (item.second.speeds.size()) {
            c_item->speeds = reinterpret_cast<TrackSectionSpeedStruct_V*>(c_data);
            if (item.second.version) {
                c_data += item.second.speeds.size() * sizeof(TrackSectionSpeedStruct_SW);
            } else {
                c_data += item.second.speeds.size() * sizeof(TrackSectionSpeedStruct_V);
            }
            GetRelocationManager()->AddRelocation(&c_item->speeds);
        }

        if (item.second.version) {

            c_item->anim_ex_s = reinterpret_cast<TrackSectionAnimation_S*>(c_data);
            if (item.second.version == 2) {
                c_data += item.second.speeds.size() * sizeof(TrackSectionAnimation_S);
            } else {
                c_data += item.second.speeds.size() * sizeof(TrackSectionAnimation_W);
            }
            GetRelocationManager()->AddRelocation(&c_item->anim_ex_s);

            RELOC_ARRAY(item.second.soaked.paths.size(), c_item_w->s.paths_ref, Spline*, c_data);
            RELOC_ARRAY(item.second.soaked.ride_station_limits.size(), c_item_w->s.ride_station_limits, RideStationLimit, c_commondata);
            RELOC_ARRAY(item.second.soaked.speeds.size(), c_item_w->s.speed_splines, TrackSectionSpeedSplines, c_commondata);

            RELOC_ARRAY(item.second.soaked.groups_is_at_entry.size(), c_item_w->s.groups_is_at_entry, char*, c_commondata);
            RELOC_ARRAY(item.second.soaked.groups_is_at_exit.size(), c_item_w->s.groups_is_at_exit, char*, c_commondata);
            RELOC_ARRAY(item.second.soaked.groups_must_have_at_entry.size(), c_item_w->s.groups_must_have_at_entry, char*, c_commondata);
            RELOC_ARRAY(item.second.soaked.groups_must_have_at_exit.size(), c_item_w->s.groups_must_have_at_exit, char*, c_commondata);
            RELOC_ARRAY(item.second.soaked.groups_must_not_be_at_entry.size(), c_item_w->s.groups_must_not_be_at_entry, char*, c_commondata);
            RELOC_ARRAY(item.second.soaked.groups_must_not_be_at_exit.size(), c_item_w->s.groups_must_not_be_at_exit, char*, c_commondata);
        }

        item.second.Fill(c_item, GetStringTable(), GetRelocationManager());
		
		
		cLoader& loader = GetLSRManager()->assignIndexElement(OVLT_UNIQUE, item.first, ovlTKSManager::TAG, c_item);

		loader.assignSymbolReference(item.second.sid, ovlSIDManager::TAG, &c_item->sid_ref);
		
		loader.assignSymbolReference(item.second.splines.car.left, ovlSPLManager::TAG, &c_item->spline_left_ref);
		loader.assignSymbolReference(item.second.splines.car.right, ovlSPLManager::TAG, &c_item->spline_right_ref);
		loader.assignSymbolReference(item.second.splines.join.left, ovlSPLManager::TAG, &c_item->join_spline_left_ref);
		loader.assignSymbolReference(item.second.splines.join.right, ovlSPLManager::TAG, &c_item->join_spline_right_ref);
		if ((item.second.splines.extra.left != "") && (item.second.splines.extra.right != "")) {
			loader.assignSymbolReference(item.second.splines.extra.left, ovlSPLManager::TAG, &c_item->extra_spline_left_ref);
			loader.assignSymbolReference(item.second.splines.extra.right, ovlSPLManager::TAG, &c_item->extra_spline_right_ref);
		}		
		
		if (item.second.version) {
			loader.assignSymbolReference(item.second.soaked.loop_path, ovlSPLManager::TAG, &c_item_w->s.loop_spline_ref, false);
            for(size_t i = 0; i < item.second.soaked.paths.size(); ++i)
				loader.assignSymbolReference(item.second.soaked.paths[i], ovlSPLManager::TAG, &c_item_w->s.paths_ref[i]);

            for(size_t i = 0; i < item.second.soaked.speeds.size(); ++i) {
				loader.assignSymbolReference(item.second.soaked.speeds[i].splines.left, ovlSPLManager::TAG, &c_item_w->s.speed_splines[i].left_ref);
				loader.assignSymbolReference(item.second.soaked.speeds[i].splines.right, ovlSPLManager::TAG, &c_item_w->s.speed_splines[i].right_ref);
			}

			if (item.second.version == 3) {
				loader.assignSymbolReference(item.second.wild.splitter_joined_other_ref, ovlTKSManager::TAG, &c_item_w->w.splitter_joined_other_ref, false);
			}
		}
		/*
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(item.first, TAG), reinterpret_cast<unsigned long*>(c_item));
        GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

        SYMREF_MAKE(OVLT_UNIQUE, item.second.sid, ovlSIDManager::TAG, &c_item->sid_ref, true);

        SYMREF_MAKE(OVLT_UNIQUE, item.second.splines.car.left, ovlSPLManager::TAG, &c_item->spline_left_ref, true);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.splines.car.right, ovlSPLManager::TAG, &c_item->spline_right_ref, true);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.splines.join.left, ovlSPLManager::TAG, &c_item->join_spline_left_ref, true);
        SYMREF_MAKE(OVLT_UNIQUE, item.second.splines.join.right, ovlSPLManager::TAG, &c_item->join_spline_right_ref, true);
        if ((item.second.splines.extra.left != "") && (item.second.splines.extra.right != "")) {
            SYMREF_MAKE(OVLT_UNIQUE, item.second.splines.extra.left, ovlSPLManager::TAG, &c_item->extra_spline_left_ref, true);
            SYMREF_MAKE(OVLT_UNIQUE, item.second.splines.extra.right, ovlSPLManager::TAG, &c_item->extra_spline_right_ref, true);
        }

        if (item.second.version) {
            SYMREF_MAKE(OVLT_UNIQUE, item.second.soaked.loop_path, ovlSPLManager::TAG, &c_item_w->s.loop_spline_ref, false);
            for(int i = 0; i < item.second.soaked.paths.size(); ++i)
                SYMREF_MAKE(OVLT_UNIQUE, item.second.soaked.paths[i], ovlSPLManager::TAG, &c_item_w->s.paths_ref[i],true);

            for(int i = 0; i < item.second.soaked.speeds.size(); ++i) {
                SYMREF_MAKE(OVLT_UNIQUE, item.second.soaked.speeds[i].splines.left, ovlSPLManager::TAG, &c_item_w->s.speed_splines[i].left_ref, true);
                SYMREF_MAKE(OVLT_UNIQUE, item.second.soaked.speeds[i].splines.right, ovlSPLManager::TAG, &c_item_w->s.speed_splines[i].right_ref, true);
            }

            if (item.second.version == 3) {
                SYMREF_MAKE(OVLT_UNIQUE, item.second.wild.splitter_joined_other_ref, ovlTKSManager::TAG, &c_item_w->w.splitter_joined_other_ref,false);
            }
        }

        GetLSRManager()->CloseLoader(OVLT_UNIQUE);
		*/
    }

}
