/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __PATH_H__
#define __PATH_H__

#include "flexitexture.h"
#include "guiicon.h"

namespace r3 {

struct Research
{
	unsigned char *ResearchCategory;
	unsigned long unk1;
	unsigned long unk2;
};
struct PathType
{
	unsigned long unk1; ///< Flags.
                        /**
                         *  0x00000001 Always present.
                         *  0x00000002 Probably under water.
                         *  0x01000000 Extened (PathType2) structure.
                         */
	char *InternalName;
	wchar_t *Name;
	GUISkinItem *gsi;
	char *Texture1;     ///< This is a (tex) texture in the InternalName_Texture ovl
	char *Texture2;
	char *Flat[4];        ///< Straight, no rails
	char *StraightA[4];   ///< Straight, left and right rails
	char *StraightB[4];   ///< Straight, right rail
	char *TurnU[4];       ///< Straight, back, front and right rail
	char *TurnLA[4];      ///< Straight, front and right rail, rail stump in open corner
	char *TurnLB[4];      ///< Straight, front and right rail
	char *TurnTA[4];      ///< Straight, back rail, rail stumps in both open corners
	char *TurnTB[4];      ///< Straight, back rail, rail stump in front left corner
	char *TurnTC[4];      ///< Straight, back rail, rail stump in front right corner
	char *TurnX[4];       ///< Straight, no rail, rail stumps in all corners
	char *CornerA[4];     ///< Straight, no rail, rail stump in back left corner
	char *CornerB[4];     ///< Straight, no rail, rail stumps in back left ard right corner
	char *CornerC[4];     ///< Straight, no rail, rail stumps in front left and back right corner
	char *CornerD[4];     ///< Straight, no rail, rail stumps in all corners except back left corner
	char *Slope[4];              ///< Steep slope, goes down left to right, rails back and front
	char *SlopeStraight[4];      ///< Shallow slope, goes down left to right, both rails (back and front)
	char *SlopeStraightLeft[4];  ///< Shallow slope, goes down left to right, left rail (front)
	char *SlopeStraightRight[4]; ///< Shallow slope, goes down left to right, right rail (right)
	char *SlopeMid[4];           ///< Shallow slope, goes down left to right, no rail
	unsigned long ResearchCategoryCount;
	Research *ResearchCategory;
};

/// Extended path type.
/**
 * Defines elements for tunnel like paths that can have closed ends.
 * Non-implemented elements are relocated to an empty string.
 */
struct PathTypeExtended
{
    unsigned long unk1; ///< Seen 0.
    unsigned long unk2; ///< Seen 1.
	char *FlatFc[4];      ///< Straight, rails on front and back, closed on left and right
	char *SlopeFc[4];              ///< Steep slope, goes down left to right, rails back and front, closed on left and right
	char *SlopeBc[4];              ///< Steep slope, goes down left to right, rails back and front, closed on right (bottom)
	char *SlopeTc[4];              ///< Steep slope, goes down left to right, rails back and front, closed on left (top)
	char *SlopeStraightFc[4];         ///< Shallow slope, goes down left to right, rails back and front, closed on left and right
	char *SlopeStraightBc[4];         ///< Shallow slope, goes down left to right, rails back and front, closed on right (bottom)
	char *SlopeStraightTc[4];         ///< Shallow slope, goes down left to right, rails back and front, closed on left (top)
	char *SlopeStraightLeftFc[4];     ///< Shallow slope, goes down left to right, rails front, closed on left and right
	char *SlopeStraightLeftBc[4];     ///< Shallow slope, goes down left to right, rails front, closed on right (bottom)
	char *SlopeStraightLeftTc[4];     ///< Shallow slope, goes down left to right, rails front, closed on left (top)
	char *SlopeStraightRightFc[4];    ///< Shallow slope, goes down left to right, rails back, closed on left and right
	char *SlopeStraightRightBc[4];    ///< Shallow slope, goes down left to right, rails back, closed on right (bottom)
	char *SlopeStraightRightTc[4];    ///< Shallow slope, goes down left to right, rails back, closed on left (top)
	char *SlopeMidFc[4];              ///< Shallow slope, goes down left to right, no rail, closed on left and right
	char *SlopeMidBc[4];              ///< Shallow slope, goes down left to right, no rail, closed on right (bottom)
	char *SlopeMidTc[4];              ///< Shallow slope, goes down left to right, no rail, closed on left (top)
	char *Paving;                     ///< Flat, no rail, bottom (underworkings) closed. No idea what this is used for. Can be non-relocated.
};

struct PathType2
{
    PathType base;
    PathTypeExtended extended;
};

struct QueueType
{
	char *InternalName;
	wchar_t *Name;
	GUISkinItem *gsi;
	FlexiTextureInfoStruct *ftx;  ///< This is not in a referenced ovl but in InternalName_Texture
	char *Straight;
	char *TurnL;
	char *TurnR;
	char *SlopeUp;
	char *SlopeDown;
	char *SlopeStraight1;
	char *SlopeStraight2;
	unsigned long ResearchCategoryCount;
	Research *ResearchCategory;
};

};

#endif
