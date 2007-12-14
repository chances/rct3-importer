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

#include "ManagerOVL.h"
#include "OVLException.h"
#include "ovlstructs.h"

using namespace std;

unsigned char* cOvlFileClass::GetBlock(int filetype, unsigned long size) {
    cOvlFile* fi= new cOvlFile(size);
    types[filetype].files.push_back(fi);
    return fi->data;
}

unsigned long cOvlFileClass::MakeRelOffsets(unsigned long from) {
    unsigned long currel = from;
    for (int i = 0; i < 9; ++i) {
        types[i].reloffset = currel;
        types[i].size = 0;
        for (vector<cOvlFile*>::iterator it = types[i].files.begin(); it != types[i].files.end(); ++it) {
            (*it)->reloffset = currel;
            currel += (*it)->size;
            types[i].size += (*it)->size;
        }
    }
    return currel;
}

void cOvlFileClass::Write(const map<string, ovlOVLManager*>& managers, const vector<ovlExtraChunk*>& extra) {
    FILE* f = fopen(filename.c_str(),"wb");
    if (!f)
        throw EOvl("cOvlFileClass::Write: cannot open unique file for writing");

    // Header 1
    OvlHeader h;
    h.magic = 0x4B524746;
    h.References = references.size();
    h.reserved = 0;
    h.version = 1;
    fwrite(&h, sizeof(OvlHeader), 1, f);

    // References
    for (unsigned int i = 0;i < references.size();i++) {
        unsigned short len = static_cast<unsigned short>(references[i].length());
        fwrite(&len,2,1,f);
        fwrite(references[i].c_str(),len,1,f);
    }

    // Header 2
    OvlHeader2 h2;
    h2.FileTypeCount = managers.size();
    h2.unk = 0;
    fwrite(&h2, sizeof(OvlHeader2), 1, f);

    // Loaders
    for (map<string, ovlOVLManager*>::const_iterator it = managers.begin(); it != managers.end(); ++it) {
        it->second->WriteLoader(f);
    }

    // Write the number of files of each type
    for (unsigned int i = 0;i < 9;i++) {
        unsigned long count = types[i].files.size();
        fwrite(&count, sizeof(unsigned long), 1, f);
    }

    // Write the files themselves.
    for (unsigned int i = 0;i < 9;i++) {
        for (unsigned long j = 0;j < types[i].files.size();j++) {
            fwrite(&types[i].files[j]->size, sizeof(unsigned long), 1, f);
            fwrite(types[i].files[j]->data, types[i].files[j]->size, 1, f);
        }
    }

    // Write fixups
    unsigned long size = fixups.size();
    fwrite(&size, sizeof(unsigned long), 1, f);
    while (fixups.empty() == false) {
        unsigned long c_fixup = fixups.front();
        fixups.pop();
        fwrite(&c_fixup, sizeof(unsigned long), 1, f);
    }

    // Write Extradata
    for (vector<ovlExtraChunk*>::const_iterator it = extra.begin(); it != extra.end(); ++it) {
        fwrite(&(*it)->size, sizeof(unsigned long), 1, f);
        fwrite((*it)->data, (*it)->size, 1, f);
    }

    // Close file
    fclose(f);

}

void cOvlInfo::WriteFiles(const map<string, ovlOVLManager*>& managers, const ovlLodSymRefManager& lsrman) {
    OpenFiles[OVLT_COMMON].Write(managers, lsrman.GetExtraChunks(OVLT_COMMON));
    OpenFiles[OVLT_UNIQUE].Write(managers, lsrman.GetExtraChunks(OVLT_UNIQUE));
}
