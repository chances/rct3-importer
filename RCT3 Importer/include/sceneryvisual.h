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
#include "manifoldmesh.h"

namespace r3 {

/*
#define SVDLOD_MESHTYPE_STATIC     0
#define SVDLOD_MESHTYPE_ANIMATED   3
#define SVDLOD_MESHTYPE_BILLBOARD  4
*/

struct SceneryItemVisualLOD {
	uint32_t                    type;           ///< LOD type
                                                /**<
                                                 * 0 = StaticShape, 3 = BoneShape, 4 = Billboard
                                                 **/
	char*                       lod_name;
	StaticShape*                shs_ref;        ///< shs reference for type 0
	uint32_t                    unk2;           ///< seen 0 so far
	BoneShape*                  bsh_ref;        ///< bsh reference for type 3
	uint32_t                    unk4;           ///< seen 0 so far
	FlexiTextureInfoStruct*     ftx_ref;        ///< ftx reference for type 4
	void*                       txs_ref;        ///< Texture Style for type 4, is always BillboardStandard.
	/// Billboard settings.
	float_t                     unk7;           ///< Display width? if not a billboard, 1.0
	float_t                     unk8;           ///< Display height? if not a billboard, 1.0
	float_t                     unk9;           ///< u or v coordinate 1? if not a billboard, 0
	float_t                     unk10;          ///< u or v coordinate 1? if not a billboard, 1.0
	float_t                     unk11;          ///< u or v coordinate 2? if not a billboard, 0
	float_t                     unk12;          ///< u or v coordinate 2? if not a billboard, 1.0
	float_t                     distance;       ///< has something to do with the distance this LOD model kicks in at or stops being used at
	uint32_t                    animation_count;
	uint32_t                    unk14;          ///< seen 0 so far
	BoneAnim***                 animations_ref; ///< ban references
};

/*
#define OVL_SIV_GREENERY              0x00000001
#define OVL_SIV_FLOWER                0x00000002
#define OVL_SIV_NO_SHADOW             0x00000002
#define OVL_SIV_ROTATION              0x00000004
#define OVL_SIV_UNKNOWN               0x00000070
#define OVL_SIV_SOAKED                0x01000000
#define OVL_SIV_WILD                  0x02000000
*/

struct SceneryItemVisual_V {
	uint32_t                    sivflags;       ///< Flags, contains structure version
                                                /**<
                                                 * 0x00000001 Set on Trees, Shrubs & Fern
                                                 * 0x00000002 Flowers
                                                 * 0x00000004 Set on Trees & Shrubs, Rotational Variation
                                                 * 0x00000070 Unknown
                                                 * 0x01000000 Soaked!
                                                 * 0x02000000 Wild!
                                                 **/
	float_t                     sway;           ///< Swaying
                                                /**<
                                                 * normally 0.0, 0.2 on trees & shrubs that move. 0.0 for safari theme trees.
                                                 **/
	float_t                     brightness;     ///< Brightness Variation
                                                /**<
                                                 * normally 1.0, 0.8 on trees & shrubs that move.
                                                 **/
	float_t                     unk4;           ///< Also seems to vary brigtness, seen 1.0 so far
	float_t                     scale;          ///< Scale Variation
                                                /**<
                                                 * normally 0.0, 0.4 on trees & shrubs that move.
                                                 **/
	uint32_t                    lod_count;
	SceneryItemVisualLOD**      lods;
	uint32_t                    unk6; //seen 0 so far
	uint32_t                    unk7; //seen 0 so far
	uint32_t                    unk8; //seen 0 so far
	uint32_t                    unk9; //seen 0 so far
	uint32_t                    unk10; //seen 0 so far
	uint32_t                    unk11; //seen 0 so far
};

struct SceneryItemVisual_Sext {
    ManifoldMesh*               proxy_ref;      ///< ManifoldMesh Proxy, whatever that is. Maybe related to water interaction
};

struct SceneryItemVisual_S {
    SceneryItemVisual_V         v;
    SceneryItemVisual_Sext      s;
};

struct SceneryItemVisual_Wext {
    uint32_t                    unk13;
};

struct SceneryItemVisual_W {
    SceneryItemVisual_V         v;
    SceneryItemVisual_Sext      s;
    SceneryItemVisual_Wext      w;
};

};

#endif
