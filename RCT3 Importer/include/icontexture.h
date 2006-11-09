/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __ICONTEXTURE_H__
#define __ICONTEXTURE_H__

struct BmpTbl
{
	unsigned long unk; //always 0 on disk files
	unsigned long count;
};

//before FlicStruct, put pointer to FlicStruct, loader points to FlicStruct
struct FlicStruct
{
	unsigned long *FlicDataPtr; //always 0 on disk files
	unsigned long unk1; //always 1
	float unk2; //always 1.0
};
//Present only in importer
struct Flic
{
	FlicStruct *fl;
	FlicStruct fl2;
};

struct FlicHeader
{
	unsigned long Format;
	unsigned long Width;
	unsigned long Height;
	unsigned long Mipcount;
};

struct FlicMipHeader
{
	unsigned long MWidth;
	unsigned long MHeight;
	unsigned long Pitch;
	unsigned long Blocks;
};


struct IconTexture
{
	char *name;
	char *filename;
	FlicHeader fh;
	FlicMipHeader fmh;
	unsigned char *data;
};

struct TextureStruct2;

struct TextureStruct
{
	unsigned long unk1; //always 0x70007
	unsigned long unk2; //always 0x70007
	unsigned long unk3; //always 0x70007
	unsigned long unk4; //always 0x70007
	unsigned long unk5; //always 0x70007
	unsigned long unk6; //always 0x70007
	unsigned long unk7; //always 0x70007
	unsigned long unk8; //always 0x70007
	unsigned long unk9; //always 1
	unsigned long unk10; //always 8
	unsigned long unk11; //seen 0x0D and 0x10, use 0x10 for icons
	unsigned long *TextureData; //always 0 on disk files, use GUIIcon:txs as a symbol resolve for icons
	unsigned long unk12; //always 1
	FlicStruct **Flic; //always points to pointer before flic data
	TextureStruct2 *ts2;
};

struct TextureStruct2
{
	TextureStruct *Texture; //points back to the TextureStruct
	FlicStruct *Flic; //points to the FlicStruct for this item
};

struct Tex
{
	TextureStruct t1;
	TextureStruct2 t2;
};

#endif
