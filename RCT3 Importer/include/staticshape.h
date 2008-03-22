/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __STATICSHAPE_H__
#define __STATICSHAPE_H__

#include "vertex.h"
#include "flexitexture.h"

namespace r3 {

/*
#define SS2_PLACE_TEXTURE_AND_MESH  0
#define SS2_PLACE_TEXTURE_ONLY      1
#define SS2_PLACE_UNKNOWN           2

#define SS2_FACES_SINGLE_SIDED      3
#define SS2_FACES_DOUBLE_SIDED      1
*/

struct Triangle {
    uint32_t a;
    uint32_t b;
    uint32_t c;
};

struct StaticShapeMesh {
	uint32_t                    support_type;   ///< Was unk1
	FlexiTextureInfoStruct*     ftx_ref;
	void*                       txs_ref;        ///< Was TextureData
	uint32_t                    transparency;   ///< was PlaceTetxuring
	uint32_t                    texture_flags;  ///< Texture flags
                                /**<
                                 * Values:
                                 *   - 0: None
                                 *   - 12: Scrolling Sign (SIOpaqueUnlit)
                                 *   - 20: Running Water (SIAlpha), Wheels (SIAlphaMask...), Chain (SIOpaque)
                                 *   - 36: Wheels (SIAlphaMask...)
                                 *   - 68: Wheels (SIAlphaMask...)
                                 *   - 12288: Terrain (useterraintexture/SIOpaque)
                                 *   - 20480: Cliff (useclifftexture/SIOpaque)
                                 *   - 32788: Billboard (UseAdTexture/SIOpaque)
                                 *   - 32768: Animated Billboard (UseAdTexture/SIOpaque)
                                 */
	uint32_t                    sides;          ///< Doublesided (1) or singlesided (3)
	uint32_t                    vertex_count;
	uint32_t                    index_count;
	VERTEX*                     vertexes;
	uint32_t*                   indices;
};

struct StaticShape {
	VECTOR                  bounding_box_min;
	VECTOR                  bounding_box_max;
	uint32_t                total_vertex_count;
	uint32_t                total_index_count;
	uint32_t                mesh_count2;
	uint32_t                mesh_count;
	StaticShapeMesh**       sh;
	uint32_t                effect_count;
	MATRIX*                 effect_positions;
	char**                  effect_names;
};

};

#endif
