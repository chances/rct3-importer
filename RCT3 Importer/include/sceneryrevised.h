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

#ifndef _WAVEFORMATEX_
#ifndef WAVEFORMATEX
struct WAVEFORMATEX {
	short wFormatTag;
	short nChannels;
	long  nSamplesPerSec;
	long  nAvgBytesPerSec;
	short nBlockAlign;
	short wBitsPerSample;
	short cbSize;
};
#endif
#endif

namespace r3 {

/*

#ifndef BOOL
#define BOOL unsigned long
#endif

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
	VECTOR position;
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
	VECTOR size;
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
	VECTOR position;
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
	VECTOR size;
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

*/

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

struct SceneryExtraSound
{
    float unk1;
    float unk2;
    unsigned long unk3;  // Usually 0
};

struct SoundScript1
{
        float unk1;         // != 0.0
        unsigned long unk2; // 0 terminates list, otherwise always 1
};
struct SoundScript2 // Some kind of script
{
        unsigned long unk1; // == 0
        // repeated till instruction == 0
        unsigned long instruction; // 1, 2, 3 or 4
        float parameter[];         // x = 1 for i = 1 or 2
                                   // x = 3 for i = 3 or 4.
                                   // the last value of a triple and the only value of 1/2 always increase in the list
                                   // the maximum of this increase seems to be the length of the animation in the case
                                   // of a ride event.
                                   // I found no kind of index into the SoundArray, so either they are played in parallel,
                                   // as a whole or in turn.
                                   // I suspect intsruction 1 is like a SoundScript1 entry. It probably means "Play sounds at
                                   // this timepoint".
};
struct ScenerySound
{
    	unsigned long SoundCount;
    	Sound **SoundArray;
    	unsigned long Sound2Count; // Rides seem to have 4, Ride Events 1
    	SoundScript1 **Sound2Array; // Each entry is either a SoundScript1 or SoundScript2 "structure"
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
//	unsigned long unkf33:1; //might have something to do with whether a scenery object has 0 clearence or not
//	unsigned long unkf34:1;
//	unsigned long unkf35:1;
//	unsigned long unkf36:1;
//	unsigned long unkf37:1;
//	unsigned long unkf38:1;
//	unsigned long unkf39:1;
//	unsigned long unkf40:1;
//	unsigned long unkf41:1;
//	unsigned long unkf42:1;
//	unsigned long unkf43:1;
//	unsigned long unkf44:1;
//	unsigned long unkf45:1;
//	unsigned long unkf46:1;
//	unsigned long unkf47:1;
//	unsigned long unkf48:1;
//	unsigned long unkf49:1;
//	unsigned long unkf50:1;
//	unsigned long unkf51:1;
//	unsigned long unkf52:1;
//	unsigned long unkf53:1;
//	unsigned long unkf54:1;
//	unsigned long unkf55:1;
//	unsigned long unkf56:1;
//	unsigned long unkf57:1;
//	unsigned long unkf58:1;
//	unsigned long unkf59:1;
//	unsigned long unkf60:1;
//	unsigned long unkf61:1;
//	unsigned long unkf62:1;
//	unsigned long unkf63:1;
//	unsigned long unkf64:1;
	unsigned long flags2;
	unsigned long unk2;
	unsigned long unk3;
	unsigned long *unk4;
	unsigned long unk5;
};

struct SceneryItem
{
	unsigned long unk1; //is 0 on disk files
	//flags
//	unsigned long unkf1:1;
//	unsigned long unkf2:1; //not used anywhere that I can see
//	unsigned long unkf3:1;
//	unsigned long unkf4:1; //on for vertical ride bases, stalls, flats and some other stuff
//	unsigned long unkf5:1; //not used anywhere that I can see
//	unsigned long unkf6:1; //is set to 1 for all tracks except vertical rides (i.e. elevator etc)
//	unsigned long unkf7:1;
//	unsigned long unkf8:1;
//	unsigned long unkf9:1; //set only on the column courtyard
//	unsigned long unkf10:1; //set for some track pieces
//	unsigned long unkf11:1; //set for some track pieces
//	unsigned long unkf12:1; //set for some track pieces
//	unsigned long unkf13:1; //controls whether an object can have walls on the same square as it except when its a 1/2 tile scenery item
//	unsigned long unkf14:1;
//	unsigned long unkf15:1; //set for some support pieces
//	unsigned long unkf16:1; //set for some support pieces
//	unsigned long unkf17:1; //set for water track (i.e. jetskis etc)
//	unsigned long unkf18:1; //not used anywhere that I can see
//	unsigned long unkf19:1; //not used anywhere that I can see
//	unsigned long unkf20:1; //set for helter-skelters
//	unsigned long unkf21:1; //not used anywhere that I can see
//	unsigned long unkf22:1; //not used anywhere that I can see
//	unsigned long unkf23:1; //not used anywhere that I can see
//	unsigned long unkf24:1; //not used anywhere that I can see
//	unsigned long unkf25:1; //not used anywhere that I can see
//	unsigned long unkf26:1; //not used anywhere that I can see
//	unsigned long unkf27:1; //not used anywhere that I can see
//	unsigned long unkf28:1; //not used anywhere that I can see
//	unsigned long unkf29:1; //not used anywhere that I can see
//	unsigned long unkf30:1; //not used anywhere that I can see
//	unsigned long unkf31:1; //not used anywhere that I can see
//	unsigned long unkf32:1; //not used anywhere that I can see
	unsigned long flags1;
	unsigned short positioningtype;
	unsigned short extraversion; // 0 = None, 1 or 2
	unsigned long unk4; //valid values are 0-17
	unsigned long xsquares;
	unsigned long ysquares;
	SceneryItemData *data; // Array, xsquares*ysquares in size
	float xpos;
	float ypos;
	float zpos;
	float xsize;
	float ysize;
	float zsize;
	unsigned long unk13count; //is 0 except in track parts
	unsigned long* unk14; //is 0 except in track parts (pointer I think)
	long cost;
	long removal_cost;
	unsigned long unk17; //is 0,4,5 4 is found on the queue paths
	unsigned long type;
	char *supports;
	unsigned long svdcount;
	SceneryItemVisual **svd; // Multiple ones used for trees
	GUISkinItem *icon;
	GUISkinItem *groupicon;
	wchar_t *groupname;
	char *OvlName;
	unsigned long count;
	SceneryParams *params;
	unsigned long SoundsCount; //count for sounds structures
	ScenerySound* Sounds; //for rides, points to data for sounds & other things
	wchar_t *Name;
	unsigned long unk27; //is 0
	unsigned long unk28; //is 0
	unsigned long stallunknown1; //is 0 except in stalls
	unsigned long stallunknown2; //is 0 except in stalls
	unsigned long defaultcol1; //can have values, is usually 0 though
	unsigned long defaultcol2; //can have values, is usually 0 though
	unsigned long defaultcol3; //can have values, is usually 0 though
	unsigned long unk34; //is 0
	unsigned long unk35; //is 0
	unsigned long unk36; //is 0
	unsigned long unk37; //is 0
	unsigned long unk38; //is 0
	unsigned long unk39; //is 0
	unsigned long unk40; //is 0
	unsigned long unk41; //is 0
	unsigned long carcount; //is 0 except for flying saucers and dodgems
	char** cars;            // List of strings
	unsigned long unk44; //is 0
	unsigned long unk45; //is 0 except for trampoline
	unsigned long unk46; //is 0 except for trampoline
	unsigned long unk47; //is 0 except for trampoline
	unsigned long unk48; //is 0 except for trampoline
};
struct SceneryItemExtra1
{
	SceneryExtraSound *SoundsUnk;              // Array of SoundsCount length
	unsigned long unk2;
	unsigned long AddonPack;        // 1 for Soaked
	unsigned long GenericAddon;
};

struct SceneryItemExtra2
{
	SceneryExtraSound *SoundsUnk;
	unsigned long unk2;
	unsigned long AddonPack;       // 0 = Vanilla, 1 = Soaked, 2 = Wild
	unsigned long GenericAddon;
	float unkf;                    // So far seen 1.0
	long billboardaspect;          ///< Seems to indicate aspect ratio of billboards
                                   /**
                                    * -1: non-billboards
                                    *  0: 8x1
                                    *  1: 4x1
                                    *  2: 2x1
                                    *  3: 1x1
                                    *  4: 1x2
                                    *  5: 1x4
                                    *  6: 1x8
                                    */
};

};

#endif
