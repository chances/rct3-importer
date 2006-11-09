/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __TERRAINTYPE_H__
#define __TERRAINTYPE_H__

#include "icontexture.h"

struct TerrainType
{
	unsigned long unk1;
	unsigned long unk2;
	unsigned long unk3;
	unsigned long unk4;
	unsigned long unk5;
	TextureStruct *TerrainTexture;
	char *stringname;
	char *name;
	unsigned long unk6;
	unsigned long unk7;
	unsigned long unk8;
	unsigned long unk9;
	unsigned long unk10;
	unsigned long unk11;
};

#endif
