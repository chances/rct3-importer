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
#include "sharedride.h"

/// Animated ride structure
/**
 * This is the animated ride structure used in Vanilla
 */
struct AnimatedRideA {
	unsigned long type;         ///< second byte 02 siginifies AttractionA
	wchar_t* Name;
	wchar_t* Description;
	GUISkinItem *GSI;
	unsigned long unk5;         ///< Probably cost
	long unk6;                  ///< Probably refund
	unsigned long unk7;         ///< Seen 0
	unsigned long unk8;         ///< Seen 0
	unsigned long unk9;         ///< Seen 0, 4960
	Spline* spline;             ///< A loop on flat rides
	unsigned long pathcount;    ///< Count for the next struct
	Spline** paths;             ///< splines for peep paths. Last is the mechanic's path
	unsigned long unk13;        ///< Seen 0, 1, 2
	long unk14;                 ///< Seen 12, 16
	unsigned long unk15;        ///< Seen 20, 24, 35, 45, 50
	unsigned long seating;      ///< Seen 0, 5, 6, 9, 10, 11, 12, 13, 14, 18, 25
    RideOption** options;       ///< List terminated by a unrelocated zero pointer
	unsigned long unk18;        ///< Seen 1
	long unk19;                 ///< Seen -1
	unsigned long entryfee;     ///< Default entry fee
    SceneryItem* SID;
	unsigned long unk22;        ///< Seen 0
	unsigned long unk23;        ///< Seen 0
	unsigned long unk24;        ///< Seen 0
	unsigned long unk25;        ///< Seen 10
};


struct AnimatedRideExtra {
    unsigned long index;
    unsigned long unk2;
    unsigned long unk3;
    unsigned long unk3;
};

struct ShowItem {
    unsigned long index;
    char* animname;
};

/// Secondary structure for AnimatedRideB
struct AnimatedRideB2 {
    unsigned long unk1; ///< always 0xFFFFFFFF
    unsigned long unk2;
    RideOption** options;       ///< List terminated by a unrelocated zero pointer
    unsigned long unk4;
    long unk5;
    unsigned long unk6;
    AttractionA* att;
    unsigned long unk8;
    unsigned long extracount;
    AnimatedRideExtra* extras;
    unsigned long unk11;
    unsigned long unk12;
    unsigned long unk13;
    unsigned long unk14;
    unsigned long unk15;
    unsigned long unk16;
    unsigned long unk17;
};

/// New animated ride structure
/**
 * This is the animated ride structure used in Soaked and Wild
 */
struct AnimatedRideB {
    unsigned long unk1; ///< always 0xFFFFFFFF, signifies AnimatedRideB
    unsigned long unk2;
    unsigned long unk3;
    unsigned long unk4;
    unsigned long unk5;
    AnimatedRideB2* ride2;
    SceneryItem* SID;
    unsigned long unk6;
    unsigned long unk7;
    unsigned long unk8;
    unsigned long showitemcount;
    ShowItem* showitems;
};


#endif // ANIMATEDRIDE_H_INCLUDED
