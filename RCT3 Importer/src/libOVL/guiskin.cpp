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
#include "guiskin.h"
std::vector<GUISkin *> GUISkin::GUISkins;
int GUISkin::LoaderNumber;

unsigned long GUISkin::ComputeSize()
{
 	unsigned long guiskinsize = 0;
   	for (unsigned int i = 0;i < GUISkins.size();i++)
	{
		AddStringSize(GUISkins[i]->name);
		guiskinsize += sizeof(GUISkinItemPos);
		guiskinsize += sizeof(GUISkinItem);
		loadercount++;
		symbolcount++;
		symbolrefcount++;
	}
	return guiskinsize;
}
GUISkin::GUISkin(char *Name,char *Texture,unsigned long Top, unsigned long Left, unsigned long Bottom, unsigned long Right)
{
    name = new char[strlen(Name)+1];
    strcpy(name,Name);
    texture = new char[strlen(Texture)+1];
    strcpy(texture,Texture);
    top = Top;
    left = Left;
    right = Right;
    bottom = Bottom;
}

GUISkin::~GUISkin()
{
    delete name;
    delete texture;
}

void GUISkin::AddData(unsigned long *data)
{
	GSI *gsistr = (GSI *)data;
	for (unsigned int i = 0;i < GUISkins.size();i++)
	{
	    AddLoader((unsigned long *)&gsistr[i],LoaderNumber,1,GUISkins[i]->name,":gsi",false);
		gsistr[i].g1.pos = &gsistr[i].g2;
		gsistr[i].g1.tex = 0;
		gsistr[i].g1.unk1 = 0;
		gsistr[i].g1.unk2 = 0;
		gsistr[i].g2.top = GUISkins[i]->top;
		gsistr[i].g2.left = GUISkins[i]->left;
		gsistr[i].g2.bottom = GUISkins[i]->bottom;
		gsistr[i].g2.right = GUISkins[i]->right;
		char *Symbol = FindString(GUISkins[i]->texture,":tex");
		AddRef((unsigned long *)&gsistr[i].g1.tex,Symbol);
		relocations.push((unsigned long *)&gsistr[i].g1.pos);
	}
}

void GUISkin::WriteFileHeader(FILE *f)
{
	unsigned short len1 = 4;
	char *str1 = "FGDK";
	unsigned short len2 = 0xB;
	char *str2 = "GUISkinItem";
	unsigned long type = 1;
	unsigned short len3 = 3;
	char *str3 = "gsi";
	fwrite(&len1,sizeof(len1),1,f);
	fwrite(str1,len1,1,f);
	fwrite(&len2,sizeof(len2),1,f);
	fwrite(str2,len2,1,f);
	fwrite(&type,sizeof(type),1,f);
	fwrite(&len3,sizeof(len3),1,f);
	fwrite(str3,len3,1,f);
}
