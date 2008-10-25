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
#include <string.h>
#include <vector>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/shared_array.hpp>
#include <boost/unordered_map.hpp>

#include "ovlstructs.h"
#include "ovldumperstructs.h"
#include "boost_unordered_extension.h"

#define RelocationFromVar(m, s) \
    ((reinterpret_cast<unsigned long>(&m) + s.reloffset) - reinterpret_cast<unsigned long>(s.data))
	
class cOVLDump {
public:
	typedef boost::unordered_map<std::string, OvlRelocation*, boost::algorithm::ihash, boost::algorithm::iequal_to> symbolmap_t;
	typedef std::pair<std::string, OvlRelocation*> symbolmap_pair_t;
	typedef boost::unordered_map<std::string, symbolmap_t, boost::algorithm::ihash, boost::algorithm::iequal_to> structmap_t;
	typedef std::pair<std::string, symbolmap_t> structmap_pair_t;
protected:
    std::string m_file[2];
    boost::shared_array<char> m_data[2];
    char* m_dataend[2];
    unsigned long m_size[2];

    unsigned long* mp_referencecount[2];
    char* mp_references[2];
    std::vector<std::string> m_injectreferences[2];

    OvlHeader m_header[2];
    OvlExtendedHeader m_exheader[2];
    std::vector<std::string> m_references[2];
    OvlHeader2 m_header2[2];
    std::vector<OvlLoaderHeader> m_loaderheaders[2];
    OvlFileTypeBlock m_fileblocks[2][9];
    OvlPostBlockUnknowns m_pbunknowns[2];
    std::vector<OvlRelocation> m_relocations[2];
    std::vector<OvlSymbol> m_symbols[2];
    std::vector<OvlLoader> m_loaders[2];
    std::set<OvlStringTableEntry, OvlStringTableComp> m_strings;

    unsigned long m_crc[2];

    unsigned long m_reloffset;
    std::map<unsigned long, OvlRelocation*> m_relmap;
    std::map<unsigned long, void*> m_targets;
    std::map<void*, std::string> m_symbolreferences;
    structmap_t m_structmap[2];
    std::map<unsigned long, OvlFlicData> m_flics;


    void Init();
    void ReadFile(cOvlType type);
    void WriteFile(cOvlType type);
    void ResolveRelocation(const unsigned long relocation, void** target, cOvlType& filetype, int& file, unsigned long& block);
    void MakeStrings(cOvlType type);
    void MakeSymbols(cOvlType type);
    void MakeLoaders(cOvlType type);
    void MakeStructureSizes(cOvlType type);
    void MakeFlics(cOvlType type);
    void MakeCRC(cOvlType type);

public:
    cOVLDump() {Init();};
    virtual ~cOVLDump();

    void Load(const char* filename);
    void MakeMoreInfo(); // Adds information mostly important for analysing ovls
    void Save(); // Warning! clobbers the original files without further ado!

    unsigned long GetVersion() {
        return m_header[OVLT_COMMON].version;
    }

    std::string GetFilename(cOvlType type) const {
        return m_file[type];
    }

    unsigned long GetCRC(cOvlType type) {
        return m_crc[type];
    }
    unsigned long calcCRC(unsigned char* pdata, unsigned long len, bool padded);

    void InjectReference(cOvlType type, const char* ref) {
        m_injectreferences[type].push_back(ref);
    }
    const std::vector<std::string>& GetReferences(cOvlType type) const {
        return m_references[type];
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
    std::vector<unsigned long> FindSymbols(cOvlType type, const char* query);

    unsigned long GetLoaderCount(cOvlType type) {
        return m_loaders[type].size();
    }

    const OvlStringTableEntry& GetString(unsigned long id);
    void SetString(unsigned long id, const char* newstr);

    const structmap_t& GetStructures(cOvlType type) const {
        return m_structmap[type];
    }
    std::map<unsigned long, void*>& GetTargets() {
        return m_targets;
    }
    std::map<void*, std::string>& GetSymbolReferences() {
        return m_symbolreferences;
    }

    std::string GetSymbolReference(void* ptr) const;
    std::string GetStringFromVarPtr(void* ptr) const;
    std::string IdentifyVarPtr(void* ptr) const;

    long RelocationFromVarPtr(void* ptr) const;
    bool IsVarPointerRelocated(void* ptr) const;
};

#endif
