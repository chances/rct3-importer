/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __SCENERYVISUAL_H__
#define __SCENERYVISUAL_H__

#include "boneanim.h"
#include "boneshape.h"
#include "staticshape.h"

namespace r3 {

#define SVDLOD_MESHTYPE_STATIC     0
#define SVDLOD_MESHTYPE_ANIMATED   3
#define SVDLOD_MESHTYPE_BILLBOARD  4

struct SceneryItemVisualLOD
{
	unsigned long MeshType; //seems to be flag for what type of mesh this is, 0 = StaticShape, 3 = BoneShape, 4 = Billboard
	char *LODName;
	StaticShape *ss; //Set to 0 in disk files, is the StaticStape for static meshes in ram
	unsigned long unk2; //seen 0 so far
	BoneShape* bs; //Set to 0 in disk files, is the BoneStape for animated meshes in ram
	unsigned long unk4; //seen 0 so far
	FlexiTextureInfoStruct *fts; //Set to 0 in disk files, is the FlexiTexture for a billboard object
	unsigned long *TextureData; //Set to 0 in disk files, is the Texture Style for a billboard object, is always BillboardStandard.
	float unk7; //seen 1.0 so far
	float unk8; //seen 1.0 so far
	float unk9; //seen 0 so far
	float unk10; //seen 1.0 so far
	float unk11; //seen 0 so far
	float unk12; //seen 1.0 so far
	float distance; //has something to do with the distance this LOD model kicks in at or stops being used at
	unsigned long AnimationCount;
	unsigned long unk14; //seen 0 so far
	BoneAnim ***AnimationArray; //points to one or more BoneAnim pointers which are 0 in disk files
};

#define OVL_SIV_GREENERY              0x00000001
#define OVL_SIV_FLOWER                0x00000002
#define OVL_SIV_NO_SHADOW             0x00000002
#define OVL_SIV_ROTATION              0x00000004
#define OVL_SIV_UNKNOWN               0x00000070
#define OVL_SIV_SOAKED                0x01000000
#define OVL_SIV_WILD                  0x02000000

struct SceneryItemVisual
{
	unsigned long sivflags; // Flags
	                    // 0x00000001 Set on Trees, Shrubs & Fern
	                    // 0x00000002 Flowers
	                    // 0x00000004 Set on Trees & Shrubs, Rotational Variation
	                    // 0x00000070 Unknown
	                    // 0x01000000 Soaked!
	                    // 0x02000000 Wild!
	float sway;         //seen 0.0 so far, 0.2 on trees & shrubs that move. 0.0 for safari theme trees. Amount of Swaying
	float brightness;   //seen 1.0 so far, 0.8 on trees & shrubs that move. Brightness Variation
	float unk4;         //seen 1.0 so far
	float scale;        //seen 0.0 so far, 0.4 on trees & shrubs that move. Scale Variation
	unsigned long LODCount;
	SceneryItemVisualLOD **LODMeshes;
	unsigned long unk6; //seen 0 so far
	unsigned long unk7; //seen 0 so far
	unsigned long unk8; //seen 0 so far
	unsigned long unk9; //seen 0 so far
	unsigned long unk10; //seen 0 so far
	unsigned long unk11; //seen 0 so far
};

struct SceneryItemVisualB {
    SceneryItemVisual vA;
    unsigned long unk12;
};

struct SceneryItemVisualC {
    SceneryItemVisual vA;
    unsigned long unk12;
    unsigned long unk13;
};

};

#endif
