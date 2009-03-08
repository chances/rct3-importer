///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// OVL support classes
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
// Based on libOVL by Jonathan Wilson
//
///////////////////////////////////////////////////////////////////////////////


#ifndef OVLCLASSES_H_INCLUDED
#define OVLCLASSES_H_INCLUDED

#include <map>
#include <set>
#include <string>
#include <string.h>
#include <vector>

#include "ovlstructs.h"

class ovlOVLManager;
class ovlLodSymRefManager;
class ovlExtraChunk;
class cLoader;

class cOvlMemBlob {
public:
    r3::cOvlType type;
    int file;
    unsigned long size;
    unsigned char* data;

    cOvlMemBlob() {
        type = r3::OVLT_COMMON;
        file = 0;
        size = 0;
        data = NULL;
    };
    cOvlMemBlob(r3::cOvlType t, int f, unsigned long s) {
        type = t;
        file = f;
        size = s;
        data = NULL;
    };
};

class cOvlFile {
private:
    void Copy(const cOvlFile& o) {
        if (this == &o)
            return;

        delete data;

	    size = o.size;
	    reloffset = o.reloffset;
	    if (size) {
            data = new unsigned char[size];
            memcpy(data, o.data, size);
	    } else {
	        data = NULL;
	    }
    }
public:
	unsigned long size;
	unsigned long reloffset;
	unsigned char *data;
	//unsigned long unk;
	cOvlFile() {
	    size = 0;
	    reloffset = 0;
	    data = NULL;
	};
	cOvlFile(unsigned long s) {
	    size = s;
	    reloffset = 0;
	    if (size) {
            data = new unsigned char[size];
            memset(data, 0, size);
	    } else {
	        data = NULL;
	    }
	};
	cOvlFile(const cOvlFile& o) {
	    data = NULL;
	    Copy(o);
	}
    cOvlFile& operator=(const cOvlFile& rhs) {
      Copy(rhs);
      return *this;
    }
	~cOvlFile() {
	    delete data;
	}
};

class cOvlFileType {
public:
    std::vector<cOvlFile*> files;
	unsigned long reloffset;
	unsigned long size;
	cOvlFileType() {
	    reloffset = 0;
	    size = 0;
	};
	~cOvlFileType() {
	    for (unsigned int i = 0; i < files.size(); ++i) {
	        delete files[i];
	    }
	}
};

class cOvlFileClass {
public:
	int version;
	std::string filename;
	cOvlFileType types[9];
    std::vector<std::string> references;
    std::set<unsigned long> fixups;

    cOvlFileClass(): version(1) {};
    unsigned char* GetBlock(int filetype, unsigned long size);
    unsigned long MakeRelOffsets(unsigned long from);
    //void Write(const std::map<std::string, ovlOVLManager*>& managers, const std::vector<ovlExtraChunk*>& extra);
    void WriteStreamed(r3::cOvlType type, const std::map<std::string, ovlOVLManager*>& managers, const std::vector<cLoader>& loaders);
};

class cOvlInfo {
public:
    //unsigned long CurrentFile;
    cOvlFileClass OpenFiles[2];

    cOvlInfo(){};
    void MakeOffsets() {
        unsigned long off = OpenFiles[r3::OVLT_COMMON].MakeRelOffsets(0);
        OpenFiles[r3::OVLT_UNIQUE].MakeRelOffsets(off);
    };
    void WriteFiles(const std::map<std::string, ovlOVLManager*>& managers, const ovlLodSymRefManager& lsrman);
	int version() {
		return OpenFiles[0].version;
	}
};

#endif
