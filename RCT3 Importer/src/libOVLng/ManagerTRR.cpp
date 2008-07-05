///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for TRR structures
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

#include <boost/algorithm/string.hpp>

#include "pretty.h"

#include "ManagerTRR.h"

#include "ManagerSPL.h"
#include "ManagerSVD.h"
#include "OVLException.h"

using namespace r3;
using namespace std;

const char* ovlTRRManager::LOADER = "FGDK";
const char* ovlTRRManager::NAME = "TrackedRide";
const char* ovlTRRManager::TAG = "trr";

#define DO_ASSIGN(var, subvar)  var.subvar = subvar

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::WildStuff::Fill(r3::TrackedRide_W& trr, ovlStringTable* tab, ovlRelocationManager* rel) const {
    trr.w.robo_flag = robo_flag;
    trr.w.spinner_control = spinner_control;
    trr.w.unk107 = unk107;
    trr.w.unk108 = unk108;
    trr.w.split_val = split_val;
    trr.w.split_flag = split_flag;
    STRINGLIST_ASSIGN(trr.w.internalname, internalname, false, tab, rel);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::SoakedStuff::Fill(r3::TrackedRide_S& trr) const {
    DO_ASSIGN(trr, unk80);
    DO_ASSIGN(trr, unk81);
    DO_ASSIGN(trr, unk82);
    DO_ASSIGN(trr, unk85);
    DO_ASSIGN(trr, unk86);
    DO_ASSIGN(trr, unk87);
    DO_ASSIGN(trr, unk88);
    DO_ASSIGN(trr, unk89);
    DO_ASSIGN(trr, unk90);
    DO_ASSIGN(trr, unk93);
    DO_ASSIGN(trr, unk94);
    DO_ASSIGN(trr, unk95);
    DO_ASSIGN(trr, unk96);
    DO_ASSIGN(trr, unk97);
    DO_ASSIGN(trr, short_struct);
    DO_ASSIGN(trr, unk99);
    DO_ASSIGN(trr, unk100);
    DO_ASSIGN(trr, unk101);
    DO_ASSIGN(trr, unk102);
    if (!short_struct)
        DO_ASSIGN(trr, unk103);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Unknowns::Fill(r3::TrackedRide_Common& trr) const {
    DO_ASSIGN(trr, unk31);
    DO_ASSIGN(trr, unk32);
    DO_ASSIGN(trr, unk33);
    DO_ASSIGN(trr, unk34);
    DO_ASSIGN(trr, unk38);
    DO_ASSIGN(trr, unk43);
    DO_ASSIGN(trr, unk45);
    DO_ASSIGN(trr, unk48);
    DO_ASSIGN(trr, unk49);
    DO_ASSIGN(trr, unk50);
    DO_ASSIGN(trr, unk51);
    DO_ASSIGN(trr, unk57);
    DO_ASSIGN(trr, unk58);
    DO_ASSIGN(trr, unk59);
    DO_ASSIGN(trr, unk60);
    DO_ASSIGN(trr, unk61);
    DO_ASSIGN(trr, unk69);
    DO_ASSIGN(trr, unk95);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Tower::Fill(r3::TrackedRide_Common& trr) const {
    trr.tower_top_duration = top_duration;
    trr.tower_top_distance = top_distance;
    trr.tower_top_ref = NULL;
    trr.tower_mid_ref = NULL;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Splines::Fill(r3::TrackedRide_Common& trr) const {
    DO_ASSIGN(trr, auto_complete);
    DO_ASSIGN(trr, free_space_profile_height);
    trr.track_spline_ref = NULL;
    trr.track_big_spline_ref = NULL;
    trr.car_spline_ref = NULL;
    trr.car_swing_spline_ref = NULL;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Cost::Fill(r3::TrackedRide_Common& trr) const {
    DO_ASSIGN(trr, upkeep_per_train);
    DO_ASSIGN(trr, upkeep_per_car);
    DO_ASSIGN(trr, upkeep_per_station);
    DO_ASSIGN(trr, ride_cost_preview1);
    DO_ASSIGN(trr, ride_cost_preview2);
    DO_ASSIGN(trr, cost_per_4h_height);
    DO_ASSIGN(trr, ride_cost_preview3);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Constant::Fill(r3::TrackedRide_Common& trr) const {
    trr.constant_speed_preset = preset;
    trr.constant_speed_min = min;
    trr.constant_speed_max = max;
    trr.constant_speed_step = step;
    trr.speed_up_down_variation = up_down_variation;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Chain::Fill(r3::TrackedRide_Common& trr) const {
    trr.chain_speed_preset = preset;
    trr.chain_speed_min = min;
    trr.chain_speed_max = max;
    trr.chain_speed_step = step;
    trr.chain_lock = lock;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Launched::Fill(r3::TrackedRide_Common& trr) const {
    trr.launched_speed_preset = preset;
    trr.launched_speed_min = min;
    trr.launched_speed_max = max;
    trr.launched_speed_step = step;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Station::Fill(r3::TrackedRide_Common& trr, ovlStringTable* tab, ovlRelocationManager* rel) const {
    STRINGLIST_ASSIGN(trr.station_name, name, true, tab, rel);
    DO_ASSIGN(trr, platform_height_over_track);
    DO_ASSIGN(trr, start_preset);
    DO_ASSIGN(trr, start_possibilities);
    DO_ASSIGN(trr, station_roll_speed);
    DO_ASSIGN(trr, modus_flags);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Options::Fill(r3::TrackedRide_Common& trr) const {
    DO_ASSIGN(trr, only_on_water);
    DO_ASSIGN(trr, blocks_possible);
    DO_ASSIGN(trr, car_rotation);
    DO_ASSIGN(trr, loop_not_allowed);
    DO_ASSIGN(trr, deconstruct_everywhere);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Specials::Fill(r3::TrackedRide_Common& trr, ovlStringTable* tab, ovlRelocationManager* rel) const {
    if (cable_lift != "")
        STRINGLIST_ASSIGN(trr.cable_lift, cable_lift, true, tab, rel);
    if (lift_car != "")
        STRINGLIST_ASSIGN(trr.lift_car, lift_car, true, tab, rel);
    DO_ASSIGN(trr, cable_lift_unk1);
    DO_ASSIGN(trr, cable_lift_unk2);
    DO_ASSIGN(trr, unk28);
    DO_ASSIGN(trr, unk29);
}


/** @brief FillCommon
  *
  * @todo: document this function
  */
void cTrackedRide::FillCommon(r3::TrackedRide_Common* trr, ovlStringTable* tab, ovlRelocationManager* rel) const {
    trr->track_section_struct_count = sections.size();
    for(int i = 0; i < sections.size(); ++i) {
//        trr->track_section_structs[i].name = tab->FindString(boost::algorithm::to_lower_copy(sections[i].name));
//        rel->AddRelocation(reinterpret_cast<unsigned long*>(&trr->common.track_path));
        STRINGLIST_ASSIGN(trr->track_section_structs[i].name, boost::algorithm::to_lower_copy(sections[i].name), true, tab, rel);
        trr->track_section_structs[i].cost = sections[i].cost;
    }
    trr->train_name_count = trains.size();
    for(int i = 0; i < trains.size(); ++i) {
        STRINGLIST_ASSIGN(trr->train_names[i], trains[i], true, tab, rel);
    }
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Fill(r3::TrackedRide_V* trr, ovlStringTable* tab, ovlRelocationManager* rel) const {
    specials.Fill(trr->common, tab, rel);
    options.Fill(trr->common);
    station.Fill(trr->common, tab, rel);
    launched.Fill(trr->common);
    chain.Fill(trr->common);
    constant.Fill(trr->common);
    cost.Fill(trr->common);
    splines.Fill(trr->common);
    tower.Fill(trr->common);
    unknowns.Fill(trr->common);
    attraction.Fill(&trr->att);
    ride.Fill(&trr->ride);

    //trr->common.track_path = tab->FindString(trackpaths[0]);
    //rel->AddRelocation(reinterpret_cast<unsigned long*>(&trr->common.track_path));
    STRINGLIST_ASSIGN(trr->common.track_path, trackpaths[0], true, tab, rel);
    trr->common.track_section_count = sections.size();
    FillCommon(&trr->common, tab, rel);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cTrackedRide::Fill(r3::TrackedRide_S* trr, ovlStringTable* tab, ovlRelocationManager* rel) const {
    specials.Fill(trr->common, tab, rel);
    options.Fill(trr->common);
    station.Fill(trr->common, tab, rel);
    launched.Fill(trr->common);
    chain.Fill(trr->common);
    constant.Fill(trr->common);
    cost.Fill(trr->common);
    splines.Fill(trr->common);
    tower.Fill(trr->common);
    unknowns.Fill(trr->common);
    soaked.Fill(*trr);

    if (attraction.wild()) {
        ride.Fill(trr->ride_sub_w);
    } else {
        ride.Fill(trr->ride_sub_s);
    }
    // Must come after the above, otherwise the attraction type for some completely mysterious reason gets trashed
    attraction.Fill(trr->ride_sub_s->s.att);

    if (attraction.wild()) {
        wild.Fill(reinterpret_cast<TrackedRide_W&>(*trr), tab, rel);
        trr->short_struct = 0;
        trr->unk103 = soaked.unk103;
    }

    trr->common.track_path = 0;
    trr->common.v2 = -1;
    trr->extraarrays = 0;
    trr->extraarray_count = 0;
    trr->common.blocks_possible |= 0x0100;
    trr->track_section_count_sw = sections.size();
    trr->track_path_count = trackpaths.size();
    for(int i = 0; i < trackpaths.size(); ++i) {
        STRINGLIST_ASSIGN(trr->track_paths[i], trackpaths[i], true, tab, rel);
    }
    FillCommon(&trr->common, tab, rel);

}

/** @brief MakeCommonPointers
  *
  * @todo: document this function
  */
void cTrackedRide::MakeCommonPointers(r3::TrackedRide_Common* ptr, unsigned char*& uni_ptr, unsigned char*& com_ptr, ovlRelocationManager* rel) const {
    ptr->track_sections_ref = reinterpret_cast<TrackSection_V**>(uni_ptr);
    uni_ptr += sections.size() * 4;
    rel->AddRelocation(reinterpret_cast<unsigned long*>(&ptr->track_sections_ref));

    ptr->train_names = reinterpret_cast<char**>(com_ptr);
    com_ptr += trains.size() * 4;
    rel->AddRelocation(reinterpret_cast<unsigned long*>(&ptr->train_names));

    ptr->track_section_structs = reinterpret_cast<TrackedRideTrackSectionStruct*>(com_ptr);
    com_ptr += sections.size() * sizeof(TrackedRideTrackSectionStruct);
    rel->AddRelocation(reinterpret_cast<unsigned long*>(&ptr->track_section_structs));
}


/** @brief MakeCommonSymRefs
  *
  * @todo: document this function
  */
void cTrackedRide::MakeCommonSymRefs(r3::TrackedRide_Common* trr, ovlLodSymRefManager* lsr, ovlStringTable* st) const {
    for(int i = 0; i < sections.size(); ++i) {
        lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(sections[i].name, ovlTKSManager::TAG),
                             reinterpret_cast<unsigned long*>(&trr->track_sections_ref[i]));
    }

    if (tower.other_top != "") {
        lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(tower.other_top, ovlTKSManager::TAG),
                             reinterpret_cast<unsigned long*>(&trr->other_top_ref));
    }
    if (tower.other_mid != "") {
        lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(tower.other_mid, ovlTKSManager::TAG),
                             reinterpret_cast<unsigned long*>(&trr->other_mid_ref));
    }
    if (tower.top != "") {
        lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(tower.top, ovlTKSManager::TAG),
                             reinterpret_cast<unsigned long*>(&trr->tower_top_ref));
    }
    if (tower.mid != "") {
        lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(tower.mid, ovlTKSManager::TAG),
                             reinterpret_cast<unsigned long*>(&trr->tower_mid_ref));
    }
    if (splines.track != "") {
        lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(splines.track, ovlSPLManager::TAG),
                             reinterpret_cast<unsigned long*>(&trr->track_spline_ref));
    }
    if (splines.track_big != "") {
        lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(splines.track_big, ovlSPLManager::TAG),
                             reinterpret_cast<unsigned long*>(&trr->track_big_spline_ref));
    }
    if (splines.car != "") {
        lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(splines.car, ovlSPLManager::TAG),
                             reinterpret_cast<unsigned long*>(&trr->car_spline_ref));
    }
    if (splines.car_swing != "") {
        lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(splines.car_swing, ovlSPLManager::TAG),
                             reinterpret_cast<unsigned long*>(&trr->car_swing_spline_ref));
    }
}

/** @brief GetCommonSize
  *
  * @todo: document this function
  */
unsigned long cTrackedRide::GetCommonSize() const {
    if (attraction.isnew()) {
// TODO (belgabor#1#): extraarrays
        return trackpaths.size() * 4; // Trackpath pointers
    }
    return 0;
}

/** @brief GetUniqueSize
  *
  * @todo: document this function
  */
unsigned long cTrackedRide::GetUniqueSize() const {
    if (attraction.isnew()) {
        if (attraction.wild()) {
            return sizeof(TrackedRide_W);
        } else {
            if (!soaked.short_struct)
                return sizeof(TrackedRide_S);
            else
                return sizeof(TrackedRide_S)-4;
        }
// TODO (belgabor#1#): extraarrays
    } else {
        return sizeof(TrackedRide_V);
    }

}



void ovlTRRManager::AddRide(const cTrackedRide& item) {
    Check("ovlTRRManager::AddRide");
    if (item.name == "")
        throw EOvl("ovlTRRManager::AddRide called without name");
    if (m_items.find(item.name) != m_items.end())
        throw EOvl("ovlTRRManager::AddRide: Item with name '"+item.name+"' already exists");
    if (item.attraction.name == "")
        throw EOvl("ovlTRRManager::AddRide called without name text");
    if (item.attraction.description == "")
        throw EOvl("ovlTRRManager::AddRide called without description text");
    if (item.attraction.icon == "")
        throw EOvl("ovlTRRManager::AddRide called without icon");
    if (!item.trackpaths.size())
        throw EOvl("ovlTRRManager::AddRide called without a track path");

    m_items[item.name] = item;

    // Base structures
    m_size += item.GetUniqueSize();
    m_commonsize += item.GetCommonSize();
    m_size += item.attraction.GetUniqueSize();
    m_commonsize += item.attraction.GetCommonSize();
    m_size += item.ride.GetUniqueSize(item.attraction);
    m_commonsize += item.ride.GetCommonSize(item.attraction);

    // track_sections_ref
    m_size += item.sections.size() * 4;

    // train_names
    m_commonsize += item.trains.size() * 4;

    // track_section_structs
    m_commonsize += item.sections.size() * sizeof(TrackedRideTrackSectionStruct);

    GetLSRManager()->AddSymbol(OVLT_UNIQUE);
    GetLSRManager()->AddLoader(OVLT_UNIQUE);
    GetStringTable()->AddSymbolString(item.name, ovlTRRManager::TAG);

    item.attraction.DoAdd(GetStringTable(), GetLSRManager());
    item.ride.DoAdd(GetStringTable(), GetLSRManager());

    foreach(const cTrackedRideSection& section, item.sections) {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(section.name, ovlTKSManager::TAG);
        GetStringTable()->AddString(boost::to_lower_copy(section.name));
    }
    foreach(const string& train, item.trains) {
        GetStringTable()->AddString(train);
    }
    if (item.specials.cable_lift != "")
        GetStringTable()->AddString(item.specials.cable_lift);
    if (item.specials.lift_car != "")
        GetStringTable()->AddString(item.specials.lift_car);
    if (item.station.name != "")
        GetStringTable()->AddString(item.station.name);

    if (item.tower.other_top != "") {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(item.tower.other_top, ovlTKSManager::TAG);
    }
    if (item.tower.other_mid != "") {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(item.tower.other_mid, ovlTKSManager::TAG);
    }
    if (item.tower.top != "") {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(item.tower.top, ovlTKSManager::TAG);
    }
    if (item.tower.mid != "") {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(item.tower.mid, ovlTKSManager::TAG);
    }
    if (item.splines.track != "") {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(item.splines.track, ovlSPLManager::TAG);
    }
    if (item.splines.track_big != "") {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(item.splines.track_big, ovlSPLManager::TAG);
    }
    if (item.splines.car != "") {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(item.splines.car, ovlSPLManager::TAG);
    }
    if (item.splines.car_swing != "") {
        GetLSRManager()->AddSymRef(OVLT_UNIQUE);
        GetStringTable()->AddSymbolString(item.splines.car_swing, ovlSPLManager::TAG);
    }

    if (item.attraction.isnew()) {
        foreach(const string& trpath, item.trackpaths) {
            GetStringTable()->AddString(trpath);
        }
// TODO (belgabor#1#): extraarrays
        if (item.tower.other_top_flipped != "") {
            GetLSRManager()->AddSymRef(OVLT_UNIQUE);
            GetStringTable()->AddSymbolString(item.tower.other_top_flipped, ovlTKSManager::TAG);
        }
        if (item.tower.other_mid_flipped != "") {
            GetLSRManager()->AddSymRef(OVLT_UNIQUE);
            GetStringTable()->AddSymbolString(item.tower.other_mid_flipped, ovlTKSManager::TAG);
        }
        if (item.attraction.wild()) {
            if (item.wild.splitter != "") {
                GetLSRManager()->AddSymRef(OVLT_UNIQUE);
                GetStringTable()->AddSymbolString(item.wild.splitter, ovlSVDManager::TAG);
            }
            if (item.wild.internalname != "") {
                GetStringTable()->AddString(item.wild.internalname);
            }
        }
    } else {
        GetStringTable()->AddString(item.trackpaths[0]);
    }

}

void ovlTRRManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlTRRManager::Make()");
    Check("ovlTRRManager::Make");

    m_blobs["0"] = cOvlMemBlob(OVLT_UNIQUE, 2, m_size);
    m_blobs["1"] = cOvlMemBlob(OVLT_COMMON, 2, m_commonsize);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs["0"].data;
    unsigned char* c_commondata = m_blobs["1"].data;

    foreach(const cTrackedRide::mapentry item, m_items) {
        if (item.second.attraction.isnew()) {
            TrackedRide_S* c_item = reinterpret_cast<TrackedRide_S*>(c_data);
            c_data += item.second.GetUniqueSize();

            item.second.ride.MakePointers(&c_item->ride_sub_s, c_data, c_commondata, item.second.attraction, GetRelocationManager());
            item.second.MakeCommonPointers(&c_item->common, c_data, c_commondata, GetRelocationManager());

            c_item->track_paths = reinterpret_cast<char**>(c_commondata);
            c_commondata += item.second.trackpaths.size() * 4;
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_item->track_paths));

            item.second.Fill(c_item, GetStringTable(), GetRelocationManager());

            SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(item.first, TAG), reinterpret_cast<unsigned long*>(c_item));
            GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

            item.second.ride.MakeSymRefs(c_item->ride_sub_s, item.second.attraction, GetLSRManager(), GetStringTable());
            item.second.MakeCommonSymRefs(&c_item->common, GetLSRManager(), GetStringTable());

            if (item.second.tower.other_top_flipped != "") {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(item.second.tower.other_top_flipped, ovlTKSManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_item->other_top_flipped_ref));
            }
            if (item.second.tower.other_mid_flipped != "") {
                GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(item.second.tower.other_mid_flipped, ovlTKSManager::TAG),
                                     reinterpret_cast<unsigned long*>(&c_item->other_mid_flipped_ref));
            }
            if (item.second.attraction.wild()) {
                if (item.second.wild.splitter != "") {
                    GetLSRManager()->MakeSymRef(OVLT_UNIQUE, GetStringTable()->FindSymbolString(item.second.wild.splitter, ovlSVDManager::TAG),
                                         reinterpret_cast<unsigned long*>(&reinterpret_cast<TrackedRide_W*>(c_item)->w.splitter_ref));
                }
            }

            GetLSRManager()->CloseLoader(OVLT_UNIQUE);
        } else {
            TrackedRide_V* c_item = reinterpret_cast<TrackedRide_V*>(c_data);
            c_data += sizeof(TrackedRide_V);

            item.second.attraction.MakePointers(&c_item->att, c_data, c_commondata, GetRelocationManager());
            item.second.ride.MakePointers(&c_item->ride, c_data, c_commondata, item.second.attraction, GetRelocationManager());
            item.second.MakeCommonPointers(&c_item->common, c_data, c_commondata, GetRelocationManager());

            item.second.Fill(c_item, GetStringTable(), GetRelocationManager());

            SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_UNIQUE, GetStringTable()->FindSymbolString(item.first, TAG), reinterpret_cast<unsigned long*>(c_item));
            GetLSRManager()->OpenLoader(OVLT_UNIQUE, TAG, reinterpret_cast<unsigned long*>(c_item), false, c_symbol);

            item.second.attraction.MakeSymRefs(&c_item->att, GetLSRManager(), GetStringTable());
            item.second.ride.MakeSymRefs(&c_item->ride, item.second.attraction, GetLSRManager(), GetStringTable());
            item.second.MakeCommonSymRefs(&c_item->common, GetLSRManager(), GetStringTable());

            GetLSRManager()->CloseLoader(OVLT_UNIQUE);
        }
    }

}
