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
	unsigned long unk1;
	char *InternalName;
	wchar_t *Name;
	GUISkinItem *gsi;
	TextureStruct *Texture1;
	TextureStruct *Texture2;
	char *Flat1;
	char *Flat2;
	char *Flat3;
	char *Flat4;
	char *StraightA1;
	char *StraightA2;
	char *StraightA3;
	char *StraightA4;
	char *StraightB1;
	char *StraightB2;
	char *StraightB3;
	char *StraightB4;
	char *TurnU1;
	char *TurnU2;
	char *TurnU3;
	char *TurnU4;
	char *TurnLA1;
	char *TurnLA2;
	char *TurnLA3;
	char *TurnLA4;
	char *TurnLB1;
	char *TurnLB2;
	char *TurnLB3;
	char *TurnLB4;
	char *TurnTA1;
	char *TurnTA2;
	char *TurnTA3;
	char *TurnTA4;
	char *TurnTB1;
	char *TurnTB2;
	char *TurnTB3;
	char *TurnTB4;
	char *TurnTC1;
	char *TurnTC2;
	char *TurnTC3;
	char *TurnTC4;
	char *TurnX1;
	char *TurnX2;
	char *TurnX3;
	char *TurnX4;
	char *CornerA1;
	char *CornerA2;
	char *CornerA3;
	char *CornerA4;
	char *CornerB1;
	char *CornerB2;
	char *CornerB3;
	char *CornerB4;
	char *CornerC1;
	char *CornerC2;
	char *CornerC3;
	char *CornerC4;
	char *CornerD1;
	char *CornerD2;
	char *CornerD3;
	char *CornerD4;
	char *Slope1;
	char *Slope2;
	char *Slope3;
	char *Slope4;
	char *SlopeStraight1;
	char *SlopeStraight2;
	char *SlopeStraight3;
	char *SlopeStraight4;
	char *SlopeStraightLeft1;
	char *SlopeStraightLeft2;
	char *SlopeStraightLeft3;
	char *SlopeStraightLeft4;
	char *SlopeStraightRight1;
	char *SlopeStraightRight2;
	char *SlopeStraightRight3;
	char *SlopeStraightRight4;
	char *SlopeMid1;
	char *SlopeMid2;
	char *SlopeMid3;
	char *SlopeMid4;
	unsigned long ResearchCategoryCount;
	Research *ResearchCategory;
};
struct QueueType
{
	char *InternalName;
	wchar_t *Name;
	GUISkinItem *gsi;
	TextureStruct *Texture;
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
