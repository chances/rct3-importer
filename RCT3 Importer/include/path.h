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
	char *Flat1;        ///< Straight, no rails
	char *Flat2;
	char *Flat3;
	char *Flat4;
	char *StraightA1;   ///< Straight, left and right rails
	char *StraightA2;
	char *StraightA3;
	char *StraightA4;
	char *StraightB1;   ///< Straight, right rail
	char *StraightB2;
	char *StraightB3;
	char *StraightB4;
	char *TurnU1;       ///< Straight, back, front and right rail
	char *TurnU2;
	char *TurnU3;
	char *TurnU4;
	char *TurnLA1;      ///< Straight, front and right rail, rail stump in open corner
	char *TurnLA2;
	char *TurnLA3;
	char *TurnLA4;
	char *TurnLB1;      ///< Straight, front and right rail
	char *TurnLB2;
	char *TurnLB3;
	char *TurnLB4;
	char *TurnTA1;      ///< Straight, back rail, rail stumps in both open corners
	char *TurnTA2;
	char *TurnTA3;
	char *TurnTA4;
	char *TurnTB1;      ///< Straight, back rail, rail stump in front left corner
	char *TurnTB2;
	char *TurnTB3;
	char *TurnTB4;
	char *TurnTC1;      ///< Straight, back rail, rail stump in front right corner
	char *TurnTC2;
	char *TurnTC3;
	char *TurnTC4;
	char *TurnX1;       ///< Straight, no rail, rail stumps in all corners
	char *TurnX2;
	char *TurnX3;
	char *TurnX4;
	char *CornerA1;     ///< Straight, no rail, rail stump in back left corner
	char *CornerA2;
	char *CornerA3;
	char *CornerA4;
	char *CornerB1;     ///< Straight, no rail, rail stumps in back left ard right corner
	char *CornerB2;
	char *CornerB3;
	char *CornerB4;
	char *CornerC1;     ///< Straight, no rail, rail stumps in front left and back right corner
	char *CornerC2;
	char *CornerC3;
	char *CornerC4;
	char *CornerD1;     ///< Straight, no rail, rail stumps in all corners except back left corner
	char *CornerD2;
	char *CornerD3;
	char *CornerD4;
	char *Slope1;              ///< Steep slope, goes down left to right, rails back and front
	char *Slope2;
	char *Slope3;
	char *Slope4;
	char *SlopeStraight1;      ///< Shallow slope, goes down left to right, both rails (back and front)
	char *SlopeStraight2;
	char *SlopeStraight3;
	char *SlopeStraight4;
	char *SlopeStraightLeft1;  ///< Shallow slope, goes down left to right, left rail (front)
	char *SlopeStraightLeft2;
	char *SlopeStraightLeft3;
	char *SlopeStraightLeft4;
	char *SlopeStraightRight1; ///< Shallow slope, goes down left to right, right rail (right)
	char *SlopeStraightRight2;
	char *SlopeStraightRight3;
	char *SlopeStraightRight4;
	char *SlopeMid1;           ///< Shallow slope, goes down left to right, no rail
	char *SlopeMid2;
	char *SlopeMid3;
	char *SlopeMid4;
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
	char *FlatFc1;      ///< Straight, rails on front and back, closed on left and right
	char *FlatFc2;
	char *FlatFc3;
	char *FlatFc4;
	char *SlopeFc1;              ///< Steep slope, goes down left to right, rails back and front, closed on left and right
	char *SlopeFc2;
	char *SlopeFc3;
	char *SlopeFc4;
	char *SlopeBc1;              ///< Steep slope, goes down left to right, rails back and front, closed on right (bottom)
	char *SlopeBc2;
	char *SlopeBc3;
	char *SlopeBc4;
	char *SlopeTc1;              ///< Steep slope, goes down left to right, rails back and front, closed on left (top)
	char *SlopeTc2;
	char *SlopeTc3;
	char *SlopeTc4;
	char *SlopeStraightFc1;              ///< Shallow slope, goes down left to right, rails back and front, closed on left and right
	char *SlopeStraightFc2;
	char *SlopeStraightFc3;
	char *SlopeStraightFc4;
	char *SlopeStraightBc1;              ///< Shallow slope, goes down left to right, rails back and front, closed on right (bottom)
	char *SlopeStraightBc2;
	char *SlopeStraightBc3;
	char *SlopeStraightBc4;
	char *SlopeStraightTc1;              ///< Shallow slope, goes down left to right, rails back and front, closed on left (top)
	char *SlopeStraightTc2;
	char *SlopeStraightTc3;
	char *SlopeStraightTc4;
	char *SlopeStraightLeftFc1;              ///< Shallow slope, goes down left to right, rails front, closed on left and right
	char *SlopeStraightLeftFc2;
	char *SlopeStraightLeftFc3;
	char *SlopeStraightLeftFc4;
	char *SlopeStraightLeftBc1;              ///< Shallow slope, goes down left to right, rails front, closed on right (bottom)
	char *SlopeStraightLeftBc2;
	char *SlopeStraightLeftBc3;
	char *SlopeStraightLeftBc4;
	char *SlopeStraightLeftTc1;              ///< Shallow slope, goes down left to right, rails front, closed on left (top)
	char *SlopeStraightLeftTc2;
	char *SlopeStraightLeftTc3;
	char *SlopeStraightLeftTc4;
	char *SlopeStraightRightFc1;              ///< Shallow slope, goes down left to right, rails back, closed on left and right
	char *SlopeStraightRightFc2;
	char *SlopeStraightRightFc3;
	char *SlopeStraightRightFc4;
	char *SlopeStraightRightBc1;              ///< Shallow slope, goes down left to right, rails back, closed on right (bottom)
	char *SlopeStraightRightBc2;
	char *SlopeStraightRightBc3;
	char *SlopeStraightRightBc4;
	char *SlopeStraightRightTc1;              ///< Shallow slope, goes down left to right, rails back, closed on left (top)
	char *SlopeStraightRightTc2;
	char *SlopeStraightRightTc3;
	char *SlopeStraightRightTc4;
	char *SlopeMidFc1;              ///< Shallow slope, goes down left to right, no rail, closed on left and right
	char *SlopeMidFc2;
	char *SlopeMidFc3;
	char *SlopeMidFc4;
	char *SlopeMidBc1;              ///< Shallow slope, goes down left to right, no railk, closed on right (bottom)
	char *SlopeMidBc2;
	char *SlopeMidBc3;
	char *SlopeMidBc4;
	char *SlopeMidTc1;              ///< Shallow slope, goes down left to right, no rail, closed on left (top)
	char *SlopeMidTc2;
	char *SlopeMidTc3;
	char *SlopeMidTc4;
	char *Paving;                   ///< Flat, no rail, bottom (underworkings) closed. No idea what this is used for. Can be non-relocated.
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

#endif
