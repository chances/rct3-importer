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

#include "ovlstructs.h"
#include "RelocationManager.h"

using namespace std;

class ovlLodSymRefManager {
private:
    LoaderStruct* m_loaders;
    LoaderStruct* m_cloader;
    SymbolStruct* m_symbols;
    SymbolStruct* m_csymbol;
    SymbolRefStruct* m_symrefs;
    SymbolRefStruct* m_csymref;
    ovlRelocationManager* m_relman;

    int m_loadercount;
    int m_symbolcount;
    int m_symrefcount;
    int m_uloadercount;
    int m_usymbolcount;
    int m_usymrefcount;

    bool m_made;
    bool m_loaderopen;
public:
    ovlLodSymRefManager() {
        m_relman = NULL;
        m_made = false;
        m_loaderopen = false;
    };
    virtual ~ovlLodSymRefManager();

    void Init(ovlRelocationManager* relman);
    void Assign();
    void Make();

    void AddLoader();
    LoaderStruct* OpenLoader(unsigned long type, unsigned long *data, bool hasextradata, SymbolStruct *sym);
    LoaderStruct* CloseLoader();
    LoaderStruct* GetLoaders();
    int GetLoaderCount() {
        return m_loadercount;
    }
    void AddSymbol();
    SymbolStruct* MakeSymbol(char *symbol, unsigned long *data, bool ispointer);
    SymbolStruct* GetSymbols();
    int GetSymbolCount() {
        return m_symbolcount;
    }
    void AddSymRef();
    SymbolRefStruct* MakeSymRef(char *symbol, unsigned long *ref);
    SymbolRefStruct* GetSymRefs();
    int GetSymRefCount() {
        return m_symrefcount;
    }

};

#endif
