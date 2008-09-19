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

#ifndef ANIMATEDRIDE_H_INCLUDED
#define ANIMATEDRIDE_H_INCLUDED

#include "attraction.h"
#include "sceneryrevised.h"
#include "sharedride.h"

namespace r3 {

/// Animated ride structure
/**
 * This is the animated ride structure used in Vanilla
 */
struct AnimatedRide_V {
/*
	unsigned long type;
	wchar_t* name;
	wchar_t* description;
	GUISkinItem *icon;
	unsigned long unk5;         ///< Probably cost
	long unk6;                  ///< Probably refund
	unsigned long unk7;         ///< Seen 0
	unsigned long unk8;         ///< Seen 0
	unsigned long unk9;         ///< Seen 0, 4960
	Spline* loop;               ///< A loop on flat rides
	unsigned long pathcount;    ///< Count for the next struct
	Spline** paths;             ///< splines for peep paths. Last is the mechanic's path
	unsigned long unk13;        ///< Seen 0, 1, 2
	long unk14;                 ///< Seen 12, 16
*/
    Attraction_V        att;
/*
	uint32_t            attractivity; ///< Seen 20, 24, 35, 45, 50. Probably Attractivity (How many peeps are drawn into the park by it)
	uint32_t            seating;      ///< Seen 0, 5, 6, 9, 10, 11, 12, 13, 14, 18, 25
    RideOption**        options;      ///< List terminated by a unrelocated zero pointer
	uint32_t            unk18;        ///< Seen 1
	int32_t             unk19;        ///< Seen -1
	uint32_t            entry_fee;    ///< Default entry fee
*/
    Ride_V              ride;
    SceneryItem_V*      sid_ref;
	uint32_t            unk22;        ///< Seen 0
	uint32_t            unk23;        ///< Seen 0
	uint32_t            unk24;        ///< Seen 0
	uint32_t            unk25;        ///< Seen 10
};


struct AnimatedRideShowItem {
    uint32_t            animation_index;
    char*               name;
};

/// New animated ride structure
/**
 * This is the animated ride structure used in Soaked and Wild
 */
struct AnimatedRide_SW {
    uint32_t            unk1;           ///< always 0xFFFFFFFF, signifies AnimatedRide_SW
    uint32_t            unk2;           ///< Seen 0
    uint32_t            unk3;           ///< Seen 0
    uint32_t            unk4;           ///< Seen 0
    uint32_t            unk5;           ///< Seen 10
    union {
        Ride_S*         ride_sub_s;
        Ride_W*         ride_sub_w;
    };
    SceneryItem_V*      sid_ref;
    uint32_t            unk8;           ///< Seen 0
    uint32_t            unk9;           ///< Seen 0
    uint32_t            showitem_count;
    AnimatedRideShowItem* showitems;
};

};

#endif // ANIMATEDRIDE_H_INCLUDED
