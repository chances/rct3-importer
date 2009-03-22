/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "rct3basetypes.h"

namespace r3 {

struct VECTOR {
    float_t x;
    float_t y;
    float_t z;
};

struct QUATERNION {
	// Not sure, w might be first
    float_t x;
    float_t y;
    float_t z;
    float_t w;
};

struct MATRIX {
	union {
		struct {
			float_t _11, _12, _13, _14;
			float_t _21, _22, _23, _24;
			float_t _31, _32, _33, _34;
			float_t _41, _42, _43, _44;
		};
		float_t m[4][4];
	};
};

struct VERTEX {
	VECTOR position;
	VECTOR normal;
	uint32_t color;
	float_t tu, tv;
};

struct VERTEX2 {
	VECTOR position;
	VECTOR normal;
	int8_t bone[4];
	uint8_t boneweight[4];
	uint32_t color;
	float_t tu, tv;
};

struct COLOURQUAD {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
};

struct txyz {
    union {
        struct {
            float_t Time;
            float_t X;
            float_t Y;
            float_t Z;
        };
        struct {
            float_t time;
            union {
                struct {
                    float_t x;
                    float_t y;
                    float_t z;
                };
                VECTOR v;
            };
        };
    };
};


};

#endif // VERTEX_H_INCLUDED
