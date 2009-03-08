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

#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <boost/shared_array.hpp>

#include "OVLClasses.h"
#include "OVLException.h"
#include "ovlstructs.h"
#include "StringTable.h"

#include "pretty.h"

class ovlRelocationManager;

extern const r3::ExtraDataInfoV5 V5INFO_1;
extern const r3::ExtraDataInfoV5 V5INFO_2;

class SymbolSorter;
class cSymbol {
friend class SymbolSorter;
public:
	std::string name;
	std::string tag;
	char* data;
	bool is_pointer;
private:
	bool m_assigned;
	std::string m_nametag;
	uint32_t m_hash;
public:
	cSymbol(const std::string& name_, const std::string& tag_):
		name(name_), tag(tag_), data(NULL), is_pointer(true), m_assigned(false), m_nametag(name_+":"+tag_) {
		m_hash = cSymbol::calcSymbolNameHash(m_nametag);
	}
	cSymbol(const std::string& name_, const std::string& tag_, unsigned long data_):
		name(name_), tag(tag_), data(reinterpret_cast<char*>(data_)), is_pointer(false), m_assigned(true), m_nametag(name_+":"+tag_) {
		m_hash = cSymbol::calcSymbolNameHash(m_nametag);
	}

	void assign(void* data_) {
		if (m_assigned)
			BOOST_THROW_EXCEPTION(EOvl("Symbol '"+name+":"+tag+"' already assigned"));
		data = reinterpret_cast<char*>(data_);
		m_assigned = true;
	}
	
	inline bool assigned() const {
		return m_assigned;
	}
	
	void fill(r3::SymbolStruct& s, ovlStringTable* stable) const;
	void fill(r3::SymbolStruct2& s, ovlStringTable* stable, r3::uint16_t unknown) const;
	
	static unsigned long calcSymbolNameHash(const std::string& name);
};

class cSymbolReference {
public:
	std::string name;
	std::string tag;
	char* data;
private:
	bool m_assigned;
	
public:
	cSymbolReference(const std::string& name_, const std::string& tag_):
		name(name_), tag(tag_), data(NULL), m_assigned(false) {}
		
	void assign(void* data_) {
		if (m_assigned)
			BOOST_THROW_EXCEPTION(EOvl("Symbol reference '"+name+":"+tag+"' already assigned"));
		data = reinterpret_cast<char*>(data_);
		m_assigned = true;
	}
	
	inline bool assigned() const {
		return m_assigned;
	}
	
	void fill(r3::SymbolRefStruct& sr, r3::LoaderStruct* loader, ovlStringTable* stable) const;
	void fill(r3::SymbolRefStruct2& sr, r3::LoaderStruct* loader, ovlStringTable* stable) const;
};
 
class cExtraChunk {
public:
	unsigned long size;
	boost::shared_array<unsigned char> data;
	
	cExtraChunk(unsigned long size_, boost::shared_array<unsigned char>& data_):
		size(size_), data(data_) {}
		
	void write(std::fstream& fstr) const {
        fstr.write(reinterpret_cast<const char*>(&size),4);
        fstr.write(reinterpret_cast<const char*>(data.get()),size);		
	}
};

class cLoader {
public:
	std::string name;
	std::string tag;
	char* data;
private:
	std::vector<cSymbolReference> m_symrefs;
	std::vector<cExtraChunk> m_extras;
	std::vector<r3::ExtraDataInfoV5> m_v5extra;
	bool m_assigned;
	bool m_identified;
	bool m_unnamed;
	r3::SymbolStruct* m_sym;
	
    ovlStringTable* m_stable;	
	
public:
	cLoader(ovlStringTable* stable, const std::string& name_, const std::string& tag_): 
		name(name_), tag(tag_), data(NULL), m_assigned(false), m_sym(NULL), m_stable(stable) {
		m_identified = name != "";
		m_unnamed = !m_identified;
	}
	
	void identify(const std::string& name_) {
		name = name_;
		m_identified = name != "";
	}

	void assign(void* data_) {
		if (m_assigned)
			BOOST_THROW_EXCEPTION(EOvl("Loader '"+name+":"+tag+"' already assigned"));
		data = reinterpret_cast<char*>(data_);
		m_assigned = true;
	}
	
	inline bool assigned() const {
		return m_assigned;
	}
	
	inline bool identified() const {
		return m_identified;
	}
	
	inline bool unnamed() const {
		return m_unnamed;
	}
	
	void fill(r3::LoaderStruct& l, r3::uint32_t nr, const std::map<std::string, unsigned long>& loadernumbers) {
		std::map<std::string, unsigned long>::const_iterator ld = loadernumbers.find(tag);
		if (ld == loadernumbers.end())
			BOOST_THROW_EXCEPTION(EOvl("cLoader::fill: Loader not registered: "+tag));
		l.LoaderType = ld->second;
		l.data = reinterpret_cast<r3::uint32_t*>(data);
		l.HasExtraData = m_extras.size();
		l.Sym = m_sym;
		l.SymbolsToResolve = m_symrefs.size();
		foreach(r3::ExtraDataInfoV5& ex, m_v5extra)
			ex.index=nr;
	}
	
	void checkSymbolReferences() const {
		foreach(const cSymbolReference& s, m_symrefs)
			if (!s.assigned())
				BOOST_THROW_EXCEPTION(EOvl("ovlLodSymRefManager::Make called with unassigned symbol reference '"+s.name+":"+s.tag+"' in loader '"+name+":"+tag+"'"));		
	}
	
	inline void reserveSymbolReference(const std::string& name_, const std::string& tag_, bool force = true) {
		if (force || (name_!="")) {
			m_symrefs.push_back(cSymbolReference(name_, tag_));
			m_stable->AddSymbolString(name_, tag_);
		}
	}
	void assignSymbolReference(const std::string& name_, const std::string& tag_, void* data_, bool force = true);
	inline void addExtraChunk(unsigned long size_, boost::shared_array<unsigned char>& data_) {
		m_extras.push_back(cExtraChunk(size_, data_));
	}
	inline void writeExtraChunks(std::fstream& fstr) const {
		foreach(const cExtraChunk& c, m_extras)
			c.write(fstr);
	}
	inline void addExtraDataV5Info(const r3::ExtraDataInfoV5& v5info) {
		m_v5extra.push_back(v5info);
	}
	inline void writeV5Extras(std::fstream& fstr) const {
		foreach(const r3::ExtraDataInfoV5& i, m_v5extra)
			fstr.write(reinterpret_cast<const char*>(&i), 14);
	}
	
	inline const std::vector<cSymbolReference>& getSymbolReferences() const {
		return m_symrefs;
	}
	
	inline const std::vector<r3::ExtraDataInfoV5> getExtraDataV5Infos() const {
		return m_v5extra;
	}

	inline void setSymbol(r3::SymbolStruct* s) {
		m_sym = s;
	}
	inline void setSymbol(r3::SymbolStruct2* s) {
		m_sym = reinterpret_cast<r3::SymbolStruct*>(s);
	}
	
};
/*
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
*/
class ovlLodSymRefManager {
private:
    r3::LoaderStruct* m_ploaders[2];
    //r3::LoaderStruct* m_cloader[2];
    r3::SymbolStruct* m_psymbols[2];
    //r3::SymbolStruct* m_csymbol[2];
    r3::SymbolRefStruct* m_psymrefs[2];
    //r3::SymbolRefStruct* m_csymref[2];
    std::map<std::string, unsigned long> m_npsymbols[2];
    //std::vector<ovlExtraChunk*> m_loaderextras[2];
	//std::vector<r3::ExtraDataInfoV5> m_v5extra[2];

    //std::vector<std::string> m_loadernames[2];
	
	std::vector<cSymbol> m_symbols[2];
	std::vector<cLoader> m_loaders[2];

    //int m_loadercount[2];
    //int m_symbolcount[2];
    //int m_symrefcount;
    //int m_uloadercount[2];
    //int m_usymbolcount[2];
    //int m_usymrefcount[2];

    bool m_assigned;
    bool m_made;
	int m_version;
    //bool m_loaderopen[2];
    //unsigned long m_loaderextracount[2];
    ovlRelocationManager* m_relman;
    ovlStringTable* m_stable;
public:
    ovlLodSymRefManager() {
        m_relman = NULL;
        m_stable = NULL;
        m_assigned = false;
        m_made = false;
		/*
        m_loaderopen[0] = false;
        m_loaderopen[1] = false;
        m_loaderextracount[0] = 0;
        m_loaderextracount[1] = 0;
		*/
		m_version = 0;
    };
    virtual ~ovlLodSymRefManager();

    void Init(ovlRelocationManager* relman, ovlStringTable* stable);
    void Assign(cOvlInfo* info);
    void Make(const std::map<std::string, unsigned long>& loadernumbers);
	
	inline const std::vector<cLoader>& getLoaders(r3::cOvlType type) const {
		return m_loaders[type];
	}

    void addDataSymbol(r3::cOvlType type, const std::string& name, const std::string& tag, unsigned long data); // Non-pointer symbols
	cLoader& reserveIndexElement(r3::cOvlType type, const std::string& name, const std::string& tag);
	inline cLoader& reserveIndexElement(r3::cOvlType type, const std::string& tag) {
		return reserveIndexElement(type, "", tag);
	}
	cLoader& assignIndexElement(r3::cOvlType type, const std::string& name, const std::string& tag, void* data);
/*
    void AddLoader(r3::cOvlType type);
    r3::LoaderStruct* OpenLoader(r3::cOvlType type, const char* ctype, unsigned long *data, unsigned long extradatacount, r3::SymbolStruct *sym);
    void AddExtraData(r3::cOvlType type, unsigned long size, unsigned char* data);
	void AddExtraDataV5Info(r3::cOvlType type, r3::ExtraDataInfoV5 v5info);
    r3::LoaderStruct* CloseLoader(r3::cOvlType type);
    int GetLoaderCount(r3::cOvlType type) {
        return m_loadercount[type];
    }
    void AddSymbol(r3::cOvlType type);
    void AddSymbol(r3::cOvlType type, const char *symbol, unsigned long data);                // Non-pointer symbols
    r3::SymbolStruct* MakeSymbol(r3::cOvlType type, char *symbol, unsigned long *data);     // Pointer symbols
    r3::SymbolStruct* GetSymbols();
    int GetSymbolCount(r3::cOvlType type) {
        return m_symbolcount[type];
    }
    void AddSymRef(r3::cOvlType type);
    r3::SymbolRefStruct* MakeSymRef(r3::cOvlType type, char *symbol, unsigned long *ref);
    r3::SymbolRefStruct* GetSymRefs();
    int GetSymRefCount(r3::cOvlType type) {
        return m_symrefcount[type];
    }
*/
/*
    const std::vector<ovlExtraChunk*>& GetExtraChunks(r3::cOvlType type) const {
        return m_loaderextras[type];
    }
    const std::vector<r3::ExtraDataInfoV5>& GetV5Extra(r3::cOvlType type) const {
        return m_v5extra[type];
    }
*/
private:
/*
	r3::SymbolStruct* IncSymbol(int type) {
		if (m_version > 1) {
			r3::SymbolStruct* os = m_csymbol[type];
			m_csymbol[type] = reinterpret_cast<r3::SymbolStruct*>(reinterpret_cast<r3::SymbolStruct2*>(m_csymbol[type])+1);
			return os;
		} else {
			return m_csymbol[type]++;
		}
	}
	r3::SymbolRefStruct* IncSymbolRef(int type) {
		if (m_version > 1) {
			r3::SymbolRefStruct* os = m_csymref[type];
			m_csymref[type] = reinterpret_cast<r3::SymbolRefStruct*>(reinterpret_cast<r3::SymbolRefStruct2*>(m_csymref[type])+1);
			return os;
		} else {
			return m_csymref[type]++;
		}
	}
*/
};

#endif
