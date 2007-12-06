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


#include "LodSymRefManager.h"

#include "OVLDebug.h"
#include "OVLException.h"

int comparesymbols(const void *s1,const void *s2) {
	return stricmp(((SymbolStruct *)s1)->Symbol,((SymbolStruct *)s2)->Symbol);
}

ovlLodSymRefManager::~ovlLodSymRefManager() {
    /*
    for (int i = 0; i < 2; ++i) {
        if (m_loaders[i])
            delete[] m_loaders[i];
        if (m_symbols[i])
            delete[] m_symbols[i];
        if (m_symrefs[i])
            delete[] m_symrefs[i];
    }
    */
}

void ovlLodSymRefManager::Init(ovlRelocationManager* relman) {
    m_relman = relman;
    for (int i = 0; i < 2; ++i) {
        m_loaders[i] = NULL;
        m_cloader[i] = NULL;
        m_symbols[i] = NULL;
        m_csymbol[i] = NULL;
        m_symrefs[i] = NULL;
        m_csymref[i] = NULL;
        m_loadercount[i] = 0;
        m_symbolcount[i] = 0;
        m_symrefcount[i] = 0;
    }
}

void ovlLodSymRefManager::Assign(cOvlInfo* info) {
    if (m_loaders[0] || m_symbols[0] || m_symrefs[0] || m_loaders[1] || m_symbols[1] || m_symrefs[1])
        throw EOvl("ovlLodSymRefManager::Assign called twice");
    if (!info)
        throw EOvl("ovlLodSymRefManager::Assign called without valid info");


    for (int i = 0; i < 2; ++i) {
        //m_symbols = new SymbolStruct[m_symbolcount];
        m_symbols[i] = reinterpret_cast<SymbolStruct*>(info->OpenFiles[i].GetBlock(2, m_symbolcount[i]*sizeof(SymbolStruct)));
        m_csymbol[i] = m_symbols[i];

        //m_loaders = new LoaderStruct[m_loadercount];
        m_loaders[i] = reinterpret_cast<LoaderStruct*>(info->OpenFiles[i].GetBlock(2, m_loadercount[i]*sizeof(LoaderStruct)));
        m_cloader[i] = m_loaders[i];

        if (m_symrefcount[i]) {
            //m_symrefs = new SymbolRefStruct[m_symrefcount];
            m_symrefs[i] = reinterpret_cast<SymbolRefStruct*>(info->OpenFiles[i].GetBlock(2, m_symrefcount[i]*sizeof(SymbolRefStruct)));
            m_csymref[i] = m_symrefs[i];
        }

        m_uloadercount[i] = 0;
        m_usymbolcount[i] = 0;
        m_usymrefcount[i] = 0;
    }
}

void ovlLodSymRefManager::Make(const map<string, unsigned long>& loadernumbers) {
    if (m_made)
        throw EOvl("ovlLodSymRefManager::Make called twice");
    if (!m_relman)
        throw EOvl("ovlLodSymRefManager::Make called with unassigned relocation manager");
    if (!(m_loaders[0] || m_symbols[0] || m_symrefs[0] || m_loaders[1] || m_symbols[1] || m_symrefs[1]))
        throw EOvl("ovlLodSymRefManager::Make called before assignment");
    if (m_uloadercount[0] < m_loadercount[0])
        throw EOvl("ovlLodSymRefManager::Make called with unassigned common loader(s)");
    if (m_uloadercount[1] < m_loadercount[1])
        throw EOvl("ovlLodSymRefManager::Make called with unassigned unique loader(s)");
    if (m_usymbolcount[0] < m_symbolcount[0])
        throw EOvl("ovlLodSymRefManager::Make called with unassigned common symbol(s)");
    if (m_usymbolcount[1] < m_symbolcount[1])
        throw EOvl("ovlLodSymRefManager::Make called with unassigned unique symbol(s)");
    if (m_usymrefcount[0] < m_symrefcount[0])
        throw EOvl("ovlLodSymRefManager::Make called with unassigned common symbol reference(s)");
    if (m_usymrefcount[1] < m_symrefcount[1])
        throw EOvl("ovlLodSymRefManager::Make called with unassigned unique symbol reference(s)");
    if (m_loadercount[0] != m_loadernames[0].size())
        throw EOvl("ovlLodSymRefManager::Make: Inconsistency between common loadercount and size of loadername array");
    if (m_loadercount[1] != m_loadernames[1].size())
        throw EOvl("ovlLodSymRefManager::Make: Inconsistency between unique loadercount and size of loadername array");

    for (int i = 0; i < 2; ++i) {
        SymbolStruct *syms2 = (SymbolStruct *)m_symbols[i];
        qsort(syms2, m_symbolcount[i], sizeof(SymbolStruct), comparesymbols);
        for (int m = 0; m < m_symbolcount[i]; m++) {
            m_relman->AddRelocation((unsigned long *)&(syms2[m].Symbol));
            DUMP_RELOCATION_STR("ovlLodSymRefManager::Make, Symbol", syms2[m].Symbol);
            m_relman->AddRelocation((unsigned long *)&(syms2[m].data));
            DUMP_RELOCATION("ovlLodSymRefManager::Make, Data", syms2[m].data);
        }
        LoaderStruct *ldrs2 = (LoaderStruct *)m_loaders[i];
        for (int m = 0; m < m_loadercount[i]; m++) {
            map<string, unsigned long>::const_iterator ld = loadernumbers.find(m_loadernames[i][m]);
            if (ld == loadernumbers.end())
                throw EOvl("ovlLodSymRefManager::Make: Loader not registered: "+m_loadernames[i][m]);
            ldrs2[m].LoaderType = ld->second;

            for (int j = 0; j < m_symbolcount[i]; j++) {
                if (ldrs2[m].data == syms2[m].data) {
                    ldrs2[m].Sym = &syms2[m];
                }
            }
        }
    }

    m_made = true;
}

void ovlLodSymRefManager::AddLoader(cOvlType type) {
    if (m_loaders[0] || m_symbols[0] || m_symrefs[0] || m_loaders[1] || m_symbols[1] || m_symrefs[1])
        throw EOvl("ovlLodSymRefManager::AddLoader called after assignment");
    m_loadercount[type]++;
}

LoaderStruct* ovlLodSymRefManager::OpenLoader(cOvlType type, const char* ctype, unsigned long *data, bool hasextradata, SymbolStruct *sym) {
    DUMP_LOG("Trace: ovlLodSymRefManager::OpenLoader('%s', %08lx, %d, %08lx)", ctype, DPTR(data), hasextradata, DPTR(sym));
    if (!m_loaders[type])
        throw EOvl("ovlLodSymRefManager::OpenLoader called before assignment");
    if (!m_relman)
        throw EOvl("ovlLodSymRefManager::OpenLoader called with unassigned relocation manager");
    if (m_uloadercount[type] == m_loadercount[type])
        throw EOvl("ovlLodSymRefManager::OpenLoader called when all loaders are already assigned");
    if (m_loaderopen[type])
        throw EOvl("ovlLodSymRefManager::OpenLoader called with open loader");

    m_cloader[type]->LoaderType = 0; // Init to 0, fill out later
    m_cloader[type]->data = data;
    m_relman->AddRelocation((unsigned long *)&m_cloader[type]->data);
    DUMP_RELOCATION("ovlLodSymRefManager::OpenLoader, Data", m_cloader[type]->data);
    m_cloader[type]->HasExtraData = hasextradata;
    m_cloader[type]->Sym = sym;
    m_relman->AddRelocation((unsigned long *)&m_cloader[type]->Sym);
    DUMP_RELOCATION("ovlLodSymRefManager::OpenLoader, Sym", m_cloader[type]->Sym);
    m_cloader[type]->SymbolsToResolve = 0;

    string t = ctype;
    m_loadernames[type].push_back(t);

    m_loaderopen[type] = true;

    return m_cloader[type];
}

LoaderStruct* ovlLodSymRefManager::CloseLoader(cOvlType type) {
    if (!m_loaders[type])
        throw EOvl("ovlLodSymRefManager::CloseLoader called before assignment");
    if (!m_loaderopen[type])
        throw EOvl("ovlLodSymRefManager::CloseLoader called with closed loader");

    m_loaderopen[type] = false;
    m_uloadercount[type]++;

    return m_cloader[type]++;
}
/*
LoaderStruct* ovlLodSymRefManager::GetLoaders() {
    if (!m_made)
        throw EOvl("ovlLodSymRefManager::GetLoaders called before calling Make()");
    return m_loaders;
}
*/
void ovlLodSymRefManager::AddSymbol(cOvlType type) {
    if (m_loaders[0] || m_symbols[0] || m_symrefs[0] || m_loaders[1] || m_symbols[1] || m_symrefs[1])
        throw EOvl("ovlLodSymRefManager::AddSymbol called after assignment");
    m_symbolcount[type]++;
}

SymbolStruct* ovlLodSymRefManager::MakeSymbol(cOvlType type, char* symbol, unsigned long *data, bool ispointer) {
    if (!m_symbols[type])
        throw EOvl("ovlLodSymRefManager::MakeSymbol called before assignment");
    if (m_usymbolcount[type] == m_symbolcount[type])
        throw EOvl("ovlLodSymRefManager::MakeSymbol called when all symbols are already assigned");

    m_csymbol[type]->Symbol = symbol;
    m_csymbol[type]->data = data;
    m_csymbol[type]->IsPointer = ispointer;
    // We do not push relocations, that's done in Make() after sorting

    m_usymbolcount[type]++;

    return m_csymbol[type]++;
}
/*
SymbolStruct* ovlLodSymRefManager::GetSymbols() {
    if (!m_made)
        throw EOvl("ovlLodSymRefManager::GetSymbols called before calling Make()");
    return m_symbols;
}
*/
void ovlLodSymRefManager::AddSymRef(cOvlType type) {
    if (m_loaders[0] || m_symbols[0] || m_symrefs[0] || m_loaders[1] || m_symbols[1] || m_symrefs[1])
        throw EOvl("ovlLodSymRefManager::AddSymRef called after assignment");
    m_symrefcount[type]++;
}

SymbolRefStruct* ovlLodSymRefManager::MakeSymRef(cOvlType type, char* symbol, unsigned long *ref) {
    if (!m_symrefs[type])
        throw EOvl("ovlLodSymRefManager::MakeSymRef called before assignment");
    if (!m_relman)
        throw EOvl("ovlLodSymRefManager::MakeSymRef called with unassigned relocation manager");
    if (!m_loaderopen[type])
        throw EOvl("ovlLodSymRefManager::MakeSymRef called with closed loader");
    if (m_usymrefcount[type] == m_symrefcount[type])
        throw EOvl("ovlLodSymRefManager::MakeSymRef called when all symbol references are already assigned");

    m_csymref[type]->Symbol = symbol;
    m_relman->AddRelocation((unsigned long *)&m_csymref[type]->Symbol);
    DUMP_RELOCATION_STR("ovlLodSymRefManager::MakeSymRef, Symbol", m_csymref[type]->Symbol);

    m_csymref[type]->reference = ref;
    m_relman->AddRelocation((unsigned long *)&m_csymref[type]->reference);
    DUMP_RELOCATION("ovlLodSymRefManager::MakeSymRef, Reference", m_csymref[type]->reference);

    m_csymref[type]->ldr = m_cloader[type];
    m_relman->AddRelocation((unsigned long *)&m_csymref[type]->ldr);
    DUMP_RELOCATION("ovlLodSymRefManager::MakeSymRef, Loader", m_csymref[type]->ldr);
    m_cloader[type]->SymbolsToResolve++;

    m_usymrefcount[type]++;

    return m_csymref[type]++;
}
/*
SymbolRefStruct* ovlLodSymRefManager::GetSymRefs() {
    if (!m_made)
        throw EOvl("ovlLodSymRefManager::GetSymRefs called before calling Make()");
    return m_symrefs;
}
*/
