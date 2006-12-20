///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Validator to check texture bitmap filenames
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
///////////////////////////////////////////////////////////////////////////////

#ifndef WXTEXTUREVALIDATOR_H_INCLUDED
#define WXTEXTUREVALIDATOR_H_INCLUDED

#include "wx_pch.h"

#include <wx/filename.h>

#include "valext.h"

class wxTextureValidator: public wxExtendedValidator {
DECLARE_DYNAMIC_CLASS(wxTextureValidator)
protected:
    wxString m_context;
    unsigned int *m_forceSize;
    bool m_alphaNecessary;
public:
    wxTextureValidator():wxExtendedValidator(),m_context(wxT("")){};
    wxTextureValidator(wxString *val, const wxString& context = wxT(""), bool empty_allowed = false, bool alpha_necessary = false, unsigned int *force_size = NULL);
    wxTextureValidator(wxFileName *val, const wxString& context = wxT(""), bool empty_allowed = false, bool alpha_necessary = false, unsigned int *force_size = NULL);
    wxTextureValidator(const wxTextureValidator& val);

    virtual ~wxTextureValidator(){}

    virtual wxObject *Clone() const { return new wxTextureValidator(*this); }
    bool Copy(const wxTextureValidator& val);

    virtual bool Validate(wxWindow *parent);
private:
    wxTextureValidator& operator=(const wxTextureValidator&);

};

#endif // WXTEXTUREVALIDATOR_H_INCLUDED
