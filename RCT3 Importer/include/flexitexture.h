/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __FLEXITEXTURE_H__
#define __FLEXITEXTURE_H__

#define FTX_CHOICE_FIRST    1
#define FTX_CHOICE_SECOND   2
#define FTX_CHOICE_THIRD    4

struct FlexiTextureStruct
{
	unsigned long scale;
	unsigned long width;
	unsigned long height;
	unsigned long Recolorable; //1 = first color recolorable, 2 = second color recolorable, 4 = third color recolorable, combination of them = multiple colors recolorable
	unsigned char *palette;
	unsigned char *texture;
	unsigned char *alpha;
};
struct FlexiTextureInfoStruct
{
	unsigned long scale;
	unsigned long width;
	unsigned long height;
	unsigned long fps;         // Animation Speed, approx. frames per second
	unsigned long Recolorable; //1 = first color recolorable, 2 = second color recolorable, 4 = third color recolorable, combination of them = multiple colors recolorable
	unsigned long offsetCount;
	unsigned long *offset1;
	unsigned long fts2Count;
	FlexiTextureStruct *fts2;
};

#endif
