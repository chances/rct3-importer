///////////////////////////////////////////////////////////////////////////////
//
// OVL dumping library
// Copyright (C) 2007 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
// Based on the ovl dumper by Jonathan Wilson
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _OVLDUMPERSTRUCTS_H_
#define _OVLDUMPERSTRUCTS_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "icontexture.h"
#include "ovlstructs.h"

class OvlExtendedHeader {
public:
    // V4 and V5
    unsigned long ReferencesE;
    // V5 only
    unsigned long unknownv5_subvesion_flag;			// Either 1 or 0
    unsigned long unknownv5[3];
    std::vector<unsigned char> unknownv5_bytes;
    std::vector<unsigned char> unknownv5_padding;

    OvlExtendedHeader(): ReferencesE(0), unknownv5_subvesion_flag(0){
		for (int i = 0; i < 3; ++i)
			unknownv5[i] = 0;
    };
    ~OvlExtendedHeader() {
    };
};

struct OvlExtraChuck {
    unsigned long size;
    unsigned long fileoffset;
    unsigned char* data;
};

class OvlLoaderHeader {
public:
    std::string loader;
    std::string name;
    unsigned long type;
    std::string tag;
    unsigned long symbol_count;
    unsigned long symbol_count_order;
    OvlLoaderHeader(){};
};

class OvlRelocation {
public:
    int filetype;

    unsigned long relocation;
    unsigned long* relocationsite;
    r3::cOvlType r_filetype; // Common or unique
    int r_file;     // 0-8
    unsigned long r_block;
    std::string r_inwhat;

    unsigned long targetrelocation;
    void* target;
    r3::cOvlType t_filetype; // Common or unique
    int t_file;     // 0-8
    unsigned long t_block;
    std::string t_usedfor;
    std::string t_data;
    bool t_issymref;
    unsigned long t_apparentlength; // valid only if MakeMoreInfo was called


    OvlRelocation() {
        relocation = 0;
        relocationsite = NULL;
        r_filetype = r3::OVLT_COMMON;
        r_file = -1;
        r_block = 0xFFFFFFFF;
        r_inwhat = "";
        targetrelocation = 0;
        target = NULL;
        t_filetype = r3::OVLT_COMMON;
        t_file = -1;
        t_block = 0xFFFFFFFF;
        t_usedfor = "";
        t_issymref = false;
        t_apparentlength = 0;
    };
};

struct OvlRelocationRComp {
    bool operator() (const OvlRelocation* lhs, const OvlRelocation* rhs) const {
        return lhs->relocation<rhs->relocation;
    }
};

struct OvlRelocationTComp {
    bool operator() (const OvlRelocation* lhs, const OvlRelocation* rhs) const {
        if (lhs->targetrelocation == rhs->targetrelocation) {
            return lhs->relocation < rhs->relocation;
        } else
            return lhs->targetrelocation < rhs->targetrelocation;
    }
};

class OvlFileBlock {
public:
	unsigned long size;
	unsigned long reloffset;
	unsigned char *data; //This in unsigned char* in importer
	unsigned long unk;
	unsigned long fileoffset;
//	std::map<unsigned long, OvlRelocation*> relocs;
//	std::map<unsigned long, OvlRelocation*> targets;
	std::set<OvlRelocation*, OvlRelocationRComp> relocs;
	std::set<OvlRelocation*, OvlRelocationTComp> targets;
	OvlFileBlock() {
	    size = 0;
	    reloffset = 0;
	    data = NULL;
	    unk = 0;
	};
};

class OvlFileTypeBlock {
public:
    unsigned long count;
    unsigned long size;
    unsigned long reloffset;
    std::vector<OvlFileBlock> blocks;

    unsigned long unknownv4v5_1;
    unsigned long unknownv5_extra;

    //char* data;

    OvlFileTypeBlock(){
        count = 0;
        size = 0;
        reloffset = 0;
        unknownv4v5_1 = 0;
    };
};

class OvlPostBlockUnknowns {
public:
    unsigned long unknownv4[2];
    unsigned long unknownv5_bytes_count;
	unsigned long unknownv5_extra_struct_count;
	char* unknownv5_extra_structs;
	unsigned long unknownv5_bytes_final_int;
    std::vector<unsigned long> unknownv5_longs;
    unsigned long unknownv45_postrelocationlong;
    OvlPostBlockUnknowns():
		unknownv5_bytes_count(0),
		unknownv5_extra_struct_count(0),
		unknownv5_extra_structs(NULL),
		unknownv5_bytes_final_int(0),
		unknownv45_postrelocationlong(0)
		{
			unknownv4[0] = 0;
			unknownv4[1] = 0;
		};
};

class OvlSymbol {
public:
    OvlRelocation* symbol;
    OvlRelocation* data;
    unsigned long IsPointer;
    unsigned long unknown;
    unsigned long hash;
    r3::SymbolStruct* orgsymbol;
    OvlSymbol() {
        data = NULL;
        IsPointer = 0;
        unknown = 0;
        hash = 0;
        orgsymbol = NULL;
    };
    void SetData(unsigned long data);
};

class OvlLoader {
public:
    unsigned long loadertype;
    std::string name;
    OvlRelocation* data;
    unsigned long hasextradata;
    unsigned short unknownv5; // Actually the upper word of hasextradata in v5 ovls
    OvlRelocation* symbol;
    std::string symbolname;
    unsigned long symbolstoresolve;
    std::vector<OvlExtraChuck> extradata;

    OvlLoader() {
        data = NULL;
        loadertype = 0;
        hasextradata = 0;
        symbolstoresolve = 0;
        unknownv5 = 0;
    };
};

class OvlStringTableEntry {
public:
    std::string entry;
    unsigned long reloffset;
    unsigned long fileoffset;
    char* orgstring;
    unsigned long orglength;
	std::set<OvlRelocation*, OvlRelocationTComp> relocs;
	OvlStringTableEntry() {
	    orglength = 0;
	    orgstring = NULL;
    };
	void SetString(const char* newstr) const;
};

struct OvlStringTableComp {
    bool operator() (const OvlStringTableEntry& lhs, const OvlStringTableEntry& rhs) const {
        if (lhs.entry != rhs.entry)
            return lhs.entry<rhs.entry;
        return lhs.reloffset<rhs.fileoffset;
    }
};

struct OvlFlicData {
    FlicHeader fh;
    long bmptable;
    unsigned long index;
    unsigned long bmpindex;
    unsigned char* data;
    std::string texture;
};


#endif
