///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// MS3D Loader
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

#ifndef AN8LOADER_H_INCLUDED
#define AN8LOADER_H_INCLUDED

#include "3DLoader.h"

class cAN8Loader: public c3DLoader
{
public:
    cAN8Loader(const char *filename);
    virtual int GetType() {return C3DLOADER_AN8;};
    virtual c3DLoaderOrientation GetOrientation() {return ORIENTATION_RIGHT_ZUP;};
};

#endif // MS3DLOADER_H_INCLUDED
