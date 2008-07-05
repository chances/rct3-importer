///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for TXT structures
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


#include "ManagerTXT.h"

#include <stdlib.h>

#include "OVLException.h"

using namespace std;

const char* ovlTXTManager::LOADER = "FGDK";
const char* ovlTXTManager::NAME = "Text";
const char* ovlTXTManager::TAG = "txt";
const unsigned long ovlTXTManager::TYPE = 2;

void ovlTXTManager::AddText(const char* name, const char* str) {
    counted_array_ptr<wchar_t> newstr(new wchar_t[strlen(str)+1]);
    mbstowcs(newstr.get(), str, strlen(str)+1);
    AddText(name, newstr.get());
}

void ovlTXTManager::AddText(const char* name, const wchar_t* str) {
    Check("ovlTXTManager::AddText");
    if (m_items.find(string(name)) != m_items.end())
        throw EOvl(string("ovlTXTManager::AddText: Item with name '")+name+"' already exists");

    counted_array_ptr<wchar_t> newstr(new wchar_t[wcslen(str)+1]);
    wcscpy(newstr.get(), str);
    m_items[string(name)] = newstr;

    // Text
    m_size += (wcslen(str)+1) * 2;

    GetLSRManager()->AddLoader(OVLT_COMMON);
    GetLSRManager()->AddSymbol(OVLT_COMMON);
    GetStringTable()->AddSymbolString(name, Tag());
}

void ovlTXTManager::Make(cOvlInfo* info) {
    Check("ovlFTXManager::Make");
    if (!info)
        throw EOvl("ovlFTXManager::Make called without valid info");

    m_blobs[""] = cOvlMemBlob(OVLT_COMMON, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[""].data;

    for (map<string, counted_array_ptr<wchar_t> >::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        // Data Transfer, Text
        wchar_t* c_txt = reinterpret_cast<wchar_t*>(c_data);
        c_data += (wcslen(it->second.get())+1) * 2;
        wcscpy(c_txt, it->second.get());

        SymbolStruct* s_txt = GetLSRManager()->MakeSymbol(OVLT_COMMON, GetStringTable()->FindSymbolString(it->first.c_str(), Tag()), reinterpret_cast<unsigned long*>(c_txt));
        GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(c_txt), 1, s_txt);
        unsigned char* ex = new unsigned char[4];
        *reinterpret_cast<unsigned long*>(ex) = 1;
        GetLSRManager()->AddExtraData(OVLT_COMMON, 4, ex);
        GetLSRManager()->CloseLoader(OVLT_COMMON);
    }

}
