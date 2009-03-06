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


#include "StringTable.h"

#include "OVLDebug.h"
#include "OVLException.h"

using namespace std;

char* ovlStringTable::FindRawString(const string& findit) const {
    map<string, char*>::const_iterator fst = m_ptrmap.find(findit);
    if (fst == m_ptrmap.end()) {
        BOOST_THROW_EXCEPTION(EOvl("ovlStringTable::FindRawString: Could not find '"+findit+"'"));
        return NULL;
    }
    DUMP_LOG("ovlStringTable::FindRawString '%s': %s (%x/%x)", UNISTR(findit.c_str()), UNISTR(fst->second), fst->second, m_table);
    return fst->second;
}

ovlStringTable::ovlStringTable() {
    m_size = 0;
    m_table = NULL;
}

ovlStringTable::~ovlStringTable() {
//    if (m_table)
//        delete[] m_table;
}

void ovlStringTable::AddString(const string& lstring) {
    m_strings.push_back(lstring);
    DUMP_LOG("TRACE: ovlStringTable::AddString '%s'", UNISTR(lstring.c_str()));
}

void ovlStringTable::AddSymbolString(const string& lstring, const string& lextension) {
    m_strings.push_back(lstring + ':' + lextension);
    DUMP_LOG("TRACE: ovlStringTable::AddSymbolString '%s':'%s'", UNISTR(lstring.c_str()), UNISTR(lextension.c_str()));
}

char* ovlStringTable::Make(cOvlInfo* info) {
    if (!info)
        BOOST_THROW_EXCEPTION(EOvl("ovlStringTable::Make called without valid info"));

    char* tempstr;
    if (m_table)
        return m_table;
    m_strings.sort();
    m_strings.unique();
    for (list<string>::iterator it = m_strings.begin(); it != m_strings.end(); ++it) {
        m_size += it->length() + 1;
    }

    //m_table = new char[m_size];
    m_table = reinterpret_cast<char*>(info->OpenFiles[OVLT_COMMON].GetBlock(0, m_size));
    tempstr = m_table;
    for (list<string>::iterator it = m_strings.begin(); it != m_strings.end(); ++it) {
        strcpy(tempstr, it->c_str());
        m_ptrmap[*it] = tempstr;
        tempstr += it->length() + 1;
    }
    return m_table;
}

char* ovlStringTable::FindString(const string& lstring) const {
    return FindRawString(lstring);
}

char* ovlStringTable::FindSymbolString(const string& lstring, const string& lextension) const {
    return FindRawString(lstring + ':' + lextension);
}

int ovlStringTable::GetSize() const {
    return m_size;
}

