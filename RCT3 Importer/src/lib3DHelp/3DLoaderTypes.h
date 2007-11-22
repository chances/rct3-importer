///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Loader constants
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

#ifndef _3DLOADERTYPES_H_INCLUDED
#define _3DLOADERTYPES_H_INCLUDED

#define C3DLOADER_GENERIC       0
#define C3DLOADER_ASE           1
#define C3DLOADER_MS3D          2

#define C3DMESH_INVALID         0
#define C3DMESH_VALID           1

enum {
    ORIENTATION_UNKNOWN = 0L,
    ORIENTATION_RIGHT_XUP,
    ORIENTATION_RIGHT_YUP,
    ORIENTATION_RIGHT_ZUP,
    ORIENTATION_LEFT_XUP,
    ORIENTATION_LEFT_YUP,
    ORIENTATION_LEFT_ZUP
};

typedef unsigned int c3DLoaderOrientation;

#endif // 3DLOADERTYPES_H_INCLUDED
