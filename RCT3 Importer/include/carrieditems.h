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

#ifndef __carrieditems_h__
#define __carrieditems_h__

#include "guiicon.h"
#include "staticshape.h"

struct CarriedItemExtra
{
    unsigned long unk1; // 0
    wchar_t* name;
	GUISkinItem *icon;  // 20x20
	unsigned long unk4; // 0 usually, 4 for Mustard
	float hunger;         // >= 0.0, <= 0.3   Probably influences hunger
	float thirst;         // >= -0.3, <=0.3   Probably influences thirst, negative increases, positive quenches
	float unk7;         // 0.0 usually, 0.3 for Parmesan, Chilli, ChilliSauce and Marshmellow. Nausea?
};

// Carrieditem flags
#define CARRIEDITEM_NOTSOLD         0x00000000  // None of the other < 0x1000
                                                // Used in Autograph, Camera, Camcorder, GolfBall,
                                                // GolfClub + KidGolfClub
#define CARRIEDITEM_FOOD            0x00000001
#define CARRIEDITEM_DRINK           0x00000002  // Icecream is 0x3
#define CARRIEDITEM_USED            0x00000004  // Used up version of something
#define CARRIEDITEM_MAP             0x00000008
#define CARRIEDITEM_VOUCHER         0x00000010
#define CARRIEDITEM_UMBRELLA_OPEN   0x00000020  // Open umbrella has 0x60
#define CARRIEDITEM_UMBRELLA        0x00000040
#define CARRIEDITEM_BALOON          0x00000080  // Ballon is 0x280
#define CARRIEDITEM_SOUVENIR        0x00000200
#define CARRIEDITEM_KIDDIE_VERISON  0x00001000  // Seen for umbrella
#define CARRIEDITEM_CAMERA          0x00002000
#define CARRIEDITEM_BALOON2         0x00004000  // Set in Baloon + Sunglasses, maybe recolorable
#define CARRIEDITEM_CAMCORDER       0x00008000
#define CARRIEDITEM_SUNBLOCK        0x00010000
#define CARRIEDITEM_INFLATABLE      0x00020000
#define CARRIEDITEM_SWIMSUIT        0x00080000

// Carreiditem types, probably flags as well
#define CARRIEDITEM_TYPE_NOTVISIBLE     1
#define CARRIEDITEM_TYPE_HAND           2
#define CARRIEDITEM_TYPE_BODY           4 // Shirts, SunBlocker
#define CARRIEDITEM_TYPE_TWOHANDS       6 // Maybe body or hand
#define CARRIEDITEM_TYPE_HEAD           8
#define CARRIEDITEM_TYPE_SUNGLASSES     16
#define CARRIEDITEM_TYPE_SWIMSUIT       36


struct CarriedItem
{
        unsigned long unk1;             // Always 1
        unsigned long addonpack;        // 0 vanilla, 1 soaked, 2 wild
        unsigned long unk3;             // 0 usually, 2 for MoreCarriedItems
        wchar_t* name;
        wchar_t* pluralname;
        GUISkinItem *GSI;               // :gsi for none 40x40
        StaticShape1 *shape1;           // So far shape2 always matches shape1
        StaticShape1 *shape2;           // Seems like alpha textures don't work, at least not with full alpha range
        float unk9;                     // 0.0 for hidden items, 40.0 usually for shape items, 200.0 for billboard items. Probably a LOD distance
        CarriedItem *wrapper;           // Used up version or open umbrella, :cid for none
        unsigned long flags;            // see above
        long ageclass;                  // -1 Usually, for Soaked complex items: 3 kids, 12 teens and 48 adults
        unsigned long type;             // see above
        float hunger;                   // Hunger mod, positive quences, negative icreases
        float thirst;                   // Thirst mod
        float lightprotectionfactor;    // 0.0 usually, 4.0, 15.0 and 50.0 for the respective sunblockers
        unsigned long unk17;           // unk17-20 are somehow related to package size and/or desirability
        unsigned long unk18;           // unk19 >= unk18 >= unk17, unk20 can be smaller than unk19
        unsigned long unk19;           // The values of a bigger version of an item are larger
        unsigned long unk20;           // Could be a price
        float trash1;                   // 0.0 usually, 0.01 for trash
        long trash2;                    // -1 usually, 1 for FoodWrapper and Stick, 2 for boxes and bottles
        unsigned long defaultcolour;    // 0 usually, 7 for Baloon. Defaultcolour for flexis?
        unsigned long extracount;
        CarriedItemExtra **extras;
        unsigned long unk26;
        unsigned long unk27;
        unsigned long unk28;
        unsigned long shaped;           // Usually 1, 0 for billboard objects
        FlexiTextureInfoStruct *fts;    // is the FlexiTexture for a billboard object. Seems to use AlphaMask or AlphaMaskLow
        float scalex;                    // These scale the billboard
        float scaley;                    //
        unsigned long unk33;            // 0 usually, 16 for inflatables and swimsuits, 32 for sunglasses
        char* male_morphmesh_body;      // All char*s point to empty strings if not set
        char* male_morphmesh_legs;
        char* female_morphmesh_body;
        char* female_morphmesh_legs;
        unsigned long unk38;            // 0 usually, 1 for SwimSuitV1
        char* textureoption;            // "Inflatable", "DefaultFlexiColours" (Sunglasses), "SwimSuitV1" or "SwimSuitV2"
//        CarriedItemExtra* carried;
//        StaticShape1* shape3;
};

#endif
