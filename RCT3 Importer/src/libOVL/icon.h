/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/
class Texture {
public:
    char *name;
    char *texture;
	FlicHeader fh;
	FlicMipHeader fmh;
	unsigned char *data;
    Texture(char *Name,char *Texture,FlicHeader Fh,FlicMipHeader Fmh,unsigned char *Data);
    ~Texture();
    static vector<Texture *> IconTextures;
    static int LoaderNumber;
    static int LoaderNumber2;
    static unsigned long ComputeSize();
    static void AddData(unsigned long *data,unsigned long *data2);
    static void WriteExtraData(FILE *f);
    static void WriteFileHeader(FILE *f);
    static unsigned long ComputeSizeFlic();
};

