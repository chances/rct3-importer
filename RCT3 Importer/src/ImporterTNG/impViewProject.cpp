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

#include "impViewProject.h"

#include "impControlText.h"
#include "importerxpaths.h"

impViewProject::impViewProject(wxWindow* parent): rcpanViewProject(parent) {
    initInfo(wxT("/"));
    m_textName->init(*this, wxT(XPATH_PROJECT_NAME));
    m_textName->validator(impValidatorAscii(true));
    m_textOutput->init(*this, wxT("/r:" XML_TAG_OVL "[@role='project']/@basedir"));
    m_textInstall->init(*this, wxT("/r:" XML_TAG_OVL "[@role='project']/@installdir"));
}

impViewProject::~impViewProject() {
    //dtor
}

wxString impViewProject::getName() {
    return _("Project settings");
}

void impViewProject::update(const wxString& value) {
    update_controls(value);
}
