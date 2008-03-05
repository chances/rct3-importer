///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Importer TNG
// Custom object importer for Ataris Roller Coaster Tycoon 3
// Copyright (C) 2008 Belgabor (Tobias Minich)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, <http://www.gnu.org/licenses/>.
//
// Written by
//   Belgabor (Tobias Minich) - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#ifndef IMPTYPES_H_INCLUDED
#define IMPTYPES_H_INCLUDED

namespace impTypes {
    enum COMMAND {
        COMMAND_NOTHING,
        COMMAND_NEW,
        COMMAND_VIEW,
        COMMAND_CONTEXT
    };
    enum ELEMENTTYPE {
        ELEMENTTYPE_NONE,
        ELEMENTTYPE_PROJECT,
        ELEMENTTYPE_CATEGORY_SCENERY,
        ELEMENTTYPE_CATEGORY_TEXTURE,
        ELEMENTTYPE_CATEGORY_ALL,
        ELEMENTTYPE_OVL_SCENERY,
        ELEMENTTYPE_OVL_TEXTURE,

        ELEMENTTYPE_END
    };
    enum VIEWTYPE {
        VIEWTYPE_ABSTRACT =     0,
        VIEWTYPE_TREE =         1,
        VIEWTYPE_PROJECT =      2,
        VIEWTYPE_REFERENCES =   3,
    };

}

#endif // IMPTYPES_H_INCLUDED
