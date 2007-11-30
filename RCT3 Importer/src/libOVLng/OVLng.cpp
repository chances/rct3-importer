///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// OVL creation class
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


#include "OVLng.h"

#include "OVLDebug.h"
#include "OVLException.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
//  cOVL class
//
////////////////////////////////////////////////////////////////////////////////

cOvl::cOvl(string file) {
    m_file = file;

    string temp = m_file + ".common.ovl";
    m_ovlinfo.OpenFiles[OVL_COMMON].FileName = new char[temp.length()+1];
    strcpy(m_ovlinfo.OpenFiles[OVL_COMMON].FileName, temp.c_str());
    m_ovlinfo.OpenFiles[OVL_COMMON].ReferenceCount=0;
    m_ovlinfo.OpenFiles[OVL_COMMON].References=NULL;
    memset(m_ovlinfo.OpenFiles[OVL_COMMON].Types,0,sizeof(m_ovlinfo.OpenFiles[OVL_COMMON].Types));

    temp = m_file + ".unique.ovl";
    m_ovlinfo.OpenFiles[OVL_UNIQUE].FileName = new char[temp.length()+1];
    strcpy(m_ovlinfo.OpenFiles[OVL_UNIQUE].FileName, temp.c_str());
    memset(m_ovlinfo.OpenFiles[OVL_UNIQUE].Types,0,sizeof(m_ovlinfo.OpenFiles[OVL_UNIQUE].Types));

    m_relmanager.Init(&m_ovlinfo);
    m_lsrmanager.Init(&m_relmanager);
}

cOvl::~cOvl() {
    delete[] m_ovlinfo.OpenFiles[OVL_COMMON].FileName;
    delete[] m_ovlinfo.OpenFiles[OVL_UNIQUE].FileName;
}

ovlOVLManager* cOvl::GetManager(const char* tag) {
    map<string, ovlOVLManager*>::iterator it = m_managers.find(string(tag));
    if (it != m_managers.end())
        return it->second;
    else
        return NULL;
}

void cOvl::InitAndAddManager(ovlOVLManager* man) {
    if (!man)
        throw EOvl("cOvl::AddAndInitManager called with invalid manager");

    man->Init(&m_lsrmanager, &m_relmanager, &m_stringtable);
    m_managers[man->Tag()] = man;
}

void cOvl::AddReference(const char* ref) {
    m_references.push_back(string(ref));
}

void cOvl::Save() {
    // First assign space for the loaders, symobls and symbol references
    m_lsrmanager.Assign();

    // Then make the string table
    m_stringtable.Make();

    // Now make all the managers' contents
    for (map<string, ovlOVLManager*>::iterator it = m_managers.begin(); it != m_managers.end(); ++it) {
        it->second->Make();
    }

    // Now we make the loaders, symobls and symbol references
    {
        map<string, unsigned long> c_loadertypes;
        unsigned long l = 0;
        for (map<string, ovlOVLManager*>::iterator it = m_managers.begin(); it != m_managers.end(); ++it) {
            DUMP_LOG("Assigning loader type %ld to %s", l, it->second->Tag());
            c_loadertypes[string(it->second->Tag())] = l++;
        }

        m_lsrmanager.Make(c_loadertypes);
    }


    ////////////////////////
    // Prepare UNIQUE file
    m_ovlinfo.CurrentFile = OVL_UNIQUE;

    // Type 0 files, ie the string table
    File* c_type0file = new File;
    c_type0file->size = m_stringtable.GetSize();
    c_type0file->reloffset = 0;
    c_type0file->data = reinterpret_cast<unsigned long*>(m_stringtable.Make());
    c_type0file->unk = 0;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[0].count = 1;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[0].dataptr = c_type0file;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[0].reloffset = 0;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[0].size = 0;  // doesn't get written anywhere
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[1].reloffset = c_type0file->reloffset + c_type0file->size;

    // Type 2 files, ie everything else
    File* c_type2files = new File[3 + m_managers.size()]; // managers plus 3 for loaders, symbols and refs
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].size = 0;
    int c_type2n = 0;

    // Symbols
    c_type2files[c_type2n].size = m_lsrmanager.GetSymbolCount() * sizeof(SymbolStruct);
    c_type2files[c_type2n].reloffset = m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[1].reloffset;
    c_type2files[c_type2n].data = reinterpret_cast<unsigned long*>(m_lsrmanager.GetSymbols());
    c_type2files[c_type2n].unk = 0;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].size += c_type2files[c_type2n].size;
    c_type2n++;

    // Loaders
    c_type2files[c_type2n].size = m_lsrmanager.GetLoaderCount() * sizeof(LoaderStruct);
    c_type2files[c_type2n].reloffset = c_type2files[c_type2n-1].reloffset + c_type2files[c_type2n-1].size;
    c_type2files[c_type2n].data = reinterpret_cast<unsigned long*>(m_lsrmanager.GetLoaders());
    c_type2files[c_type2n].unk = 0;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].size += c_type2files[c_type2n].size;
    c_type2n++;

    // Symbol References
    c_type2files[c_type2n].size = m_lsrmanager.GetSymRefCount() * sizeof(SymbolRefStruct);
    c_type2files[c_type2n].reloffset = c_type2files[c_type2n-1].reloffset + c_type2files[c_type2n-1].size;
    c_type2files[c_type2n].data = reinterpret_cast<unsigned long*>(m_lsrmanager.GetSymRefs());
    c_type2files[c_type2n].unk = 0;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].size += c_type2files[c_type2n].size;
    c_type2n++;

    // Other managers
    for (map<string, ovlOVLManager*>::iterator it = m_managers.begin(); it != m_managers.end(); ++it) {
        ovlOVLManager* c_man = it->second;

        c_type2files[c_type2n].size = c_man->GetSize();
        c_type2files[c_type2n].reloffset = c_type2files[c_type2n-1].reloffset + c_type2files[c_type2n-1].size;
        c_type2files[c_type2n].data = reinterpret_cast<unsigned long*>(c_man->GetData());
        c_type2files[c_type2n].unk = 0;
        m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].size += c_type2files[c_type2n].size;
        c_type2n++;
    }

    // Finish type 2 data
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].count = c_type2n;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].dataptr = c_type2files;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].reloffset = c_type2files[m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].count-1].reloffset;

    // Set other data
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[3].reloffset = m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].reloffset;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[4].reloffset = m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].reloffset;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[5].reloffset = m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].reloffset;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[6].reloffset = m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].reloffset;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[7].reloffset = m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].reloffset;
    m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[8].reloffset = m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[2].reloffset;

    // After setting up the file data, we can resolve relocations
    m_relmanager.Make();


    ////////////////////////
    // Write UNIQUE file
    FILE* f = fopen(m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].FileName,"wb");
    if (!f)
        throw EOvl("cOvl::Save: cannot open unique file for writing");

    // Header 1
    OvlHeader h;
    h.magic = 0x4B524746;
    h.References = m_references.size();
    h.reserved = 0;
    h.version = 1;
    fwrite(&h, sizeof(OvlHeader), 1, f);

    // References
    for (unsigned int i = 0;i < m_references.size();i++) {
        unsigned short len = static_cast<unsigned short>(m_references[i].length());
        fwrite(&len,2,1,f);
        fwrite(m_references[i].c_str(),len,1,f);
    }

    // Header 2
    OvlHeader2 h2;
    h2.FileTypeCount = m_managers.size();
    h2.unk = 0;
    fwrite(&h2, sizeof(OvlHeader2), 1, f);

    // Loaders
    for (map<string, ovlOVLManager*>::iterator it = m_managers.begin(); it != m_managers.end(); ++it) {
        it->second->WriteLoader(f);
    }

    // Write the number of files of each type
    for (unsigned int i = 0;i < 9;i++) {
        fwrite(&m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[i].count, sizeof(unsigned long), 1, f);
    }

    // Write the files themselves.
    for (unsigned int i = 0;i < 9;i++) {
        for (unsigned long j = 0;j < m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[i].count;j++) {
            fwrite(&m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[i].dataptr[j].size, sizeof(unsigned long), 1, f);
            fwrite(m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[i].dataptr[j].data, m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].Types[i].dataptr[j].size,1,f);
        }
    }

    // Write fixups
    m_relmanager.WriteFixups(f);

    // Close file
    fclose(f);


    ////////////////////////
    // Write COMMON file
    m_ovlinfo.CurrentFile = OVL_COMMON;
    f = fopen(m_ovlinfo.OpenFiles[m_ovlinfo.CurrentFile].FileName,"wb");
    if (!f)
        throw EOvl("cOvl::Save: cannot open common file for writing");

    // Header 1
    //h.magic = 0x4B524746;
    h.References = 0;
    //h.reserved = 0;
    //h.version = 1;
    fwrite(&h, sizeof(OvlHeader), 1, f);

    // Header 2
    //h2.FileTypeCount = m_managers.size();
    //h2.unk = 0;
    fwrite(&h2, sizeof(OvlHeader2), 1, f);

    // Loaders
    for (map<string, ovlOVLManager*>::iterator it = m_managers.begin(); it != m_managers.end(); ++it) {
        it->second->WriteLoader(f);
    }

    // Write number of files (zero)
    unsigned long num = 0;
    for (unsigned int i = 0;i < 9;i++) {
        fwrite(&num, sizeof(unsigned long), 1, f);
    }

    // Write number of fixups (zero)
    fwrite(&num, sizeof(unsigned long), 1, f);

    // Close file
    fclose(f);


    // Clean Up
    delete c_type0file;
    delete[] c_type2files;

}

