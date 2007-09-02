/*	RCT3 libOVL
	Copyright 2006 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

//#define WRITE_SCENERYITEMEXTRA

#include "Common.h"
#include "CommonOVL.h"
#include "StyleOVL.h"
#include "textstring.h"
#include "icon.h"
#include "guiskin.h"
#include "stalls.h"
#include "attract.h"
extern FILE *f;
extern unsigned long CurrentFile;
extern OvlFile OpenFiles[2];
extern std::queue<unsigned long *> relocations;
extern std::queue<unsigned long> fixups;
extern std::vector <libOVL_Resource *> ResourceItems;
std::vector <Scenery *> SceneryItems;
/*
const char *stdstrings =  "StyleIndex:int\0GUIIcon:txs\0"
                    "GuestInjectionPoint\0"
                    "LitterBin\0 Capacity 4.0\0"
                    "ParkBench\0"
                    "ParkEntrance\0"
                    "QueueLineEntertainer\0"
                    "RideEvent\0"
                    "Sign\0 EnableNoEntry\0"
                    "VistaPoint\0"
                    "Waterable\0 Capacity 300.0\0"
                    "adventure\0crazygolf\0crazygolfholes\0girder\0ornate\0pathasphalt\0pathcrazy\0pathdirt\0pathleafy\0marble\0pathtarmac\0flatqueueset1\0romanpillar\0scaffolding\0space\0spooky\0steel\0ts1\0ts2\0ts3\0ts4\0ts5\0flatwildwest\0"
                    "\0"
                    "MapColourBlue:flt\0MapColourRed:flt\0MapColorGreen:flt\0"
                    "PathFountain\0"
                    "LoudSpeaker\0"
                    "PassportPost\0"
                    "WaterCannon\0"
                    "aquarium\0aquariumcorner\0defaultpool\0defaultpoolpath\0defaultpoolsmall\0sand\0pathunderwater\0ts1a\0ts6\0ts7\0ts8";
*/
#define STDSTRINGS_SIZE 81
const char *stdstrings =
                    "StyleIndex:int\0" // 15
                    "GUIIcon:txs\0" // 12
                    "\0" // 1
                    "MapColourBlue:flt\0" // 18
                    "MapColourRed:flt\0" // 17
                    "MapColorGreen:flt\0"; // 18 /// 81
/*
                    "GuestInjectionPoint\0"
                    "LitterBin\0 Capacity 4.0\0"
                    "ParkBench\0"
                    "ParkEntrance\0"
                    "QueueLineEntertainer\0"
                    "RideEvent\0"
                    "Sign\0 EnableNoEntry\0"
                    "VistaPoint\0"
                    "Waterable\0 Capacity 300.0\0"
                    "adventure\0crazygolf\0crazygolfholes\0girder\0ornate\0pathasphalt\0pathcrazy\0pathdirt\0pathleafy\0marble\0pathtarmac\0flatqueueset1\0romanpillar\0scaffolding\0space\0spooky\0steel\0ts1\0ts2\0ts3\0ts4\0ts5\0flatwildwest\0"
                    "PathFountain\0"
                    "LoudSpeaker\0"
                    "PassportPost\0"
                    "WaterCannon\0"
                    "aquarium\0aquariumcorner\0defaultpool\0defaultpoolpath\0defaultpoolsmall\0sand\0pathunderwater\0ts1a\0ts6\0ts7\0ts8";
*/

typedef struct
{
    const char* type;
    const char* parameter;
} extra_struct;

typedef struct
{
    bool used;
    char* type;         // These will point to the correct position in the string list
    char* parameter;
} extra_use_struct;

#define EXTRA_COUNT 25
const extra_struct def_extras[EXTRA_COUNT] = {
    {NULL, NULL},
    {"GuestInjectionPoint", NULL},
    {"LitterBin", " Capacity 4.0"},
    {"ParkBench", NULL},
    {"ParkEntrance", NULL},
    {"QueueLineEntertainer", NULL},
    {"RideEvent", NULL},
    {"Sign", " EnableNoEntry"},
    {"VistaPoint", NULL},
    {"Waterable", " Capacity 300.0"},
    {"PathFountain", NULL},
    {"LoudSpeaker", NULL},
    {"PassportPost", NULL},
    {"WaterCannon", NULL},
    {"PoolExtra", " Side CentralWater OppositeWater DivingBoard"}, // Divisg Board
    {"PoolExtra", " Side UnderWaterWall Ladder"}, // Ladder
    {"PoolExtra", " Light UnderWaterWall"}, // Light
    {"PoolExtra", " Light CentralSolid"}, // Pool Lamp
    {"PoolExtra", " CentralSolid WholeTile Shower"}, // Shower
    {"PoolExtra", " Side HeightStep 1"}, // Small Ramp
    {"PoolExtra", " Side CentralSolid WholeTile HeightStep 2"}, // Big Ramp
    {"PoolExtra", " CentralSolid WholeTile SunLounger"}, // Sun Loungers
    {"PoolExtra", " LifeGuard CentralSolid"}, // Lifeguard
    {"PoolExtra", " WaveMachine UnderWaterWall"}, // Wave Machine
    {"LifeguardChair", NULL} // Lifeguard
};

typedef struct
{
    bool used;
    char* supports;
} supports_use_struct;

#define SUPPORTS_COUNT 35
const char* def_supports[SUPPORTS_COUNT] = {
    NULL,
    "adventure",
    "crazygolf",
    "crazygolfholes",
    "girder",
    "ornate",
    "pathasphalt",
    "pathcrazy",
    "pathdirt",
    "pathleafy",
    "marble",
    "pathtarmac",
    "flatqueueset1",
    "romanpillar",
    "scaffolding",
    "space",
    "spooky",
    "steel",
    "ts1",
    "ts2",
    "ts3",
    "ts4",
    "ts5",
    "flatwildwest",
    "aquarium",
    "aquariumcorner",
    "defaultpool",
    "defaultpoolpath",
    "defaultpoolsmall",
    "sand",
    "pathunderwater",
    "ts1a",
    "ts6",
    "ts7",
    "ts8"
};


void clearStyleOVLData();
unsigned long* styleval;
int compare(const void *s1,const void *s2);


////////////////////////////////////////////////////////////////////////////////
//
//  CreateStyleOvl
//  Used
//
////////////////////////////////////////////////////////////////////////////////

//Filename - style.common.ovl
LIBOVL_API unsigned long CreateStyleOvl(char *filename, unsigned long ThemeCode)
{
	char *UniqueOVLFilename = new char[strlen(filename)+1];
	strcpy(UniqueOVLFilename,filename);
	strcpy(strstr(UniqueOVLFilename,".common.ovl"),".unique.ovl");

	OpenFiles[OVL_UNIQUE].FileName = UniqueOVLFilename;
	OpenFiles[OVL_COMMON].FileName= new char[strlen(filename)+1];
	strcpy(OpenFiles[OVL_COMMON].FileName , filename);


	OpenFiles[OVL_COMMON].ReferenceCount=0;
	OpenFiles[OVL_COMMON].References=NULL;
	OpenFiles[OVL_UNIQUE].ReferenceCount=0;
	OpenFiles[OVL_UNIQUE].References=NULL;
	memset(OpenFiles[OVL_COMMON].Types,0,sizeof(OpenFiles[OVL_COMMON].Types));
	memset(OpenFiles[OVL_UNIQUE].Types,0,sizeof(OpenFiles[OVL_UNIQUE].Types));

	clearStyleOVLData();
	styleval = (unsigned long *) ThemeCode;

	return 0;

}


////////////////////////////////////////////////////////////////////////////////
//
//  saveStyleOvl
//  Used
//
////////////////////////////////////////////////////////////////////////////////

unsigned long loadercount;
unsigned long symbolcount;
unsigned long symbolrefcount;
unsigned long stringtablesize;
unsigned long LoaderPos;
unsigned long SymbolPos;
unsigned long SymbolRefPos;
unsigned char *stringtable;
char *strings;
char *GUIIcon;
//char *extra[14];
//char *extraparams[14];
//char *supports[35];
LoaderStruct *loaders;
SymbolStruct *symbols;
SymbolRefStruct *symrefs;
void AddStringSize(const char *string)
{
    stringtablesize += strlen(string)+5;
}
void AddStringSize2(const char *string)
{
    stringtablesize += strlen(string)+1;
}
char *AddSymbol(const char *SymbolName, const char *Type)
{
    char *strx = strings;
   	strcpy(strings,SymbolName);
	strings += strlen(SymbolName);
	strcpy(strings,Type);
	strings += strlen(Type);
	strings++;
	return strx;
}
char *AddString(const char *String)
{
    char *strx = strings;
   	strcpy(strings,String);
	strings += strlen(String);
	strings++;
	return strx;
}
char *FindString(const char *name, const char *type)
{
    int x;
    if (name)
    {
        x = strlen(name);
    }
    else
    {
        x = 0;
    }
	for (char *s = (char *)stringtable;s < strings;s++)
	{
		if (memcmp(name,s,x) == 0)
		{
			if (memcmp(s+x,type,strlen(type)) == 0)
			{
				return s;
				break;
			}
		}
	}
	return 0;
}
void AddLoader(unsigned long *data,int loadertype,int SymbolsToResolve, const char *SymbolName, const char *Type,bool HasExtraData)
{
    loaders[LoaderPos].data = data;
    loaders[LoaderPos].LoaderType = loadertype;
    loaders[LoaderPos].Sym = &symbols[SymbolPos];
    loaders[LoaderPos].SymbolsToResolve = SymbolsToResolve;
    loaders[LoaderPos].HasExtraData = HasExtraData;
    symbols[SymbolPos].data = data;
    symbols[SymbolPos].Symbol = AddSymbol(SymbolName,Type);
    symbols[SymbolPos].IsPointer = true;
	relocations.push((unsigned long *)&loaders[LoaderPos].data);
	relocations.push((unsigned long *)&loaders[LoaderPos].Sym);
    SymbolPos++;
    LoaderPos++;
}
void AddLoader2(unsigned long *data,int type,int SymbolsToResolve,bool HasExtraData)
{
    loaders[LoaderPos].data = data;
    loaders[LoaderPos].LoaderType = type;
    loaders[LoaderPos].Sym = 0;
    loaders[LoaderPos].SymbolsToResolve = SymbolsToResolve;
    loaders[LoaderPos].HasExtraData = HasExtraData;
	relocations.push((unsigned long *)&loaders[LoaderPos].data);
	relocations.push((unsigned long *)&loaders[LoaderPos].Sym);
    LoaderPos++;
}
void AddRef(unsigned long *reference, char *SymbolName)
{
    symrefs[SymbolRefPos].ldr = &loaders[LoaderPos-1];
    symrefs[SymbolRefPos].Symbol = SymbolName;
    symrefs[SymbolRefPos].reference = reference;
	relocations.push((unsigned long *)&symrefs[SymbolRefPos].reference);
	relocations.push((unsigned long *)&symrefs[SymbolRefPos].ldr);
	relocations.push((unsigned long *)&symrefs[SymbolRefPos].Symbol);
	SymbolRefPos++;
}
void AddRef2(unsigned long *reference, const char *SymbolName, const char *Type)
{
    AddRef(reference,AddSymbol(SymbolName,Type));
}
LIBOVL_API bool saveStyleOVL()
{
    unsigned long loadernum = 0;
    loadercount = 0;
    symbolcount = 0;
    symbolrefcount = 0;
//	stringtablesize = 590;
	stringtablesize = STDSTRINGS_SIZE;
    LoaderPos = 0;
    SymbolPos = 0;
    SymbolRefPos = 0;

    extra_use_struct use_extra[EXTRA_COUNT];
    memset(&use_extra, 0, EXTRA_COUNT*sizeof(extra_use_struct));
    supports_use_struct use_supports[SUPPORTS_COUNT];
    memset(&use_supports, 0, SUPPORTS_COUNT*sizeof(supports_use_struct));

	unsigned long textstringsize = TextString::ComputeSize();
	unsigned long flicsize = Texture::ComputeSizeFlic();
	unsigned long texturesize = Texture::ComputeSize();
	unsigned long gsisize = GUISkin::ComputeSize();
	unsigned long sidsize = 0;
	unsigned char *textstrings;
	unsigned char *flics;
	unsigned char *textures;
	unsigned char *gsis;
	unsigned char *sids;
	unsigned char *stalls;
	unsigned char *stallitems;
    unsigned char *attractions;
	symbolcount++;
	symbolcount++;
	symbolcount++;
	symbolcount++;
	unsigned long i;
	char *Symbol;
	for (unsigned long i = 0;i < SceneryItems.size();i++)
	{
		char fname[_MAX_FNAME];
		_splitpath(SceneryItems[i]->ovl3,NULL,NULL,fname,NULL);
		strchr(fname,'.')[0] = 0;
		AddStringSize(SceneryItems[i]->InternalOVLName);
		AddStringSize(SceneryItems[i]->InternalOVLName);
		AddStringSize2(SceneryItems[i]->ovlWithoutExt);
		loadercount++;
		symbolcount++;
		symbolrefcount++;
		symbolrefcount++;
		symbolrefcount++;
		if (SceneryItems[i]->wallicon != 0)
		{
			symbolrefcount++;
			symbolrefcount++;
		}
		sidsize += sizeof(SceneryItem);
#ifdef WRITE_SCENERYITEMEXTRA
		sidsize += sizeof(SceneryItemExtra);
#endif
		sidsize += sizeof(SceneryItemData);
		sidsize += 8;
		if (SceneryItems[i]->extra != 0)
		{
			sidsize += sizeof(SceneryParams);
			if (!use_extra[SceneryItems[i]->extra].used) {
                use_extra[SceneryItems[i]->extra].used = true;
                AddStringSize2(def_extras[SceneryItems[i]->extra].type);
                if (def_extras[SceneryItems[i]->extra].parameter) {
                    AddStringSize2(def_extras[SceneryItems[i]->extra].parameter);
                }
			}
		}
		if (SceneryItems[i]->supportstype != 0)
		{
			if (!use_supports[SceneryItems[i]->supportstype].used) {
                use_supports[SceneryItems[i]->supportstype].used = true;
                AddStringSize2(def_supports[SceneryItems[i]->supportstype]);
			}
		}
	}
	unsigned long stallsize = StallData::ComputeSize();
	unsigned long stallitemsize = StallData::ComputeItemSize();
	unsigned long attractionsize = AttractionData::ComputeSize();
	stringtable = new unsigned char[stringtablesize];
	memset(stringtable, 0, stringtablesize);
	symbols = new SymbolStruct[symbolcount];
	loaders = new LoaderStruct[loadercount];
	symrefs = new SymbolRefStruct[symbolrefcount];
	textstrings = new unsigned char[textstringsize];
	flics = new unsigned char[flicsize];
	textures = new unsigned char[texturesize];
	gsis = new unsigned char[gsisize];
	sids = new unsigned char[sidsize];
	if (stallsize)
	{
        stalls = new unsigned char[stallsize];
	}
	else
	{
	    stalls = 0;
	}
	if (stallitemsize)
	{
        stallitems = new unsigned char[stallitemsize];
	}
	else
	{
	    stallitems = 0;
	}
	if (attractionsize)
	{
	    attractions = new unsigned char[attractionsize];
	}
	else
	{
	    attractions = 0;
	}
	unsigned char *sidsx = sids;

	File Type2Files[10];
	File Type0Files[1];
	Type0Files[0].data = (unsigned long *)stringtable;
	Type0Files[0].reloffset = 0;
	Type0Files[0].size = stringtablesize;

    int curfile = 0;
	Type2Files[curfile].data = (unsigned long *)symbols;
	Type2Files[curfile].reloffset = Type0Files[0].size;
	Type2Files[curfile].size = symbolcount * sizeof(SymbolStruct);
	curfile++;
	Type2Files[curfile].data = (unsigned long *)loaders;
	Type2Files[curfile].reloffset = Type2Files[curfile-1].reloffset + Type2Files[curfile-1].size;
	Type2Files[curfile].size = loadercount * sizeof(LoaderStruct);
	curfile++;
	Type2Files[curfile].data = (unsigned long *)symrefs;
	Type2Files[curfile].reloffset = Type2Files[curfile-1].reloffset + Type2Files[curfile-1].size;
	Type2Files[curfile].size = symbolrefcount * sizeof(SymbolRefStruct);
	curfile++;
	Type2Files[curfile].data = (unsigned long *)textstrings;
	Type2Files[curfile].reloffset = Type2Files[curfile-1].reloffset + Type2Files[curfile-1].size;
	Type2Files[curfile].size = textstringsize;
	curfile++;
	Type2Files[curfile].data = (unsigned long *)flics;
	Type2Files[curfile].reloffset = Type2Files[curfile-1].reloffset + Type2Files[curfile-1].size;
	Type2Files[curfile].size = flicsize;
	curfile++;
	Type2Files[curfile].data = (unsigned long *)textures;
	Type2Files[curfile].reloffset = Type2Files[curfile-1].reloffset + Type2Files[curfile-1].size;
	Type2Files[curfile].size = texturesize;
	curfile++;
	Type2Files[curfile].data = (unsigned long *)gsis;
	Type2Files[curfile].reloffset = Type2Files[curfile-1].reloffset + Type2Files[curfile-1].size;
	Type2Files[curfile].size = gsisize;
	curfile++;
	Type2Files[curfile].data = (unsigned long *)sids;
	Type2Files[curfile].reloffset = Type2Files[curfile-1].reloffset + Type2Files[curfile-1].size;
	Type2Files[curfile].size = sidsize;
	curfile++;
	if (stalls)
	{
        Type2Files[curfile].data = (unsigned long *)stalls;
        Type2Files[curfile].reloffset = Type2Files[curfile-1].reloffset + Type2Files[curfile-1].size;
        Type2Files[curfile].size = stallsize;
        curfile++;
	}
	if (stallitems)
	{
        Type2Files[curfile].data = (unsigned long *)stallitems;
        Type2Files[curfile].reloffset = Type2Files[curfile-1].reloffset + Type2Files[curfile-1].size;
        Type2Files[curfile].size = stallitemsize;
        curfile++;
	}
	if (attractions)
	{
        Type2Files[curfile].data = (unsigned long *)attractions;
        Type2Files[curfile].reloffset = Type2Files[curfile-1].reloffset + Type2Files[curfile-1].size;
        Type2Files[curfile].size = attractionsize;
        curfile++;
	}
	OpenFiles[OVL_COMMON].Types[0].count = 1;
	OpenFiles[OVL_COMMON].Types[0].dataptr = Type0Files;
	OpenFiles[OVL_COMMON].Types[0].reloffset = 0;
	OpenFiles[OVL_COMMON].Types[0].size = stringtablesize;
	OpenFiles[OVL_UNIQUE].Types[0].count = 0;
	OpenFiles[OVL_UNIQUE].Types[0].dataptr = 0;
	OpenFiles[OVL_UNIQUE].Types[0].reloffset = stringtablesize;
	OpenFiles[OVL_UNIQUE].Types[0].size = 0;
	OpenFiles[OVL_UNIQUE].Types[1].count = 0;
	OpenFiles[OVL_UNIQUE].Types[1].dataptr = 0;
	OpenFiles[OVL_UNIQUE].Types[1].size = 0;
	OpenFiles[OVL_UNIQUE].Types[1].reloffset = stringtablesize;
	OpenFiles[OVL_UNIQUE].Types[2].reloffset = OpenFiles[OVL_COMMON].Types[1].reloffset;
	OpenFiles[OVL_UNIQUE].Types[2].dataptr = Type2Files;
	OpenFiles[OVL_UNIQUE].Types[2].count = 8;
	if (stallsize)
	{
	    OpenFiles[OVL_UNIQUE].Types[2].count++;
	}
	if (stallitems)
	{
	    OpenFiles[OVL_UNIQUE].Types[2].count++;
	}
	if (attractions)
	{
	    OpenFiles[OVL_UNIQUE].Types[2].count++;
	}
	OpenFiles[OVL_UNIQUE].Types[2].size = 0;
	for (unsigned int sz = 0;sz < OpenFiles[OVL_UNIQUE].Types[2].count;sz++)
	{
	    OpenFiles[OVL_UNIQUE].Types[2].size += OpenFiles[OVL_UNIQUE].Types[2].dataptr[sz].size;
	}
	OpenFiles[OVL_UNIQUE].Types[3].count = 0;
	OpenFiles[OVL_UNIQUE].Types[3].dataptr = 0;
	OpenFiles[OVL_UNIQUE].Types[3].reloffset = OpenFiles[OVL_COMMON].Types[2].reloffset+OpenFiles[0].Types[2].size;
	OpenFiles[OVL_UNIQUE].Types[3].size = 0;
	OpenFiles[OVL_UNIQUE].Types[4].count = 0;
	OpenFiles[OVL_UNIQUE].Types[4].dataptr = 0;
	OpenFiles[OVL_UNIQUE].Types[4].reloffset = OpenFiles[OVL_COMMON].Types[2].reloffset+OpenFiles[0].Types[2].size;
	OpenFiles[OVL_UNIQUE].Types[4].size = 0;
	OpenFiles[OVL_UNIQUE].Types[5].count = 0;
	OpenFiles[OVL_UNIQUE].Types[5].dataptr = 0;
	OpenFiles[OVL_UNIQUE].Types[5].reloffset = OpenFiles[OVL_COMMON].Types[2].reloffset+OpenFiles[0].Types[2].size;
	OpenFiles[OVL_UNIQUE].Types[5].size = 0;
	OpenFiles[OVL_UNIQUE].Types[6].count = 0;
	OpenFiles[OVL_UNIQUE].Types[6].dataptr = 0;
	OpenFiles[OVL_UNIQUE].Types[6].reloffset = OpenFiles[OVL_COMMON].Types[2].reloffset+OpenFiles[0].Types[2].size;
	OpenFiles[OVL_UNIQUE].Types[6].size = 0;
	OpenFiles[OVL_UNIQUE].Types[7].count = 0;
	OpenFiles[OVL_UNIQUE].Types[7].dataptr = 0;
	OpenFiles[OVL_UNIQUE].Types[7].reloffset = OpenFiles[OVL_COMMON].Types[2].reloffset+OpenFiles[0].Types[2].size;
	OpenFiles[OVL_UNIQUE].Types[7].size = 0;
	OpenFiles[OVL_UNIQUE].Types[8].count = 0;
	OpenFiles[OVL_UNIQUE].Types[8].dataptr = 0;
	OpenFiles[OVL_UNIQUE].Types[8].reloffset = OpenFiles[OVL_COMMON].Types[2].reloffset+OpenFiles[0].Types[2].size;
	OpenFiles[OVL_UNIQUE].Types[8].size = 0;

	strings = (char *)stringtable;
	memcpy(strings,stdstrings,STDSTRINGS_SIZE);
	char *StyleIndex = strings;
	GUIIcon = StyleIndex+(strlen(StyleIndex)+1);
	char *nullstring = GUIIcon+(strlen(GUIIcon)+1);
	char *mapblue = nullstring+1;
	char *mapred = mapblue+strlen(mapblue)+1;
	char *mapgreen = mapred+strlen(mapred)+1;
	strings = mapgreen+strlen(mapgreen)+1;
	use_extra[0].type = nullstring;
	use_extra[0].parameter = nullstring;
	for(unsigned long i = 1; i < EXTRA_COUNT; i++) {
	    if (use_extra[i].used) {
	        use_extra[i].type = strings;
            strcpy(strings, def_extras[i].type);
            strings += strlen(def_extras[i].type)+1;
            if (def_extras[i].parameter) {
                use_extra[i].parameter = strings;
                strcpy(strings, def_extras[i].parameter);
                strings += strlen(def_extras[i].parameter)+1;
            } else {
                use_extra[i].parameter = nullstring;
            }
	    }
	}
	use_supports[0].supports = nullstring;
	for(unsigned long i = 1; i < SUPPORTS_COUNT; i++) {
	    if (use_supports[i].used) {
	        use_supports[i].supports = strings;
	        strcpy(strings, def_supports[i]);
	        strings += strlen(def_supports[i])+1;
	    }
	}
/*
	char *mapblue;
	char *mapgreen;
	char *mapred;
	extra[1] = GUIIcon+(strlen(GUIIcon)+1);
	extra[2] = extra[1]+(strlen(extra[1])+1);
	extraparams[2] = extra[2]+(strlen(extra[2])+1);
	extra[3] = extraparams[2]+(strlen(extraparams[2])+1);
	extra[4] = extra[3]+(strlen(extra[3])+1);
	extra[5] = extra[4]+(strlen(extra[4])+1);
	extra[6] = extra[5]+(strlen(extra[5])+1);
	extra[7] = extra[6]+(strlen(extra[6])+1);
	extraparams[7] = extra[7]+(strlen(extra[7])+1);
	extra[8] = extraparams[7]+(strlen(extraparams[7])+1);
	extra[9] = extra[8]+(strlen(extra[8])+1);
	extraparams[9] = extra[9]+(strlen(extra[9])+1);
	supports[1] = extraparams[9]+(strlen(extraparams[9])+1);
	supports[2] = supports[1]+(strlen(supports[1])+1);
	supports[3] = supports[2]+(strlen(supports[2])+1);
	supports[4] = supports[3]+(strlen(supports[3])+1);
	supports[5] = supports[4]+(strlen(supports[4])+1);
	supports[6] = supports[5]+(strlen(supports[5])+1);
	supports[7] = supports[6]+(strlen(supports[6])+1);
	supports[8] = supports[7]+(strlen(supports[7])+1);
	supports[9] = supports[8]+(strlen(supports[8])+1);
	supports[10] = supports[9]+(strlen(supports[9])+1);
	supports[11] = supports[10]+(strlen(supports[10])+1);
	supports[12] = supports[11]+(strlen(supports[11])+1);
	supports[13] = supports[12]+(strlen(supports[12])+1);
	supports[14] = supports[13]+(strlen(supports[13])+1);
	supports[15] = supports[14]+(strlen(supports[14])+1);
	supports[16] = supports[15]+(strlen(supports[15])+1);
	supports[17] = supports[16]+(strlen(supports[16])+1);
	supports[18] = supports[17]+(strlen(supports[17])+1);
	supports[19] = supports[18]+(strlen(supports[18])+1);
	supports[20] = supports[19]+(strlen(supports[19])+1);
	supports[21] = supports[20]+(strlen(supports[20])+1);
	supports[22] = supports[21]+(strlen(supports[21])+1);
	supports[23] = supports[22]+(strlen(supports[22])+1);
	supports[0] = supports[23]+(strlen(supports[23])+1);
	extra[0] = supports[23]+(strlen(supports[23])+1);
	extraparams[0] = supports[23]+(strlen(supports[23])+1);
	extraparams[1] = supports[23]+(strlen(supports[23])+1);
	extraparams[3] = supports[23]+(strlen(supports[23])+1);
	extraparams[4] = supports[23]+(strlen(supports[23])+1);
	extraparams[5] = supports[23]+(strlen(supports[23])+1);
	extraparams[6] = supports[23]+(strlen(supports[23])+1);
	extraparams[8] = supports[23]+(strlen(supports[23])+1);
	extraparams[9] = supports[23]+(strlen(supports[23])+1);
	extraparams[10] = supports[23]+(strlen(supports[23])+1);
	extraparams[11] = supports[23]+(strlen(supports[23])+1);
	extraparams[12] = supports[23]+(strlen(supports[23])+1);
	extraparams[13] = supports[23]+(strlen(supports[23])+1);
	mapblue = extraparams[0]+1;
	mapred = mapblue+strlen(mapblue)+1;
	mapgreen = mapred+strlen(mapred)+1;
	extra[10] = mapgreen+strlen(mapgreen)+1;
	extra[11] = extra[10]+strlen(extra[10])+1;
	extra[12] = extra[11]+strlen(extra[11])+1;
	extra[13] = extra[12]+strlen(extra[12])+1;
	supports[24] = extra[13]+strlen(extra[13])+1;
	supports[25] = supports[24]+strlen(supports[24])+1;
	supports[26] = supports[25]+strlen(supports[25])+1;
	supports[27] = supports[26]+strlen(supports[26])+1;
	supports[28] = supports[27]+strlen(supports[27])+1;
	supports[29] = supports[28]+strlen(supports[28])+1;
	supports[30] = supports[29]+strlen(supports[29])+1;
	supports[31] = supports[30]+strlen(supports[30])+1;
	supports[32] = supports[31]+strlen(supports[31])+1;
	supports[33] = supports[32]+strlen(supports[32])+1;
	supports[34] = supports[33]+strlen(supports[33])+1;
	strings = supports[34]+strlen(supports[34])+1;
*/
	symbols[SymbolPos].Symbol = StyleIndex;
	symbols[SymbolPos].data = (unsigned long *)styleval;
	symbols[SymbolPos].IsPointer = 0;
	SymbolPos++;
	symbols[SymbolPos].Symbol = mapred;
	symbols[SymbolPos].data = 0;
	symbols[SymbolPos].IsPointer = 0;
	SymbolPos++;
	symbols[SymbolPos].Symbol = mapgreen;
	symbols[SymbolPos].data = 0;
	symbols[SymbolPos].IsPointer = 0;
	SymbolPos++;
	symbols[SymbolPos].Symbol = mapblue;
	symbols[SymbolPos].data = 0;
	symbols[SymbolPos].IsPointer = 0;
	SymbolPos++;
	TextString::LoaderNumber = loadernum;
	loadernum++;
	Texture::LoaderNumber = loadernum;
	loadernum++;
	Texture::LoaderNumber2 = loadernum;
	loadernum++;
	GUISkin::LoaderNumber = loadernum;
	loadernum++;
	loadernum++; //account for SceneryItem
	if (stallsize)
	{
        StallData::LoaderNumber = loadernum;
        loadernum++;
	}
	if (attractionsize)
	{
        AttractionData::LoaderNumber = loadernum;
        loadernum++;
	}
	TextString::AddData((unsigned long *)textstrings);
	Texture::AddData((unsigned long *)textures,(unsigned long *)flics);
	GUISkin::AddData((unsigned long *)gsis);
	for (unsigned long i = 0;i < SceneryItems.size();i++)
	{
		Scenery *sc = SceneryItems[i];
		char fname[_MAX_FNAME];
		_splitpath(sc->ovl3,NULL,NULL,fname,NULL);
		strchr(fname,'.')[0] = 0;
		SceneryItem *s = (SceneryItem *)sids;
		sids += sizeof(SceneryItem);
#ifdef WRITE_SCENERYITEMEXTRA
		SceneryItemExtra *sext = (SceneryItemExtra *)sids;
		sids += sizeof(SceneryItemExtra);
#endif
		SceneryItemData *s2 = (SceneryItemData *)sids;
		sids += sizeof(SceneryItemData);
		long *s3 = (long *)sids;
		sids += 4;
		SceneryItemVisual **svd = (SceneryItemVisual **)sids;
		unsigned int SymbolsToResolve = 5;
		if (!sc->wallicon)
		{
		    SymbolsToResolve--;
		}
		if (!sc->wallname)
		{
		    SymbolsToResolve--;
		}
        AddLoader((unsigned long *)s,4,SymbolsToResolve,sc->InternalOVLName,":sid",false);
        AddRef2((unsigned long *)svd,sc->InternalOVLName,":svd");
		char *str3 = AddString(sc->ovlWithoutExt);
		sids += 4;
		SceneryParams *s4 = 0;
		if (sc->extra != 0)
		{
			s4 = (SceneryParams *)sids;
			sids += sizeof(SceneryParams);
		}
		s->unk1 = 0;
		s->unkf1 = sc->flag1;
		s->unkf2 = sc->flag2;
		s->unkf3 = sc->flag3;
		s->unkf4 = sc->flag4;
		s->unkf5 = sc->flag5;
		s->unkf6 = sc->flag6;
		s->unkf7 = sc->flag7;
		s->unkf8 = sc->flag8;
		s->unkf9 = sc->flag9;
		s->unkf10 = sc->flag10;
		s->unkf11 = sc->flag11;
		s->unkf12 = sc->flag12;
		s->unkf13 = sc->flag13;
		s->unkf14 = sc->flag14;
		s->unkf15 = sc->flag15;
		s->unkf16 = sc->flag16;
		s->unkf17 = sc->flag17;
		s->unkf18 = sc->flag18;
		s->unkf19 = sc->flag19;
		s->unkf20 = sc->flag20;
		s->unkf21 = sc->flag21;
		s->unkf22 = sc->flag22;
		s->unkf23 = sc->flag23;
		s->unkf24 = sc->flag24;
		s->unkf25 = sc->flag25;
		s->unkf26 = sc->flag26;
		s->unkf27 = sc->flag27;
		s->unkf28 = sc->flag28;
		s->unkf29 = sc->flag29;
		s->unkf30 = sc->flag30;
		s->unkf31 = sc->flag31;
		s->unkf32 = sc->flag32;
		s->size = sc->sizeflag;
#ifdef WRITE_SCENERYITEMEXTRA
		s->size |= 0xFFFF0000; // Activate extra;
#endif
		s->unk4 = sc->unk4;
		s->xsquares = sc->sizex;
		s->ysquares = sc->sizey;
		s->data = s2;
		relocations.push((unsigned long *)&s->data);
		s->xpos = sc->position.x;
		s->ypos = sc->position.y;
		s->zpos = sc->position.z;
		s->xsize = sc->size.x;
		s->ysize = sc->size.y;
		s->zsize = sc->size.z;
		s->unk13 = 0;
		s->unk14 = 0;
		s->cost = sc->cost;
		s->refund = sc->refund;
		s->unk17 = 0xFFFFFFFF;
		if (sc->unk17 == 0)
		{
			s->unk17 = 0;
		}
		if (sc->unk17 == 1)
		{
			s->unk17 = 4;
		}
		if (sc->unk17 == 2)
		{
			s->unk17 = 5;
		}
		s->type = sc->type;
		s->supports = use_supports[sc->supportstype].supports;
		relocations.push((unsigned long *)&s->supports);
		s->svdcount = 1;
		s->svd = svd;
		relocations.push((unsigned long *)&s->svd);
		s->gsi = 0;
		s->wallsetgsi = 0;
		s->wallsetname = 0;
		s->OvlName = str3;
		relocations.push((unsigned long *)&s->OvlName);
		if (s4 != 0)
		{
			s->params = s4;
			relocations.push((unsigned long *)&s->params);
			s->count = 1;
			s4->type = use_extra[sc->extra].type;
			s4->params = use_extra[sc->extra].parameter;
			relocations.push((unsigned long *)&s4->type);
			relocations.push((unsigned long *)&s4->params);
		}
		else
		{
			s->params = 0;
			s->count = 0;
		}
		s->SoundsCount = 0;
		s->Sounds = 0;
		s->Name = 0;
		s->unk27 = 0;
		s->unk28 = 0;
		if (s->type == TYPE_STALL)
		{
		    s->unk29 = 1440;
		    s->unk30 = 2880;
		}
		else
		{
            s->unk29 = 0;
            s->unk30 = 0;
		}
		s->unk31 = sc->unk31;
		s->unk32 = sc->unk32;
		s->unk33 = sc->unk33;
		s->unk34 = 0;
		s->unk35 = 0;
		s->unk36 = 0;
		s->unk37 = 0;
		s->unk38 = 0;
		s->unk39 = 0;
		s->unk40 = 0;
		s->unk41 = 0;
		s->unk42 = 0;
		s->unk43 = 0;
		s->unk44 = 0;
		s->unk45 = 0;
		s->unk46 = 0;
		s->unk47 = 0;
		s->unk48 = 0;
#ifdef WRITE_SCENERYITEMEXTRA
		sext->SoundsUnk = 0;
		sext->unk2 = 0;
		sext->AddonPack = 0;
		sext->GenericAddon = 0;
#endif
		s2->unkf33 = sc->flag33;
		s2->unkf34 = sc->flag34;
		s2->unkf35 = sc->flag35;
		s2->unkf36 = sc->flag36;
		s2->unkf37 = sc->flag37;
		s2->unkf38 = sc->flag38;
		s2->unkf39 = sc->flag39;
		s2->unkf40 = sc->flag40;
		s2->unkf41 = sc->flag41;
		s2->unkf42 = sc->flag42;
		s2->unkf43 = sc->flag43;
		s2->unkf44 = sc->flag44;
		s2->unkf45 = sc->flag45;
		s2->unkf46 = sc->flag46;
		s2->unkf47 = sc->flag47;
		s2->unkf48 = sc->flag48;
		s2->unkf49 = sc->flag49;
		s2->unkf50 = sc->flag50;
		s2->unkf51 = sc->flag51;
		s2->unkf52 = sc->flag52;
		s2->unkf53 = sc->flag53;
		s2->unkf54 = sc->flag54;
		s2->unkf55 = sc->flag55;
		s2->unkf56 = sc->flag56;
		s2->unkf57 = sc->flag57;
		s2->unkf58 = sc->flag58;
		s2->unkf59 = sc->flag59;
		s2->unkf60 = sc->flag60;
		s2->unkf61 = sc->flag61;
		s2->unkf62 = sc->flag62;
		s2->unkf63 = sc->flag63;
		s2->unkf64 = sc->flag64;
		s2->unk2 = sc->dunk2;
		s2->unk3 = sc->dunk3;
		if (sc->dunk4 != 0xFFFFFFFF)
		{
			s2->unk4 = s3;
		}
		else
		{
			s2->unk4 = 0;
		}
		relocations.push((unsigned long *)&s2->unk4);
		s2->unk5 = sc->dunk5;
		*s3 = sc->dunk4;
		*svd = 0;
		Symbol = FindString(sc->icon,":gsi");
		AddRef((unsigned long *)&s->gsi,Symbol);
		if (sc->wallicon != 0)
		{
            Symbol = FindString(sc->wallicon,":gsi");
			AddRef((unsigned long *)&s->wallsetgsi,Symbol);
		}
        Symbol = FindString(sc->name,":txt");
		AddRef((unsigned long *)&s->Name,Symbol);
		if (sc->wallname != 0)
		{
            Symbol = FindString(sc->wallname,":txt");
            AddRef((unsigned long *)&s->wallsetname,Symbol);
		}
	}
	if (stallsize)
	{
        StallData::AddData((unsigned long *)stalls,(unsigned long *)stallitems);
	}
	if (attractionsize)
	{
	    AttractionData::AddData((unsigned long *)attractions);
	}
	SymbolStruct *syms2 = (SymbolStruct *)symbols;
	qsort(syms2,symbolcount,sizeof(SymbolStruct),compare);
	for (i = 0;i < symbolcount;i++)
	{
		relocations.push((unsigned long *)&(syms2[i].Symbol));
		if ((syms2[i].Symbol != mapblue) && (syms2[i].Symbol != mapgreen) && (syms2[i].Symbol != mapred) && (syms2[i].Symbol != StyleIndex))
		{
			relocations.push((unsigned long *)&(syms2[i].data));
		}
	}
	LoaderStruct *ldrs2 = (LoaderStruct *)loaders;
	for (i = 0;i < loadercount;i++)
	{
		for (unsigned long j = 0;j < symbolcount;j++)
		{
			if (ldrs2[i].data == syms2[j].data)
			{
				ldrs2[i].Sym = &syms2[j];
			}
		}
	}
	OvlHeader h;
	h.magic = 0x4B524746;
	h.References = 0;
	h.reserved = 0;
	h.version = 1;
	OvlHeader2 h2;
	h2.FileTypeCount = 5;
    if (stallsize)
    {
        h2.FileTypeCount++;
    }
    if (attractionsize)
    {
        h2.FileTypeCount++;
    }
	h2.unk = 0;
	FILE *f = fopen(OpenFiles[OVL_UNIQUE].FileName,"wb");
	fwrite(&h,sizeof(h),1,f);
	fwrite(&h2,sizeof(h2),1,f);
	TextString::WriteFileHeader(f);
    Texture::WriteFileHeader(f);
    GUISkin::WriteFileHeader(f);
	unsigned short len1;
	const char *str1;
	unsigned short len2;
	const char *str2;
	unsigned long type;
	unsigned short len3;
	const char *str3;
	len1 = 4;
	str1 = "RCT3";
	len2 = 0xB;
	str2 = "SceneryItem";
	type = 1;
	len3 = 3;
	str3 = "sid";
	fwrite(&len1,sizeof(len1),1,f);
	fwrite(str1,len1,1,f);
	fwrite(&len2,sizeof(len2),1,f);
	fwrite(str2,len2,1,f);
	fwrite(&type,sizeof(type),1,f);
	fwrite(&len3,sizeof(len3),1,f);
	fwrite(str3,len3,1,f);
	if (stallsize)
	{
        StallData::WriteFileHeader(f);
	}
	if (attractionsize)
	{
        AttractionData::WriteFileHeader(f);
	}
	for (i = 0;i < 9;i++)
	{
		fwrite(&OpenFiles[OVL_UNIQUE].Types[i].count,sizeof(OpenFiles[OVL_UNIQUE].Types[i].count),1,f);
	}

	CurrentFile = OVL_UNIQUE;

	while (relocations.empty() == false)
	{
		unsigned long *relocation = relocations.front();
		relocations.pop();
		unsigned long *reloc = *(unsigned long **)relocation;
		if (reloc != 0)
		{
			unsigned long fixup = DoRelocationForSaving(reloc);
			if (fixup == 0xFFFFFFFF)
			{
				fixup = 0;
			}
			*relocation = fixup;
			unsigned long fixup2 = DoRelocationForSaving(relocation);
			fixups.push(fixup2);
		}
	}
	for (i = 0;i < 9;i++)
	{
		for (unsigned long j = 0;j < OpenFiles[OVL_UNIQUE].Types[i].count;j++)
		{
			fwrite(&OpenFiles[OVL_UNIQUE].Types[i].dataptr[j].size,sizeof(OpenFiles[OVL_UNIQUE].Types[i].dataptr[j].size),1,f);
			fwrite(OpenFiles[OVL_UNIQUE].Types[i].dataptr[j].data,OpenFiles[OVL_UNIQUE].Types[i].dataptr[j].size,1,f);
		}
	}
	unsigned long size = fixups.size();
	fwrite(&size,sizeof(size),1,f);
	while (fixups.empty() == false)
	{
		unsigned long fixup = fixups.front();
		fixups.pop();
		fwrite(&fixup,sizeof(fixup),1,f);
	}
	TextString::WriteExtraData(f);
	Texture::WriteExtraData(f);
	fclose(f);
	h.magic = 0x4B524746;
	h.References = 0;
	h.reserved = 0;
	h.version = 1;
	h2.FileTypeCount = 5;
    if (stallsize)
    {
        h2.FileTypeCount++;
    }
    if (attractionsize)
    {
        h2.FileTypeCount++;
    }
	h2.unk = 0;
	strcpy(strstr(OpenFiles[OVL_UNIQUE].FileName,".unique.ovl"),".common.ovl");
	strcpy(OpenFiles[0].FileName,OpenFiles[OVL_UNIQUE].FileName);
	f = fopen(OpenFiles[0].FileName,"wb");
	fwrite(&h,sizeof(h),1,f);
	fwrite(&h2,sizeof(h2),1,f);
	TextString::WriteFileHeader(f);
	Texture::WriteFileHeader(f);
	GUISkin::WriteFileHeader(f);
	len1 = 4;
	str1 = "RCT3";
	len2 = 0xB;
	str2 = "SceneryItem";
	type = 1;
	len3 = 3;
	str3 = "sid";
	fwrite(&len1,sizeof(len1),1,f);
	fwrite(str1,len1,1,f);
	fwrite(&len2,sizeof(len2),1,f);
	fwrite(str2,len2,1,f);
	fwrite(&type,sizeof(type),1,f);
	fwrite(&len3,sizeof(len3),1,f);
	fwrite(str3,len3,1,f);
	if (stallsize)
	{
        StallData::WriteFileHeader(f);
	}
	if (attractionsize)
	{
        AttractionData::WriteFileHeader(f);
	}
	unsigned long tmp = 1;
	fwrite(&tmp,4,1,f);
	tmp = 0;
	for (i = 0;i < 8;i++)
	{
		fwrite(&tmp,4,1,f);
	}
	fwrite(&Type0Files[0].size,4,1,f);
	fwrite(Type0Files[0].data,Type0Files[0].size,1,f);
	fwrite(&tmp,4,1,f);
	fclose(f);
	//delete data;
	delete sidsx;
	delete gsis;
	delete textures;
	delete flics;
	delete textstrings;
	delete symrefs;
	delete loaders;
	delete symbols;
	delete stringtable;

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
//  clearStyleOvlData()
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void clearStyleOVLData()
{
	DeleteStyleOVLInfo_Icons();
	DeleteStyleOVLInfo_IconTextures();
	DeleteStyleOVLInfo_SceneryItems();
	DeleteStyleOVLInfo_TextStrings();
    DeleteStyleOVLInfo_Stalls();
    DeleteStyleOVLInfo_Attractions();
	DeleteInfo_Resources();
	DeleteInfo_FileReferences(OVL_COMMON);
	DeleteInfo_FileReferences(OVL_UNIQUE);
}


////////////////////////////////////////////////////////////////////////////////
//
//  AddStyleOVLInfo_SceneryItem
//  Used
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void AddStyleOVLInfo_SceneryItem(Scenery *scn)
{
	long curItem=-1;

	Scenery *t =new Scenery;
	(*t)=(*scn);
	SceneryItems.push_back(t);
	curItem = SceneryItems.size()-1;

	Scenery *s = SceneryItems[curItem];

	s->name = new char[strlen(scn->name)+1];
	strcpy(s->name,scn->name);
	s->ovl2 = new char[strlen(scn->ovl2)+1];
	strcpy(s->ovl2,scn->ovl2);
	s->ovl3 = new char[strlen(scn->ovl3)+1];
	strcpy(s->ovl3,scn->ovl3);
	s->ovlWithoutExt = new char[strlen(scn->ovlWithoutExt)+1];
	strcpy(s->ovlWithoutExt,scn->ovlWithoutExt);
	s->InternalOVLName = new char[strlen(scn->InternalOVLName)+1];
	strcpy(s->InternalOVLName,scn->InternalOVLName);
	s->location = new char[strlen(scn->location)+1];
	strcpy(s->location,scn->location);
	s->icon = new char[strlen(scn->icon)+1];
	strcpy(s->icon,scn->icon);

	if (scn->wallicon != 0)
	{
		s->wallicon = new char[strlen(scn->wallicon)+1];
		strcpy(s->wallicon,scn->wallicon);
		s->wallname = new char[strlen(scn->wallname)+1];
		strcpy(s->wallname,scn->wallname);
	}

}


////////////////////////////////////////////////////////////////////////////////
//
//  DeleteStyleOVLInfo_SceneryItems
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void DeleteStyleOVLInfo_SceneryItems()
{
	for (unsigned long i = 0;i < SceneryItems.size();i++)
	{
		delete SceneryItems[i]->name;
		delete SceneryItems[i]->ovl2;
		delete SceneryItems[i]->ovl3;
		delete SceneryItems[i]->location;
		delete SceneryItems[i]->icon;
		if (SceneryItems[i]->wallicon != 0)
		{
			delete SceneryItems[i]->wallicon;
			delete SceneryItems[i]->wallname;
		}
		delete SceneryItems[i];
	}

	SceneryItems.clear();
}


////////////////////////////////////////////////////////////////////////////////
//
//  AddStyleOVLInfo_IconTexture
//  Used
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void AddStyleOVLInfo_IconTexture(IconTexture *ict)
{
	Texture *t = new Texture(ict->name,ict->filename,ict->fh,ict->fmh,ict->data);
	Texture::IconTextures.push_back(t);
}


////////////////////////////////////////////////////////////////////////////////
//
//  DeleteStyleOVLInfo_IconTextures
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void DeleteStyleOVLInfo_IconTextures()
{
	for (unsigned long i = 0;i < Texture::IconTextures.size();i++)
	{
		delete Texture::IconTextures[i];
	}
	Texture::IconTextures.clear();
}


////////////////////////////////////////////////////////////////////////////////
//
//  AddStyleOVLInfo_Icon
//  Used
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void AddStyleOVLInfo_Icon(Icon *ico)
{
	GUISkin *t = new GUISkin(ico->name,ico->texture,ico->top,ico->left,ico->bottom,ico->right);
	GUISkin::GUISkins.push_back(t);
}


////////////////////////////////////////////////////////////////////////////////
//
//  DeleteStyleOVLInfo_Icons
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void DeleteStyleOVLInfo_Icons()
{
	for (unsigned long i = 0;i < GUISkin::GUISkins.size();i++)
	{
		delete GUISkin::GUISkins[i];
	}
	GUISkin::GUISkins.clear();
}


////////////////////////////////////////////////////////////////////////////////
//
//  AddStyleOVLInfo_TextString
//  Used
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void AddStyleOVLInfo_TextString(Text *txt)
{
	TextString *t = new TextString(txt->name,txt->text);
	TextString::TextStrings.push_back(t);
}


////////////////////////////////////////////////////////////////////////////////
//
//  DeleteStyleOVLInfo_TextStrings
//
////////////////////////////////////////////////////////////////////////////////

LIBOVL_API void DeleteStyleOVLInfo_TextStrings()
{
	for (unsigned long i = 0;i < TextString::TextStrings.size();i++)
	{
		delete TextString::TextStrings[i];
	}
	TextString::TextStrings.clear();
}


LIBOVL_API void AddStyleOVLInfo_Stalls(StallStr *str)
{
	StallData *t = new StallData(str->Name,str->StallType,str->NameString,str->DescriptionString,str->Unk2,str->Unk3,str->SID,str->ItemCount,str->Items,str->Unk11,str->Unk12,str->Unk13,str->Unk14,str->Unk15,str->Unk16);
	StallData::Stalls.push_back(t);
}

LIBOVL_API void DeleteStyleOVLInfo_Stalls()
{
	for (unsigned long i = 0;i < StallData::Stalls.size();i++)
	{
		delete StallData::Stalls[i];
	}
	StallData::Stalls.clear();
}

LIBOVL_API void AddStyleOVLInfo_Attractions(AttractionStr *str)
{
	AttractionData *t = new AttractionData(str->Name,str->AttractionType,str->NameString,str->DescriptionString,str->Unk2,str->Unk3,str->SID);
	AttractionData::Attractions.push_back(t);
}

LIBOVL_API void DeleteStyleOVLInfo_Attractions()
{
	for (unsigned long i = 0;i < AttractionData::Attractions.size();i++)
	{
		delete AttractionData::Attractions[i];
	}
	AttractionData::Attractions.clear();
}
////////////////////////////////////////////////////////////////////////////////
//
//  compare
//
////////////////////////////////////////////////////////////////////////////////

int compare(const void *s1,const void *s2)
{
	return stricmp(((SymbolStruct *)s1)->Symbol,((SymbolStruct *)s2)->Symbol);
}
