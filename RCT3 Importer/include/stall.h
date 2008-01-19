/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __STALL_H__
#define __STALL_H__

#include "guiicon.h"
#include "sceneryrevised.h"

#include "spline.h"

struct StallItem
{
	char *CID;
	unsigned long cost;
};
struct Attraction
{
	unsigned long type; // second byte 02
	wchar_t* Name;
	wchar_t* Description;
	GUISkinItem *GSI;
	unsigned long unk2;
	long unk3;
	unsigned long unk4;
	unsigned long unk5;
	unsigned long unk6;
	Spline* spline;             // A loop on flat rides
	unsigned long pathcount;    // Count for the next struct
	Spline** paths;             // splines for peep paths. Last is the mechanic's path
	unsigned long unk9;
	long unk10;
	unsigned long unk11;
	unsigned long unk12;
};
struct Attraction2
{
	unsigned long type; // second byte 03
	wchar_t* Name;
	wchar_t* Description;
	GUISkinItem *GSI;
	unsigned long unk2;
	long unk3;
	unsigned long unk4;
	unsigned long unk5;
	unsigned long unk6;
	Spline* spline;             // A loop on flat rides
	unsigned long pathcount;    // Count for the next struct
	Spline** paths;             // splines for peep paths. Last is the mechanic's path
	unsigned long unk9;
	long unk10;
	unsigned long unk11;
	unsigned long unk12;
	unsigned long unk13;
};
struct StallStr
{
    char *Name;
    unsigned long StallType;
    char *NameString;
    char *DescriptionString;
    unsigned long Unk2;
    long Unk3;
    char *SID;
    unsigned long ItemCount;
    StallItem *Items;
    unsigned long Unk11;
    unsigned long Unk12;
    unsigned long Unk13;
    unsigned long Unk14;
    unsigned long Unk15;
    unsigned long Unk16;
};
struct AttractionStr
{
    char *Name;
    unsigned long AttractionType;
    char *NameString;
    char *DescriptionString;
    unsigned long Unk2;
    long Unk3;
    char *SID;
};
struct Stall
{
	unsigned long type; //7 = food, 8 = drinks, 9 = misc, 14 = info/umbrella
	wchar_t* Name;
	wchar_t* Description;
	GUISkinItem *GSI;
	unsigned long unk2;
	long unk3;
	unsigned long unk4;
	unsigned long unk5;
	unsigned long unk6;
	unsigned char *spline;
	unsigned long unk7;
	unsigned long unk8;
	unsigned long unk9;
	long unk10;
	SceneryItem *SID;
	unsigned long itemcount;
	StallItem *Items;
	unsigned long unk11;
	unsigned long unk12;
	unsigned long unk13;
	unsigned long unk14;
	unsigned long unk15;
	unsigned long unk16;
};
struct Stall2
{
	unsigned long unk1;
	unsigned long itemcount;
	StallItem *Items;
	unsigned long unk2;
	unsigned long unk3;
	unsigned long unk4;
	unsigned long unk5;
	unsigned long unk6;
	unsigned long unk7;
	Attraction *att;
	SceneryItem *SID;
};
struct ChangingRoom
{
	Attraction *att;
	SceneryItem *SID;
	unsigned char *spline;
};
struct SpecialAttraction2
{
	unsigned long unk;
	Attraction *att;
	SceneryItem *SID;
};
struct SpecialAttraction
{
	unsigned long AttractionType; //13 = 1st aid, 12 = ATM, 11 = toilet
	wchar_t* Name;
	wchar_t* Description;
	GUISkinItem *GSI;
	unsigned long unk2;
	long unk3;
	unsigned long unk4;
	unsigned long unk5;
	unsigned long unk6;
	unsigned char *spline;
	unsigned long unk7;
	unsigned long unk8;
	unsigned long unk9;
	long unk10;
	SceneryItem *SID;
};

#endif
