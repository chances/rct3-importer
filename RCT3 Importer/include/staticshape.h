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
#include <string>

using namespace std;

/*
#define SS2_PLACE_TEXTURE_AND_MESH  0
#define SS2_PLACE_TEXTURE_ONLY      1
#define SS2_PLACE_UNKNOWN           2

#define SS2_FACES_SINGLE_SIDED      3
#define SS2_FACES_DOUBLE_SIDED      1
*/

struct SharedShape {
    struct Supports {
        enum {
            Below = 1,
            Top = 2,
            Mid = 4,
            GroundAttach = 8,
            None = 0xFFFFFFFF
        };
    };
    struct Transparency {
        enum {
            None = 0,
            Simple = 1,
            Complex = 2
        };
    };
    struct Flags {
        enum {
            None = 0,
            ScrollingSign = 12,
            WaterWheelChain = 20,
            Wheels1 = 36,
            Wheels2 = 68,
            Terrain = 12288,
            Cliff = 20480,
            AnimatedBillboard = 32768,
            Billboard = 32788
        };
    };
    struct Sides {
        enum {
            Doublesided = 1,
            Singlesided = 3
        };
    };
};

struct Triangle {
    unsigned long a;
    unsigned long b;
    unsigned long c;
};

struct StaticShape2 {
	unsigned long unk1; ///< always 0xFFFFFFFFF
	FlexiTextureInfoStruct *fts; ///< is 0 in disk files
	unsigned long *TextureData; ///< is 0 in disk files
	unsigned long PlaceTexturing; ///< Transparency
                                  /**
                                   * 0: none
                                   * 1: simple (SIAlphaMask...)
                                   * 2: complex (SIAlpha..., SIGlass & SIWater)
                                   */
	unsigned long textureflags; ///< Texture flags
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
	unsigned long sides; ///< Doublesided (1) or singlesided (3)
	unsigned long VertexCount;
	unsigned long IndexCount;
	VERTEX *Vertexes;
	unsigned long *Triangles;
};
struct StaticShape1 {
	VECTOR BoundingBox1;
	VECTOR BoundingBox2;
	unsigned long TotalVertexCount;
	unsigned long TotalIndexCount;
	unsigned long MeshCount2;
	unsigned long MeshCount;
	StaticShape2 **sh;
	unsigned long EffectCount;
	MATRIX *EffectPosition;
	char **EffectName;
};

#endif
