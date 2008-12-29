/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __GUIICON_H__
#define __GUIICON_H__

#include "icontexture.h"

namespace r3old {

//presnt in importer
struct Icon
{
	char *name;
	unsigned long top;
	unsigned long left;
	unsigned long bottom;
	unsigned long right;
	char *texture;
};

struct GUISkinItemPos
{
	unsigned long left; //Modded by KLN - swapped left & top, swapped right & bottom
	unsigned long top;
	unsigned long right;
	unsigned long bottom;
};
struct GUISkinItem
{
	unsigned long unk1; //0 on disk files
	TextureStruct *tex; //0 on disk files
	GUISkinItemPos *pos;
	unsigned long unk2; //0 on disk files
};
struct GSI
{
	GUISkinItem g1;
	GUISkinItemPos g2;
};

};

namespace r3 {

struct GUISkinItem {
	uint32_t 		gsi_type;
	TextureStruct*	tex_ref; //0 on disk files
	uint32_t* 		pos;
	uint32_t 		unk2; //0 on disk files
};

};

#endif
