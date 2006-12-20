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

#include "wx_pch.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include "valtexture.h"
#include "wxdevil.h"
#include "rct3log.h"

IMPLEMENT_DYNAMIC_CLASS(wxTextureValidator, wxExtendedValidator)

wxTextureValidator::wxTextureValidator(wxString *val, const wxString& context, bool empty_allowed, bool alpha_necessary, unsigned int *force_size):wxExtendedValidator(val, empty_allowed, true) {
    m_context = context;
    m_forceSize = force_size;
    m_alphaNecessary = alpha_necessary;
}

wxTextureValidator::wxTextureValidator(wxFileName *val, const wxString& context, bool empty_allowed, bool alpha_necessary, unsigned int *force_size):wxExtendedValidator(val, empty_allowed, true) {
    m_context = context;
    m_forceSize = force_size;
    m_alphaNecessary = alpha_necessary;
}

wxTextureValidator::wxTextureValidator(const wxTextureValidator &val):wxExtendedValidator() {
    Copy(val);
}

bool wxTextureValidator::Copy(const wxTextureValidator& val) {
    m_context = val.m_context;
    m_forceSize = val.m_forceSize;
    m_alphaNecessary = val.m_alphaNecessary;
    return wxExtendedValidator::Copy(val);
}

bool wxTextureValidator::Validate(wxWindow *parent) {
    if ( !wxExtendedValidator::Validate(parent) ) {
        return false;
    }

    if ( !m_validatorWindow->IsEnabled() )
        return true;

    wxString val = wxT("");

    if (dynamic_cast<wxTextCtrl*>(m_validatorWindow)) {
        val = (dynamic_cast<wxTextCtrl*>(m_validatorWindow))->GetValue();
    } else if (dynamic_cast<wxComboCtrl*>(m_validatorWindow)) {
        val = (dynamic_cast<wxComboCtrl*>(m_validatorWindow))->GetValue();
    } else {
        return false;
    }

    if (m_zeroAllowed && (val == wxT("")))
        return true;

    bool ok = true;

    wxString errormsg;
//    ILenum Error;

    ILinfo inf;
    bool valid_file = getBitmapInfo(val.fn_str(), inf);
/*
    wxMutexLocker lock (wxILMutex);
    ILuint img = ilGenImage();
    ILuint old = ilGetInteger(IL_ACTIVE_IMAGE);
    ilBindImage(img);
*/
    if (!valid_file) {
        ok = false;
        errormsg = _("Error loading file '%s'.");
/*
        while ((Error = ilGetError())) {
            errormsg += wxT("\n");
            errormsg += iluErrorString(Error);
        }
*/
    } else {
/*
        int width = ilGetInteger(IL_IMAGE_WIDTH);
        int height = ilGetInteger(IL_IMAGE_HEIGHT);
*/
        if (inf.Width != inf.Height) {
            ok = false;
            errormsg = _("Texture '%s' is not square, ");
            errormsg += wxString::Format(_("it has %dx%d pixels."), inf.Width, inf.Height);
        } else if ((1 << local_log2(inf.Width)) != inf.Width) {
            ok = false;
            errormsg = _("The width/height of texture '%s' is not a power of 2.");
        } else {
            if (m_forceSize) {
                if ((*m_forceSize > 0) && (*m_forceSize != inf.Width)) {
                    ok = false;
                    errormsg = _("The width/height of texture '%s' has to be ") + wxString::Format("%d", *m_forceSize) + wxT(".");
                }
            }
            if (ok && m_alphaNecessary) {
                if (!ilInfoHasAlpha(inf)) {
                    ok = false;
                    errormsg = _("Texture '%s' requires an alpha channel.");
                }
            }
        }
    }
/*
    ilBindImage(old);
    ilDeleteImage(img);
*/

    if ((!ok) && (!m_silent)) {
        wxASSERT_MSG( !errormsg.empty(), _T("you forgot to set errormsg") );

        m_validatorWindow->SetFocus();

        wxString buf;
        buf.Printf(errormsg, val.c_str());

        wxString title;
        title.Printf(_("Validation conflict (%s)"), m_context.c_str());

        wxMessageBox(buf, title,
                     wxOK | wxICON_EXCLAMATION, parent);
    }

    return ok;
}
