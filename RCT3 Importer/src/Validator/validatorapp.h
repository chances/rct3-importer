///////////////////////////////////////////////////////////////////////////////
//
// Schema Validator
// Validator for RelaxNG and Schematron
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

#ifndef _VALIDATORAPP_H_
#define _VALIDATORAPP_H_

#include <wx/app.h>
#include <wx/filename.h>

#include "cXmlInitHandler.h"
#include "wxXmlInputCallbackFileSystem.h"
#include "validatormain.h"

class ValidatorApp : public wxApp, public xmlcpp::cXmlInitHandler {
private:
    wxFileName m_appdir;
    wxString m_title;
    std::map<wxString, wxString> m_abbreviations;
    std::map<wxString, wxString> m_mappings;

public:
    virtual bool OnInit();
    virtual int OnRun();
    virtual int OnExit();

    typedef std::map<wxString, wxString>::iterator mapfind;

    inline const wxFileName& getAppDir() const              { return m_appdir; }
    inline const wxString& getTitle() const                 { return m_title; }
    inline std::map<wxString, wxString>& getAbbreviations() { return m_abbreviations; }
    inline std::map<wxString, wxString>& getMappings()      { return m_mappings; }
};

DECLARE_APP(ValidatorApp)

#endif
