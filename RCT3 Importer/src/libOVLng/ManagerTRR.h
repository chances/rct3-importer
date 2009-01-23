///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for TRR structures
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
    typedef std::pair<std::string, cTrackedRide> mapentry;

    std::string name;
    cAttraction attraction;
    cRide ride;
    std::vector<cTrackedRideSection> sections;
    std::vector<std::string> trains;
    std::vector<std::string> trackpaths;
    struct Specials {
        std::string cable_lift;
        r3::float_t cable_lift_unk1;
        r3::float_t cable_lift_unk2;
        r3::float_t unk28;
        r3::float_t unk29;
        std::string lift_car;
        Specials(): cable_lift_unk1(0), cable_lift_unk2(0), unk28(6.0), unk29(-10.0) {};
        void Fill(r3::TrackedRide_Common& trr, ovlStringTable* tab, ovlRelocationManager* rel) const;
    } specials;
    struct Options {
        r3::uint32_t            only_on_water;
        r3::uint32_t            blocks_possible;
        r3::uint32_t            car_rotation;
        r3::uint32_t            loop_not_allowed;
        r3::uint32_t            deconstruct_everywhere;
        Options(): only_on_water(0), blocks_possible(1), car_rotation(0), loop_not_allowed(0), deconstruct_everywhere(1) {}
        void Fill(r3::TrackedRide_Common& trr) const;
    } options;
    struct Station {
        std::string name;
        r3::uint32_t            platform_height_over_track;
        r3::uint32_t            start_preset;
        r3::uint32_t            start_possibilities;
        r3::float_t             station_roll_speed;            ///< Speed the train rolls in and out of the station
        r3::int32_t             modus_flags;
        Station(): name("standard"), platform_height_over_track(0), start_preset(0), start_possibilities(3),
            station_roll_speed(8.0), modus_flags(-1) {}
        void Fill(r3::TrackedRide_Common& trr, ovlStringTable* tab, ovlRelocationManager* rel) const;
    } station;
    struct Launched {
        r3::float_t             preset;
        r3::float_t             min;
        r3::float_t             max;
        r3::float_t             step;
        Launched(): preset(20.0), min(1.0), max(50.0), step(0.1) {}
        void Fill(r3::TrackedRide_Common& trr) const;
    } launched;
    struct Chain {
        r3::float_t             preset;
        r3::float_t             min;
        r3::float_t             max;
        r3::float_t             step;
        r3::float_t             lock;                     ///< Somehow influences how cars stick to the chain
        Chain(): preset(4.0), min(1.0), max(25.0), step(0.1), lock(30000.0) {}
        void Fill(r3::TrackedRide_Common& trr) const;
    } chain;
    struct Constant {
        r3::float_t             preset;          ///< If < 0, no constant speed
        r3::float_t             min;
        r3::float_t             max;
        r3::float_t             step;
        r3::float_t             up_down_variation;
        Constant(): preset(-1.0), min(1.0), max(25.0), step(1.0), up_down_variation(1.0) {}
        void Fill(r3::TrackedRide_Common& trr) const;
    } constant;
    struct Cost {
        r3::uint32_t            upkeep_per_train;
        r3::uint32_t            upkeep_per_car;
        r3::uint32_t            upkeep_per_station;
        r3::uint32_t            ride_cost_preview1;             ///< cost preview = 0.75 * preview1 * preview2, rounded to 50 full currency steps
        r3::uint32_t            ride_cost_preview2;             ///< See above
        r3::uint32_t            cost_per_4h_height;
        r3::uint32_t            ride_cost_preview3;             ///< According to Markus no effect in game
        Cost(): upkeep_per_train(1100), upkeep_per_car(300), upkeep_per_station(1000), ride_cost_preview1(45),
            ride_cost_preview2(200), cost_per_4h_height(1600), ride_cost_preview3(675000) {}
        void Fill(r3::TrackedRide_Common& trr) const;
    } cost;
    struct Splines {
        r3::uint32_t        auto_complete;
        std::string         track;
        std::string         track_big;
        std::string         car;
        std::string         car_swing;
        r3::float_t         free_space_profile_height;      ///< ???
        Splines(): auto_complete(0), free_space_profile_height(0.0) {}
        void Fill(r3::TrackedRide_Common& trr) const;
    } splines;
    struct Tower {
        r3::float_t             top_duration;
        r3::float_t	            top_distance;
        std::string         top;
        std::string         mid;
        std::string         other_top;
        std::string         other_mid;
        std::string         other_top_flipped;
        std::string         other_mid_flipped;
        Tower(): top_duration(10.0), top_distance(0.01) {}
        void Fill(r3::TrackedRide_Common& trr) const;
    } tower;
    struct Unknowns {
        r3::float_t             unk31;
        r3::float_t             unk32;
        r3::float_t             unk33;
        r3::float_t             unk34;
        r3::int32_t             on_water_offset;                          ///< Markus: always 0
        r3::float_t             unk43;
        r3::float_t             unk45;
        r3::float_t             unk48;
        r3::float_t             unk49;
        r3::float_t             unk50;
        r3::float_t             unk51;
        r3::uint32_t            unk57;
        r3::uint32_t            unk58;
        r3::uint32_t            unk59;
        r3::uint32_t            unk60;
        r3::uint32_t            unk61;
        r3::uint32_t            unk69;                          ///< Markus: always 0
        r3::float_t             acceleration_behaviour;                          ///< Markus: always -1.0
        Unknowns(): unk31(4.0), unk32(-8.0), unk33(1.0), unk34(0.5), on_water_offset(0), unk43(10000), unk45(50000),
            unk48(4.0), unk49(5000.0), unk50(4.0), unk51(10000.0), unk57(8000), unk58(2000), unk59(2000),
            unk60(4000), unk61(1000), unk69(0), acceleration_behaviour(-1.0) {}
        void Fill(r3::TrackedRide_Common& trr) const;
    } unknowns;
    struct SoakedStuff {
        r3::uint32_t            unk80;                      ///< Always 0
        r3::uint32_t            unk81;                      ///< Always 0
        r3::float_t             unk82;                      ///< Usually 0.001, SkySwinger has 0.5
        r3::uint32_t            unk85;                      ///< Always 0
        r3::uint32_t            unk86;                      ///< Always 0
        r3::uint32_t            unk87;                      ///< Usually 0, BodySlide, MasterBlaster, ProSlide, RingSlide and SinkingShip have 2
        r3::int32_t             unk88;                      ///< Usually -1, BodySlide, MasterBlaster and ProSlide have 1
        r3::float_t             unk89;                      ///< Usually 1.0, BodySlide, MasterBlaster and ProSlide have 4.0
        r3::float_t             unk90;                      ///< Usually 1.0, BodySlide, MasterBlaster and ProSlide have 0.25
        r3::float_t             unk93;                      ///< Usually -1.0, MasterBlaster has 1.5
        r3::float_t             unk94;                      ///< Usually -1.0, MasterBlaster has 2.0
        r3::uint32_t            unk95;                      ///< Usually 0, ElephantTransport, HoponTram and SafariTransport have 1
        r3::int32_t             min_length;                      ///< Usually -1, Elevator has 2, GiantSlide has 3
        r3::uint32_t            unk97;                      ///< Usually 0, LazyRiver has 1
        r3::uint32_t            water_section_flag;               ///< Usually 0, if 1, the struct is one long shorter and 99-101 have special values
        r3::float_t             water_section_speed;                      ///< -1.0 if short_struct 0. Rafts and SplashBoats have 3.0. WaterCoaster has 5.0
        r3::float_t             water_section_accel;                     ///< 0.0 if short_struct 0. Rafts and SplashBoats have 1000.0. WaterCoaster has 2500.0
        r3::float_t             water_section_decel;                     ///< 0.0 if short_struct 0. Rafts and SplashBoats have 2000.0. WaterCoaster has 5000.0
        r3::float_t             unk102;                     ///< Always 4.0
        r3::float_t             unk103;                     ///< Usually 0.0, Aquarium, InsectHouse, NocturnalHouse and ReptileHouse have 4.0
        SoakedStuff(): unk80(0), unk81(0), unk82(0.001), unk85(0), unk86(0),
            unk87(0), unk88(-1), unk89(1.0), unk90(1.0), unk93(-1.0), unk94(-1.0), unk95(0), min_length(-1), unk97(0),
            water_section_flag(0), water_section_speed(-1.0), water_section_accel(0.0), water_section_decel(0.0), unk102(4.0), unk103(0.0) {}
        void Fill(r3::TrackedRide_S& trr) const;
    } soaked;
    struct WildStuff {
        std::string             splitter;
        r3::uint32_t            robo_flag;                  ///< Usually 0, RoboCoaster has 1
                                                        /**<
                                                         * Activating this (alone) does not give the control
                                                         **/
        r3::uint32_t            spinner_control;            ///< Usually 0, EuroSpinner has 1
        r3::uint32_t            unk107;                     ///< Usually 0, BallCoaster, Endless and FrequentFaller 1
        r3::uint32_t            unk108;                     ///< Usually 1, ElephantTransport has 0
        r3::int32_t             trains_default;                  ///< Usually -1, Seizmic has 2
        r3::uint32_t            station_sync_default;                 ///< Usually 0, Seizmic has 1
        std::string             internalname;
        WildStuff(): robo_flag(0), spinner_control(0), unk107(0), unk108(1), trains_default(-1), station_sync_default(0),
            internalname("customtrr") {}
        void Fill(r3::TrackedRide_W& trr, ovlStringTable* tab, ovlRelocationManager* rel) const;
    } wild;


// 60
// 70

    cTrackedRide() {
        attraction.type = r3::Constants::Attraction::Type::Ride_Coaster_Steel;
        attraction.unk2 = 0;
        attraction.unk3 = 0;
	    attraction.unk6 = r3::Constants::Attraction::BaseUpkeep::Other;
	    attraction.unk10 = 100;
	    attraction.unk12 = r3::Constants::Attraction::Unknown12::Default;
	    // ride defaults are fine
    };
    void FillCommon(r3::TrackedRide_Common* trr, ovlStringTable* tab, ovlRelocationManager* rel) const;
    void Fill(r3::TrackedRide_V* trr, ovlStringTable* tab, ovlRelocationManager* rel) const;
    void Fill(r3::TrackedRide_S* trr, ovlStringTable* tab, ovlRelocationManager* rel) const;

    void MakeCommonPointers(r3::TrackedRide_Common* ptr, unsigned char*& uni_ptr, unsigned char*& com_ptr, ovlRelocationManager* rel) const;

    void MakeCommonSymRefs(r3::TrackedRide_Common* trr, ovlLodSymRefManager* lsr, ovlStringTable* st) const;

    unsigned long GetCommonSize() const;
    unsigned long GetUniqueSize() const;
};

class ovlTRRManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cTrackedRide>  m_items;
    unsigned long m_commonsize;

public:
    ovlTRRManager(): ovlOVLManager() {
        m_commonsize = 0;
    };

    void AddRide(const cTrackedRide& item);

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
