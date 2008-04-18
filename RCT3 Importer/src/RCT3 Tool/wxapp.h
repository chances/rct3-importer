///////////////////////////////////////////////////////////////////////////////
//
// wxWidgets App
// Copyright (C) 2006 Tobias Minch
//
// Part of rct3tool
// Copyright 2005 Jonathan Wilson
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

#ifndef WXAPP_H_INCLUDED
#define WXAPP_H_INCLUDED

#include "wx_pch.h"

#include <wx/app.h>
#include <wx/filename.h>

#include <list>

#include "xrc/xrcres.h"
#include "resources.h"
#include "rct3config.h"

class ToolApp : public wxApp
{
	public:
        std::list<wxString> g_texturecache;
        wxFileName g_workdir;
//        RGBQUAD g_recolpalette_bmy[256];
        RGBQUAD g_recolpalette_rgb[256];

		virtual bool OnInit();
		virtual int OnRun();
		//RCT3Config* GetConfig();
};

DECLARE_APP(ToolApp)

#endif // WXAPP_H_INCLUDED
