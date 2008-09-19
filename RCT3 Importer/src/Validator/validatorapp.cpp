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

#include "wx_pch.h"
#include "validatorapp.h"

#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/fs_arc.h>
#include <wx/fs_filter.h>
#include <wx/fs_mem.h>
#include <wx/link.h>
//#include <wx/xrc/xmlres.h>
//#include <wx/xrc/xh_all.h>

#include "xmldefs.h"

#include "bzipstream.h"
#include "cXmlDoc.h"
#include "cXmlNode.h"
#include "wxXmlInputCallbackFileSystem.h"

#include "validatormain.h"

#include "version.h"

IMPLEMENT_APP(ValidatorApp);

wxFORCE_LINK_MODULE(bzipstream)

bool ValidatorApp::OnInit() {
	::wxInitAllImageHandlers();
    //wxXmlResource::Get()->InitAllHandlers();
    wxFileSystem::AddHandler(new wxArchiveFSHandler);
    wxFileSystem::AddHandler(new wxFilterFSHandler);
    wxFileSystem::AddHandler(new wxMemoryFSHandler);

    wxBZipClassFactory cf;

    xmlcpp::wxXmlInputCallbackFileSystem::Init();

    m_appdir = wxFileName(argv[0]).GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
    m_title = wxString::Format(_("%s v%ld.%ld%s (Build %ld)"),
        wxT("Schema Validator"), AutoVersion::MAJOR, AutoVersion::MINOR,
        wxString(AutoVersion::STATUS_SHORT, wxConvLocal).c_str(), AutoVersion::BUILD);

    m_abbreviations["xml"] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n|";
    m_abbreviations["rawr"] = "<rawovl basedir=\"$(basedir:output\\)\" installdir=\"$(installdir)\" xmlns=\"http://rct3.sourceforge.net/rct3xml/raw\">\n\t|\n</rawovl>\n";
    m_abbreviations["rawb"] = "<rawovl basedir=\"$(basedir)\">\n\t|\n</rawovl>\n";
    m_abbreviations["raw"] = "<rawovl file=\"$(file:|)\">\n\t|\n</rawovl>\n";
    m_abbreviations["banf"] = "<ban name=\"$(name:|)\" modelFile=\"$(Model file;Model Files|*.modxml;*.ms3d)\" />\n|";
    m_abbreviations["splf"] = "<spl name=\"$(name:|)\" modelFile=\"$(Model file;Model Files|*.modxml)\" />\n|";
    m_abbreviations["iimport"] = "<import file=\"$(Scenery file to import;Scenery Files|*.xml;*.modxml;*.ms3d;*.scn)\" name=\"$(name:|)\" id=\"mainimport\"/>\n|";
    m_abbreviations["simport"] = "<import file=\"$(Scenery file to import;Scenery Files|*.xml;*.modxml;*.ms3d;*.scn)\" use=\"splines\" id=\"$(id:|)\"/>\n|";
    m_abbreviations["import"] = "<import file=\"$(Scenery file to import;Scenery Files|*.xml;*.modxml;*.ms3d;*.scn)\" name=\"$(name:|)\"/>\n|";
    m_abbreviations["ftx"] = "<ftx name=\"$(name:|)\">\n\t<ftxframe>\n\t\t<image usealpha=\"$(alpha:0)\">$(texture;Image files|*.png;*.bmp;*.jpg;*.jpeg|All files|*.*)</image>\n\t</ftxframe>\n\t<ftxanimation index=\"0\"/>\n</ftx>\n|";
    m_abbreviations["sid"] = "<sid name=\"$(name:|)\" nametxt=\"$(name)\" icon=\"$(name)\" ovlpath=\"$(ovlpath:Style\\Custom\\)\" svd=\"$(name)\">\n\t<position xsize=\"4\" ysize=\"4\" zsize=\"4\" xsquares=\"1\" zsquares=\"1\"/>\n</sid>\n|";
    m_abbreviations["tex"] = "<tex name=\"$(name:|)\" format=\"20\">\n\t<texture><data type=\"file\">$(texture;Image files|*.png;*.bmp;*.jpg;*.jpeg|All files|*.*)</data></texture>\n</tex>\n|";
    m_abbreviations["gsi"] = "<gsi name=\"$(name:|)\" tex=\"$(tex)\" left=\"$(left:0)\" top=\"$(top:0)\" right=\"$(right:64)\" bottom=\"$(bottom:64)\"/>\n|";
    m_abbreviations["txt"] = "<txt name=\"$(name:|)\" type=\"text\">|</txt>\n";
    m_abbreviations["ref"] = "<reference>|</reference>\n";

    m_mappings["ns:" XML_NAMESPACE_SCENERY] = "??Scenery";
    m_mappings["ns:" XML_NAMESPACE_RAW] = "??Raw";
    m_mappings["ns:" XML_NAMESPACE_COMPILER] = "??Compiler";
    m_mappings["ns:" XML_NAMESPACE_MODEL] = "??Model";
    m_mappings["root:ms3d"] = "??MS3D";

    frmMain* main = new frmMain(NULL);
    main->Show(true);
    SetTopWindow(main);

    return true;

}

int ValidatorApp::OnRun() {
    #ifdef __WXDEBUG__
    wxLogWindow log(NULL, wxT("Log"));
    #endif

    return wxApp::OnRun();
}

int ValidatorApp::OnExit() {
    return 0;
}

