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


#ifndef _OVLDUMPDLL_H_
#define _OVLDUMPDLL_H_

#ifdef OVLDUMP_DLL

#ifdef OVLDUMP_DLL_BUILDING
#define LIBOVLDUMP_API extern "C" __declspec(dllexport)
#else
#define LIBOVLDUMP_API extern "C" __declspec(dllimport)
#endif

#define THEME_ERROR_NOTOPEN -1
#define THEME_ERROR_NOTFOUND -2
#define THEME_ERROR_OTHER -3

#ifdef __cplusplus
extern "C" {
#endif

LIBOVLDUMP_API bool ovlOpen(const char* filename);
LIBOVLDUMP_API bool ovlSave();
LIBOVLDUMP_API bool ovlClose();
LIBOVLDUMP_API const char* ovlGetLastError();
LIBOVLDUMP_API long ovlGetThemeCategory();
LIBOVLDUMP_API long ovlSetThemeCategory(unsigned long newcat);

#ifdef __cplusplus
}
#endif

#endif

#endif
