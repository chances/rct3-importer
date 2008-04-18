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
#include "sharedride.h"
#include "tracksection.h"

namespace r3 {

struct TrackedRideTrackSectionStruct {
	char*           name;
	uint32_t        cost;
};

/// Tracked ride structure
/**
 * This is the tracked ride structure used in Vanilla
 */
struct TrackedRide_V {
/*
	unsigned long type;
	wchar_t* name;
	wchar_t* description;
	GUISkinItem *icon;
	unsigned long unk5;   ///< Seen 0. Would map to cost in similar structures
	unsigned long unk6;   ///< Seen 0. Would map to refund in similar structures
	unsigned long unk7;   ///< Seen 0
	unsigned long unk8;   ///< Seen 0
	unsigned long unk9;   ///< Seen 4960
	Spline*	loop;
	unsigned long pathcount;
	Spline** paths;
	unsigned long unk13;
	unsigned long unk14;
*/
    Attraction_V        att;
/*
	uint32_t            attractivity;       ///< How many peeps are drawn into the park by this ride
	uint32_t            unk16;              ///< Would be seating if similar to AnimatedRideA
	RideOption**        options;
	uint32_t            unk18;
	uint32_t            unk19;
	uint32_t            unk20;
*/
    Ride_V              ride;
	uint32_t            track_section_count;
	TrackSection_V**    track_sections_ref;
	uint32_t            train_name_count;
	char**              train_names;
	uint32_t            unk25;
	uint32_t            unk26;
	uint32_t            unk27;
	float_t             unk28;
	float_t             unk29;
	float_t             unk30;
	float_t             unk31;
	float_t             unk32;
	float_t             unk33;
	float_t             unk34;
	char*               platform;
	uint32_t            unk36;
	uint32_t            unk37;
	uint32_t            unk38;
	uint32_t            unk39;
	uint32_t            unk40;
	uint32_t            unk41;
	float_t             unk42;
	float_t             unk43;
	float_t             unk44;
	float_t             unk45;
	float_t             unk46;
	float_t             unk47;
	float_t             unk48;
	float_t             unk49;
	float_t             unk50;
	float_t             unk51;
	float_t             unk52;
	float_t             unk53;
	float_t             unk54;
	float_t             unk55;
	float_t             unk56;
	uint32_t            unk57;
	uint32_t            unk58;
	uint32_t            unk59;
	uint32_t            unk60;
	uint32_t            unk61;
	uint32_t            unk62;
	uint32_t            unk63;
	uint32_t            unk64;
	uint32_t            unk65;
	uint32_t            unk66;
	float_t	            unk67;
	uint32_t            unk68;
	uint32_t            unk69;
	uint32_t            unk70;
	uint32_t            unk71;
	uint32_t            unk72;
	char*               track_path;
	uint32_t            unk74;
	uint32_t            unk75;
	TrackSection_V*     tower_top_ref;
	TrackSection_V*     tower_mid_ref;
	uint32_t            unk78;
	uint32_t            unk79;
	uint32_t            unk80;
	float_t             unk81;
	float_t             unk82;
	float_t             unk83;
	uint32_t            unk84;
	uint32_t            track_section_struct_count;
	TrackedRideTrackSectionStruct* track_section_structs;
	float_t             unk87;
	float_t             unk88;
	float_t             unk89;
	Spline*             track_spline_ref;
	Spline*             track_big_spline_ref;
	Spline*             car_spline_ref;
	Spline*             car_swing_spline_ref;
	float_t unk94;
	float_t unk95;
};

/*
/// Secondary structure for TrackedRideB
struct TrackedRideB2 {
    unsigned long unk1; ///< always 0xFFFFFFFF
    unsigned long unk2;         ///< Would be seating if similar to AnimatedRideB2
    RideOption** options;       ///< List terminated by a unrelocated zero pointer
    unsigned long unk4;         ///< Seen 1
    long unk5;                  ///< Seen -1, 1 (FunHouse, LionShow, TigerShow, DolphinShow, KillerWhaleShow)
    unsigned long entryfee;     ///< if similar to AnimatedRideB2!!!
    AttractionA* att;
    unsigned long unk8;         ///< Seen 95
    unsigned long extracount;   ///< if similar to AnimatedRideB2!!!
    RideExtra* extras;          ///< if similar to AnimatedRideB2!!!
    unsigned long unk11;        ///< Seen 3
    unsigned long unk12;        ///< Seen 3
    long unk13;                 ///< Seen -2
    long unk14;                 ///< Seen -2
    long unk15;                 ///< Seen -2
    unsigned long unk16;        ///< Seen 1. Only present in Wild ovls
    unsigned long unk17;        ///< Seen 1. Only present in Wild ovls
};
*/

/// Soaked animated ride structure
/**
 * This is the animated ride structure used in Soaked
 */
struct TrackedRide_S {
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
	union {
	    Ride_S*         ride_sub_s;
	    Ride_W*         ride_sub_w;
	};
	uint32_t            unk77;
	uint32_t            unk78;
	uint32_t            unk79;
	uint32_t            unk80;
	uint32_t            unk81;
	float_t             unk82;
	uint32_t            unk83;
	uint32_t            unk84;
	uint32_t            unk85;
	uint32_t            unk86;
	uint32_t            unk87;
	uint32_t            unk88;
	float_t             unk89;
	float_t             unk90;
	uint32_t            track_path_count;
	char**              track_paths;
	float_t             unk93;
	float_t             unk94;
	uint32_t            unk95;
	uint32_t            unk96;
	uint32_t            unk97;
	uint32_t            unk98;
	float_t             unk99;
	uint32_t            unk100;
	uint32_t            unk101;
	float_t             unk102;
	uint32_t            unk103;
};

struct TrackedRide_Wext {
    uint32_t            unk104;
    uint32_t            unk105;
    uint32_t            unk106;
    uint32_t            unk107;
    uint32_t            unk108;
    int32_t             unk109;
    uint32_t            unk110;
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
