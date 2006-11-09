/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/
class AttractionData {
public:
    char *name;
    unsigned long attractiontype;
    char *namestring;
    char *descriptionstring;
    unsigned long unk2;
    long unk3;
    char *sid;
    AttractionData(char *Name,unsigned long AttractionType,char *NameString,char *DescriptionString,unsigned long Unk2,long Unk3,char *SID);
    ~AttractionData();
    static vector<AttractionData *> Attractions;
    static int LoaderNumber;
    static unsigned long ComputeSize();
    static void AddData(unsigned long *data);
    static void WriteFileHeader(FILE *f);
};
