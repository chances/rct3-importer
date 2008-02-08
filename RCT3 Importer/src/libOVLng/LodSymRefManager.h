///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for Loaders, Symbols and SymbolRefs
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


#ifndef LODSYMREFMANAGER_H_INCLUDED
#define LODSYMREFMANAGER_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "OVLClasses.h"
#include "ovlstructs.h"

class ovlRelocationManager;
class ovlStringTable;

using namespace std;

class ovlExtraChunk {
public:
    unsigned long size;
    unsigned char* data;

    ovlExtraChunk() {
        size = 0;
        data = NULL;
    };
    ovlExtraChunk(unsigned long s, unsigned char* d) {
        size = s;
        data = d;
    };
    ~ovlExtraChunk() {
        delete[] data;
    };
};

class ovlLodSymRefManager {
private:
    LoaderStruct* m_loaders[2];
    LoaderStruct* m_cloader[2];
    SymbolStruct* m_symbols[2];
    SymbolStruct* m_csymbol[2];
    SymbolRefStruct* m_symrefs[2];
    SymbolRefStruct* m_csymref[2];
    map<string, unsigned long> m_npsymbols[2];
    vector<ovlExtraChunk*> m_loaderextras[2];

    vector<string> m_loadernames[2];

    int m_loadercount[2];
    int m_symbolcount[2];
    int m_symrefcount[2];
    int m_uloadercount[2];
    int m_usymbolcount[2];
    int m_usymrefcount[2];

    bool m_assigned;
    bool m_made;
    bool m_loaderopen[2];
    unsigned long m_loaderextracount[2];
    ovlRelocationManager* m_relman;
    ovlStringTable* m_stable;
public:
    ovlLodSymRefManager() {
        m_relman = NULL;
        m_stable = NULL;
        m_assigned = false;
        m_made = false;
        m_loaderopen[0] = false;
        m_loaderopen[1] = false;
        m_loaderextracount[0] = 0;
        m_loaderextracount[1] = 0;
    };
    virtual ~ovlLodSymRefManager();

    void Init(ovlRelocationManager* relman, ovlStringTable* stable);
    void Assign(cOvlInfo* info);
    void Make(const map<string, unsigned long>& loadernumbers);

    void AddLoader(cOvlType type);
    LoaderStruct* OpenLoader(cOvlType type, const char* ctype, unsigned long *data, unsigned long extradatacount, SymbolStruct *sym);
    void AddExtraData(cOvlType type, unsigned long size, unsigned char* data);
    LoaderStruct* CloseLoader(cOvlType type);
    //LoaderStruct* GetLoaders();
    int GetLoaderCount(cOvlType type) {
        return m_loadercount[type];
    }
    void AddSymbol(cOvlType type);
    void AddSymbol(cOvlType type, const char *symbol, unsigned long data);                // Non-pointer symbols
    SymbolStruct* MakeSymbol(cOvlType type, char *symbol, unsigned long *data);     // Pointer symbols
    SymbolStruct* GetSymbols();
    int GetSymbolCount(cOvlType type) {
        return m_symbolcount[type];
    }
    void AddSymRef(cOvlType type);
    SymbolRefStruct* MakeSymRef(cOvlType type, char *symbol, unsigned long *ref);
    SymbolRefStruct* GetSymRefs();
    int GetSymRefCount(cOvlType type) {
        return m_symrefcount[type];
    }

    const vector<ovlExtraChunk*>& GetExtraChunks(cOvlType type) const {
        return m_loaderextras[type];
    }
};

#endif
