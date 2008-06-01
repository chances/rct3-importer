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


#ifndef MANAGERTRR_H_INCLUDED
#define MANAGERTRR_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "trackedride.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

struct cTrackedRideSection {
    std::string name;
    unsigned long cost;

    cTrackedRideSection(): cost(0) {}
};

class cTrackedRide {
public:
    std::string name;
    cAttraction attraction;
    cRide ride;
    std::vector<cTrackedRideSection> sections;
    std::vector<std::string> cars;
    std::vector<std::string> trackpaths;
    struct Specials {
        std::string cable_lift;
        float cable_lift_unk1;
        float cable_lift_unk2;
        float unk28;
        float unk29;
        std::string lift_car;
        Specials(): cable_lift_unk1(0), cable_lift_unk2(0), unk28(0), unk29(0) {};
    } specials;
    struct Options {
        uint32_t            only_on_water;
        uint32_t            blocks_possible;
        uint32_t            car_rotation;
        uint32_t            loop_not_allowed;
        uint32_t            auto_complete;
        uint32_t            deconstruct_everywhere;
    } options;
    struct Station {
        std::string name;
        uint32_t            platform_height_over_track;
        uint32_t            start_preset;
        uint32_t            start_possibilities;
        float_t             station_roll_speed;            ///< Speed the train rolls in and out of the station
    } station;
    struct Launched {
        float_t             preset;
        float_t             min;
        float_t             max;
        float_t             step;
    } launched;
    struct Chain {
        float_t             preset;
        float_t             min;
        float_t             max;
        float_t             step;
        float_t             lock;                     ///< Somehow influences how cars stick to the chain
    } chain;
    struct Constant {
        float_t             preset;          ///< If < 0, no constant speed
        float_t             min;
        float_t             max;
        float_t             step;
        float_t             up_down_variation;
    } constant;
    struct Cost {
        uint32_t            upkeep_per_train;
        uint32_t            upkeep_per_car;
        uint32_t            upkeep_per_station;
        uint32_t            ride_cost_preview1;             ///< cost preview = 0.75 * preview1 * preview2, rounded to 50 full currency steps
        uint32_t            ride_cost_preview2;             ///< See above
        uint32_t            cost_per_4h_height;
        uint32_t            ride_cost_preview3;             ///< According to Markus no effect in game
    } cost;
    struct Splines {
        std::string         track;
        std::string         track_big;
        std::string         car;
        std::string         car_swing;
        float_t             free_space_profile_height;      ///< ???
    } splines;
    struct Tower {
        float_t             top_duration;
        float_t	            top_distance;
        std::string         top;
        std::string         mid;
    } tower;
    struct Unknowns {
        float_t             unk31;
        float_t             unk32;
        float_t             unk33;
        float_t             unk34;
        uint32_t            unk38;                          ///< Markus: always 0
        float_t             unk43;
        float_t             unk45;
        float_t             unk48;
        float_t             unk49;
        float_t             unk50;
        float_t             unk51;
        uint32_t            unk57;
        uint32_t            unk58;
        uint32_t            unk59;
        uint32_t            unk60;
        uint32_t            unk61;
        uint32_t            unk69;                          ///< Markus: always 0
        uint32_t            unk74;                          ///< Markus: always 0
        uint32_t            unk75;                          ///< Markus: always 0
        float_t             unk95;                          ///< Markus: always -1.0
    } unknowns;


// 60
	uint32_t            modus_flags;
// 70

    cAnimatedRide() {
        attraction.type = r3::Constants::Attraction::Type::Ride_Thrill;
	    attraction.unk6 = r3::Constants::Attraction::BaseUpkeep::Other;
	    attraction.unk10 = 16;
	    attraction.unk12 = r3::Constants::Attraction::Unknown12::Default;
	    // ride defaults are fine
	    unk22 = 0;
	    unk23 = 0;
	    unk24 = 0;
	    unk25 = 10;
	    unk8 = 0;
	    unk9 = 0;
    };
    void Fill(r3::AnimatedRide_V* anr) const;
    void Fill(r3::AnimatedRide_SW* anr) const;
};

class ovlANRManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cAnimatedRide>  m_items;
    unsigned long m_commonsize;

public:
    ovlANRManager(): ovlOVLManager() {
        m_commonsize = 0;
    };

    void AddRide(const cAnimatedRide& item);

    virtual void Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
    virtual const char* Loader() const {
        return LOADER;
    };
};



#endif
