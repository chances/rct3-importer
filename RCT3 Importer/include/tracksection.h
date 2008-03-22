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

#ifndef TRACKSECTION_H_INCLUDED
#define TRACKSECTION_H_INCLUDED

namespace r3 {

struct TrackSectionChainStruct {
    float_t             unk1;
    float_t             unk2;
    float_t             unk3;
};

struct TrackSection_V {
        char* internalname;
        SceneryItem* SID;
        unsigned long entry_curve;	// Probably curve related
        unsigned long exit_curve;	// Probablycurve related
        unsigned long special_curves;	// Special curve types
        unsigned long direction;	// 0 straight, 1 left, 2 right
        unsigned long unk7; 		// Bitflags
        				// 0x00000002 indicates chain,
        				// 0x00000004 indicates special piece, eg. loop, sbend, roll, brakes, etc...
        				// 0x00000100 indicates block break, also set for TiltSection.
        				// 0x00400000 set for track 5, inverted?
        				// 0x00600000 set for the thrill lift pieces
        				// 0x00000200 thrill lift bottom
        				// 0x00000400 thrill lift middle
        				// 0x00000800 thrill lift top
        unsigned long unk8; 		// Seems to be identical to above on track types that I've examined.
        Spline*	spline1;
        Spline*	spline2;
        Spline*	spline3;
        Spline*	spline4;
        unsigned long unk13;
        unsigned long unk14;
        unsigned long unk15;
        unsigned long unk16;
        unsigned long unk17;
        unsigned long unk18;		// Set to either 0 or 25 (0x19)
        unsigned long entry_slope; 	// 0: flat
					// 1: medium up
					// 2: medium down
					// 3: steep up
					// 4: steep down
					// 5: vertical
        unsigned long entry_bank;	// 0: flat
					// 1: bank left
					// 2: bank full left
					// 3: inverted bank left
					// 4: inverted
					// 5: inverted bank right
					// 6: bank full right
					// 7: bank right
        char* entry_trackgroup;
        unsigned long unk22;
        unsigned long unk23;
        unsigned long unk24;
        unsigned long unk25;		// Corresponds with unk18 in all known cases
        unsigned long exit_slope;	// see entry_slope
        unsigned long exit_bank;	// see entry_bank
        char* exit_trackgroup;
        unsigned long unk29; 		// count for below?
        ChainStruct*  unk30; 		// array? set for chain pieces
        unsigned long unk31;
        unsigned long unk32;
        unsigned long unk33;
        float unk34;
        float unk35;
        union {
            int32_t         unk36_v;            ///< -1 for _V
            int32_t         long_count_sw; 		///< entries in the following array for _S and _W. ??? If -1 then the structure ends here. ???
        };
        union {
            int32_t         unk37_v;
            int32_t*        longs_sw;
        };
        unsigned long unk38;
        unsigned long unk39;
        unsigned long unk40;
        unsigned long unk41;
        unsigned long unk42;
        unsigned long unk43;
        unsigned long unk44;
        unsigned long unk45;
        unsigned long unk46;
        float unk47;
        float unk48;
        float unk49;
        float unk50;
        unsigned long unk51;
        unsigned long unk52;
        unsigned long unk53;
        unsigned long unk54;
        unsigned long unk55;
        unsigned long unk56;
        unsigned long unk57;
};

struct TrackSectionHouseStruct {
    uint32_t                unk1;
    uint32_t                unk2;
    uint32_t                unk3;
    uint32_t                unk4;
};

struct TrackSection_Sext {
        unsigned long unk58;
        unsigned long unk59;
        unsigned long unk60;
        unsigned long unk61;
/*
        unsigned long unk62;
        float unk63;
*/
        uint32_t            house_struct_count;
        TrackSectionHouseStruct* house_structs;     ///< Present in eg Insect House
        float unk64;
        float unk65;
        unsigned long unk66;
        unsigned long unk67;
        unsigned long unk68;
        unsigned long unk69;
        unsigned long unk70;
        unsigned long unk71;
        unsigned long unk72;
        float unk73;
        float unk74;
        float unk75;
        float unk76;
        float unk77;
        float unk78;
        float unk79;
        unsigned long unk80;
        unsigned long unk81;
        unsigned long unk82;
        unsigned long unk83;
        unsigned long unk84;
        unsigned long unk85;
        unsigned long unk86;
        unsigned long unk87;
        unsigned long unk88;
        unsigned long unk89;
        unsigned long unk90;
        unsigned long unk91;
};

struct TrackSection_Wext {
        unsigned long unk92;
        unsigned long unk93;
        float unk94;
        float unk95;
        unsigned long unk96;
        float unk97;
        float_t unk98;
};

};

#endif // TRACKSECTION_H_INCLUDED
