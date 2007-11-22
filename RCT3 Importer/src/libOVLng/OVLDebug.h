///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Debugging stuff
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


#ifndef OVLDEBUG_H_INCLUDED
#define OVLDEBUG_H_INCLUDED

#ifdef DEBUGLOG
extern FILE* f_debug_log;
extern FILE* f_debug_relocations;

#else
#define INIT_LOGGING()
#define DUMP_RELOCATION(...)
#define DUMP_LOG(...)
#define DEINIT_LOGGING()
#endif

#endif

