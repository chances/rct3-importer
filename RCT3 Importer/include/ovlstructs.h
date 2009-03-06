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
	unsigned long magic;		// Always 0x0x4b524746
	unsigned long reserved;		// 0 for v1, 2 for v4/5
	unsigned long version;
	unsigned long References;	// number of references for v1, for v4/5 always 16/0x10
};
struct OvlHeader2 {
	unsigned long unk;				// always 0 for v1/v4
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
	unsigned short IsPointer;
	unsigned short unknown; // FFFF for v5, 0000 for v4 it seems
	unsigned long hash; // hash of the symbol name
	/*
	 * Hash algorithm:
	 * The hash algorithm is the so-called djb2 one, applied after converting the name to lower case.
	 * Code (provided str is already converted to lower case):
	 *   unsigned long hash(unsigned char *str) {
     *       unsigned long hash = 5381;
     *       int c;
     *		 while (c = *str++)
     *            hash = ((hash << 5) + hash) + c;
     *
     *		 return hash;
     *   }
	 */
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
	unsigned long hash; // hash of the symbol name, see SymbolStruct2
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

enum cOvlType {
    OVLT_COMMON = 0,
    OVLT_UNIQUE = 1
};


#endif

