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
#include "attract.h"

using namespace r3;

std::vector<AttractionData *> AttractionData::Attractions;
int AttractionData::LoaderNumber;

unsigned long AttractionData::ComputeSize()
{
 	unsigned long Attractionsize = 0;
    if (Attractions.size())
    {
        AddStringSize2(":gsi");
        AddStringSize2(":spl");
    }
   	for (unsigned int i = 0;i < Attractions.size();i++)
	{
		AddStringSize(Attractions[i]->name);
		Attractionsize += sizeof(SpecialAttractionA);
		loadercount++;
		symbolcount++;
		symbolrefcount++;
		symbolrefcount++;
		symbolrefcount++;
		symbolrefcount++;
		symbolrefcount++;
	}
	return Attractionsize;
}
AttractionData::AttractionData(char *Name,unsigned long AttractionType,char *NameString,char *DescriptionString,unsigned long Unk2,long Unk3,char *SID)
{
    name = new char[strlen(Name)+1];
    strcpy(name,Name);
    namestring = new char[strlen(NameString)+1];
    strcpy(namestring,NameString);
    descriptionstring = new char[strlen(DescriptionString)+1];
    strcpy(descriptionstring,DescriptionString);
    sid = new char[strlen(SID)+1];
    strcpy(sid,SID);
    attractiontype = AttractionType;
    unk2 = Unk2;
    unk3 = Unk3;
}

AttractionData::~AttractionData()
{
    delete name;
    delete namestring;
    delete sid;
}

void AttractionData::AddData(unsigned long *data)
{
    char *gsi = AddString(":gsi");
    char *spl = AddString(":spl");
    SpecialAttractionA *datas = (SpecialAttractionA *)data;
    unsigned int itempos = 0;
	for (unsigned int i = 0;i < Attractions.size();i++)
	{
	    AddLoader((unsigned long *)&datas[i],LoaderNumber,5,Attractions[i]->name,":sat",false);
   		AddRef((unsigned long *)&datas[i].Name,FindString(Attractions[i]->namestring,":txt"));
   		AddRef((unsigned long *)&datas[i].Description,FindString(Attractions[i]->descriptionstring,":txt"));
   		AddRef((unsigned long *)&datas[i].SID,FindString(Attractions[i]->sid,":sid"));
   		AddRef((unsigned long *)&datas[i].GSI,gsi);
   		AddRef((unsigned long *)&datas[i].spline,spl);
   		datas[i].Name = 0;
   		datas[i].Description = 0;
   		datas[i].SID = 0;
   		datas[i].GSI = 0;
   		datas[i].spline = 0;
   		switch (Attractions[i]->attractiontype)
   		{
        case 0:
            datas[i].AttractionType = 11;
            break;
        case 1:
            datas[i].AttractionType = 12;
            break;
        case 2:
            datas[i].AttractionType = 13;
            break;
   		}
   		datas[i].unk2 = Attractions[i]->unk2;
   		datas[i].unk3 = Attractions[i]->unk3;
   		datas[i].unk4 = 0;
   		datas[i].unk5 = 0;
   		datas[i].unk6 = 4960;
   		datas[i].unk7 = 0;
   		datas[i].unk8 = 0;
   		datas[i].unk9 = 0;
   		datas[i].unk10 = 12;
	}
}

void AttractionData::WriteFileHeader(FILE *f)
{
	unsigned short len1 = 4;
	char *str1 = "FGDK";
	unsigned short len2 = 17;
	char *str2 = "SpecialAttraction";
	unsigned long type = 1;
	unsigned short len3 = 3;
	char *str3 = "sat";
	fwrite(&len1,sizeof(len1),1,f);
	fwrite(str1,len1,1,f);
	fwrite(&len2,sizeof(len2),1,f);
	fwrite(str2,len2,1,f);
	fwrite(&type,sizeof(type),1,f);
	fwrite(&len3,sizeof(len3),1,f);
	fwrite(str3,len3,1,f);
}
