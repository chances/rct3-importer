/*
    Originally appeared in
    RCT3 File Dumper
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 File Dumper
	The RCT3 File Dumper Program is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef TRACKEDRIDE_H_INCLUDED
#define TRACKEDRIDE_H_INCLUDED

#include "attraction.h"
#include "sceneryvisual.h"
#include "sharedride.h"
#include "tracksection.h"

namespace r3 {

struct TrackedRideTrackSectionStruct {
	char*           name;
	uint32_t        cost;
};

struct TrackedRideGroupDefinitionPair {
    uint32_t        flags;
    union {
        char*           group_name;
        TrackSection_V* section_ref;
    };
};

struct TrackedRideGroupDefinition {
    TrackedRideGroupDefinitionPair a;
    TrackedRideGroupDefinitionPair b;
};

struct TrackedRideGroupDefinitionArray {
    uint32_t                definition_count;
    TrackedRideGroupDefinition*  definitions;
};

struct TrackedRide_Common {
    union {
        uint32_t            track_section_count;            ///< 0xFFFFFFFF signals TrackedRide_S or TrackedRide_W
        int32_t             v2;
    };
	TrackSection_V**    track_sections_ref;
	uint32_t            train_name_count;
	char**              train_names;
	char*               cable_lift;
	float_t             cable_lift_unk1;
	float_t             cable_lift_unk2;
	float_t             unk28;
	float_t             unk29;
// 10
	char*               lift_car;
	float_t             unk31;
	float_t             unk32;
	float_t             unk33;
	float_t             unk34;
	char*               station_name;
	uint32_t            platform_height_over_track;
	uint32_t            only_on_water;
	int32_t             unk38;                          ///< Usually 0, -3 for MiniSubs
	uint32_t            start_preset;
// 20
	uint32_t            start_possibilities;
	uint32_t            blocks_possible;                ///< + 0x0100 in Soaked and Wild structures
	float_t             station_roll_speed;            ///< Speed the train rolls in and out of the station
	float_t             unk43;
	float_t             launched_speed_preset;
	float_t             unk45;
	float_t             chain_speed_preset;
	float_t             chain_lock;                     ///< ???
	float_t             unk48;
	float_t             unk49;
// 30
	float_t             unk50;
	float_t             unk51;
	float_t             constant_speed_preset;          ///< If < 0, no constant speed
	float_t             constant_speed_min;
	float_t             constant_speed_max;
	float_t             constant_speed_step;
	float_t             speed_up_down_variation;
	uint32_t            unk57;                          ///< Probably related to the upkeep
	uint32_t            unk58;                          ///< Probably related to the upkeep
	uint32_t            unk59;                          ///< Probably related to the upkeep
// 40
	uint32_t            unk60;
	uint32_t            unk61;
	uint32_t            upkeep_per_train;
	uint32_t            upkeep_per_car;
	uint32_t            upkeep_per_station;
	uint32_t            car_rotation;
	float_t             tower_top_duration;
	float_t	            tower_top_distance;
	uint32_t            loop_not_allowed;
	uint32_t            unk69;                          ///< Always 0
// 50
	uint32_t            ride_cost_preview1;             ///< cost preview = 75 * preview1 * preview2, rounded to 5000 full currency steps
	uint32_t            ride_cost_preview2;             ///< See above
	uint32_t            cost_per_4h_height;
	char*               track_path;                     ///< In S and W structures 0.
	TrackSection_V*     other_top_ref;                  ///< Used in Chair lift and Giant Slide
	TrackSection_V*     other_mid_ref;
	TrackSection_V*     tower_top_ref;                  ///< Used in Chair lift and Tower rides
	TrackSection_V*     tower_mid_ref;
	uint32_t            auto_complete;
	uint32_t            deconstruct_everywhere;
// 60
	int32_t             modus_flags;
	float_t             chain_speed_min;
	float_t             chain_speed_max;
	float_t             chain_speed_step;
	uint32_t            ride_cost_preview3;             ///< According to Markus no effect in game
	uint32_t            track_section_struct_count;
	TrackedRideTrackSectionStruct* track_section_structs;
	float_t             launched_speed_min;
	float_t             launched_speed_max;
	float_t             launched_speed_step;
// 70
	Spline*             track_spline_ref;
	Spline*             track_big_spline_ref;
	Spline*             car_spline_ref;
	Spline*             car_swing_spline_ref;
	float_t             free_space_profile_height;      ///< ???
	float_t             unk95;                          ///< Markus: always -1.0
};

/// Tracked ride structure, Vanilla
/**
 * This is the tracked ride structure used in Vanilla
 * Storage:
 *  Main Structure: unique
 *  Ride Options: common
 *  track_sections_ref: unique
 *  train_names: common
 *  track_section_structs: common
 */
struct TrackedRide_V {
    Attraction_V        att;
    Ride_V              ride;
    TrackedRide_Common  common;
};


/// Tracked ride structure, Soaked
/**
 * This is the tracked ride structure used in Soaked
 * Storage:
 *  Main Structure: unique
 *  track_sections_ref: unique
 *  track_section_structs: common
 *  train_names: common
 *  ride: unique
 *  extraarrays: unique
 *  track_paths: common
 */
struct TrackedRide_S {
/*
	uint32_t            unk1;                   ///< 0xFFFFFFFF signals TrackedRide_S or TrackedRide_W
	TrackSection_S**    track_sections_ref;
	uint32_t            train_name_count;
	char**              train_names;
	uint32_t            unk5;
	uint32_t            unk6;
	uint32_t            unk7;
	float_t             unk8;
	float_t             unk9;
	float_t             unk10;
	float_t             unk11;
	float_t             unk12;
	float_t             unk13;
	float_t             unk14;
	char*               platform;
	uint32_t            unk16;
	uint32_t            unk17;
	uint32_t            unk18;
	uint32_t            unk19;
	uint32_t            unk20;
	uint32_t            unk21;
	float_t             unk22;
	float_t             unk23;
	float_t             unk24;
	float_t             unk25;
	float_t             unk26;
	float_t             unk27;
	float_t             unk28;
	float_t             unk29;
	float_t             unk30;
	float_t             unk31;
	float_t             unk32;
	float_t             unk33;
	float_t             unk34;
	float_t             unk35;
	float_t             unk36;
	uint32_t            unk37;
	uint32_t            unk38;
	uint32_t            unk39;
	uint32_t            unk40;
	uint32_t            unk41;
	uint32_t            unk42;
	uint32_t            unk43;
	uint32_t            unk44;
	uint32_t            unk45;
	uint32_t            unk46;
	float_t             unk47;
	uint32_t            unk48;
	uint32_t            unk49;
	uint32_t            unk50;
	uint32_t            unk51;
	uint32_t            unk52;
	uint32_t            unk53;
	uint32_t            unk54;
	uint32_t            unk55;
	uint32_t            unk56;
	uint32_t            unk57;
	uint32_t            unk58;
	uint32_t            unk59;
	uint32_t            unk60;
	float_t             unk61;
	float_t             unk62;
	float_t             unk63;
	uint32_t            unk64;
	uint32_t            track_section_count;        ///< Determines tracksections and tracksectionstructs length
	TrackedRideTrackSectionStruct* track_section_structs;
	float_t             unk67;
	float_t             unk68;
	float_t             unk69;
	Spline*             track_spline_ref;
	Spline*             track_big_spline_ref;
	Spline*             car_spline_ref;
	Spline*             car_swing_spline_ref;
	float_t             unk74;
	float_t             unk75;
*/
    TrackedRide_Common  common;
    union {
        Ride_S*         ride_sub_s;
        Ride_W*         ride_sub_w;
    };
    uint32_t            track_section_count_sw;
	TrackSection_V*     other_top_flipped_ref;
	TrackSection_V*     other_mid_flipped_ref;
    uint32_t            unk80;                      ///< Always 0
    uint32_t            unk81;                      ///< Always 0
    float_t             unk82;                      ///< Usually 0.001, SkySwinger has 0.5
    uint32_t                    group_definition_count; ///< These define grouping behavior, ie automatically choosing start/mid/end pieces
    TrackedRideGroupDefinitionArray* group_definitions;
    uint32_t            unk85;                      ///< Always 0
    uint32_t            unk86;                      ///< Always 0
    uint32_t            unk87;                      ///< Usually 0, BodySlide, MasterBlaster, ProSlide, RingSlide and SinkingShip have 2
    int32_t             unk88;                      ///< Usually -1, BodySlide, MasterBlaster and ProSlide have 1
    float_t             unk89;                      ///< Usually 1.0, BodySlide, MasterBlaster and ProSlide have 4.0
    float_t             unk90;                      ///< Usually 1.0, BodySlide, MasterBlaster and ProSlide have 0.25
    uint32_t            track_path_count;
    char**              track_paths;
    float_t             unk93;                      ///< Usually -1.0, MasterBlaster has 1.5
    float_t             unk94;                      ///< Usually -1.0, MasterBlaster has 2.0
    uint32_t            unk95;                      ///< Usually 0, ElephantTransport, HoponTram and SafariTransport have 1
    int32_t             unk96;                      ///< Usually -1, Elevator has 2, GiantSlide has 3
    uint32_t            unk97;                      ///< Usually 0, LazyRiver has 1
    uint32_t            short_struct;               ///< Usually 0, if 1, the struct is one long shorter and 99-101 have special values
    float_t             unk99;                      ///< -1.0 if short_struct 0. Rafts and SplashBoats have 3.0. WaterCoaster has 5.0
    float_t             unk100;                     ///< 0.0 if short_struct 0. Rafts and SplashBoats have 1000.0. WaterCoaster has 2500.0
    float_t             unk101;                     ///< 0.0 if short_struct 0. Rafts and SplashBoats have 2000.0. WaterCoaster has 5000.0
    float_t             unk102;                     ///< Always 4.0
    float_t             unk103;                     ///< Usually 0.0, Aquarium, InsectHouse, NocturnalHouse and ReptileHouse have 4.0
};

struct TrackedRide_Wext {
    SceneryItemVisual_V* splitter_ref;
    uint32_t            robo_flag;                  ///< Usually 0, 1 activates the joint control
    uint32_t            spinner_control;            ///< Usually 0, 1 activates the eurospinner control
    uint32_t            unk107;                     ///< Usually 0, BallCoaster, Endless and FrequentFaller 1
    uint32_t            unk108;                     ///< Usually 1, ElephantTransport has 0
    int32_t             split_val;                  ///< Usually -1, Seizmic has 2
    uint32_t            split_flag;                 ///< Usually 0, Seizmic has 1
    char*               internalname;
};

/// Wild animated ride structure
/**
 * This is the animated ride structure used in Wild.
 */
struct TrackedRide_W {
    TrackedRide_S       s;
    TrackedRide_Wext    w;
};

};

#endif // TRACKEDRIDE_H_INCLUDED
