/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#include <vector>

class GUISkin {
public:
	char *name;
	unsigned long top;
	unsigned long left;
	unsigned long bottom;
	unsigned long right;
	char *texture;
    GUISkin(char *Name,char *Texture,unsigned long Top, unsigned long Left, unsigned long Bottom, unsigned long Right);
    ~GUISkin();
    static std::vector<GUISkin *> GUISkins;
    static int LoaderNumber;
    static unsigned long ComputeSize();
    static void AddData(unsigned long *data);
    static void WriteFileHeader(FILE *f);
};

