///////////////////////////////////////////////////////////////////////////////
//
// OVL dumping library
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
// Based on the ovl dumper by Jonathan Wilson
//
///////////////////////////////////////////////////////////////////////////////


#ifdef OVLDUMP_DLL

#include "OVLDumpDLL.h"
#include "OVLDump.h"
#include "OVLDException.h"

#include <exception>
#include <string>

using namespace std;

string g_lasterror = "";
bool g_ovlopen = false;
cOVLDump g_ovl;

LIBOVLDUMP_API bool ovlOpen(const char* filename) {
    if (g_ovlopen)
        g_ovl = cOVLDump();
    try {
        g_ovl.Load(filename);
        g_ovlopen = true;
        return true;
    } catch (exception& e) {
        g_ovl = cOVLDump();
        g_ovlopen = false;
        g_lasterror = e.what();
        return false;
    }
}

LIBOVLDUMP_API bool ovlSave() {
    if (!g_ovlopen) {
        g_lasterror = "No ovl file opened.";
        return false;
    }
    try {
        g_ovl.Save();
        return true;
    } catch (exception& e) {
        g_lasterror = e.what();
        return false;
    }
}

LIBOVLDUMP_API bool ovlClose() {
    if (!g_ovlopen) {
        g_lasterror = "No ovl file opened.";
        return false;
    }
    g_ovl = cOVLDump();
    g_ovlopen = false;
    return true;
}

LIBOVLDUMP_API const char* ovlGetLastError() {
    return g_lasterror.c_str();
}

LIBOVLDUMP_API long ovlGetThemeCategory() {
    if (!g_ovlopen) {
        g_lasterror = "No ovl file opened.";
        return -1;
    }
    try {
        try {
            unsigned long cindex = g_ovl.FindSymbol(OVLT_COMMON, "StyleIndex:int");
            return reinterpret_cast<unsigned long>(g_ovl.GetSymbol(OVLT_COMMON, cindex).orgsymbol->data);
        } catch (EOvlD) {
            try {
                unsigned long uindex = g_ovl.FindSymbol(OVLT_UNIQUE, "StyleIndex:int");
                return reinterpret_cast<unsigned long>(g_ovl.GetSymbol(OVLT_UNIQUE, uindex).orgsymbol->data);
            } catch (EOvlD) {
                g_lasterror = "Theme category symbol not found";
                return -2;
            }
        }
    } catch (exception& e) {
        g_lasterror = e.what();
        return -3;
    }
}

LIBOVLDUMP_API long ovlSetThemeCategory(unsigned long newcat) {
    if (!g_ovlopen) {
        g_lasterror = "No ovl file opened.";
        return -1;
    }
    try {
        try {
            unsigned long cindex = g_ovl.FindSymbol(OVLT_COMMON, "StyleIndex:int");
            g_ovl.SetSymbolData(OVLT_COMMON, cindex, newcat);
            return 0;
        } catch (EOvlD) {
            try {
                unsigned long uindex = g_ovl.FindSymbol(OVLT_UNIQUE, "StyleIndex:int");
                g_ovl.SetSymbolData(OVLT_UNIQUE, uindex, newcat);
                return 0;
            } catch (EOvlD& e) {
                g_lasterror = e.swhat();
                return -2;
            }
        }
    } catch (exception& e) {
        g_lasterror = e.what();
        return -3;
    }
}

#endif

