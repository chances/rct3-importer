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
#include "vertex.h"

namespace r3 {

struct TerrainType {
	uint32_t 		unk01;				///< maybe structure version. Only saw 1, but Jonwils original didn't have unk15
	uint32_t		unk02;				///< Always 0
	uint32_t		addon;				///< 0 for vanilla textures, 1 for soaked
	uint32_t		number;				///< Terrains are numbered, this is it
	uint32_t 		type;				///< Terrain type
										/**<
										 * 0 = Ground, Unblended
										 * 1 = Cliff
										 * 2 = Ground, Blended
										 **/
	TextureStruct*	texture_ref;
	char*			description_name;	///< Name of a txt structure in the localization ovls
	char*			icon_name;			///< Name of a gsi structure for the icon in Main ovl (40x40)
	union {
		COLOURQUAD 		colour_simple;
		uint32_t		colour_simple_int;
	};
	union {
		COLOURQUAD 		colour_map;		///< Seems to be clamped to certain predefined colours
		uint32_t		colour_map_int;
	};
	float_t			inv_width;			///< Inverse width of the texture in meters (ie 0.25 makes it one square wide)
	float_t			inv_height;			///< Inverse height of the texture
	float_t			unk13;
	float_t			unk14;
	float_t			unk15;
};
	
};

#endif
