///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Customization Manager
// Installer for RCT3 customizations
// Copyright (C) 2008 Tobias Minch
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
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/filesys.h>
#include <wx/fs_mem.h>
//#include <wx/xrc/xmlres.h>
//#include <wx/xrc/xh_all.h>

//#include "crystal\icons\filenew_16x16.png.h"

#include "managerresourcefiles.h"
#include "resources/about.html.h"
#include "resources/gpl-3.0-standalone.html.h"
#include "resources/GUID-List-Authors.xml.h"
#include "resources/GUID-List-Sets.xml.h"
//#include "xrc_images.h"
#include "images\own\minus_10x10.png.h"
#include "images\own\plus_10x10.png.h"

void InitResources()
{

    // Check for memory FS. If not present, load the handler:
    {
        wxMemoryFSHandler::AddFile(wxT("resources/dummy_file"), wxT("dummy one"));
        wxFileSystem fsys;
        wxFSFile *f = fsys.OpenFile(wxT("memory:resources/dummy_file"));
        wxMemoryFSHandler::RemoveFile(wxT("resources/dummy_file"));
        if (f) delete f;
        else wxFileSystem::AddHandler(new wxMemoryFSHandler);
    }

    wxMemoryFSHandler::AddFile(RES_HTML_ABOUT, about_html, sizeof(about_html));
    wxMemoryFSHandler::AddFile(RES_HTML_GPL3, gpl_3_0_standalone_html, sizeof(gpl_3_0_standalone_html));
    wxMemoryFSHandler::AddFile(RES_XML_AUTHORS, GUID_List_Authors_xml, sizeof(GUID_List_Authors_xml));
    wxMemoryFSHandler::AddFile(RES_XML_PACKAGES, GUID_List_Sets_xml, sizeof(GUID_List_Sets_xml));
    wxMemoryFSHandler::AddFile(RES_IMAGE_MINUS, minus_10x10_png, sizeof(minus_10x10_png));
    wxMemoryFSHandler::AddFile(RES_IMAGE_PLUS, plus_10x10_png, sizeof(plus_10x10_png));

    //InitXmlImages();
}
