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


#ifndef _OVLDUMP_H_
#define _OVLDUMP_H_

#include <map>
#include <string>
#include <vector>

#include "ovlstructs.h"
#include "ovldumperstructs.h"

using namespace std;

class cOVLDump {
protected:
    string m_file[2];
    char* m_data[2];
    char* m_dataend[2];
    unsigned long m_size[2];

    unsigned long* mp_referencecount[2];
    char* mp_references[2];
    vector<string> m_injectreferences[2];

    OvlHeader m_header[2];
    OvlExtendedHeader m_exheader[2];
    vector<string> m_references[2];
    OvlHeader2 m_header2[2];
    vector<OvlLoaderHeader> m_loaderheaders[2];
    OvlFileTypeBlock m_fileblocks[2][9];
    OvlPostBlockUnknowns m_pbunknowns[2];
    vector<OvlRelocation> m_relocations[2];
    vector<OvlSymbol> m_symbols[2];
    vector<OvlLoader> m_loaders[2];
    set<OvlStringTableEntry, OvlStringTableComp> m_strings;

    unsigned long m_reloffset;
    map<unsigned long, OvlRelocation*> m_relmap;
    map<unsigned long, void*> m_targets;
    map<string, map<string, OvlRelocation*> > m_structmap[2];


    void Init();
    void ReadFile(cOvlType type);
    void WriteFile(cOvlType type);
    void ResolveRelocation(const unsigned long relocation, void** target, int& filetype, int& file, unsigned long& block);
    void MakeStrings(cOvlType type);
    void MakeSymbols(cOvlType type);
    void MakeLoaders(cOvlType type);
    void MakeStructureSizes(cOvlType type);
public:
    cOVLDump() {Init();};
    virtual ~cOVLDump();

    void Load(const char* filename);
    void MakeMoreInfo(); // Adds information mostly important for analysing ovls
    void Save(); // Warning! clobbers the original files without further ado!

    unsigned long GetVersion() {
        return m_header[OVLT_COMMON].version;
    }

    string GetFilename(cOvlType type) {
        return m_file[type];
    }

    void InjectReference(cOvlType type, const char* ref) {
        m_injectreferences[type].push_back(ref);
    }

    unsigned long GetBlockCount(cOvlType type, int file);
    unsigned long GetBlockSize(cOvlType type, int file, unsigned long block);
    unsigned long GetBlockOffset(cOvlType type, int file, unsigned long block);

    unsigned long GetSymbolCount(cOvlType type) {
        return m_symbols[type].size();
    }
    const OvlSymbol& GetSymbol(cOvlType type, unsigned long id);
    void SetSymbolData(cOvlType type, unsigned long id, unsigned long data);
    unsigned long FindSymbol(cOvlType type, const char* name);
    vector<unsigned long> FindSymbols(cOvlType type, const char* query);

    unsigned long GetLoaderCount(cOvlType type) {
        return m_loaders[type].size();
    }

    const OvlStringTableEntry& GetString(unsigned long id);
    void SetString(unsigned long id, const char* newstr);

};

#endif
