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
#include <queue>
#include <string>
#include <vector>

//#include "ovlstructs.h"

using namespace std;

enum cOvlType {
    OVLT_COMMON = 0,
    OVLT_UNIQUE = 1
};

class ovlOVLManager;

class cOvlMemBlob {
public:
    cOvlType type;
    int file;
    unsigned long size;
    unsigned char* data;

    cOvlMemBlob() {
        type = OVLT_COMMON;
        file = 0;
        size = 0;
        data = NULL;
    };
    cOvlMemBlob(cOvlType t, int f, unsigned long s) {
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
    vector<cOvlFile*> files;
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
	string filename;
	cOvlFileType types[9];
    vector<string> references;
    queue<unsigned long> fixups;

    cOvlFileClass(){};
    unsigned char* GetBlock(int filetype, unsigned long size);
    unsigned long MakeRelOffsets(unsigned long from);
    void Write(const map<string, ovlOVLManager*>& managers);
};

class cOvlInfo {
public:
    //unsigned long CurrentFile;
    cOvlFileClass OpenFiles[2];

    cOvlInfo(){};
    void MakeOffsets() {
        unsigned long off = OpenFiles[OVLT_COMMON].MakeRelOffsets(0);
        OpenFiles[OVLT_UNIQUE].MakeRelOffsets(off);
    };
    void WriteFiles(const map<string, ovlOVLManager*>& managers) {
        OpenFiles[OVLT_COMMON].Write(managers);
        OpenFiles[OVLT_UNIQUE].Write(managers);
    }
};

#endif
