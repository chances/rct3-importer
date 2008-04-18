/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __SCENARY_H__
#define __SCENARY_H__

#include "guiicon.h"
#include "sceneryvisual.h"
#include "vertex.h"

struct Scenery
{
	char *ovl;
	char *ovl2;
	char *ovl3;
	char *wallicon;
	char *ovlWithoutExt;
	char *InternalOVLName;
	char *icon;
	char *location;
	long cost;
	long refund;
	char *wallname;
	char *name;
	r3::VECTOR position;
	unsigned long type;
	BOOL flag1;
	BOOL flag2;
	BOOL flag3;
	BOOL flag4;
	BOOL flag5;
	BOOL flag6;
	BOOL flag7;
	BOOL flag8;
	BOOL flag9;
	BOOL flag10;
	BOOL flag11;
	BOOL flag12;
	BOOL flag13;
	BOOL flag14;
	BOOL flag15;
	BOOL flag16;
	BOOL flag17;
	BOOL flag18;
	BOOL flag19;
	BOOL flag20;
	BOOL flag21;
	BOOL flag22;
	BOOL flag23;
	BOOL flag24;
	BOOL flag25;
	BOOL flag26;
	BOOL flag27;
	BOOL flag28;
	BOOL flag29;
	BOOL flag30;
	BOOL flag31;
	BOOL flag32;
	unsigned long unk4;
	r3::VECTOR size;
	unsigned long supportstype;
	unsigned long unk17;
	unsigned long sizex;
	unsigned long sizey;
	unsigned long sizeflag;
	unsigned long extra;
	unsigned long unk31;
	unsigned long unk32;
	unsigned long unk33;
	BOOL flag33;
	BOOL flag34;
	BOOL flag35;
	BOOL flag36;
	BOOL flag37;
	BOOL flag38;
	BOOL flag39;
	BOOL flag40;
	BOOL flag41;
	BOOL flag42;
	BOOL flag43;
	BOOL flag44;
	BOOL flag45;
	BOOL flag46;
	BOOL flag47;
	BOOL flag48;
	BOOL flag49;
	BOOL flag50;
	BOOL flag51;
	BOOL flag52;
	BOOL flag53;
	BOOL flag54;
	BOOL flag55;
	BOOL flag56;
	BOOL flag57;
	BOOL flag58;
	BOOL flag59;
	BOOL flag60;
	BOOL flag61;
	BOOL flag62;
	BOOL flag63;
	BOOL flag64;
	long dunk2;
	unsigned long dunk3;
	long dunk4;
	unsigned long dunk5;
};

struct SIDData
{
	r3::VECTOR position;
	unsigned long type;
	BOOL flag1;
	BOOL flag2;
	BOOL flag3;
	BOOL flag4;
	BOOL flag5;
	BOOL flag6;
	BOOL flag7;
	BOOL flag8;
	BOOL flag9;
	BOOL flag10;
	BOOL flag11;
	BOOL flag12;
	BOOL flag13;
	BOOL flag14;
	BOOL flag15;
	BOOL flag16;
	BOOL flag17;
	BOOL flag18;
	BOOL flag19;
	BOOL flag20;
	BOOL flag21;
	BOOL flag22;
	BOOL flag23;
	BOOL flag24;
	BOOL flag25;
	BOOL flag26;
	BOOL flag27;
	BOOL flag28;
	BOOL flag29;
	BOOL flag30;
	BOOL flag31;
	BOOL flag32;
	unsigned long unk4;
	r3::VECTOR size;
	unsigned long unk17;
	unsigned long sizex;
	unsigned long sizey;
	unsigned long sizeflag;
	unsigned long extra;
	unsigned long unk31;
	unsigned long unk32;
	unsigned long unk33;
	BOOL flag33;
	BOOL flag34;
	BOOL flag35;
	BOOL flag36;
	BOOL flag37;
	BOOL flag38;
	BOOL flag39;
	BOOL flag40;
	BOOL flag41;
	BOOL flag42;
	BOOL flag43;
	BOOL flag44;
	BOOL flag45;
	BOOL flag46;
	BOOL flag47;
	BOOL flag48;
	BOOL flag49;
	BOOL flag50;
	BOOL flag51;
	BOOL flag52;
	BOOL flag53;
	BOOL flag54;
	BOOL flag55;
	BOOL flag56;
	BOOL flag57;
	BOOL flag58;
	BOOL flag59;
	BOOL flag60;
	BOOL flag61;
	BOOL flag62;
	BOOL flag63;
	BOOL flag64;
	long dunk2;
	unsigned long dunk3;
	long dunk4;
	unsigned long dunk5;
};

struct Sound
{
	WAVEFORMATEX format;
	int unk6; 		// always 0x00000000 could also be a float
	float unk7; 		// always 0.050000001
	float unk8; 		// always 3.9999999e-005
	float unk9; 		// always 1.0
	float unk10; 		// always 0.0
	float unk11; 		// always 15000000.0
	float unk12; 		// always 0.0
	float unk13; 		// always 0.050000001
	float unk14; 		// always 2.0
	float unk15; 		// always 30.0
	float unk16; 		// always 0.0
	char* channel1; 	// shorts
	int channel1size; 	// size of the above data
	char* channel2; 	// shorts, NULL in no channel2
	int channel2size; 	// 0 if not channel2
};

struct Sound2
{
        float unk1;
        unsigned long unk2; // 0 terminates list.
};
struct Sound1
{
    	unsigned long SoundCount;
    	Sound **SoundArray;
    	unsigned long Sound2Count;
    	Sound2 **Sound2Array;
};

struct SceneryParams
{
	char *type;
	char *params;
};
/*
valid values for "type" I have found so far are:
GuestInjectionPoint
LitterBin (params is Capacity 4.0, dont know if changing it will work)
ParkBench
ParkEntrance
QueueLineEntertainer (for the queueline tv)
RideEvent (params may be a string like doom,shark,truck)
Sign (params can be EnableNoEntry or EnableNoEntry QueueSign)
VistaPoint
Waterable (for the flowers, params is Capacity 300.0, dont know if changing it will work)
Soaked only:
PathFountain
LoudSpeaker
PassportPost
WaterCannon
*/
struct SceneryItemData //set all to 0 and see what happens
{
	unsigned long unkf33:1; //might have something to do with whether a scenery object has 0 clearence or not
	unsigned long unkf34:1;
	unsigned long unkf35:1;
	unsigned long unkf36:1;
	unsigned long unkf37:1;
	unsigned long unkf38:1;
	unsigned long unkf39:1;
	unsigned long unkf40:1;
	unsigned long unkf41:1;
	unsigned long unkf42:1;
	unsigned long unkf43:1;
	unsigned long unkf44:1;
	unsigned long unkf45:1;
	unsigned long unkf46:1;
	unsigned long unkf47:1;
	unsigned long unkf48:1;
	unsigned long unkf49:1;
	unsigned long unkf50:1;
	unsigned long unkf51:1;
	unsigned long unkf52:1;
	unsigned long unkf53:1;
	unsigned long unkf54:1;
	unsigned long unkf55:1;
	unsigned long unkf56:1;
	unsigned long unkf57:1;
	unsigned long unkf58:1;
	unsigned long unkf59:1;
	unsigned long unkf60:1;
	unsigned long unkf61:1;
	unsigned long unkf62:1;
	unsigned long unkf63:1;
	unsigned long unkf64:1;
	long unk2;
	unsigned long unk3;
	long *unk4;
	unsigned long unk5;
};
#define TYPE_TREE 0
#define TYPE_PLANT 1
#define TYPE_SHRUB 2
#define TYPE_FLOWERS 3
#define TYPE_FENCE 4
#define TYPE_WALL_MISC 5
#define TYPE_PATH_LAMP 6
#define TYPE_SCENERY_SMALL 7
#define TYPE_SCENERY_MEDIUM 8
#define TYPE_SCENERY_LARGE 9
#define TYPE_SCENERY_ANAMATRONIC 10
#define TYPE_SCENERY_MISC 11
#define TYPE_SUPPORT_MIDDLE 12
#define TYPE_SUPPORT_TOP 13
#define TYPE_SUPPORT_BOTTOM 14
#define TYPE_SUPPORT_BOTTOM_EXTRA 15
#define TYPE_SUPPORT_GIRDER 16
#define TYPE_SUPPORT_CAP 17
#define TYPE_RIDE_TRACK 18
#define TYPE_PATH 19
#define TYPE_PARK_ENTRANCE 20
#define TYPE_LITTER 21
#define TYPE_GUEST_INJECT 22
#define TYPE_RIDE 23
#define TYPE_RIDE_ENTRANCE 24
#define TYPE_RIDE_EXIT 25
#define TYPE_KEEP_CLEAR_FENCE 26
#define TYPE_STALL 27
#define TYPE_RIDE_EVENT 28
#define TYPE_FIREWORK 29
#define TYPE_LITTER_BIN 30
#define TYPE_BENCH 31
#define TYPE_SIGN 32
#define TYPE_PHOTO_POINT 33
#define TYPE_WALL_STRAIGHT 34
#define TYPE_WALL_ROOF 35
#define TYPE_WALL_CORNER 36
//new for Soaked!
#define TYPE_WATER_CANNON 37
#define TYPE_POOL_PIECE 38
#define TYPE_POOL_EXTRA 39
#define TYPE_CHANGING_ROOM 40
#define TYPE_LASER_DOME 41 //???
#define TYPE_WATER_JET 42 //???
#define TYPE_TERRAIN_PIECE 43 //???
#define TYPE_PARTICLE_EFFECT 44 //???
//new for Wild!
#define TYPE_ANIMAL_FENCE 45 //???
#define TYPE_ANIMAL_MISC 46 //???

#define SIZE_FULLTILE 0
#define SIZE_PATHEDGEINNER 1
#define SIZE_PATHEDGEOUTER 2
#define SIZE_WALL 3
#define SIZE_QUARTER 4
#define SIZE_HALF 5
#define SIZE_PATHCENTER 6
#define SIZE_CORNER 7
#define SIZE_PATHEDGEJOIN 8

struct SceneryItem
{
	unsigned long unk1; //is 0 on disk files
	//flags
	unsigned long unkf1:1;
	unsigned long unkf2:1; //not used anywhere that I can see
	unsigned long unkf3:1;
	unsigned long unkf4:1; //on for vertical ride bases, stalls, flats and some other stuff
	unsigned long unkf5:1; //not used anywhere that I can see
	unsigned long unkf6:1; //is set to 1 for all tracks except vertical rides (i.e. elevator etc)
	unsigned long unkf7:1;
	unsigned long unkf8:1;
	unsigned long unkf9:1; //set only on the column courtyard
	unsigned long unkf10:1; //set for some track pieces
	unsigned long unkf11:1; //set for some track pieces
	unsigned long unkf12:1; //set for some track pieces
	unsigned long unkf13:1; //controls whether an object can have walls on the same square as it except when its a 1/2 tile scenery item
	unsigned long unkf14:1;
	unsigned long unkf15:1; //set for some support pieces
	unsigned long unkf16:1; //set for some support pieces
	unsigned long unkf17:1; //set for water track (i.e. jetskis etc)
	unsigned long unkf18:1; //not used anywhere that I can see
	unsigned long unkf19:1; //not used anywhere that I can see
	unsigned long unkf20:1; //set for helter-skelters
	unsigned long unkf21:1; //not used anywhere that I can see
	unsigned long unkf22:1; //not used anywhere that I can see
	unsigned long unkf23:1; //not used anywhere that I can see
	unsigned long unkf24:1; //not used anywhere that I can see
	unsigned long unkf25:1; //not used anywhere that I can see
	unsigned long unkf26:1; //not used anywhere that I can see
	unsigned long unkf27:1; //not used anywhere that I can see
	unsigned long unkf28:1; //not used anywhere that I can see
	unsigned long unkf29:1; //not used anywhere that I can see
	unsigned long unkf30:1; //not used anywhere that I can see
	unsigned long unkf31:1; //not used anywhere that I can see
	unsigned long unkf32:1; //not used anywhere that I can see
	unsigned long size;
	unsigned long unk4; //valid values are 0-17
	unsigned long xsquares;
	unsigned long ysquares;
	SceneryItemData *data;
	float xpos;
	float ypos;
	float zpos;
	float xsize;
	float ysize;
	float zsize;
	unsigned long unk13; //is 0 except in track parts
	unsigned long unk14; //is 0 except in track parts (pointer I think)
	long cost;
	long refund;
	unsigned long unk17; //is 0,4,5 4 is found on the queue paths
	unsigned long type;
	char *supports;
	unsigned long svdcount;
	r3::SceneryItemVisual_V **svd;
	GUISkinItem *gsi;
	GUISkinItem *wallsetgsi;
	wchar_t *wallsetname;
	char *OvlName;
	unsigned long count;
	SceneryParams *params;
	unsigned long SoundsCount; //count for sounds structures
	Sound1* Sounds; //for rides, points to data for sounds & other things
	wchar_t *Name;
	unsigned long unk27; //is 0
	unsigned long unk28; //is 0
	unsigned long unk29; //is 0 except in stalls
	unsigned long unk30; //is 0 except in stalls
	unsigned long unk31; //can have values, is usually 0 though
	unsigned long unk32; //can have values, is usually 0 though
	unsigned long unk33; //can have values, is usually 0 though
	unsigned long unk34; //is 0
	unsigned long unk35; //is 0
	unsigned long unk36; //is 0
	unsigned long unk37; //is 0
	unsigned long unk38; //is 0
	unsigned long unk39; //is 0
	unsigned long unk40; //is 0
	unsigned long unk41; //is 0
	unsigned long unk42; //is 0 except for flying saucers and dodgems
	unsigned long unk43; //is 0 except for flying saucers and dodgems (pointer I think)
	unsigned long unk44; //is 0
	unsigned long unk45; //is 0 except for trampoline
	unsigned long unk46; //is 0 except for trampoline
	unsigned long unk47; //is 0 except for trampoline
	unsigned long unk48; //is 0 except for trampoline
};
struct SceneryItemExtra
{
	unsigned char *SoundsUnk;
	unsigned long unk2;
	unsigned long AddonPack;
	unsigned long GenericAddon;
};

#endif
