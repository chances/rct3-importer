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

#ifndef ATTRACTION_H_INCLUDED
#define ATTRACTION_H_INCLUDED

#include "spline.h"

/// General attraction structure (Soaked)
/**
 * This structure appears as sub-structure in all attractions added or updtaed in Soaked
 * Used by:
 *   - AnimatedRideB
 *   - ChangingRoom
 *   - SpecialAttractionB
 *   - StallB
 */
struct AttractionA {
	unsigned long type;         ///< second byte 02 siginifies AttractionA
	wchar_t* Name;
	wchar_t* Description;
	GUISkinItem *GSI;
	unsigned long unk2;         ///< Probably cost
	long unk3;                  ///< Probably refund
	unsigned long unk4;
	unsigned long unk5;
	unsigned long unk6;
	Spline* spline;             ///< A loop on flat rides
	unsigned long pathcount;    ///< Count for the next struct
	Spline** paths;             ///< splines for peep paths. Last is the mechanic's path
	unsigned long unk9;
	long unk10;
	unsigned long unk11;
	unsigned long unk12;
};

/// General attraction structure (Wild)
/**
 * This structure appears as sub-structure in all attractions added or updated in Wild
 * Used by:
 *   - AnimatedRideB
 *   - ChangingRoom
 *   - SpecialAttractionB
 *   - StallB
 */
struct AttractionB {
	unsigned long type;        ///< second byte 03 signifies AttractionB
	wchar_t* Name;
	wchar_t* Description;
	GUISkinItem *GSI;
	unsigned long unk2;
	long unk3;
	unsigned long unk4;
	unsigned long unk5;
	unsigned long unk6;
	Spline* spline;             ///< A loop on flat rides
	unsigned long pathcount;    ///< Count for the next struct
	Spline** paths;             ///< splines for peep paths. Last is the mechanic's path
	unsigned long unk9;
	long unk10;
	unsigned long unk11;
	unsigned long unk12;
	unsigned long unk13;
};


#endif // ATTRACTION_H_INCLUDED
