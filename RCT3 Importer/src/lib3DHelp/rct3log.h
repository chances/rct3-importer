///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Original log2 function of the importer
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
// Parts of this code are based on code written for rct3tool (Copyright 2005
// Jonathan Wilson).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef RCT3LOG_H_INCLUDED
#define RCT3LOG_H_INCLUDED

inline unsigned long local_log2(unsigned long y) {
    unsigned long result = 0;
    while (y) {
        result++;
        y >>= 1;
    }
    return result - 1;
}

#endif // RCT3LOG_H_INCLUDED
