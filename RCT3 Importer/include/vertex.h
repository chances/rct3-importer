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

#define RCT3FLOAT float
#define COLOUR    unsigned long

struct VECTOR {
    RCT3FLOAT x;
    RCT3FLOAT y;
    RCT3FLOAT z;
};

struct MATRIX {
	union {
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};
};

struct VERTEX {
	VECTOR position;
	VECTOR normal;
	COLOUR color;
	RCT3FLOAT tu, tv;
};

struct VERTEX2 {
	VECTOR position;
	VECTOR normal;
	char bone[4];
	unsigned char boneweight[4];
	COLOUR color;
	RCT3FLOAT tu, tv;
};

struct COLOURQUAD {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char alpha;
};

#endif // VERTEX_H_INCLUDED
