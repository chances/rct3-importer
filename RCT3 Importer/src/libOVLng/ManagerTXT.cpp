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

#include "OVLException.h"

const char* ovlTXTManager::LOADER = "FGDK";
const char* ovlTXTManager::NAME = "Text";
const char* ovlTXTManager::TAG = "txt";
const unsigned long ovlTXTManager::TYPE = 2;

void ovlTXTManager::AddText(const char* name, const char* str) {
    Check("ovlTXTManager::AddText");

    m_txtnames.push_back(string(name));
    m_txtstrings.push_back(string(str));

    // Text
    m_size += (strlen(str)+1) * 2;

    GetLSRManager()->AddLoader(OVLT_COMMON);
    GetLSRManager()->AddSymbol(OVLT_COMMON);
    GetStringTable()->AddSymbolString(name, Tag());
}

unsigned char* ovlTXTManager::Make(cOvlInfo* info) {
    Check("ovlFTXManager::Make");
    if (!info)
        throw EOvl("ovlFTXManager::Make called without valid info");

    m_blobs[0] = cOvlMemBlob(OVLT_COMMON, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[0].data;

    for (unsigned long t = 0; t < m_txtnames.size(); ++t) {
        // Data Transfer, Text
        wchar_t* c_txt = reinterpret_cast<wchar_t*>(c_data);
        c_data += (m_txtstrings[t].length()+1) * 2;
        mbstowcs(c_txt, m_txtstrings[t].c_str(), m_txtstrings[t].length()+1);

        SymbolStruct* s_txt = GetLSRManager()->MakeSymbol(OVLT_COMMON, GetStringTable()->FindSymbolString(m_txtnames[t].c_str(), Tag()), reinterpret_cast<unsigned long*>(c_txt));
        GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(c_txt), 1, s_txt);
        unsigned char* ex = new unsigned char[4];
        *reinterpret_cast<unsigned long*>(ex) = 1;
        GetLSRManager()->AddExtraData(OVLT_COMMON, 4, ex);
        GetLSRManager()->CloseLoader(OVLT_COMMON);
    }

    return m_data;
}
