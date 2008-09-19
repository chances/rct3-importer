/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __libOVL_CommonOVL_h__
#define __libOVL_CommonOVL_h__

#include "Common.h"

#define LIBOVL_VERSION 2

namespace r3old {

struct OvlHeader {
	unsigned long magic;
	unsigned long reserved;
	unsigned long version;
	unsigned long References;
};
struct OvlHeader2 {
	unsigned long unk;
	unsigned long FileTypeCount;
};
struct File
{
	unsigned long size;
	unsigned long reloffset;
	unsigned long *data; //This in unsigned char* in importer
	unsigned long unk;
};
struct FileType
{
	unsigned long count;
	File *dataptr;
	unsigned long reloffset;
	unsigned long size;
};
struct SymbolStruct
{
	char *Symbol;
	unsigned long *data;
	unsigned long IsPointer;
};
struct SymbolStruct2
{
	char *Symbol;
	unsigned long *data;
	unsigned long IsPointer;
	unsigned long Checksum;
};
struct LoaderStruct
{
	unsigned long LoaderType;
	unsigned long *data;
	unsigned long HasExtraData;
	SymbolStruct *Sym;
	unsigned long SymbolsToResolve;
};
struct SymbolRefStruct
{
	unsigned long *reference;
	char *Symbol;
	LoaderStruct *ldr;
};
struct SymbolRefStruct2
{
	unsigned long *reference;
	char *Symbol;
	LoaderStruct *ldr;
	unsigned long Checksum;
};

struct OvlFile
{
	char *FileName; // is char FileName[MAX_PATH]; in importer
	FileType Types[9];
	char **References;
	unsigned long ReferenceCount;
	char *InternalName; //Added to store the unique id
};

}

extern unsigned long CurrentFile;
extern r3old::OvlFile OpenFiles[2];
extern std::queue<unsigned long *> relocations;
extern std::queue<unsigned long> fixups;
const int OVL_COMMON =0;
const int OVL_UNIQUE =1;

typedef struct
{
	unsigned long length;
	unsigned long *data;
} libOVL_Resource; //Used in Type0Files

//Has all the names of the items in the OVL file
extern std::vector <libOVL_Resource *> ResourceItems;

//D3DFORMAT RCT3DFormatToD3DFormat(unsigned long format);
unsigned long DoRelocationForSaving(unsigned long *reloc);
LIBOVL_API void AddInfo_Resource(unsigned long length, unsigned long* data);
LIBOVL_API void DeleteInfo_Resources();
LIBOVL_API void AddInfo_FileReference(char* reference, unsigned long CurrentFile);
LIBOVL_API void DeleteInfo_FileReferences(unsigned long CurrentFile);

LIBOVL_API unsigned int LibOVLCheckVersion(unsigned int version = 0);

#endif
