/*	RCT3 libOVL
	Copyright 2006 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#include "Common.h"
#include "CommonOVL.h"
#include "StyleOVL.h"
#include "icon.h"
std::vector<Texture *> Texture::IconTextures;
int Texture::LoaderNumber;
int Texture::LoaderNumber2;

unsigned long Texture::ComputeSize()
{
 	unsigned long icontexturesize = 0;
   	for (unsigned int i = 0;i < IconTextures.size();i++)
	{
		AddStringSize(IconTextures[i]->name.c_str());
		icontexturesize += sizeof(TextureStruct2);
		icontexturesize += sizeof(TextureStruct);
		loadercount++;
		symbolcount++;
		symbolrefcount++;
	}
	return icontexturesize;
}
unsigned long Texture::ComputeSizeFlic()
{
 	unsigned long flicsize = 0;
   	for (unsigned int i = 0;i < IconTextures.size();i++)
	{
		loadercount++;
		flicsize += sizeof(FlicStruct);
		flicsize += 4;
	}
	return flicsize;
}

Texture::Texture(const std::string& _Name,const std::string& _Texture,FlicHeader Fh,FlicMipHeader Fmh,unsigned char *Data):
	name(_Name),
	texture(_Texture)
{
    fh = Fh;
    fmh = Fmh;
    data = new unsigned char[fmh.Pitch * fmh.Blocks];
    memcpy(data,Data,fmh.Pitch * fmh.Blocks);
}

Texture::~Texture()
{
	delete[] data;
}

void Texture::AddData(unsigned long *data,unsigned long *data2)
{
    Tex *datax = (Tex *)data;
    Flic *data2f = (Flic *)data2;
	for (unsigned int i = 0;i < IconTextures.size();i++)
	{
	    AddLoader2((unsigned long *)&data2f[i].fl2,LoaderNumber2,0,true);
	    AddLoader((unsigned long *)&datax[i],LoaderNumber,1,IconTextures[i]->name.c_str(),":tex",false);
   		AddRef((unsigned long *)&datax[i].t1.TextureData,GUIIcon);
		relocations.push((unsigned long *)&data2f[i].fl);
		data2f[i].fl = &data2f[i].fl2;
		data2f[i].fl2.FlicDataPtr = 0;
		data2f[i].fl2.unk1 = 1;
		data2f[i].fl2.unk2 = 1.0;
		datax[i].t1.unk1 = 0x70007;
		datax[i].t1.unk2 = 0x70007;
		datax[i].t1.unk3 = 0x70007;
		datax[i].t1.unk4 = 0x70007;
		datax[i].t1.unk5 = 0x70007;
		datax[i].t1.unk6 = 0x70007;
		datax[i].t1.unk7 = 0x70007;
		datax[i].t1.unk8 = 0x70007;
		datax[i].t1.unk9 = 1;
		datax[i].t1.unk10 = 8;
		datax[i].t1.unk11 = 0x10;
		datax[i].t1.TextureData = 0;
		datax[i].t1.unk12 = 1;
		datax[i].t1.ts2 = &datax[i].t2;
		datax[i].t1.Flic = &data2f[i].fl;
		datax[i].t2.Texture = &datax[i].t1;
		datax[i].t2.Flic = &data2f[i].fl2;
		relocations.push((unsigned long *)&datax[i].t1.ts2);
		relocations.push((unsigned long *)&datax[i].t1.Flic);
		relocations.push((unsigned long *)&datax[i].t2.Texture);
		relocations.push((unsigned long *)&datax[i].t2.Flic);
	}
}

void Texture::WriteExtraData(FILE *f)
{
	for (unsigned int i = 0;i < IconTextures.size();i++)
	{
        unsigned long imgsize = IconTextures[i]->fmh.Blocks * IconTextures[i]->fmh.Pitch;
        unsigned long size = sizeof(FlicHeader) + sizeof(FlicMipHeader) + imgsize + sizeof(FlicMipHeader);
        fwrite(&size,4,1,f);
        fwrite(&IconTextures[i]->fh,sizeof(FlicHeader),1,f);
        fwrite(&IconTextures[i]->fmh,sizeof(FlicMipHeader),1,f);
        fwrite(IconTextures[i]->data,imgsize,1,f);
        IconTextures[i]->fmh.Blocks = 0;
        IconTextures[i]->fmh.MHeight = 0;
        IconTextures[i]->fmh.MWidth = 0;
        IconTextures[i]->fmh.Pitch = 0;
        fwrite(&IconTextures[i]->fmh,sizeof(FlicMipHeader),1,f);
	}
}

void Texture::WriteFileHeader(FILE *f)
{
	unsigned short len1 = 4;
	char *str1 = "FGDK";
	unsigned short len2 = 7;
	char *str2 = "Texture";
	unsigned long type = 2;
	unsigned short len3 = 3;
	char *str3 = "tex";
	fwrite(&len1,sizeof(len1),1,f);
	fwrite(str1,len1,1,f);
	fwrite(&len2,sizeof(len2),1,f);
	fwrite(str2,len2,1,f);
	fwrite(&type,sizeof(type),1,f);
	fwrite(&len3,sizeof(len3),1,f);
	fwrite(str3,len3,1,f);
	len1 = 4;
	str1 = "FGDK";
	len2 = 4;
	str2 = "Flic";
	type = 2;
	len3 = 4;
	str3 = "flic";
	fwrite(&len1,sizeof(len1),1,f);
	fwrite(str1,len1,1,f);
	fwrite(&len2,sizeof(len2),1,f);
	fwrite(str2,len2,1,f);
	fwrite(&type,sizeof(type),1,f);
	fwrite(&len3,sizeof(len3),1,f);
	fwrite(str3,len3,1,f);
}

