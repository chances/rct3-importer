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


#include "OVLDump.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "OVLDException.h"

cOVLDump::~cOVLDump() {
    delete m_data[OVLT_COMMON];
    delete m_data[OVLT_UNIQUE];
}

void cOVLDump::Init() {
    m_data[OVLT_COMMON] = NULL;
    m_data[OVLT_UNIQUE] = NULL;
}

void cOVLDump::Load(const char* filename) {
    {
        string fn = filename;
        m_file[OVLT_COMMON] = fn;
        int r = fn.rfind(".common.", fn.size());
        if (r == string::npos)
            throw EOvlD("Filename passed to Load must include the 'common.ovl' extension.");
        fn.replace(r, 8, ".unique.");
        m_file[OVLT_UNIQUE] = fn;
    }

    // Read files. Order matters due to the reloffset
    m_reloffset = 0;
    ReadFile(OVLT_COMMON);
    ReadFile(OVLT_UNIQUE);

    // Resolve relocations
    for (std::vector<OvlRelocation>::iterator itr = m_relocations[OVL_COMMON].begin(); itr != m_relocations[OVL_COMMON].end(); ++itr) {
        ResolveRelocation(itr->relocation, reinterpret_cast<void**>(&itr->relocationsite), itr->r_filetype, itr->r_file, itr->r_block);
        if (itr->relocationsite) {
            m_fileblocks[itr->r_filetype][itr->r_file].blocks[itr->r_block].relocs.insert(&(*itr));
            m_relmap[itr->relocation] = &(*itr);
            itr->targetrelocation = *(itr->relocationsite);
            ResolveRelocation(itr->targetrelocation, &itr->target, itr->t_filetype, itr->t_file, itr->t_block);
            if (itr->target) {
                m_fileblocks[itr->t_filetype][itr->t_file].blocks[itr->t_block].targets.insert(&(*itr));
                //m_relmap[itr->targetrelocation] = &(*itr);
                m_targets[itr->targetrelocation] = itr->target;
            }
        } else {
            itr->target = NULL;
        }
    }
    for (std::vector<OvlRelocation>::iterator itr = m_relocations[OVL_UNIQUE].begin(); itr != m_relocations[OVL_UNIQUE].end(); ++itr) {
        ResolveRelocation(itr->relocation, reinterpret_cast<void**>(&itr->relocationsite), itr->r_filetype, itr->r_file, itr->r_block);
        if (itr->relocationsite) {
            m_fileblocks[itr->r_filetype][itr->r_file].blocks[itr->r_block].relocs.insert(&(*itr));
            m_relmap[itr->relocation] = &(*itr);
            itr->targetrelocation = *(itr->relocationsite);
            ResolveRelocation(itr->targetrelocation, &itr->target, itr->t_filetype, itr->t_file, itr->t_block);
            if (itr->target) {
                m_fileblocks[itr->t_filetype][itr->t_file].blocks[itr->t_block].targets.insert(&(*itr));
                //m_relmap[itr->targetrelocation] = &(*itr);
                m_targets[itr->targetrelocation] = itr->target;
            }
        } else {
            itr->target = NULL;
        }
    }

    // Parse StringTable
    MakeStrings(OVLT_COMMON);
    MakeStrings(OVLT_UNIQUE);

    // Parse Symbols
    MakeSymbols(OVLT_COMMON);
    MakeSymbols(OVLT_UNIQUE);

    // Parse Loaders
    MakeLoaders(OVLT_COMMON);
    MakeLoaders(OVLT_UNIQUE);

}

void cOVLDump::MakeMoreInfo() {
    // Try to deduce structure sizes
    MakeStructureSizes(OVLT_COMMON);
    MakeStructureSizes(OVLT_UNIQUE);
    MakeFlics(OVLT_COMMON);
    MakeFlics(OVLT_UNIQUE);
}

void cOVLDump::Save() {
    WriteFile(OVLT_COMMON);
    WriteFile(OVLT_UNIQUE);
}

void cOVLDump::WriteFile(cOvlType type) {
    ofstream f(m_file[type].c_str(), ios::binary | ios::out);
    if (m_injectreferences[type].size()) {
        *mp_referencecount[type] += m_injectreferences[type].size();
        unsigned long presize = mp_references[type] - m_data[type];
        f.write(m_data[type], presize);
        for (vector<string>::iterator it = m_injectreferences[type].begin(); it != m_injectreferences[type].end(); ++it) {
            unsigned short len = it->size();
            f.write(reinterpret_cast<char*>(&len), 2);
            f.write(it->c_str(), len);
        }
        f.write(mp_references[type], m_size[type] - presize);
    } else {
        f.write(m_data[type], m_size[type]);
    }
}

void cOVLDump::ReadFile(cOvlType type) {
    {
//        wxFile f(m_file[type].GetFullPath().fn_str());
//        if (!f.IsOpened())
//            throw EOvlD(_("Could not open ")+m_file[type].GetFullPath());
        ifstream f(m_file[type].c_str(), ios::ate | ios::in | ios::binary);
        m_size[type] = f.tellg();
        f.seekg(0, ios::beg);
        m_data[type] = new char[m_size[type]];
        f.read(m_data[type], m_size[type]);
    }

    // Header
    char* c_data = m_data[type];
    m_header[type] = *reinterpret_cast<OvlHeader*>(c_data);
    c_data += sizeof(OvlHeader);

    // Extra
    switch (m_header[type].version) {
        case 1: {
                m_exheader[type].ReferencesE = m_header[type].References;
                mp_referencecount[type] = &(reinterpret_cast<OvlHeader*>(m_data[type]))->References;
            }
            break;
        case 4: {
                mp_referencecount[type] = reinterpret_cast<unsigned long*>(c_data);
                m_exheader[type].ReferencesE = *reinterpret_cast<unsigned long*>(c_data);
                c_data += 4;
            }
            break;
        case 5: {
                m_exheader[type].unknownv5_count = *reinterpret_cast<unsigned long*>(c_data);
                c_data += 4;
                if (m_exheader[type].unknownv5_count) {
                    m_exheader[type].unknownv5_1 = *reinterpret_cast<unsigned long*>(c_data);
                    c_data += 4;
                    for (int i = 0; i < m_exheader[type].unknownv5_count; ++i) {
                        unsigned long t = *reinterpret_cast<unsigned long*>(c_data);
                        c_data += 4;
                        m_exheader[type].unknownv5_list.push_back(t);
                    }

                    // There seems to be another long
                    m_exheader[type].unknownv5_2 = *reinterpret_cast<unsigned long*>(c_data);
                    c_data += 4;

                    // V5 Unknown Bytes
                    unsigned char c;
                    unsigned int i = 0;
                    do {
                        c = *reinterpret_cast<unsigned char*>(c_data);
                        c_data++;
                        i++;
                        if (i == 4) {
                            i = 0;
                        }
                        m_exheader[type].unknownv5_bytes.push_back(c);
                    } while (c != 0);
                    //V5 Unknown Bytes Padding (probably zero)
                    for (unsigned int j = i; j < 4; j++) {
                        c = *reinterpret_cast<unsigned char*>(c_data);
                        c_data++;
                        m_exheader[type].unknownv5_padding.push_back(c);
                    }
                }
                mp_referencecount[type] = reinterpret_cast<unsigned long*>(c_data);
                m_exheader[type].ReferencesE = *reinterpret_cast<unsigned long*>(c_data);
                c_data += 4;
            }
            break;
        default: {
                stringstream s;
                s << "Unknown version " << m_header[type].version;
                throw EOvlD(s.str());
            }
    }

    // References
    mp_references[type] = c_data;
    for (int i = 0; i < m_exheader[type].ReferencesE; ++i) {
        if (c_data >= m_data[type] + m_size[type] - 1) {
            //return;
            throw EOvlD("File Overflow during refs");
        }

        unsigned short c_len = *reinterpret_cast<unsigned short*>(c_data);
        c_data += 2;
        char* temp = new char[c_len+1];
        temp[c_len] = 0;
        strncpy(temp, c_data, c_len);
        c_data += c_len;
        m_references[type].push_back(string(temp));
        delete[] temp;
    }

    // Header 2
    m_header2[type] = *reinterpret_cast<OvlHeader2*>(c_data);
    c_data += sizeof(OvlHeader2);

    // Loader Headers
    for (int i = 0; i < m_header2[type].FileTypeCount; ++i) {
        OvlLoaderHeader h;
        unsigned short c_len = *reinterpret_cast<unsigned short*>(c_data);
        c_data += 2;
        char* temp = new char[c_len+1];
        temp[c_len] = 0;
        strncpy(temp, c_data, c_len);
        c_data += c_len;
        h.loader = string(temp);
        delete[] temp;

        c_len = *reinterpret_cast<unsigned short*>(c_data);
        c_data += 2;
        temp = new char[c_len+1];
        temp[c_len] = 0;
        strncpy(temp, c_data, c_len);
        c_data += c_len;
        h.name = string(temp);
        delete[] temp;

        h.type = *reinterpret_cast<unsigned long*>(c_data);
        c_data += 4;

        c_len = *reinterpret_cast<unsigned short*>(c_data);
        c_data += 2;
        temp = new char[c_len+1];
        temp[c_len] = 0;
        strncpy(temp, c_data, c_len);
        c_data += c_len;
        h.tag = string(temp);
        delete[] temp;

        m_loaderheaders[type].push_back(h);
    }

    // V5 Loader Header stuff
    if (m_header[type].version == 5) {
        for (int i = 0; i < m_header2[type].FileTypeCount; ++i) {
            m_loaderheaders[type][i].unknownv5[0] = *reinterpret_cast<unsigned long*>(c_data);
            c_data += 4;
            m_loaderheaders[type][i].unknownv5[1] = *reinterpret_cast<unsigned long*>(c_data);
            c_data += 4;
        }
    }

    // Parse 9 blocks for count and unknown stuff
    for (int i = 0; i < 9; ++i) {
        // Extended header unknown count unknowns
        if (m_header[type].version == 5) {
            if (m_exheader[type].unknownv5_count) {
                for (int i = 0; i < m_exheader[type].unknownv5_count; ++i) {
                    unsigned long t = *reinterpret_cast<unsigned long*>(c_data);
                    c_data += 4;
                    m_fileblocks[type][i].unknownv5_list.push_back(t);
                }
            }
        }

        // Count
        m_fileblocks[type][i].count = *reinterpret_cast<unsigned long*>(c_data);
        c_data += 4;
        if (m_fileblocks[type][i].count) {
            m_fileblocks[type][i].blocks.resize(m_fileblocks[type][i].count);
        }
        m_fileblocks[type][i].size = 0;

        if (m_header[type].version > 1) {
            // Unknown
            m_fileblocks[type][i].unknownv4v5_1 = *reinterpret_cast<unsigned long*>(c_data);
            c_data += 4;

            // Sizes
            for (unsigned long m = 0; m < m_fileblocks[type][i].count; ++m) {
                m_fileblocks[type][i].blocks[m].size =  *reinterpret_cast<unsigned long*>(c_data);
                c_data += 4;
                m_fileblocks[type][i].size += m_fileblocks[type][i].blocks[m].size;
            }
        }
    }

    // Unknown stuff
    if (m_header[type].version == 4) {
        m_pbunknowns[type].unknownv4[0] = *reinterpret_cast<unsigned long*>(c_data);
        c_data += 4;
        m_pbunknowns[type].unknownv4[1] = *reinterpret_cast<unsigned long*>(c_data);
        c_data += 4;
    }
    if (m_header[type].version == 5) {
        unsigned long s = *reinterpret_cast<unsigned long*>(c_data);
        c_data += 4;
        for (unsigned long i = 0; i < s; ++i) {
            m_pbunknowns[type].unknownv5_bytes.push_back(*c_data);
            c_data++;
        }
        s = *reinterpret_cast<unsigned long*>(c_data);
        c_data += 4;
        for (unsigned long i = 0; i < s; ++i) {
            m_pbunknowns[type].unknownv5_longs.push_back(*reinterpret_cast<unsigned long*>(c_data));
            c_data += 4;
        }
    }

    // 'Read' Data
    for (int i = 0; i < 9; ++i) {
        m_fileblocks[type][i].reloffset = m_reloffset;
        for (unsigned long m = 0; m < m_fileblocks[type][i].count; ++m) {
            if (c_data >= m_data[type] + m_size[type]) {
                //return;
                stringstream ss;
                ss << "File Overflow in " << i << ", " << m;
                throw EOvlD(ss.str());
            }
            if (m_header[type].version == 1) {
                // Read size
                m_fileblocks[type][i].blocks[m].size =  *reinterpret_cast<unsigned long*>(c_data);
                c_data += 4;
                m_fileblocks[type][i].size += m_fileblocks[type][i].blocks[m].size;
            }
            if (c_data < m_data[type] + m_size[type]) {
                m_fileblocks[type][i].blocks[m].reloffset = m_reloffset;
                m_reloffset += m_fileblocks[type][i].blocks[m].size;
                m_fileblocks[type][i].blocks[m].fileoffset = c_data - m_data[type];
                if (m_fileblocks[type][i].blocks[m].size) {
                    m_fileblocks[type][i].blocks[m].data = reinterpret_cast<unsigned char*>(c_data);
                    c_data += m_fileblocks[type][i].blocks[m].size;
                } else {
                    m_fileblocks[type][i].blocks[m].data = NULL;
                }
            }
        }
    }

    // Relocations
    unsigned long rels = *reinterpret_cast<unsigned long*>(c_data);
    c_data += 4;
    for (unsigned long i = 0; i < rels; ++i) {
        OvlRelocation r;
        r.filetype = type;
        r.relocation = *reinterpret_cast<unsigned long*>(c_data);
        c_data += 4;
        m_relocations[type].push_back(r);
    }

    // Post Relocation unkonwns (this is assumption)
    if (m_header[type].version == 5) {
        for (unsigned long i = 0; i < m_exheader[type].unknownv5_count; ++i) {
            m_pbunknowns[type].unknownv5_postrelocationlongs.push_back(*reinterpret_cast<unsigned long*>(c_data));
            c_data += 4;
        }
    } else if (m_header[type].version == 4) {
        m_pbunknowns[type].unknownv4_postrelocationlong = *reinterpret_cast<unsigned long*>(c_data);
        c_data += 4;
    }

    m_dataend[type] = c_data;
}

void cOVLDump::ResolveRelocation(const unsigned long relocation, void** target, cOvlType& filetype, int& file, unsigned long& block) {
    *target = NULL;
    if (relocation < m_fileblocks[OVLT_UNIQUE][0].reloffset) {
        // In common
        filetype = OVLT_COMMON;
    } else {
        // In unique
        filetype = OVLT_UNIQUE;
    }
    for (int i = 0; i < 9; ++i) {
        if (relocation >= m_fileblocks[filetype][i].reloffset)
            file = i;
    }
    for (unsigned long i = 0; i < m_fileblocks[filetype][file].count; ++i) {
        if ((relocation >= m_fileblocks[filetype][file].blocks[i].reloffset) &&
            (relocation < m_fileblocks[filetype][file].blocks[i].reloffset + m_fileblocks[filetype][file].blocks[i].size)) {
            block = i;
            *target = m_fileblocks[filetype][file].blocks[i].data + (relocation - m_fileblocks[filetype][file].blocks[i].reloffset);
            return;
        }
    }
}

void cOVLDump::MakeStrings(cOvlType type) {
    if (!m_fileblocks[type][0].blocks.size())
        return;
    if (!m_fileblocks[type][0].blocks[0].size)
        return;

    for (std::set<OvlRelocation*>::iterator it = m_fileblocks[type][0].blocks[0].targets.begin(); it != m_fileblocks[type][0].blocks[0].targets.end(); ++it) {
        (*it)->t_data = string(reinterpret_cast<char*>((*it)->target));
        (*it)->t_usedfor = string("String: ") + (*it)->t_data;
    }

    char* c_data = reinterpret_cast<char*>(m_fileblocks[type][0].blocks[0].data);
    while (c_data < reinterpret_cast<char*>(m_fileblocks[type][0].blocks[0].data) + m_fileblocks[type][0].blocks[0].size) {
        OvlStringTableEntry str;
        str.entry = c_data;
        str.orgstring = c_data;
        str.orglength = strlen(str.orgstring);
        unsigned long ofs = reinterpret_cast<unsigned long>(c_data) - reinterpret_cast<unsigned long>(m_fileblocks[type][0].blocks[0].data);
        str.reloffset = m_fileblocks[type][0].blocks[0].reloffset + ofs;
        str.fileoffset = m_fileblocks[type][0].blocks[0].fileoffset + ofs;
        m_strings.insert(str);
        c_data += str.entry.size()+1;
    }

}


//inline unsigned long CalcOffset(char** member, char* source, unsigned long reloffset) {
//    return (reinterpret_cast<unsigned long>(member) * reloffset) - reinterpret_cast<unsigned long>(source);
//}

void cOVLDump::MakeSymbols(cOvlType type) {
    if (!m_fileblocks[type][2].blocks.size())
        return;
    if (!m_fileblocks[type][2].blocks[0].size)
        return;

    if (m_header[type].version == 1) {
        SymbolStruct* syms = reinterpret_cast<SymbolStruct*>(m_fileblocks[type][2].blocks[0].data);
        unsigned long symcount = m_fileblocks[type][2].blocks[0].size / sizeof(SymbolStruct);
        for (unsigned long s = 0; s < symcount; ++s) {
            OvlSymbol sy;
            sy.symbol = m_relmap[RelocationFromVar(syms[s].Symbol, m_fileblocks[type][2].blocks[0])];
            sy.symbol->r_inwhat = string("SymbolStruct(") + sy.symbol->t_data + ")->Symbol";
            sy.IsPointer = syms[s].IsPointer;
            if (sy.IsPointer) {
                sy.data = m_relmap[RelocationFromVar(syms[s].data, m_fileblocks[type][2].blocks[0])];
                sy.data->r_inwhat = string("SymbolStruct(") + sy.symbol->t_data + ")->data";
            } else {
                sy.data = reinterpret_cast<OvlRelocation*>(syms[s].data);
            }
            sy.orgsymbol = &syms[s];
            m_symbols[type].push_back(sy);
        }
    } else {
        SymbolStruct2* syms = reinterpret_cast<SymbolStruct2*>(m_fileblocks[type][2].blocks[0].data);
        unsigned long symcount = m_fileblocks[type][2].blocks[0].size / sizeof(SymbolStruct2);
        for (unsigned long s = 0; s < symcount; ++s) {
            OvlSymbol sy;
//            sy.symbol = reinterpret_cast<char*>(m_relmap[reinterpret_cast<unsigned long>(syms[s].Symbol)]->target);
//            sy.data = m_relmap[reinterpret_cast<unsigned long>(syms[s].data)];
            sy.symbol = m_relmap[RelocationFromVar(syms[s].Symbol, m_fileblocks[type][2].blocks[0])];
            sy.symbol->r_inwhat = string("SymbolStruct2(") + sy.symbol->t_data + ")->Symbol";
            sy.IsPointer = syms[s].IsPointer;
            if (sy.IsPointer) {
                sy.data = m_relmap[RelocationFromVar(syms[s].data, m_fileblocks[type][2].blocks[0])];
                if (sy.data)
                    sy.data->r_inwhat = string("SymbolStruct2(") + sy.symbol->t_data + ")->data";
            } else {
                sy.data = reinterpret_cast<OvlRelocation*>(syms[s].data);
            }
            sy.unknown = syms[s].unknown;
            sy.checksum = syms[s].Checksum;
            sy.orgsymbol = reinterpret_cast<SymbolStruct*>(&syms[s]);
            m_symbols[type].push_back(sy);
        }
    }
}

void cOVLDump::MakeLoaders(cOvlType type) {
    if (m_fileblocks[type][2].blocks.size()<=1)
        return;
    if (!m_fileblocks[type][2].blocks[1].size)
        return;

    bool symres = false;

    LoaderStruct* lods = reinterpret_cast<LoaderStruct*>(m_fileblocks[type][2].blocks[1].data);
    unsigned long lodcount = m_fileblocks[type][2].blocks[1].size / sizeof(LoaderStruct);
    for (unsigned long l = 0; l < lodcount; ++l) {
        OvlLoader lo;
        lo.loadertype = lods[l].LoaderType;
        lo.name = m_loaderheaders[type][lo.loadertype].name;
//        lo.data = m_relmap[reinterpret_cast<unsigned long>(lods[l].data)];
        lo.data = m_relmap[RelocationFromVar(lods[l].data, m_fileblocks[type][2].blocks[1])];
        lo.symbol = m_relmap[RelocationFromVar(lods[l].Sym, m_fileblocks[type][2].blocks[1])];
        if (m_header[type].version == 5) {
            lo.hasextradata = *reinterpret_cast<unsigned short*>(&lods[l].HasExtraData);
            lo.unknownv5 = *(reinterpret_cast<unsigned short*>(&lods[l].HasExtraData)+1);
        } else {
            lo.hasextradata = lods[l].HasExtraData;
        }
        lo.symbolstoresolve = lods[l].SymbolsToResolve;
        if (lo.symbolstoresolve)
            symres = true;

//        SymbolStruct* sym = reinterpret_cast<SymbolStruct*>(m_relmap[reinterpret_cast<unsigned long>(lods[l].Sym)]->target);
        if (lo.symbol) {
            SymbolStruct* sym = reinterpret_cast<SymbolStruct*>(lo.symbol->target);
            lo.symbolname = reinterpret_cast<char*>(m_targets[reinterpret_cast<unsigned long>(sym->Symbol)]);
            lo.data->r_inwhat = string("LoaderStruct(") + lo.symbolname + ")->data";
            m_structmap[type][m_loaderheaders[type][lo.loadertype].tag][lo.symbolname] = lo.data;
        } else {
            lo.symbolname = "No Symbol";
        }

        for (int i = 0; i < lo.hasextradata; ++i) {
            if (m_dataend[type] < m_data[type] + m_size[type]) {
                OvlExtraChuck ch;
                ch.fileoffset = reinterpret_cast<unsigned long>(m_dataend[type]);
                ch.size = *reinterpret_cast<unsigned long*>(m_dataend[type]);
                m_dataend[type] += 4;
                ch.data = reinterpret_cast<unsigned char*>(m_dataend[type]);
                m_dataend[type] += ch.size;
                lo.extradata.push_back(ch);
            }
        }

        m_loaders[type].push_back(lo);
    }

    if (symres) {
        if (m_header[type].version == 1) {
            SymbolRefStruct* symrefs = reinterpret_cast<SymbolRefStruct*>(m_fileblocks[type][2].blocks[2].data);
            unsigned long symrefcount = m_fileblocks[type][2].blocks[2].size / sizeof(SymbolRefStruct);
            for (unsigned long s = 0; s < symrefcount; ++s) {
                OvlRelocation* rel = m_relmap[RelocationFromVar(symrefs[s].reference, m_fileblocks[type][2].blocks[2])];
                char* name = reinterpret_cast<char*>(m_targets[reinterpret_cast<unsigned long>(symrefs[s].Symbol)]);
                rel->t_issymref = true;
                rel->r_inwhat = string("SymbolReference(") + name + ")";
                rel->t_usedfor = string("|- ") + name;
                m_symbolreferences[rel->target] = name;
            }
        } else {
            SymbolRefStruct2* symrefs = reinterpret_cast<SymbolRefStruct2*>(m_fileblocks[type][2].blocks[2].data);
            unsigned long symrefcount = m_fileblocks[type][2].blocks[2].size / sizeof(SymbolRefStruct2);
            for (unsigned long s = 0; s < symrefcount; ++s) {
                OvlRelocation* rel = m_relmap[RelocationFromVar(symrefs[s].reference, m_fileblocks[type][2].blocks[2])];
                char* name = reinterpret_cast<char*>(m_targets[reinterpret_cast<unsigned long>(symrefs[s].Symbol)]);
                rel->t_issymref = true;
                rel->r_inwhat = string("SymbolReference(") + name + ")";
                rel->t_usedfor = string("|- ") + name;
                m_symbolreferences[rel->target] = name;
            }
        }
    }
}

void cOVLDump::MakeStructureSizes(cOvlType type) {
    for (unsigned int i = 0; i < 9; ++i) {
        for (vector<OvlFileBlock>::iterator i_bl = m_fileblocks[type][i].blocks.begin(); i_bl != m_fileblocks[type][i].blocks.end(); ++i_bl) {
            set<OvlRelocation*>::iterator i_old = i_bl->targets.begin();
            for (set<OvlRelocation*>::iterator i_reloc = i_bl->targets.begin(); i_reloc != i_bl->targets.end(); ++i_reloc) {
                if (i_reloc != i_bl->targets.begin()) {
                    if ((*i_reloc)->targetrelocation == (*i_old)->targetrelocation) {
                        (*i_reloc)->t_apparentlength = (*i_old)->t_apparentlength;
                        continue;
                    }
                }

                for (set<OvlRelocation*>::iterator i_r = i_reloc; i_r != i_bl->targets.end(); ++i_r) {
                    if ((!(*i_r)->t_issymref) && ((*i_r)->targetrelocation != (*i_reloc)->targetrelocation)) {
                        (*i_reloc)->t_apparentlength = (*i_r)->targetrelocation - (*i_reloc)->targetrelocation;
                        break;
                    }
                }
                if (!(*i_reloc)->t_apparentlength) {
                    (*i_reloc)->t_apparentlength = i_bl->size - ((*i_reloc)->targetrelocation - i_bl->reloffset);
                }

                i_old = i_reloc;
            }
        }
    }
}

void cOVLDump::MakeFlics(cOvlType type) {
    unsigned long bmptablecount = 0;
    FlicHeader* bmptableheaders = NULL;
    unsigned char* bmptabledata = NULL;

    unsigned long type_flic = -1;
    unsigned long type_btbl = -1;
    for (unsigned long i = 0; i < m_loaderheaders[type].size(); ++i) {
        if (m_loaderheaders[type][i].tag == "btbl")
            type_btbl = i;
        if (m_loaderheaders[type][i].tag == "flic")
            type_flic = i;
    }
    if (type_flic == -1)
        return;
    unsigned long c_flic = 0;
    long bmptbl_index = -1;
    unsigned long flic_index = 0;
    map<string, map<string, OvlRelocation*> >::iterator i_tagC = m_structmap[OVLT_COMMON].find("tex");
    map<string, map<string, OvlRelocation*> >::iterator i_tagU = m_structmap[OVLT_UNIQUE].find("tex");
    vector<unsigned char *> datas;
    for (std::vector<OvlLoader>::iterator it = m_loaders[type].begin(); it != m_loaders[type].end(); ++it) {
        if (it->loadertype == type_btbl) {
            BmpTbl* btp = reinterpret_cast<BmpTbl*>(it->data->target);
            bmptablecount = btp->count;
            unsigned char* tmphead = it->extradata[0].data;
            tmphead += 8;
            bmptableheaders = reinterpret_cast<FlicHeader*>(tmphead);
            bmptabledata = it->extradata[1].data;
            bmptbl_index++;
            flic_index = 0;
            datas.clear();
        }
        if (it->loadertype == type_flic) {
            OvlFlicData fd;
            fd.bmptable = bmptbl_index;
            fd.index = flic_index;
            flic_index++;

            // Find texture
            if (i_tagC != m_structmap[OVLT_COMMON].end()) {
                for (map<string, OvlRelocation*>::iterator i_tex = i_tagC->second.begin(); i_tex != i_tagC->second.end(); ++i_tex) {
                    TextureStruct* ts = reinterpret_cast<TextureStruct*>(i_tex->second->target);
                    if (ts->unk9) {
                        TextureStruct2* ts2 = reinterpret_cast<TextureStruct2*>(m_targets[reinterpret_cast<unsigned long>(ts->ts2)]);
                        if (it->data->targetrelocation == reinterpret_cast<unsigned long>(ts2->Flic)) {
                            fd.texture = i_tex->first;
                        }
                    }
                }
            }
            if (i_tagU != m_structmap[OVLT_UNIQUE].end()) {
                for (map<string, OvlRelocation*>::iterator i_tex = i_tagU->second.begin(); i_tex != i_tagU->second.end(); ++i_tex) {
                    TextureStruct* ts = reinterpret_cast<TextureStruct*>(i_tex->second->target);
                    if (ts->unk9) {
                        TextureStruct2* ts2 = reinterpret_cast<TextureStruct2*>(m_targets[reinterpret_cast<unsigned long>(ts->ts2)]);
                        if (it->data->targetrelocation == reinterpret_cast<unsigned long>(ts2->Flic)) {
                            fd.texture = i_tex->first;
                        }
                    }
                }
            }
            if (bmptablecount) {
                fd.bmpindex = *reinterpret_cast<unsigned long*>(it->extradata[0].data);
                fd.fh = bmptableheaders[fd.bmpindex];
                if (fd.bmpindex != datas.size()) {
                    fd.data = datas[fd.bmpindex];
                } else {
                    datas.push_back(bmptabledata);
                    fd.data = bmptabledata;
                    unsigned long w = fd.fh.Width;
                    unsigned long h = fd.fh.Height;
                    for (unsigned long i = 0; i < fd.fh.Mipcount; ++i) {
                        switch (fd.fh.Format) {
                            case 0x02: // 32-bit RGBA
                                bmptabledata += w * h * 4;
                                break;
                            case 0x12: // Dxt1
                                bmptabledata += ((w * h)>=16)?((w * h) / 2):8;
                                break;
                            case 0x13: // Dxt3
                            case 0x14: // Dxt5
                                bmptabledata += ((w * h)>=16)?(w * h):16;
                                break;
                            default: {
                                stringstream str;
                                str << "Unknown texture format: " << fd.fh.Format;
                                throw EOvlD(str.str());
                            }
                        }
                        w = w / 2;
                        h = h / 2;
    //                    if ((w == 2) || (h == 2))
    //                        break;
                    }
                }

                m_flics[it->data->targetrelocation] = fd;
            } else {
                unsigned char* tmpdata = it->extradata[0].data;
                fd.fh = *reinterpret_cast<FlicHeader*>(tmpdata);
                tmpdata += sizeof(FlicHeader);
                tmpdata += sizeof(FlicMipHeader);
                fd.data = tmpdata;

                m_flics[it->data->targetrelocation] = fd;
            }
        }
    }
}


unsigned long cOVLDump::GetBlockCount(cOvlType type, int file) {
    return m_fileblocks[type][file].count;
}

unsigned long cOVLDump::GetBlockSize(cOvlType type, int file, unsigned long block) {
    if (block >= m_fileblocks[type][file].blocks.size())
        throw EOvlD("GetBlockSize called with illegal block");

    return m_fileblocks[type][file].blocks[block].size;
}

unsigned long cOVLDump::GetBlockOffset(cOvlType type, int file, unsigned long block) {
    if (block >= m_fileblocks[type][file].blocks.size())
        throw EOvlD("GetBlockOffset called with illegal block");

    return m_fileblocks[type][file].blocks[block].reloffset;
}

const OvlSymbol& cOVLDump::GetSymbol(cOvlType type, unsigned long id) {
    if (id >= m_symbols[type].size())
        throw EOvlD("GetSymbol called with illegal id");

    return m_symbols[type][id];
}

void cOVLDump::SetSymbolData(cOvlType type, unsigned long id, unsigned long data) {
    if (id >= m_symbols[type].size())
        throw EOvlD("SetSymbolData called with illegal id");

    m_symbols[type][id].SetData(data);
}

unsigned long cOVLDump::FindSymbol(cOvlType type, const char* name) {
    if (!name)
        throw EOvlD("SetSymbolData called without name");

    for (unsigned long i = 0; i < m_symbols[type].size(); ++i) {
        if (m_symbols[type][i].symbol->t_data == name) {
            return i;
        }
    }
    throw EOvlD(string("SetSymbolData: failed to find ")+name);
}

vector<unsigned long> cOVLDump::FindSymbols(cOvlType type, const char* query) {
    if (!query)
        throw EOvlD("SetSymbolData called without name");

    vector<unsigned long> r;
    for (unsigned long i = 0; i < m_symbols[type].size(); ++i) {
        unsigned int pos = m_symbols[type][i].symbol->t_data.find(query, 0);
        if (pos != string::npos) {
            r.push_back(i);
        }
    }
    return r;
}

const OvlStringTableEntry& cOVLDump::GetString(unsigned long id) {
    if (id >= m_strings.size())
        throw EOvlD("GetString called with illegal id");

    unsigned long i = 0;
    for (set<OvlStringTableEntry>::iterator it = m_strings.begin(); it != m_strings.end(); ++it) {
        if (i == id) {
            return *it;
        }
        i++;
    }
}

void cOVLDump::SetString(unsigned long id, const char* newstr) {
    if (id >= m_strings.size())
        throw EOvlD("SetString called with illegal id");

    unsigned long i = 0;
    for (set<OvlStringTableEntry>::iterator it = m_strings.begin(); it != m_strings.end(); ++it) {
        if (i == id) {
            it->SetString(newstr);
            return;
        }
        i++;
    }
}


