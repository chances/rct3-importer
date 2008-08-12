/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __SCENERYREVISED_H__
#define __SCENERYREVISED_H__

#include "guiicon.h"
#include "sceneryvisual.h"
#include "vertex.h"

#ifndef _WAVEFORMATEX_
#ifndef WAVEFORMATEX
#define _WAVEFORMATEX_

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

struct Sound {
	WAVEFORMATEX    format;
	int32_t         unk6; 		// always 0x00000000 could also be a float
	float_t         unk7; 		// always 0.050000001
	float_t         unk8; 		// always 3.9999999e-005
	float_t         unk9; 		// always 1.0
	float_t         unk10; 		// always 0.0
	float_t         unk11; 		// always 15000000.0
	float_t         unk12; 		// always 0.0
	float_t         unk13; 		// always 0.050000001
	float_t         unk14; 		// always 2.0
	float_t         unk15; 		// always 30.0
	float_t         unk16; 		// always 0.0
	int16_t*        channel1; 	// shorts
	int             channel1_size; 	// size of the above data
	int16_t*        channel2; 	// shorts, NULL in no channel2
	int             channel2_size; 	// 0 if not channel2
};

struct SceneryExtraSound {
    float_t         unk1;
    float_t         unk2;
    uint32_t        unk3;  // Usually 0
};

struct SoundScript1 {
    float_t         unk1;         // != 0.0
    uint32_t        unk2; // 0 terminates list, otherwise always 1
};

struct SoundScript2 { // Some kind of script
    uint32_t        unk1; // == 0
    // repeated till instruction == 0
    uint32_t        instruction;   // 1, 2, 3 or 4
    float_t         parameter[];   // x = 1 for i = 1 or 2
                                   // x = 3 for i = 3 or 4.
                                   // the last value of a triple and the only value of 1/2 always increase in the list
                                   // the maximum of this increase seems to be the length of the animation in the case
                                   // of a ride event.
                                   // I found no kind of index into the SoundArray, so either they are played in parallel,
                                   // as a whole or in turn.
                                   // I suspect intsruction 1 is like a SoundScript1 entry. It probably means "Play sounds at
                                   // this timepoint".
};

struct ScenerySound {
    uint32_t        sound_count;
    Sound**         sourd_refs;
    uint32_t        sound_script_count; // Rides seem to have 4, Ride Events 1
    SoundScript1**  sound_scripts; // Each entry is either a SoundScript1 or SoundScript2 "structure"
};

struct SceneryParams {
	char*           type;
	char*           params;
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
struct SceneryItemData { //set all to 0 and see what happens
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
    union {
        uint32_t    flags2;
        uint32_t    flags;
    };

    /**
     * Unk 6 in the importer
     * Seems to be minimal blocking height
     **/
    union {
        int32_t     unk2;
        int32_t     min_height_block;
    };
    /**
     * Unk 7 in the importer
     * Seems to be maximal blocking height
     **/
    union {
        int32_t     unk3;
        int32_t     max_height_block;
    };
    /**
     * Unk 8 in the importer
     * Points to (2^(unk3-unk2))-1
     * Can be bigger than a long
     **/
    union {
        uint32_t*   unk4;
        uint32_t*   block_bits;
    };
	uint32_t        supports;         ///< Unk 9 in the importer. Together with the respective flag and collision needed to show supports
};

struct SceneryItemTrack {
    uint32_t        unk01;          ///< >= 0
    uint32_t        unk02;          ///< >= 0
    uint32_t        unk03;          ///< 0-2
    uint32_t        unk04;          ///< 0-2
    float_t         unk05f;         ///< 0.0 - 1.0
    float_t         unk06f;         ///< 0.0 - 1.0
    float_t         unk07f;
    int32_t         unk08;          ///< >= -1
    int32_t         unk09;          ///< >= -1
    uint32_t        unk10;          ///< 0-2
    uint32_t        unk11;          ///< 0-2
    float_t         unk12f;         ///< Usually identical to unk07f
    uint32_t        unk13; //0
    uint32_t        unk14; //0
    uint32_t        unk15; //0
    uint32_t        unk16; //0
    uint32_t        unk17; //0
    float_t         unk18f;         ///< Always 0.5
    float_t         unk19f;         ///< Always 0.5
    uint32_t        unk20; //0
    uint32_t        unk21;          ///< 0-2
    uint32_t        unk22;          ///< 1-9
};

struct SceneryItem_V {
	uint32_t            unk1;                   ///< Always 0, never a symref
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
	uint32_t            flags;
	uint16_t            position_type;
	uint16_t            structure_version;      ///< 0 = None, 1 or 2
	uint32_t            unk4;                   ///< Unk 1 in the importer
                                                /**<
                                                 * valid values are 0-17
                                                 **/
	uint32_t            squares_x;
	uint32_t            squares_z;
	SceneryItemData*    data;                   ///< Array, xsquares*ysquares in size
	float_t             position_x;
	float_t             position_y;
	float_t             position_z;
	float_t             size_x;
	float_t             size_y;
	float_t             size_z;
	uint32_t            track_struct_count; //is 0 except in track parts
	SceneryItemTrack*   track_structs; //is 0 except in track parts (pointer I think)
	int32_t             cost;
	int32_t             removal_cost;
	uint32_t            unk17;                  ///< Unk 2 in the importer
                                                /**<
                                                 * is 0,4,5 4 is found on the queue paths
                                                 */
	uint32_t            type;
	char*               supports;
	uint32_t            svd_count;
	SceneryItemVisual_V** svds_ref; // Multiple ones used for trees
	GUISkinItem*        icon_ref;
	GUISkinItem*        group_icon_ref;
	wchar_t*            group_name_ref;
	char*               ovl_path;
	uint32_t            param_count;
	SceneryParams*      params;
	uint32_t            sound_count; //count for sounds structures
	ScenerySound*       sounds; //for rides, points to data for sounds & other things
	wchar_t*            name_ref;
	uint32_t            unk27;                  ///< Always 0, never a symref
	uint32_t            unk28;                  ///< Always 0, never a symref
	uint32_t            stall_unknown1; //is 0 except in stalls
	uint32_t            stall_unknown2; //is 0 except in stalls
	uint32_t            default_col1; //can have values, is usually 0 though
	uint32_t            default_col2; //can have values, is usually 0 though
	uint32_t            default_col3; //can have values, is usually 0 though
	uint32_t            unk34;                  ///< Always 0, never a symref
	uint32_t            unk35;                  ///< Always 0, never a symref
	uint32_t            unk36;                  ///< Always 0, never a symref
	uint32_t            unk37;                  ///< Always 0, never a symref
	uint32_t            unk38;                  ///< Always 0, never a symref
	uint32_t            unk39;                  ///< Always 0, never a symref
	uint32_t            unk40;                  ///< Always 0, never a symref
	uint32_t            unk41;                  ///< Always 0, never a symref
	uint32_t            individual_animation_anr_name_count; //is 0 except for flying saucers and dodgems
	char**              individual_animation_anr_names;            // List of strings
	uint32_t            unk44;                  ///< Always 0, never a symref
	uint32_t            anr_age_alternatives; //is 0 except for trampoline (3)
	uint32_t            anr_animation_chunks; //is 0 except for trampoline (4), parachute drop (12)
	uint32_t            anr_alternate_run_animations; //is 0 except for trampoline (2), parachute drop (1)
	uint32_t            anr_animation_cycles_per_circuit; //is 0 except for trampoline (4), parachute drop (1)
};

struct SceneryItem_Sext {
	SceneryExtraSound*  sounds_extra;              // Array of SoundsCount length
	uint32_t            unk2;
	uint32_t            addon_pack;        // 1 for Soaked
	uint32_t            generic_addon;
};

struct SceneryItem_S {
    SceneryItem_V       v;
    SceneryItem_Sext    s;
};

struct SceneryItem_Wext {
	float_t             unkf;                    // So far seen 1.0
	int32_t             billboard_aspect;          ///< Seems to indicate aspect ratio of billboards
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

struct SceneryItem_W {
    SceneryItem_V       v;
    SceneryItem_Sext    s;
    SceneryItem_Wext    w;
};

};

#endif
