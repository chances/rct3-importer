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

#include "OVLClasses.h"

#include <fstream>
#include <unistd.h>

#include "ManagerOVL.h"
#include "OVLException.h"
#include "ovlstructs.h"

#include "pretty.h"

//#define DUMP_FILES

using namespace std;
using namespace r3;

unsigned char* cOvlFileClass::GetBlock(int filetype, unsigned long size) {
    cOvlFile* fi= new cOvlFile(size);
    types[filetype].files.push_back(fi);
    return fi->data;
}

unsigned long cOvlFileClass::MakeRelOffsets(unsigned long from) {
    unsigned long currel = from;
    for (int i = 0; i < 9; ++i) {
        types[i].reloffset = currel;
#ifdef DUMP_FILES
fprintf(stderr, "File %d\n", i);
fprintf(stderr, "  Reloffset %08lx\n", currel);
#endif
        types[i].size = 0;
        for (vector<cOvlFile*>::iterator it = types[i].files.begin(); it != types[i].files.end(); ++it) {
            (*it)->reloffset = currel;
#ifdef DUMP_FILES
fprintf(stderr, "    Block relofs %08lx, size %08lx, data %p\n", (*it)->reloffset, (*it)->size, (*it)->data);
#endif
            currel += (*it)->size;
            types[i].size += (*it)->size;
        }
#ifdef DUMP_FILES
fprintf(stderr, "  Size %08lx\n", types[i].size);
#endif
    }
    return currel;
}

void cOvlFileClass::WriteStreamed(r3::cOvlType type, const map<string, ovlOVLManager*>& managers, const std::vector<cLoader>& loaders) {
    fstream fstr(filename.c_str(), fstream::out | fstream::binary | fstream::trunc);
    if (!fstr.is_open())
        BOOST_THROW_EXCEPTION(EOvl("cOvlFileClass::WriteStreamed: cannot open file for writing"));

    // Header 1
    OvlHeader h;
    h.magic = 0x4B524746;
    h.References = references.size();
	if (version == 1) {
		h.reserved = 0;		
	} else {
		h.reserved = 2;		
	}
    h.version = version;
    //fstr.write(reinterpret_cast<char*>(&h), sizeof(OvlHeader));
	fstr.write(reinterpret_cast<char*>(&h.magic), 4);
	fstr.write(reinterpret_cast<char*>(&h.reserved), 4);
	fstr.write(reinterpret_cast<char*>(&h.version), 4);
	
	if (version > 1) {
		int32_t unknown = 16;
		fstr.write(reinterpret_cast<char*>(&unknown), 4);
		if (version == 5) {
			unknown = 0;
			// This is the subversion flag. It is not clear what follows if it's set, alas it only seems
			// to be necessary for mdl structures, therefore we do not set it.
			fstr.write(reinterpret_cast<char*>(&unknown), 4);			
		}
	}

    // References
	fstr.write(reinterpret_cast<char*>(&h.References), 4);
    for (unsigned int i = 0;i < references.size();i++) {
        unsigned short len = static_cast<unsigned short>(references[i].length());
        fstr.write(reinterpret_cast<char*>(&len),2);
        fstr.write(references[i].c_str(),len);
    }

    // Header 2
    OvlHeader2 h2;
    h2.FileTypeCount = managers.size();
    h2.unk = 0;
    fstr.write(reinterpret_cast<char*>(&h2), sizeof(OvlHeader2));

    // Loaders
    for (map<string, ovlOVLManager*>::const_iterator it = managers.begin(); it != managers.end(); ++it) {
        it->second->WriteLoader(fstr);
    }
	
	// v5 symbol count per loader
	if (version == 5) {
		unsigned long l = 0;
		unsigned long c = 0;
		for (map<string, ovlOVLManager*>::const_iterator it = managers.begin(); it != managers.end(); ++it) {
			fstr.write(reinterpret_cast<char*>(&l), 4);
			c = it->second->GetCount(type);
			fstr.write(reinterpret_cast<char*>(&c), 4);
			l++;
		}
	}

    // Write the number of files of each type
    for (unsigned int i = 0;i < 9;i++) {
        unsigned long count = types[i].files.size();
        fstr.write(reinterpret_cast<char*>(&count),4);
		if (version > 1) {
			// count repeated
			fstr.write(reinterpret_cast<char*>(&count),4);
			// and sizes
			for (unsigned long j = 0;j < types[i].files.size();j++) {
				fstr.write(reinterpret_cast<char*>(&types[i].files[j]->size),4);
			}
		}
    }
	
	// v4/v5 stuff
	if (version == 4) {
		uint32_t unknown = 0;
		fstr.write(reinterpret_cast<char*>(&unknown), 4);		
		fstr.write(reinterpret_cast<char*>(&unknown), 4);		
	}
	if (version == 5) {
		uint32_t temp = 0;
		int extras = 0;
		foreach(const cLoader& l, loaders) {
			extras += l.getExtraDataV5Infos().size();
		}
		if (extras) {
			// We have extra
			// Calculate and write byte size. 4 bytes for structure count, 14 bytes per structure and 4 bytes for the unknown
			temp = 4 + extras * 14 + 4;
			fstr.write(reinterpret_cast<char*>(&temp), 4);		
			
			// Write structure count
			fstr.write(reinterpret_cast<char*>(&extras), 4);
			
			// Write structures
			foreach(const cLoader& l, loaders) {
				l.writeV5Extras(fstr);
			}
			
			// Write the unknown long, which seems to duplicate the following long count (which we set to 0)
			temp = 0;
			fstr.write(reinterpret_cast<char*>(&temp), 4);			
		} else {
			// Empty byte structure, allways 4/0
			temp = 4;
			fstr.write(reinterpret_cast<char*>(&temp), 4);			
			temp = 0;
			fstr.write(reinterpret_cast<char*>(&temp), 4);			
		}
		// unknown long count, set to 0
		temp = 0;
		fstr.write(reinterpret_cast<char*>(&temp), 4);			
	}

    // Write the files themselves.
    for (unsigned int i = 0;i < 9;i++) {
        for (unsigned long j = 0;j < types[i].files.size();j++) {
			if (version == 1) {
				fstr.write(reinterpret_cast<char*>(&types[i].files[j]->size),4);
			}
            fstr.write(reinterpret_cast<char*>(types[i].files[j]->data), types[i].files[j]->size);
        }
    }

    // Write fixups
    unsigned long size = fixups.size();
    fstr.write(reinterpret_cast<char*>(&size),4);
    foreach(const unsigned long& fixup, fixups) {
        fstr.write(reinterpret_cast<const char*>(&fixup),4);
    }
	
	// v4/v5 stuff (no v5 as we do not support the subversion flag)
	if (version == 4) {
		uint32_t unknown = 0;
		fstr.write(reinterpret_cast<char*>(&unknown), 4);		
	}

    // Write Extradata
    foreach(const cLoader& l, loaders) {
		/*
        fstr.write(reinterpret_cast<const char*>(&chunk->size),4);
        fstr.write(reinterpret_cast<const char*>(chunk->data),chunk->size);
		*/
		l.writeExtraChunks(fstr);
    }

	if (version == 5) {
		uint32_t unknown = 0;
		fstr.write(reinterpret_cast<char*>(&unknown), 4);		
	}

    // Close file
    fstr.close();

}

void cOvlInfo::WriteFiles(const map<string, ovlOVLManager*>& managers, const ovlLodSymRefManager& lsrman) {
    OpenFiles[OVLT_COMMON].WriteStreamed(OVLT_COMMON, managers, lsrman.getLoaders(OVLT_COMMON));
    OpenFiles[OVLT_UNIQUE].WriteStreamed(OVLT_UNIQUE, managers, lsrman.getLoaders(OVLT_UNIQUE));
}
