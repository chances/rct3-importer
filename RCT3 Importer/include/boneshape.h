/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __BONESHAPE_H__
#define __BONESHAPE_H__

#include "vertex.h"
#include "flexitexture.h"

struct BoneShape2 {
	unsigned long unk1; //always 0xFFFFFFFFF
	FlexiTextureInfoStruct *fts; //is 0 in disk files
	unsigned long *TextureData; //is 0 in disk files
	unsigned long PlaceTexturing; //0 = dont see texturing on things when being placed, 1 = see texturing on things when being placed
	unsigned long textureflags; //always 0
	unsigned long unk4; //seen values of 3 or 1 for this, 3 is more common
	unsigned long VertexCount;
	unsigned long IndexCount;
	VERTEX2 *Vertexes;
	unsigned short *Triangles;
};
struct BoneStruct {
	char *BoneName;
	unsigned long BoneNumber;
};
struct BoneShape1 {
	D3DVECTOR BoundingBox1;
	D3DVECTOR BoundingBox2;
	unsigned long TotalVertexCount;
	unsigned long TotalIndexCount;
	unsigned long MeshCount2;
	unsigned long MeshCount;
	BoneShape2 **sh;
	unsigned long BoneCount;
	BoneStruct *Bones;
	D3DMATRIX *BonePositions1;
	D3DMATRIX *BonePositions2;
};

#endif
