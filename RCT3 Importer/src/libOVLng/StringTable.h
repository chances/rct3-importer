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

using namespace std;

class ovlStringTable {
private:
    list<string> m_strings;
    map<string, char*> m_ptrmap;
    int m_size;
    char* m_table;

    char* FindRawString(const string& findit) const;
public:
    ovlStringTable();
    ~ovlStringTable();

    void AddString(const string& lstring);
    void AddSymbolString(const string& lstring, const string& lextension);
    char* Make(cOvlInfo* info);
    char* FindString(const string& lstring) const;
    char* FindSymbolString(const string& lstring, const string& lextension) const;
    int GetSize() const;
};

#endif
