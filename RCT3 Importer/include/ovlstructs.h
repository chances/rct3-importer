/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef __ovlstructs_h__
#define __ovlstructs_h__

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

struct OvlInfo
{
    unsigned long CurrentFile;
    OvlFile OpenFiles[2];
};

#define OVL_COMMON 0
#define OVL_UNIQUE 1

#endif

