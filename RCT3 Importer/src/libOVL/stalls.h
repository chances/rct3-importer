/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/
class StallData {
public:
    char *name;
    unsigned long stalltype;
    char *namestring;
    char *descriptionstring;
    unsigned long unk2;
    long unk3;
    char *sid;
    unsigned long itemcount;
    StallItem *items;
    unsigned long unk11;
    unsigned long unk12;
    unsigned long unk13;
    unsigned long unk14;
    unsigned long unk15;
    unsigned long unk16;
    StallData(char *Name,unsigned long StallType,char *NameString,char *DescriptionString,unsigned long Unk2,long Unk3,char *SID,unsigned long ItemCount,StallItem *Items,unsigned long Unk11,unsigned long Unk12,unsigned long Unk13,unsigned long Unk14,unsigned long Unk15,unsigned long Unk16);
    ~StallData();
    static vector<StallData *> Stalls;
    static int LoaderNumber;
    static unsigned long ComputeSize();
    static unsigned long ComputeItemSize();
    static void AddData(unsigned long *data,unsigned long *itemdata);
    static void WriteFileHeader(FILE *f);
};
