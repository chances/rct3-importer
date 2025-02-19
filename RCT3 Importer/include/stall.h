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
#include "sceneryold.h"
#include "sceneryrevised.h"
#include "attraction.h"
#include "carrieditems.h"

#include "spline.h"

namespace r3old {

struct StallItem {
	void*           cid_ref;
	uint32_t        cost;
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
struct StallA
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
	r3old::SceneryItem *SID;
	unsigned long itemcount;
	StallItem *Items;
	unsigned long unk11;
	unsigned long unk12;
	unsigned long unk13;
	unsigned long unk14;
	unsigned long unk15;
	unsigned long unk16;
};
struct StallB
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
	r3::Attraction_S *att;
	r3old::SceneryItem *SID;
};
struct SpecialAttractionB
{
	unsigned long unk;
	r3::Attraction_S* att;
	r3old::SceneryItem* SID;
};
struct SpecialAttractionA
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
	r3::Spline* spline;
	unsigned long unk7;
	unsigned long unk8;
	unsigned long unk9;
	long unk10;
	r3old::SceneryItem *SID;
};

};

namespace r3 {

struct StallItem {
	CarriedItem*    cid_ref;
	uint32_t        cost;
};

struct Stall_V {
    /*
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
	*/
	Attraction_V    att;
	SceneryItem_V*  sid_ref;
	uint32_t        item_count;
	StallItem*      items;
	uint32_t        unk11;
	uint32_t        unk12;
	uint32_t        unk13;
	uint32_t        unk14;
	uint32_t        unk15;
	uint32_t        unk16;
};

struct Stall_SW {
	unsigned long   unk1;
	unsigned long   item_count;
	StallItem*      items;
	uint32_t        unk11;
	uint32_t        unk12;
	uint32_t        unk13;
	uint32_t        unk14;
	uint32_t        unk15;
	uint32_t        unk16;
	Attraction_S*   att;
	SceneryItem_V*  sid_ref;
};

struct SpecialAttraction_V {
    /*
	unsigned long AttractionType; //13 = 1st aid, 12 = ATM, 11 = toilet
	wchar_t* Name;
	wchar_t* Description;
	GUISkinItem *GSI;
	unsigned long unk2;
	long unk3;
	unsigned long unk4;
	unsigned long unk5;
	unsigned long unk6;
	Spline* spline;
	unsigned long unk7;
	unsigned long unk8;
	unsigned long unk9;
	long unk10;
	*/
	Attraction_V    att;
	SceneryItem_V*  sid_ref;
};

struct SpecialAttraction_SW {
	uint32_t        unk;
	Attraction_S*   att;
	SceneryItem_V*  sid_ref;
};

struct ChangingRoom {
	Attraction_S*   att;
	SceneryItem_V*  sid_ref;
	Spline*         spline_ref;
};

};

#endif
