/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __BONEANIM_H__
#define __BONEANIM_H__

#include "vertex.h"

namespace r3 {

struct BoneAnimBone {
	char *Name;
	unsigned long TranslateCount;
	txyz *Translate;
	unsigned long RotateCount;
	txyz *Rotate;
};
struct BoneAnim {
	unsigned long BoneCount;
	BoneAnimBone *Bones;
	float TotalTime;
};

};

#endif
