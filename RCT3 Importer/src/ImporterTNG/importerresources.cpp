///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Importer TNG
// Next generation RCT3 Importer
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

#include "importerresourcefiles.h"
#include "resources/about.html.h"
#include "resources/gpl-3.0-standalone.html.h"
#include "resources/crystal/info.png.h"
#include "resources/crystal/status_unknown.png.h"
#include "resources/crystal/stop.png.h"
#include "resources/xml/template-project.xml.h"

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

    wxMemoryFSHandler::AddFile(wxT(RES_HTML_ABOUT), about_html, sizeof(about_html));
    wxMemoryFSHandler::AddFile(wxT(RES_HTML_GPL3), gpl_3_0_standalone_html, sizeof(gpl_3_0_standalone_html));

    wxMemoryFSHandler::AddFile(wxT(RES_IMAGE_INFO), info_png, sizeof(info_png));
    wxMemoryFSHandler::AddFile(wxT(RES_IMAGE_WARNING), status_unknown_png, sizeof(status_unknown_png));
    wxMemoryFSHandler::AddFile(wxT(RES_IMAGE_ERROR), stop_png, sizeof(stop_png));

    wxMemoryFSHandler::AddFile(wxT(RES_TEMPLATE_PROJECT), template_project_xml, sizeof(template_project_xml));

    //InitXmlImages();
}
