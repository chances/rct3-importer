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
#include "stalls.h"

using namespace r3;

std::vector<StallData *> StallData::Stalls;
int StallData::LoaderNumber;

unsigned long StallData::ComputeSize()
{
 	unsigned long Stallsize = 0;
    if (Stalls.size())
    {
        AddStringSize2(":gsi");
        AddStringSize2(":spl");
    }
   	for (unsigned int i = 0;i < Stalls.size();i++)
	{
		AddStringSize(Stalls[i]->name);
		Stallsize += sizeof(StallA);
		loadercount++;
		symbolcount++;
		symbolrefcount++;
		symbolrefcount++;
		symbolrefcount++;
		symbolrefcount++;
		symbolrefcount++;
	}
	return Stallsize;
}
unsigned long StallData::ComputeItemSize()
{
 	unsigned long Stallsize = 0;
   	for (unsigned int i = 0;i < Stalls.size();i++)
	{
	    if (Stalls[i]->itemcount)
	    {
            for (unsigned int j = 0;j < Stalls[i]->itemcount;j++)
            {
                AddStringSize(reinterpret_cast<char*>(Stalls[i]->items[j].cid_ref));
                symbolrefcount++;
                Stallsize += sizeof(StallItem);
            }
	    }
	}
	return Stallsize;
}
StallData::StallData(char *Name,unsigned long StallType,char *NameString,char *DescriptionString,unsigned long Unk2,long Unk3,char *SID,unsigned long ItemCount,StallItem *Items,unsigned long Unk11,unsigned long Unk12,unsigned long Unk13,unsigned long Unk14,unsigned long Unk15,unsigned long Unk16)
{
    name = new char[strlen(Name)+1];
    strcpy(name,Name);
    namestring = new char[strlen(NameString)+1];
    strcpy(namestring,NameString);
    descriptionstring = new char[strlen(DescriptionString)+1];
    strcpy(descriptionstring,DescriptionString);
    sid = new char[strlen(SID)+1];
    strcpy(sid,SID);
    stalltype = StallType;
    unk2 = Unk2;
    unk3 = Unk3;
    itemcount = ItemCount;
    if (itemcount)
    {
        items = new StallItem[ItemCount];
        memcpy(items,Items,sizeof(StallItem));
    }
    else
    {
        items = 0;
    }
    unk11 = Unk11;
    unk12 = Unk12;
    unk13 = Unk13;
    unk14 = Unk14;
    unk15 = Unk15;
    unk16 = Unk16;
}

StallData::~StallData()
{
    delete name;
    delete namestring;
    delete sid;
}

void StallData::AddData(unsigned long *data,unsigned long *itemdata)
{
    char *gsi = AddString(":gsi");
    char *spl = AddString(":spl");
    StallA *datas = (StallA *)data;
    StallItem *items = (StallItem *)itemdata;
    unsigned int itempos = 0;
	for (unsigned int i = 0;i < Stalls.size();i++)
	{
	    AddLoader((unsigned long *)&datas[i],LoaderNumber,5+Stalls[i]->itemcount,Stalls[i]->name,":sta",false);
   		AddRef((unsigned long *)&datas[i].Name,FindString(Stalls[i]->namestring,":txt"));
   		AddRef((unsigned long *)&datas[i].Description,FindString(Stalls[i]->descriptionstring,":txt"));
   		AddRef((unsigned long *)&datas[i].SID,FindString(Stalls[i]->sid,":sid"));
   		AddRef((unsigned long *)&datas[i].GSI,gsi);
   		AddRef((unsigned long *)&datas[i].spline,spl);
   		datas[i].Name = 0;
   		datas[i].Description = 0;
   		datas[i].SID = 0;
   		datas[i].GSI = 0;
   		datas[i].spline = 0;
   		switch (Stalls[i]->stalltype)
   		{
        case 0:
            datas[i].type = 7;
            break;
        case 1:
            datas[i].type = 8;
            break;
        case 2:
            datas[i].type = 9;
            break;
        case 3:
            datas[i].type = 14;
            break;
   		}
   		datas[i].unk2 = Stalls[i]->unk2;
   		datas[i].unk3 = Stalls[i]->unk3;
   		datas[i].unk4 = 0;
   		datas[i].unk5 = 0;
   		datas[i].unk6 = 1500;
   		datas[i].unk7 = 0;
   		datas[i].unk8 = 0;
   		datas[i].unk9 = 0;
   		datas[i].unk10 = 12;
   		datas[i].unk11 = Stalls[i]->unk11;
   		datas[i].unk12 = Stalls[i]->unk12;
   		datas[i].unk13 = Stalls[i]->unk13;
   		datas[i].unk14 = Stalls[i]->unk14;
   		datas[i].unk15 = Stalls[i]->unk15;
   		datas[i].unk16 = Stalls[i]->unk16;
   		datas[i].itemcount = Stalls[i]->itemcount;
   		if (items)
   		{
   		    datas[i].Items = &items[itempos];
   		    relocations.push((unsigned long *)&datas[i].Items);
            for (unsigned int j = 0;j < Stalls[i]->itemcount;j++)
            {
                AddRef2((unsigned long *)&items[itempos].cid_ref,reinterpret_cast<char*>(Stalls[i]->items[j].cid_ref),":cid");
                items[itempos].cid_ref = 0;
                items[itempos].cost = Stalls[i]->items[j].cost;
                itempos++;
            }
   		}
   		else
   		{
   		    datas[i].Items = 0;
   		}
	}
}

void StallData::WriteFileHeader(FILE *f)
{
	unsigned short len1 = 4;
	char *str1 = "FGDK";
	unsigned short len2 = 5;
	char *str2 = "Stall";
	unsigned long type = 1;
	unsigned short len3 = 3;
	char *str3 = "sta";
	fwrite(&len1,sizeof(len1),1,f);
	fwrite(str1,len1,1,f);
	fwrite(&len2,sizeof(len2),1,f);
	fwrite(str2,len2,1,f);
	fwrite(&type,sizeof(type),1,f);
	fwrite(&len3,sizeof(len3),1,f);
	fwrite(str3,len3,1,f);
}
