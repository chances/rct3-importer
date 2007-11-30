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
    if (m_loaders)
        delete[] m_loaders;
    if (m_symbols)
        delete[] m_symbols;
    if (m_symrefs)
        delete[] m_symrefs;
}

void ovlLodSymRefManager::Init(ovlRelocationManager* relman) {
    m_relman = relman;
    m_loaders = NULL;
    m_cloader = NULL;
    m_symbols = NULL;
    m_csymbol = NULL;
    m_symrefs = NULL;
    m_csymref = NULL;
    m_loadercount = 0;
    m_symbolcount = 0;
    m_symrefcount = 0;
}

void ovlLodSymRefManager::Assign() {
    if (m_loaders || m_symbols || m_symrefs)
        throw EOvl("ovlLodSymRefManager::Assign called twice");

    m_loaders = new LoaderStruct[m_loadercount];
    m_cloader = m_loaders;
    m_symbols = new SymbolStruct[m_symbolcount];
    m_csymbol = m_symbols;
    m_symrefs = new SymbolRefStruct[m_symrefcount];
    m_csymref = m_symrefs;

    m_uloadercount = 0;
    m_usymbolcount = 0;
    m_usymrefcount = 0;
}

void ovlLodSymRefManager::Make(const map<string, unsigned long>& loadernumbers) {
    if (m_made)
        throw EOvl("ovlLodSymRefManager::Make called twice");
    if (!m_relman)
        throw EOvl("ovlLodSymRefManager::Make called with unassigned relocation manager");
    if (!(m_loaders || m_symbols || m_symrefs))
        throw EOvl("ovlLodSymRefManager::Make called before assignment");
    if (m_uloadercount < m_loadercount)
        throw EOvl("ovlLodSymRefManager::Make called with unassigned loader(s)");
    if (m_usymbolcount < m_symbolcount)
        throw EOvl("ovlLodSymRefManager::Make called with unassigned symbol(s)");
    if (m_usymrefcount < m_symrefcount)
        throw EOvl("ovlLodSymRefManager::Make called with unassigned symbol reference(s)");
    if (m_loadercount != m_loadernames.size())
        throw EOvl("ovlLodSymRefManager::Make: Inconsistency between loadercount and size of loadername array");

    SymbolStruct *syms2 = (SymbolStruct *)m_symbols;
    qsort(syms2, m_symbolcount, sizeof(SymbolStruct), comparesymbols);
    for (int i = 0; i < m_symbolcount; i++) {
        m_relman->AddRelocation((unsigned long *)&(syms2[i].Symbol));
        DUMP_RELOCATION_STR("ovlLodSymRefManager::Make, Symbol", syms2[i].Symbol);
        m_relman->AddRelocation((unsigned long *)&(syms2[i].data));
        DUMP_RELOCATION("ovlLodSymRefManager::Make, Data", syms2[i].data);
    }
    LoaderStruct *ldrs2 = (LoaderStruct *)m_loaders;
    for (int i = 0; i < m_loadercount; i++) {
        map<string, unsigned long>::const_iterator ld = loadernumbers.find(m_loadernames[i]);
        if (ld == loadernumbers.end())
            throw EOvl("ovlLodSymRefManager::Make: Loader not registered: "+m_loadernames[i]);
        ldrs2[i].LoaderType = ld->second;

        for (int j = 0; j < m_symbolcount; j++) {
            if (ldrs2[i].data == syms2[j].data) {
                ldrs2[i].Sym = &syms2[j];
            }
        }
    }
    m_made = true;
}

void ovlLodSymRefManager::AddLoader() {
    if (m_loaders || m_symbols || m_symrefs)
        throw EOvl("ovlLodSymRefManager::AddLoader called after assignment");
    m_loadercount++;
}

LoaderStruct* ovlLodSymRefManager::OpenLoader(const char* type, unsigned long *data, bool hasextradata, SymbolStruct *sym) {
    DUMP_LOG("Trace: ovlLodSymRefManager::OpenLoader('%s', %08lx, %d, %08lx)", type, DPTR(data), hasextradata, DPTR(sym));
    if (!m_loaders)
        throw EOvl("ovlLodSymRefManager::OpenLoader called before assignment");
    if (!m_relman)
        throw EOvl("ovlLodSymRefManager::OpenLoader called with unassigned relocation manager");
    if (m_uloadercount == m_loadercount)
        throw EOvl("ovlLodSymRefManager::OpenLoader called when all loaders are already assigned");
    if (m_loaderopen)
        throw EOvl("ovlLodSymRefManager::OpenLoader called with open loader");

    m_cloader->LoaderType = 0; // Init to 0, fill out later
    m_cloader->data = data;
    m_relman->AddRelocation((unsigned long *)&m_cloader->data);
    DUMP_RELOCATION("ovlLodSymRefManager::OpenLoader, Data", m_cloader->data);
    m_cloader->HasExtraData = hasextradata;
    m_cloader->Sym = sym;
    m_relman->AddRelocation((unsigned long *)&m_cloader->Sym);
    DUMP_RELOCATION("ovlLodSymRefManager::OpenLoader, Sym", m_cloader->Sym);
    m_cloader->SymbolsToResolve = 0;

    string t = type;
    m_loadernames.push_back(t);

    m_loaderopen = true;

    return m_cloader;
}

LoaderStruct* ovlLodSymRefManager::CloseLoader() {
    if (!m_loaders)
        throw EOvl("ovlLodSymRefManager::CloseLoader called before assignment");
    if (!m_loaderopen)
        throw EOvl("ovlLodSymRefManager::CloseLoader called with closed loader");

    m_loaderopen = false;
    m_uloadercount++;

    return m_cloader++;
}

LoaderStruct* ovlLodSymRefManager::GetLoaders() {
    if (!m_made)
        throw EOvl("ovlLodSymRefManager::GetLoaders called before calling Make()");
    return m_loaders;
}

void ovlLodSymRefManager::AddSymbol() {
    if (m_loaders || m_symbols || m_symrefs)
        throw EOvl("ovlLodSymRefManager::AddSymbol called after assignment");
    m_symbolcount++;
}

SymbolStruct* ovlLodSymRefManager::MakeSymbol(char* symbol, unsigned long *data, bool ispointer) {
    if (!m_symbols)
        throw EOvl("ovlLodSymRefManager::MakeSymbol called before assignment");
    if (m_usymbolcount == m_symbolcount)
        throw EOvl("ovlLodSymRefManager::MakeSymbol called when all symbols are already assigned");

    m_csymbol->Symbol = symbol;
    m_csymbol->data = data;
    m_csymbol->IsPointer = ispointer;
    // We do not push relocations, that's done in Make() after sorting

    m_usymbolcount++;

    return m_csymbol++;
}

SymbolStruct* ovlLodSymRefManager::GetSymbols() {
    if (!m_made)
        throw EOvl("ovlLodSymRefManager::GetSymbols called before calling Make()");
    return m_symbols;
}

void ovlLodSymRefManager::AddSymRef() {
    if (m_loaders || m_symbols || m_symrefs)
        throw EOvl("ovlLodSymRefManager::AddSymRef called after assignment");
    m_symrefcount++;
}

SymbolRefStruct* ovlLodSymRefManager::MakeSymRef(char* symbol, unsigned long *ref) {
    if (!m_symrefs)
        throw EOvl("ovlLodSymRefManager::MakeSymRef called before assignment");
    if (!m_relman)
        throw EOvl("ovlLodSymRefManager::MakeSymRef called with unassigned relocation manager");
    if (!m_loaderopen)
        throw EOvl("ovlLodSymRefManager::MakeSymRef called with closed loader");
    if (m_usymrefcount == m_symrefcount)
        throw EOvl("ovlLodSymRefManager::MakeSymRef called when all symbol references are already assigned");

    m_csymref->Symbol = symbol;
    m_relman->AddRelocation((unsigned long *)&m_csymref->Symbol);
    DUMP_RELOCATION_STR("ovlLodSymRefManager::MakeSymRef, Symbol", m_csymref->Symbol);

    m_csymref->reference = ref;
    m_relman->AddRelocation((unsigned long *)&m_csymref->reference);
    DUMP_RELOCATION("ovlLodSymRefManager::MakeSymRef, Reference", m_csymref->reference);

    m_csymref->ldr = m_cloader;
    m_relman->AddRelocation((unsigned long *)&m_csymref->ldr);
    DUMP_RELOCATION("ovlLodSymRefManager::MakeSymRef, Loader", m_csymref->ldr);
    m_cloader->SymbolsToResolve++;

    m_usymrefcount++;

    return m_csymref++;
}

SymbolRefStruct* ovlLodSymRefManager::GetSymRefs() {
    if (!m_made)
        throw EOvl("ovlLodSymRefManager::GetSymRefs called before calling Make()");
    return m_symrefs;
}

