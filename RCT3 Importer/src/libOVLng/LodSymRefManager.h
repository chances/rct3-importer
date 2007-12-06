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
#include "RelocationManager.h"

using namespace std;

class ovlLodSymRefManager {
private:
    LoaderStruct* m_loaders[2];
    LoaderStruct* m_cloader[2];
    SymbolStruct* m_symbols[2];
    SymbolStruct* m_csymbol[2];
    SymbolRefStruct* m_symrefs[2];
    SymbolRefStruct* m_csymref[2];
    ovlRelocationManager* m_relman;

    vector<string> m_loadernames[2];

    int m_loadercount[2];
    int m_symbolcount[2];
    int m_symrefcount[2];
    int m_uloadercount[2];
    int m_usymbolcount[2];
    int m_usymrefcount[2];

    bool m_made;
    bool m_loaderopen[2];
public:
    ovlLodSymRefManager() {
        m_relman = NULL;
        m_made = false;
        m_loaderopen[0] = false;
        m_loaderopen[1] = false;
    };
    virtual ~ovlLodSymRefManager();

    void Init(ovlRelocationManager* relman);
    void Assign(cOvlInfo* info);
    void Make(const map<string, unsigned long>& loadernumbers);

    void AddLoader(cOvlType type);
    LoaderStruct* OpenLoader(cOvlType type, const char* ctype, unsigned long *data, bool hasextradata, SymbolStruct *sym);
    LoaderStruct* CloseLoader(cOvlType type);
    //LoaderStruct* GetLoaders();
    int GetLoaderCount(cOvlType type) {
        return m_loadercount[type];
    }
    void AddSymbol(cOvlType type);
    SymbolStruct* MakeSymbol(cOvlType type, char *symbol, unsigned long *data, bool ispointer);
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

};

#endif
