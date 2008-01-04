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

#include <windows.h>
#include <d3d9types.h>

struct VERTEX {
	D3DVECTOR position;
	D3DVECTOR normal;
	D3DCOLOR color;
	FLOAT tu, tv;
};
struct VERTEX2 {
	D3DVECTOR position;
	D3DVECTOR normal;
	char bone[4];
	unsigned char boneweight[4];
	D3DCOLOR color;
	FLOAT tu, tv;
};

#endif // VERTEX_H_INCLUDED
