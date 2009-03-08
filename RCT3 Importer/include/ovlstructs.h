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

#include "rct3basetypes.h"

namespace r3 {
	
struct OvlHeader {
	r3::uint32_t magic;		// Always 0x0x4b524746
	r3::uint32_t reserved;		// 0 for v1, 2 for v4/5
	r3::uint32_t version;
	r3::uint32_t References;	// number of references for v1, for v4/5 always 16/0x10
};

struct OvlHeader2 {
	r3::uint32_t unk;				// always 0 for v1/v4
	r3::uint32_t FileTypeCount;
};

struct File {
	r3::uint32_t  size;
	r3::uint32_t  reloffset;
	r3::uint32_t* data; //This in unsigned char* in importer
	r3::uint32_t  unk;
};

struct FileType {
	r3::uint32_t count;
	File*		 dataptr;
	r3::uint32_t reloffset;
	r3::uint32_t size;
};

struct SymbolStruct {
	char*			Symbol;
	r3::uint32_t*	data;
	r3::uint32_t 	IsPointer;
};

struct SymbolStruct2 {
	char*			Symbol;
	r3::uint32_t*	data;
	r3::uint16_t 	IsPointer;
	r3::uint16_t 	unknown; // mostly FFFF for v5 (0000 for mdl symbols), 0000 for v4 it seems
	r3::uint32_t 	hash; // hash of the symbol name
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

struct LoaderStruct {
	r3::uint32_t	LoaderType;
	r3::uint32_t*	data;
	r3::uint32_t 	HasExtraData;
	SymbolStruct*	Sym;
	r3::uint32_t	SymbolsToResolve;
};

struct SymbolRefStruct {
	r3::uint32_t*	reference;
	char*			Symbol;
	LoaderStruct*	ldr;
};

struct SymbolRefStruct2 {
	r3::uint32_t*	reference;
	char*			Symbol;
	LoaderStruct*	ldr;
	r3::uint32_t 	hash; // hash of the symbol name, see SymbolStruct2
};

struct ExtraDataInfoV5 {
	r3::uint32_t	index;
	r3::uint16_t	unk01;	///< Usually 0xFFFF, only the second structure for mdl extradata has 0
	r3::uint16_t	unk02;	///< 1 or 2
							/**<
							 * 1: mdl (2nd), flic, txt, modelanim
							 * 2: bmptbl, mdl (1st)
							 */
	r3::uint32_t	unk03;	///< Only ever seen 1
	r3::uint16_t	unk04;  ///< 1 or 2, usually matches unk02
};

struct OvlFile {
	char *FileName; // is char FileName[MAX_PATH]; in importer
	FileType Types[9];
	char **References;
	unsigned long ReferenceCount;
	char *InternalName; //Added to store the unique id
};

struct OvlInfo {
    unsigned long CurrentFile;
    OvlFile OpenFiles[2];
};

#define OVL_COMMON 0
#define OVL_UNIQUE 1

enum cOvlType {
    OVLT_COMMON = 0,
    OVLT_UNIQUE = 1
};

};

#endif

