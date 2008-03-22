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

#ifndef VALIDATORMAIN_H_INCLUDED
#define VALIDATORMAIN_H_INCLUDED

#include <boost/shared_ptr.hpp>
#include <wx/stc/stc.h>

#include "resources/res_main.h"

class frmMain: public rcfrmMain {
public:
    frmMain(wxWindow* parent);
    virtual ~frmMain();

    virtual void OnMenuLoad( wxCommandEvent& event );
    virtual void OnSchema( wxCommandEvent& event );
    virtual void OnSTCMarginClick (wxStyledTextEvent &event);

private:
    boost::shared_ptr<wxFileDialog> m_fdlgSchema;
    boost::shared_ptr<wxFileDialog> m_fdlgFile;

};

#endif // MANAGERMAIN_H_INCLUDED
