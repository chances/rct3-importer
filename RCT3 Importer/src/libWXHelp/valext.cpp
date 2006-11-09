/////////////////////////////////////////////////////////////////////////////
// Name:        valext.cpp
// Purpose:     wxExtendedValidator class
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "valext.h"

#include <wx/file.h>

IMPLEMENT_CLASS(wxExtendedValidator, wxValidator)

wxExtendedValidator::wxExtendedValidator(float* val, unsigned int precision, bool zero_allowed, bool use_default, float default_val) {
    Initialize();
    m_pFloat = val;
    m_precision = precision;
    m_zeroAllowed = zero_allowed;
    m_useDefault = use_default;
    m_floatDefault = default_val;
}

wxExtendedValidator::wxExtendedValidator(double* val, unsigned int precision, bool zero_allowed, bool use_default, double default_val) {
    Initialize();
    m_pDouble = val;
    m_precision = precision;
    m_zeroAllowed = zero_allowed;
    m_useDefault = use_default;
    m_floatDefault = default_val;
}

wxExtendedValidator::wxExtendedValidator(unsigned int* val, bool zero_allowed, bool use_default, unsigned int default_val) {
    Initialize();
    m_pUInt = val;
    m_zeroAllowed = zero_allowed;
    m_useDefault = use_default;
    m_uintDefault = default_val;
}

wxExtendedValidator::wxExtendedValidator(unsigned long* val, bool zero_allowed, bool use_default, unsigned long default_val) {
    Initialize();
    m_pULong = val;
    m_zeroAllowed = zero_allowed;
    m_useDefault = use_default;
    m_uintDefault = default_val;
}

wxExtendedValidator::wxExtendedValidator(wxString* val, bool empty_allowed, bool check_exist) {
    Initialize();
    m_pString = val;
    m_zeroAllowed = empty_allowed;
    m_useDefault = !check_exist; // Negate it to make the first general check in Validate work.
}

wxExtendedValidator::wxExtendedValidator(wxFileName* val, bool empty_allowed, bool check_exist) {
    Initialize();
    m_pFileName = val;
    m_zeroAllowed = empty_allowed;
    m_useDefault = !check_exist; // Negate it to make the first general check in Validate work.
}

wxExtendedValidator::wxExtendedValidator(const wxExtendedValidator& copyFrom):wxValidator() {
    Copy(copyFrom);
}

bool wxExtendedValidator::Copy(const wxExtendedValidator& val) {
    wxValidator::Copy(val);

    m_pFloat = val.m_pFloat;
    m_pDouble = val.m_pDouble;
    m_pUInt = val.m_pUInt;
    m_pULong = val.m_pULong;
    m_pString = val.m_pString;
    m_pFileName = val.m_pFileName;

    m_precision = val.m_precision;
    m_zeroAllowed = val.m_zeroAllowed;
    m_useDefault = val.m_useDefault;
    m_floatDefault = val.m_floatDefault;
    m_uintDefault = val.m_uintDefault;

    return true;
}

void wxExtendedValidator::Initialize() {
    m_pFloat = NULL;
    m_pDouble = NULL;
    m_pUInt = NULL;
    m_pULong = NULL;
    m_pString = NULL;
    m_pFileName = NULL;

    m_precision = 0;
    m_zeroAllowed = true;
    m_useDefault = true;
    m_floatDefault = 0.0;
    m_uintDefault = 0;
}

bool wxExtendedValidator::Validate(wxWindow* parent) {
    if ( !CheckValidator() )
        return false;

    if (m_zeroAllowed && m_useDefault) {
        // Validation cannot fail
        return true;
    }

    wxTextCtrl* pControl = (wxTextCtrl*) m_validatorWindow;

    if (m_pFloat || m_pDouble) {
        double test;

        if (!(pControl->GetValue().ToDouble(&test))) {
            // Conversion failed
            if (!m_useDefault) {
                // Not using default, validation error
                m_validatorWindow->SetFocus();
                ::wxMessageBox(wxT("'")+pControl->GetValue()+_("' is not a floating point number."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                return false;
            }
        } else {
            if ((!m_zeroAllowed) && (test == 0.0)) {
                // Zero not allowed
                m_validatorWindow->SetFocus();
                ::wxMessageBox(_("You may not enter zero here."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                return false;
            }
        }
    } else if (m_pUInt || m_pULong) {
        unsigned long test;

        if (!(pControl->GetValue().ToULong(&test))) {
            // Conversion failed
            if (!m_useDefault) {
                // Not using default, validation error
                m_validatorWindow->SetFocus();
                ::wxMessageBox(wxT("'")+pControl->GetValue()+_("' is not an integer number."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                return false;
            }
        } else {
            if ((!m_zeroAllowed) && (test == 0)) {
                // Zero not allowed
                m_validatorWindow->SetFocus();
                ::wxMessageBox(_("You may not enter zero here."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                return false;
            }
        }
    } else if (m_pString) {
        wxString test = pControl->GetValue();
        if ((!m_zeroAllowed) && (test == wxT(""))) {
            // Empty not allowed
            m_validatorWindow->SetFocus();
            ::wxMessageBox(_("This field may not be empty."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
            return false;
        }
        // If we arrive here, we have a value or it's allowed to have none
        if (!m_useDefault) {
            // We check if the file exists, but only if the value is non-empty
            // If it's empty at this stage, we interpret it as "either a valid file or nothing"
            if (test != wxT("")) {
                if (!::wxFileExists(test)) {
                    // File does not exist.
                    m_validatorWindow->SetFocus();
                    ::wxMessageBox(_("This file does not exist."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                    return false;
                }
            }
        }
    } else if (m_pFileName) {
        wxFileName test = pControl->GetValue();
        if ((!m_zeroAllowed) && (test == wxT(""))) {
            // Empty not allowed
            m_validatorWindow->SetFocus();
            ::wxMessageBox(_("This field may not be empty."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
            return false;
        }
        // If we arrive here, we have a value or it's allowed to have none
        if (!m_useDefault) {
            // We check if the file exists, but only if the value is non-empty
            // If it's empty at this stage, we interpret it as "either a valid file or nothing"
            if (test != wxT("")) {
                if (!::wxFileExists(test.GetFullPath())) {
                    // File does not exist.
                    m_validatorWindow->SetFocus();
                    ::wxMessageBox(_("This file does not exist."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                    return false;
                }
            }
        }
    }

    return true;
}

bool wxExtendedValidator::TransferToWindow(void) {
    if ( !CheckValidator() )
        return false;

    wxTextCtrl *pControl = (wxTextCtrl *) m_validatorWindow;
    if ( m_pFloat || m_pDouble ) {
        pControl->SetValue(wxString::Format(wxT("%.*f"), m_precision, (m_pFloat)?(*m_pFloat):(*m_pDouble)));
    } else if (m_pUInt) {
        pControl->SetValue(wxString::Format(wxT("%u"), *m_pUInt));
    } else if (m_pULong) {
        pControl->SetValue(wxString::Format(wxT("%lu"), *m_pULong));
    } else if (m_pString) {
        pControl->SetValue(*m_pString);
    } else if (m_pFileName) {
        pControl->SetValue(m_pFileName->GetFullPath());
    }

    return true;
}

bool wxExtendedValidator::TransferFromWindow(void) {
    if ( !CheckValidator() )
        return false;

    wxTextCtrl *pControl = (wxTextCtrl *) m_validatorWindow;
    if ( m_pFloat || m_pDouble ) {
        double test;

        if (!(pControl->GetValue().ToDouble(&test))) {
            // Conversion failed
            if (!m_useDefault) {
                // Not using default, validation error
                return false;
            } else {
                test = m_floatDefault;
            }
        } else {
            if ((!m_zeroAllowed) && (test == 0.0)) {
                // Zero not allowed
                return false;
            }
        }
        if (m_pFloat) {
            *m_pFloat = test;
        } else {
            *m_pDouble = test;
        }
    } else if (m_pUInt || m_pULong) {
        unsigned long test;

        if (!(pControl->GetValue().ToULong(&test))) {
            // Conversion failed
            if (!m_useDefault) {
                // Not using default, validation error
                return false;
            } else {
                test = m_uintDefault;
            }
        } else {
            if ((!m_zeroAllowed) && (test == 0)) {
                // Zero not allowed
                return false;
            }
        }
        if (m_pUInt) {
            *m_pUInt = test;
        } else {
            *m_pULong = test;
        }
    } else if (m_pString) {
        wxString test = pControl->GetValue();
        if ((!m_zeroAllowed) && (test == wxT(""))) {
            // Empty not allowed
            return false;
        }
        // If we arrive here, we have a value or it's allowed to have none
        if (!m_useDefault) {
            // We check if the file exists, but only if the value is non-empty
            // If it's empty at this stage, we interpret it as "either a valid file or nothing"
            if (test != wxT("")) {
                if (!::wxFileExists(test)) {
                    // File does not exist.
                    return false;
                }
            }
        }
        *m_pString = test;
    } else if (m_pFileName) {
        wxFileName test = pControl->GetValue();
        if ((!m_zeroAllowed) && (test == wxT(""))) {
            // Empty not allowed
            return false;
        }
        // If we arrive here, we have a value or it's allowed to have none
        if (!m_useDefault) {
            // We check if the file exists, but only if the value is non-empty
            // If it's empty at this stage, we interpret it as "either a valid file or nothing"
            if (test != wxT("")) {
                if (!::wxFileExists(test.GetFullPath())) {
                    // File does not exist.
                    return false;
                }
            }
        }
        *m_pFileName = test;
    }

    return true;
}

