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

namespace r3 {

struct Triangle2 {
    uint16_t a;
    uint16_t b;
    uint16_t c;
};

struct BoneShapeMesh {
	uint32_t                support_type;       ///< was unk1
	FlexiTextureInfoStruct* ftx_ref;
	void*                   txs_ref;            ///< was TextureData
	uint32_t                transparency;       ///< was PlaceTexturing
	uint32_t                texture_flags;
	uint32_t                sides;
	uint32_t                vertex_count;
	uint32_t                index_count;
	VERTEX2*                vertexes;
	uint16_t*               indices;
};

struct BoneStruct {
	char*       bone_name;
	uint32_t    parent_bone_number;
};

struct BoneShape {
	VECTOR              bounding_box_min;       ///< Was 1
	VECTOR              bounding_box_max;       ///< Was 2
	uint32_t            total_vertex_count;
	uint32_t            total_index_count;
	uint32_t            mesh_count2;
	uint32_t            mesh_count;
	BoneShapeMesh**     sh;
	uint32_t            bone_count;
	BoneStruct*         bones;
	MATRIX*             bone_positions1;
	MATRIX*             bone_positions2;
};

};

#endif
