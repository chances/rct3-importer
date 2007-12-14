
#ifndef _OVLDUMPERSTRUCTS_H_
#define _OVLDUMPERSTRUCTS_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ovlstructs.h"

using namespace std;

class OvlExtendedHeader {
public:
    // V4 and V5
    unsigned long ReferencesE;
    // V5 only
    unsigned long unknownv5_count;
    unsigned long unknownv5_1;
    vector<unsigned long> unknownv5_list;
    unsigned long unknownv5_2;
    vector<unsigned char> unknownv5_bytes;
    vector<unsigned char> unknownv5_padding;

    OvlExtendedHeader() {
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
    string loader;
    string name;
    unsigned long type;
    string tag;
    unsigned long unknownv5[2];
    OvlLoaderHeader(){};
};

class OvlRelocation {
public:
    int filetype;

    unsigned long relocation;
    unsigned long* relocationsite;
    int r_filetype; // Common or unique
    int r_file;     // 0-8
    unsigned long r_block;
    string r_inwhat;

    unsigned long targetrelocation;
    void* target;
    int t_filetype; // Common or unique
    int t_file;     // 0-8
    unsigned long t_block;
    string t_usedfor;
    string t_data;
    bool t_issymref;
    unsigned long t_apparentlength; // valid only if MakeMoreInfo was called


    OvlRelocation() {
        relocation = 0;
        relocationsite = NULL;
        r_filetype = 0;
        r_file = -1;
        r_block = 0xFFFFFFFF;
        r_inwhat = "";
        targetrelocation = 0;
        target = NULL;
        t_filetype = 0;
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
	set<OvlRelocation*, OvlRelocationRComp> relocs;
	set<OvlRelocation*, OvlRelocationTComp> targets;
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
    vector<OvlFileBlock> files;

    vector<unsigned long> unknownv5_list;
    unsigned long unknownv4v5_1;

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
    vector<unsigned char> unknownv5_bytes;
    vector<unsigned long> unknownv5_longs;
    vector<unsigned long> unknownv5_postrelocationlongs;
    OvlPostBlockUnknowns() {};
};

class OvlSymbol {
public:
    OvlRelocation* symbol;
    OvlRelocation* data;
    unsigned long IsPointer;
    unsigned long unknown;
    unsigned long checksum;
    SymbolStruct* orgsymbol;
    OvlSymbol() {
        data = NULL;
        IsPointer = 0;
        unknown = 0;
        checksum = 0;
        orgsymbol = NULL;
    };
    void SetData(unsigned long data);
};

class OvlLoader {
public:
    unsigned long loadertype;
    string name;
    OvlRelocation* data;
    unsigned long hasextradata;
    unsigned short unknownv5; // Actually the upper word of hasextradata in v5 ovls
    OvlRelocation* symbol;
    string symbolname;
    unsigned long symbolstoresolve;
    vector<OvlExtraChuck> extradata;

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
    string entry;
    unsigned long reloffset;
    unsigned long fileoffset;
    char* orgstring;
    unsigned long orglength;
	set<OvlRelocation*, OvlRelocationTComp> relocs;
	OvlStringTableEntry() {
	    orglength = 0;
	    orgstring = NULL;
    };
	void SetString(const char* newstr) const;
};

struct OvlStringTableComp {
    bool operator() (const OvlStringTableEntry& lhs, const OvlStringTableEntry& rhs) const {
        if (lhs.entry != rhs.entry);
            return lhs.entry<rhs.entry;
        return lhs.reloffset<rhs.fileoffset;
    }
};


#endif
