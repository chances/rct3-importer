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

#include <stdlib.h>
#include <algorithm>
#include <boost/algorithm/string/compare.hpp>

#include "OVLDebug.h"
#include "RelocationManager.h"
#include "StringTable.h"

using namespace std;
using namespace r3;

const ExtraDataInfoV5 V5INFO_1 = { 0, 0xFFFF, 1, 1, 1 };
const ExtraDataInfoV5 V5INFO_2 = { 0, 0xFFFF, 2, 1, 2 };

template<class T>
struct ovlNameTagPredicate {
    const std::string& name;
    const std::string& tag;
	bool unassigned;
    ovlNameTagPredicate(const std::string& _name, const std::string& _tag, bool _unassigned = false): 
		name(_name), tag(_tag), unassigned(_unassigned) {}
	
    bool operator() (const T& v) {
        return ((v.name == name) && (v.tag == tag) && ((!unassigned) || (!v.assigned())));
    }
};

struct SymbolSorter {
	int version;
	const map<string, unsigned long>& loadernumbers;
	
	SymbolSorter(int version_, const map<string, unsigned long>& loadernumbers_): version(version_), loadernumbers(loadernumbers_) {}
	
	bool operator()(const cSymbol& s1, const cSymbol& s2) {
		if (version == 1) {
			//return s1.m_nametag < s2.m_nametag;
			//return boost::algorithm::is_iless()(s1.m_nametag, s2.m_nametag);
			return strcasecmp(s1.m_nametag.c_str(), s2.m_nametag.c_str()) < 0;
		} else if (version == 4) {
			return s1.m_hash < s2.m_hash;
		} else if (version == 5) {
			map<string, unsigned long>::const_iterator l1 = loadernumbers.find(s1.tag);
			map<string, unsigned long>::const_iterator l2 = loadernumbers.find(s2.tag);
			if (l1 == loadernumbers.end()) {
				if (l2 == loadernumbers.end()) {
					// Two data symbols, compare tags
					if (s1.tag != s2.tag) {
						// Sort alphabetically by tag
						return s1.tag < s2.tag;
					}
					// Otherwise, fall through to hash comparison
				} else {
					// s1 is data symbol -> s2 is smaller
					return false;
				}
			} else if (l2 == loadernumbers.end()) {
				// only s2 is a data symbol -> s1 is smaller
				return true;
			} else {
				// Two pointer symbols
				if (l1->second != l2->second) {
					return l1->second < l2->second;
				}
				// Otherwise, fall through to hash comparison
			}
			// Finally compare by hash
			return s1.m_hash < s2.m_hash;
		}
        BOOST_THROW_EXCEPTION(EOvl("SymbolSorter called with invalid version"));
	}
};
/*
int comparesymbols1(const void *s1,const void *s2) {
#ifndef __BORLANDC__
	return strcasecmp(((SymbolStruct *)s1)->Symbol,((SymbolStruct *)s2)->Symbol);
#else
	return stricmp(((SymbolStruct *)s1)->Symbol,((SymbolStruct *)s2)->Symbol);
#endif
}

int comparesymbols4(const void *s1,const void *s2) {
	const SymbolStruct2* sy1 = reinterpret_cast<const SymbolStruct2*>(s1);
	const SymbolStruct2* sy2 = reinterpret_cast<const SymbolStruct2*>(s2);
	if (sy1->hash == sy2->hash)
		return 0;
	else {
		if (sy1->hash < sy2->hash)
			return -1;
		else
			return 1;
	}
}
*/
unsigned long cSymbol::calcSymbolNameHash(const std::string& name) {
	string sname = name;
	std::transform(sname.begin(), sname.end(), sname.begin(), ::tolower);
	
	unsigned long hash = 5381;
	foreach(char c, sname) {
		hash = ((hash << 5) + hash) + c;
	}
		
	return hash;
}

void cSymbol::fill(r3::SymbolStruct& s, ovlStringTable* stable) const {
	s.Symbol = stable->FindSymbolString(name, tag);
	s.data = reinterpret_cast<uint32_t*>(data);
	s.IsPointer = is_pointer?1:0;
}
void cSymbol::fill(r3::SymbolStruct2& s, ovlStringTable* stable, r3::uint16_t unknown) const {
	s.Symbol = stable->FindSymbolString(name, tag);
	s.data = reinterpret_cast<uint32_t*>(data);
	s.IsPointer = is_pointer?1:0;
	s.unknown = unknown;
	s.hash = m_hash;
}

void cSymbolReference::fill(r3::SymbolRefStruct& sr, r3::LoaderStruct* loader, ovlStringTable* stable) const {
	sr.reference = reinterpret_cast<uint32_t*>(data);
	sr.Symbol = stable->FindSymbolString(name, tag);
	sr.ldr = loader;
}
void cSymbolReference::fill(r3::SymbolRefStruct2& sr, r3::LoaderStruct* loader, ovlStringTable* stable) const {
	sr.reference = reinterpret_cast<uint32_t*>(data);
	sr.Symbol = stable->FindSymbolString(name, tag);
	sr.ldr = loader;
	sr.hash = cSymbol::calcSymbolNameHash(name+":"+tag);
}

void cLoader::assignSymbolReference(const std::string& name_, const std::string& tag_, void* data_, bool force) {
	if (force || (name_!="")) {
		std::vector<cSymbolReference>::iterator sy = pretty::find_in_if(m_symrefs, ovlNameTagPredicate<cSymbolReference>(name_, tag_, true));
		if (sy == m_symrefs.end())
			BOOST_THROW_EXCEPTION(EOvl("Tried to assign unreserved symbol reference '"+name_+":"+tag_+"'"));
		sy->assign(data_);
	}
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
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < m_loaderextras[i].size(); ++j)
            delete m_loaderextras[i][j];
    }
    */
}

void ovlLodSymRefManager::Init(ovlRelocationManager* relman, ovlStringTable* stable) {
    m_relman = relman;
    m_stable = stable;
    for (int i = 0; i < 2; ++i) {
        m_ploaders[i] = NULL;
        //m_cloader[i] = NULL;
        m_psymbols[i] = NULL;
        //m_csymbol[i] = NULL;
        m_psymrefs[i] = NULL;
        //m_csymref[i] = NULL;
        //m_loadercount[i] = 0;
        //m_symbolcount[i] = 0;
        //m_symrefcount[i] = 0;
    }
}

void ovlLodSymRefManager::Assign(cOvlInfo* info) {
    if (m_ploaders[0] || m_psymbols[0] || m_psymrefs[0] || m_ploaders[1] || m_psymbols[1] || m_psymrefs[1])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Assign called twice"));
    if (!info)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Assign called without valid info"));
    if (!m_stable)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Assign called with unassigned string table"));

	m_version = info->version();

    for (int i = 0; i < 2; ++i) {
		int symrefs = 0;
		foreach(const cLoader& l, m_loaders[i])
			symrefs += l.getSymbolReferences().size();
		
		int symsize = 0;
		int symrefsize = 0;
		if (m_version>1) {
			symsize = m_symbols[i].size()*sizeof(SymbolStruct2);
			symrefsize = symrefs*sizeof(SymbolRefStruct2);
		} else {
			symsize = m_symbols[i].size()*sizeof(SymbolStruct);			
			symrefsize = symrefs*sizeof(SymbolRefStruct);
		}
        m_psymbols[i] = reinterpret_cast<SymbolStruct*>(info->OpenFiles[i].GetBlock(2, symsize));

        m_ploaders[i] = reinterpret_cast<LoaderStruct*>(info->OpenFiles[i].GetBlock(2, m_loaders[i].size()*sizeof(LoaderStruct)));

        if (symrefsize) {
            m_psymrefs[i] = reinterpret_cast<SymbolRefStruct*>(info->OpenFiles[i].GetBlock(2, symrefsize));
        }

    }
    m_assigned = true;
}

/*
void ovlLodSymRefManager::Assign(cOvlInfo* info) {
    if (m_loaders[0] || m_symbols[0] || m_symrefs[0] || m_loaders[1] || m_symbols[1] || m_symrefs[1])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Assign called twice"));
    if (!info)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Assign called without valid info"));
    if (!m_stable)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Assign called with unassigned string table"));

	m_version = info->version();

    for (int i = 0; i < 2; ++i) {
		int symsize = 0;
		int symrefsize = 0;
		if (m_version>1) {
			symsize = m_symbolcount[i]*sizeof(SymbolStruct2);
			symrefsize = m_symrefcount[i]*sizeof(SymbolRefStruct2);
		} else {
			symsize = m_symbolcount[i]*sizeof(SymbolStruct);			
			symrefsize = m_symrefcount[i]*sizeof(SymbolRefStruct);
		}
        m_symbols[i] = reinterpret_cast<SymbolStruct*>(info->OpenFiles[i].GetBlock(2, symsize));
        m_csymbol[i] = m_symbols[i];

        m_loaders[i] = reinterpret_cast<LoaderStruct*>(info->OpenFiles[i].GetBlock(2, m_loadercount[i]*sizeof(LoaderStruct)));
        m_cloader[i] = m_loaders[i];

        if (m_symrefcount[i]) {
            m_symrefs[i] = reinterpret_cast<SymbolRefStruct*>(info->OpenFiles[i].GetBlock(2, symrefsize));
            m_csymref[i] = m_symrefs[i];
        }

        m_uloadercount[i] = 0;
        m_usymbolcount[i] = 0;
        m_usymrefcount[i] = 0;


        for (map<string, unsigned long>::iterator it = m_npsymbols[i].begin(); it != m_npsymbols[i].end(); ++it) {
            m_csymbol[i]->Symbol = m_stable->FindString(it->first.c_str());
            m_csymbol[i]->data = reinterpret_cast<uint32_t*>(it->second);
            m_csymbol[i]->IsPointer = false;
			if (m_version > 1) {
				reinterpret_cast<SymbolStruct2*>(m_csymbol[i])->hash = calcSymbolNameHash(it->first);
			}
            // We do not push relocations, that's done in Make() after sorting

            m_usymbolcount[i]++;
            IncSymbol(i);
        }
    }
    m_assigned = true;
}
*/

void ovlLodSymRefManager::Make(const map<string, unsigned long>& loadernumbers) {
    if (m_made)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called twice"));
    if (!m_relman)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called with unassigned relocation manager"));
    if (!m_assigned)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called before assignment"));
    for (int i = 0; i < 2; ++i) {
		foreach(const cSymbol& s, m_symbols[i])
			if (!s.assigned())
				BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called with unassigned symbol '"+s.name+":"+s.tag+"'"));
		foreach(const cLoader& l, m_loaders[i]) {
			if (!l.assigned())
				BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called with unassigned loader '"+l.name+":"+l.tag+"'"));
			l.checkSymbolReferences();
		}
	}

    for (int i = 0; i < 2; ++i) {
		// Sort symbols
		std::sort(m_symbols[i].begin(), m_symbols[i].end(), SymbolSorter(m_version, loadernumbers));
		// And make them
		for (int m = 0; m < m_symbols[i].size(); m++) {
			vector<cLoader>::iterator lo = pretty::find_in_if(m_loaders[i], ovlNameTagPredicate<cLoader>(m_symbols[i][m].name, m_symbols[i][m].tag));
			if ((m_symbols[i][m].is_pointer) && (lo == m_loaders[i].end()))
				BOOST_THROW_EXCEPTION(EOvl("Symbol '"+m_symbols[i][m].name+":"+m_symbols[i][m].tag+"', corresponding loader not found"));
			if (m_version > 1) {
				SymbolStruct2 *syms2 = reinterpret_cast<SymbolStruct2*>(m_psymbols[i]);
				m_symbols[i][m].fill(syms2[m], m_stable, (m_version==5)?0xFFFF:0);
				
				m_relman->AddRelocation(&(syms2[m].Symbol));
				DUMP_RELOCATION_STR("ovlLodSymRefManager::Make, Symbol", syms2[m].Symbol);
				if (syms2[m].IsPointer) {
					lo->setSymbol(&syms2[m]);
					m_relman->AddRelocation(&(syms2[m].data));
					DUMP_RELOCATION("ovlLodSymRefManager::Make, Data", syms2[m].data);
				}
			} else {
				m_symbols[i][m].fill(m_psymbols[i][m], m_stable);
				
				m_relman->AddRelocation(&(m_psymbols[i][m].Symbol));
				DUMP_RELOCATION_STR("ovlLodSymRefManager::Make, Symbol", m_psymbols[i][m].Symbol);
				if (m_psymbols[i][m].IsPointer) {
					lo->setSymbol(&m_psymbols[i][m]);
					m_relman->AddRelocation(&(m_psymbols[i][m].data));
					DUMP_RELOCATION("ovlLodSymRefManager::Make, Data", m_psymbols[i][m].data);
				}				
			}
		}
		
		// Now make loaders and symrefs
		int c_symref = 0;
		int c_loader = 0;
		SymbolRefStruct* symref1 = m_psymrefs[i];
		SymbolRefStruct2* symref2 = reinterpret_cast<SymbolRefStruct2*>(m_psymrefs[i]);
		
		foreach(cLoader& loader, m_loaders[i]) {
			loader.fill(m_ploaders[i][c_loader], c_loader, loadernumbers);
			m_relman->AddRelocation(&(m_ploaders[i][c_loader].data));
			DUMP_RELOCATION("ovlLodSymRefManager::Make, Loader data", m_ploaders[i][c_loader].data);
			if (!loader.unnamed()) {
				m_relman->AddRelocation(&(m_ploaders[i][c_loader].Sym));
				DUMP_RELOCATION("ovlLodSymRefManager::Make, Loader symbol", m_ploaders[i][c_loader].Sym);
			}
			
			foreach(const cSymbolReference& symref, loader.getSymbolReferences()) {
				if (m_version > 1) {
					symref.fill(symref2[c_symref], &m_ploaders[i][c_loader], m_stable);
					m_relman->AddRelocation(&(symref2[c_symref].reference));
					DUMP_RELOCATION("ovlLodSymRefManager::Make, SymRef ref", symref2[c_symref].reference);
					m_relman->AddRelocation(&(symref2[c_symref].Symbol));
					DUMP_RELOCATION_STR("ovlLodSymRefManager::Make, SymRef symbol", symref2[c_symref].Symbol);
					m_relman->AddRelocation(&(symref2[c_symref].ldr));
					DUMP_RELOCATION("ovlLodSymRefManager::Make, SymRef loader", symref2[c_symref].ldr);
				} else {
					symref.fill(symref1[c_symref], &m_ploaders[i][c_loader], m_stable);					
					m_relman->AddRelocation(&(symref1[c_symref].reference));
					DUMP_RELOCATION("ovlLodSymRefManager::Make, SymRef ref", symref1[c_symref].reference);
					m_relman->AddRelocation(&(symref1[c_symref].Symbol));
					DUMP_RELOCATION_STR("ovlLodSymRefManager::Make, SymRef symbol", symref1[c_symref].Symbol);
					m_relman->AddRelocation(&(symref1[c_symref].ldr));
					DUMP_RELOCATION("ovlLodSymRefManager::Make, SymRef loader", symref1[c_symref].ldr);
				}
				c_symref++;
			}
			
			c_loader++;
		}
		
    }

    m_made = true;
}

/*
void ovlLodSymRefManager::Make(const map<string, unsigned long>& loadernumbers) {
    if (m_made)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called twice"));
    if (!m_relman)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called with unassigned relocation manager"));
    if (!m_assigned)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called before assignment"));
    if (m_uloadercount[0] < m_loadercount[0])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called with unassigned common loader(s)"));
    if (m_uloadercount[1] < m_loadercount[1])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called with unassigned unique loader(s)"));
    if (m_usymbolcount[0] < m_symbolcount[0])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called with unassigned common symbol(s)"));
    if (m_usymbolcount[1] < m_symbolcount[1])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called with unassigned unique symbol(s)"));
    if (m_usymrefcount[0] < m_symrefcount[0])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called with unassigned common symbol reference(s)"));
    if (m_usymrefcount[1] < m_symrefcount[1])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called with unassigned unique symbol reference(s)"));
    if (m_loadercount[0] != m_loadernames[0].size())
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make: Inconsistency between common loadercount and size of loadername array"));
    if (m_loadercount[1] != m_loadernames[1].size())
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make: Inconsistency between unique loadercount and size of loadername array"));

    for (int i = 0; i < 2; ++i) {
		if (m_version > 1) {
			SymbolStruct2 *syms2 = reinterpret_cast<SymbolStruct2*>(m_symbols[i]);
			if (version == 4) {
				qsort(syms2, m_symbolcount[i], sizeof(SymbolStruct2), comparesymbols2);										
			} else {
				// Here be dragons
			}
			for (int m = 0; m < m_symbolcount[i]; m++) {
				m_relman->AddRelocation((unsigned long *)&(syms2[m].Symbol));
				DUMP_RELOCATION_STR("ovlLodSymRefManager::Make, Symbol", syms2[m].Symbol);
				if (syms2[m].IsPointer) {
					m_relman->AddRelocation((unsigned long *)&(syms2[m].data));
					DUMP_RELOCATION("ovlLodSymRefManager::Make, Data", syms2[m].data);
				}
				if (m_version == 5) {
					syms2[m].unknown = 0xffff;
				} else {
					syms2[m].unknown = 0;					
				}
			}
			
			LoaderStruct *ldrs2 = (LoaderStruct *)m_loaders[i];
			for (int m = 0; m < m_loadercount[i]; m++) {
				map<string, unsigned long>::const_iterator ld = loadernumbers.find(m_loadernames[i][m]);
				if (ld == loadernumbers.end())
					BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make: Loader not registered: "+m_loadernames[i][m]));
				ldrs2[m].LoaderType = ld->second;

				for (int j = 0; j < m_symbolcount[i]; j++) {
					if ((ldrs2[m].data == syms2[j].data) && (syms2[j].IsPointer)) {
						ldrs2[m].Sym = reinterpret_cast<SymbolStruct*>(&syms2[j]);
					}
				}
			}
		} else {
			SymbolStruct *syms2 = reinterpret_cast<SymbolStruct*>(m_symbols[i]);
			qsort(syms2, m_symbolcount[i], sizeof(SymbolStruct), comparesymbols);			
			for (int m = 0; m < m_symbolcount[i]; m++) {
				m_relman->AddRelocation((unsigned long *)&(syms2[m].Symbol));
				DUMP_RELOCATION_STR("ovlLodSymRefManager::Make, Symbol", syms2[m].Symbol);
				if (syms2[m].IsPointer) {
					m_relman->AddRelocation((unsigned long *)&(syms2[m].data));
					DUMP_RELOCATION("ovlLodSymRefManager::Make, Data", syms2[m].data);
				}
			}
			
			LoaderStruct *ldrs2 = (LoaderStruct *)m_loaders[i];
			for (int m = 0; m < m_loadercount[i]; m++) {
				map<string, unsigned long>::const_iterator ld = loadernumbers.find(m_loadernames[i][m]);
				if (ld == loadernumbers.end())
					BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make: Loader not registered: "+m_loadernames[i][m]));
				ldrs2[m].LoaderType = ld->second;

				for (int j = 0; j < m_symbolcount[i]; j++) {
					if ((ldrs2[m].data == syms2[j].data) && (syms2[j].IsPointer)) {
						ldrs2[m].Sym = &syms2[j];
					}
				}
			}
		}
    }

    m_made = true;
}
*/
/*
void ovlLodSymRefManager::AddLoader(cOvlType type) {
    if (m_loaders[0] || m_symbols[0] || m_symrefs[0] || m_loaders[1] || m_symbols[1] || m_symrefs[1])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::AddLoader called after assignment"));
    m_loadercount[type]++;
}

LoaderStruct* ovlLodSymRefManager::OpenLoader(cOvlType type, const char* ctype, unsigned long *data, unsigned long extradatacount, SymbolStruct *sym) {
    DUMP_LOG("Trace: ovlLodSymRefManager::OpenLoader('%s', %08lx, %ld, %08lx)", UNISTR(ctype), DPTR(data), extradatacount, DPTR(sym));
    if (!m_loaders[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::OpenLoader called before assignment"));
    if (!m_relman)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::OpenLoader called with unassigned relocation manager"));
    if (m_uloadercount[type] == m_loadercount[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::OpenLoader called when all loaders are already assigned"));
    if (m_loaderopen[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::OpenLoader called with open loader"));

    m_cloader[type]->LoaderType = 0; // Init to 0, fill out later
    m_cloader[type]->data = reinterpret_cast<uint32_t*>(data);
    m_relman->AddRelocation((unsigned long *)&m_cloader[type]->data);
    DUMP_RELOCATION("ovlLodSymRefManager::OpenLoader, Data", m_cloader[type]->data);
    m_cloader[type]->HasExtraData = extradatacount;
    if (sym) {
        m_cloader[type]->Sym = sym;
        m_relman->AddRelocation((unsigned long *)&m_cloader[type]->Sym);
        DUMP_RELOCATION("ovlLodSymRefManager::OpenLoader, Sym", m_cloader[type]->Sym);
    } else {
        m_cloader[type]->Sym = NULL;
    }
    m_cloader[type]->SymbolsToResolve = 0;

    string t = ctype;
    m_loadernames[type].push_back(t);

    m_loaderopen[type] = true;
    m_loaderextracount[type] = extradatacount;

    return m_cloader[type];
}

void ovlLodSymRefManager::AddExtraData(cOvlType type, unsigned long size, unsigned char* data) {
    if (!m_loaderopen[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::AddExtraData called with closed loader"));
    if (!m_loaderextracount[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::AddExtraData called with no unadded extra data left"));

    m_loaderextras[type].push_back(new ovlExtraChunk(size, data));
    m_loaderextracount[type]--;
}

void ovlLodSymRefManager::AddExtraDataV5Info(cOvlType type, ExtraDataInfoV5 v5info) {
    if (!m_loaderopen[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::AddExtraDataV5Info called with closed loader"));
	v5info.index = m_uloadercount[type];
	m_v5extra[type].push_back(v5info);
}

LoaderStruct* ovlLodSymRefManager::CloseLoader(cOvlType type) {
    if (!m_loaders[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::CloseLoader called before assignment"));
    if (!m_loaderopen[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::CloseLoader called with closed loader"));
    if (m_loaderextracount[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::CloseLoader called with unadded extra data"));

    m_loaderopen[type] = false;
    m_uloadercount[type]++;

    return m_cloader[type]++;
}

void ovlLodSymRefManager::AddSymbol(cOvlType type) {
    if (m_loaders[0] || m_symbols[0] || m_symrefs[0] || m_loaders[1] || m_symbols[1] || m_symrefs[1])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::AddSymbol called after assignment"));
    m_symbolcount[type]++;
}

void ovlLodSymRefManager::AddSymbol(cOvlType type, const char* symbol, unsigned long data) {
    if (m_loaders[0] || m_symbols[0] || m_symrefs[0] || m_loaders[1] || m_symbols[1] || m_symrefs[1])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::AddSymbol(2) called after assignment"));
    if (!m_stable)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::AddSymbol(2) called with unassigned string table"));

    m_symbolcount[type]++;
    m_npsymbols[type][string(symbol)] = data;
    m_stable->AddString(symbol);
}

SymbolStruct* ovlLodSymRefManager::MakeSymbol(cOvlType type, char* symbol, unsigned long *data) {
    if (!m_symbols[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::MakeSymbol called before assignment"));
    if (m_usymbolcount[type] == m_symbolcount[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::MakeSymbol called when all symbols are already assigned"));

    m_csymbol[type]->Symbol = symbol;
    m_csymbol[type]->data = reinterpret_cast<uint32_t*>(data);
    m_csymbol[type]->IsPointer = true;
	if (m_version > 1) {
		reinterpret_cast<SymbolStruct2*>(m_csymbol[type])->hash = calcSymbolNameHash(symbol);
	}
    // We do not push relocations, that's done in Make() after sorting

    m_usymbolcount[type]++;

    return IncSymbol(type);
}

void ovlLodSymRefManager::AddSymRef(cOvlType type) {
    if (m_loaders[0] || m_symbols[0] || m_symrefs[0] || m_loaders[1] || m_symbols[1] || m_symrefs[1])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::AddSymRef called after assignment"));
    m_symrefcount[type]++;
}

SymbolRefStruct* ovlLodSymRefManager::MakeSymRef(cOvlType type, char* symbol, unsigned long *ref) {
    DUMP_LOG("ovlLodSymRefManager::MakeSymRef(%s)", symbol);
    if (!m_symrefs[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::MakeSymRef called before assignment"));
    if (!m_relman)
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::MakeSymRef called with unassigned relocation manager"));
    if (!m_loaderopen[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::MakeSymRef called with closed loader"));
    if (m_usymrefcount[type] == m_symrefcount[type])
        BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::MakeSymRef called when all symbol references are already assigned"));

    m_csymref[type]->Symbol = symbol;
    m_relman->AddRelocation((unsigned long *)&m_csymref[type]->Symbol);
    DUMP_RELOCATION_STR("ovlLodSymRefManager::MakeSymRef, Symbol", m_csymref[type]->Symbol);
	if (m_version > 1) {
		reinterpret_cast<SymbolRefStruct2*>(m_csymref[type])->hash = calcSymbolNameHash(symbol);
	}

    m_csymref[type]->reference = reinterpret_cast<uint32_t*>(ref);
    m_relman->AddRelocation((unsigned long *)&m_csymref[type]->reference);
    DUMP_RELOCATION("ovlLodSymRefManager::MakeSymRef, Reference", m_csymref[type]->reference);

    m_csymref[type]->ldr = m_cloader[type];
    m_relman->AddRelocation((unsigned long *)&m_csymref[type]->ldr);
    DUMP_RELOCATION("ovlLodSymRefManager::MakeSymRef, Loader", m_csymref[type]->ldr);
    m_cloader[type]->SymbolsToResolve++;

    m_usymrefcount[type]++;

    return IncSymbolRef(type);
}
*/

void ovlLodSymRefManager::addDataSymbol(r3::cOvlType type, const std::string& name, const std::string& tag, unsigned long data) {
	m_symbols[type].push_back(cSymbol(name, tag, data));
	m_stable->AddSymbolString(name, tag);
}

cLoader& ovlLodSymRefManager::reserveIndexElement(r3::cOvlType type, const std::string& name, const std::string& tag) {
	if (name != "") {
		m_symbols[type].push_back(cSymbol(name, tag));
		m_stable->AddSymbolString(name, tag);		
	}
	m_loaders[type].push_back(cLoader(m_stable, name, tag));
	return m_loaders[type].back();
}

cLoader& ovlLodSymRefManager::assignIndexElement(r3::cOvlType type, const std::string& name, const std::string& tag, void* data) {
	DUMP_LOG("ovlLodSymRefManager::assignIndexElement %s %s %08x", UNISTR(name.c_str()), UNISTR(tag.c_str()), data);
	vector<cLoader>::iterator lo = pretty::find_in_if(m_loaders[type], ovlNameTagPredicate<cLoader>(name, tag));
	if (lo == m_loaders[type].end())
		BOOST_THROW_EXCEPTION(EOvl("Tried to assign unreserved loader '"+name+":"+tag+"'"));
	if (!lo->identified())
		BOOST_THROW_EXCEPTION(EOvl("Tried to assign unidentified loader '"+name+":"+tag+"'"));
	lo->assign(data);
	
	if (!lo->unnamed()) {
		vector<cSymbol>::iterator sy = pretty::find_in_if(m_symbols[type], ovlNameTagPredicate<cSymbol>(name, tag));
		if (sy == m_symbols[type].end())
			BOOST_THROW_EXCEPTION(EOvl("Tried to assign unreserved symbol '"+name+":"+tag+"'"));
		sy->assign(data);
	}
	return *lo;
}
