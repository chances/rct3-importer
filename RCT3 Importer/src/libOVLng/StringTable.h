///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for the string table
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


#ifndef STRINGTABLE_H_INCLUDED
#define STRINGTABLE_H_INCLUDED

#include <list>
#include <map>
#include <string>

#include "OVLClasses.h"

#define STRINGLIST_ASSIGN(var, name, alwaysassign, st, rm) \
	{ \
		if (alwaysassign || (name != "")) { \
			var = st->FindString(name); \
			rm->AddRelocation(reinterpret_cast<unsigned long*>(&var)); \
		} else { \
			var = NULL; \
		} \
	}
#define STRINGLIST_ADD(name, alwaysassign) \
    { \
        if (alwaysassign || (name != "")) \
            GetStringTable()->AddString(name); \
    }
#define SYMREF_ADD(ovl, name, tag, alwaysassign) \
	{ \
		if (alwaysassign || (name != "")) { \
			GetLSRManager()->AddSymRef(ovl); \
			GetStringTable()->AddSymbolString(name, tag); \
		} \
	}
#define SYMREF_MAKE(ovl, name, tag, ptr_to_var, alwaysassign) \
	{ \
		if (alwaysassign || (name != "")) { \
		GetLSRManager()->MakeSymRef(ovl, GetStringTable()->FindSymbolString(name, tag), \
										 reinterpret_cast<unsigned long*>(ptr_to_var)); \
		} \
    }
#define RELOC_ARRAY(size, var, structid, data) \
	{ \
		if (size) { \
			var = reinterpret_cast<structid *>(data); \
			data += size * sizeof(structid); \
			GetRelocationManager()->AddRelocation(& var); \
		} \
    }

class ovlStringTable {
private:
    std::list<std::string> m_strings;
    std::map<std::string, char*> m_ptrmap;
    int m_size;
    char* m_table;

    char* FindRawString(const std::string& findit) const;
public:
    ovlStringTable();
    ~ovlStringTable();

    void AddString(const std::string& lstring);
    void AddSymbolString(const std::string& lstring, const std::string& lextension);
    char* Make(cOvlInfo* info);
    char* FindString(const std::string& lstring) const;
    char* FindSymbolString(const std::string& lstring, const std::string& lextension) const;
    int GetSize() const;
};

#endif
