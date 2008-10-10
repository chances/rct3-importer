///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Strings
// Copyright (C) 2006 Tobias Minch
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
///////////////////////////////////////////////////////////////////////////////

#ifndef _3DSTRING_H_INCLUDED
#define _3DSTRING_H_INCLUDED

#ifdef STD_ONLY

#include <string>
#include <vector>
#define STRING3D std::string
#define STRING3D_EMPTY ""
#define STRINGLIST3D std::vector<std::string>

#else

#define STRING3D wxString
#define STRING3D_EMPTY wxT("")
#define STRINGLIST3D wxArrayString

#endif

#endif // 3DSTRING_H_INCLUDED
