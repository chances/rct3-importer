///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Logging helper
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
// Jonathan Wilson) by DragonsIOA.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WXLOCALLOG_H_
#define _WXLOCALLOG_H_

#ifdef __WXDEBUG__
#include "wx_pch.h"
#define wxLocalLog(...) \
    wxLogDebug( __VA_ARGS__ ); \
    wxLog::FlushActive(); \
    wxTheApp->Yield();
#else
#define wxLocalLog(...)
#endif

#endif
