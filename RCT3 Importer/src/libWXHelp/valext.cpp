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

#ifndef WXHELP_BASE

#include "valext.h"

#include <wx/file.h>

IMPLEMENT_CLASS(wxExtendedValidator, wxValidator)

wxExtendedValidator::wxExtendedValidator(float* val, unsigned int precision, bool zero_allowed, bool use_default, float default_val):wxSilent() {
    Initialize();
    m_pFloat = val;
    m_precision = precision;
    m_zeroAllowed = zero_allowed;
    m_useDefault = use_default;
    m_floatDefault = default_val;
}

wxExtendedValidator::wxExtendedValidator(double* val, unsigned int precision, bool zero_allowed, bool use_default, double default_val):wxSilent() {
    Initialize();
    m_pDouble = val;
    m_precision = precision;
    m_zeroAllowed = zero_allowed;
    m_useDefault = use_default;
    m_floatDefault = default_val;
}

wxExtendedValidator::wxExtendedValidator(unsigned char* val, bool zero_allowed, bool use_default, unsigned int default_val):wxSilent() {
    Initialize();
    m_pUChar = val;
    m_zeroAllowed = zero_allowed;
    m_useDefault = use_default;
    m_uintDefault = default_val;
}

wxExtendedValidator::wxExtendedValidator(unsigned int* val, bool zero_allowed, bool use_default, unsigned int default_val):wxSilent() {
    Initialize();
    m_pUInt = val;
    m_zeroAllowed = zero_allowed;
    m_useDefault = use_default;
    m_uintDefault = default_val;
}

wxExtendedValidator::wxExtendedValidator(unsigned long* val, bool zero_allowed, bool use_default, unsigned long default_val):wxSilent() {
    Initialize();
    m_pULong = val;
    m_zeroAllowed = zero_allowed;
    m_useDefault = use_default;
    m_uintDefault = default_val;
}

wxExtendedValidator::wxExtendedValidator(wxString* val, bool empty_allowed, bool check_exist):wxSilent() {
    Initialize();
    m_pString = val;
    m_zeroAllowed = empty_allowed;
    m_useDefault = !check_exist; // Negate it to make the first general check in Validate work.
}

wxExtendedValidator::wxExtendedValidator(wxFileName* val, bool empty_allowed, bool check_exist, bool dir):wxSilent() {
    Initialize();
    m_pFileName = val;
    m_zeroAllowed = empty_allowed;
    m_useDefault = !check_exist; // Negate it to make the first general check in Validate work.
    m_isDir = dir;
}

wxExtendedValidator::wxExtendedValidator(const wxExtendedValidator& copyFrom):wxValidator(),wxSilent() {
    Copy(copyFrom);
}

bool wxExtendedValidator::Copy(const wxExtendedValidator& val) {
    wxValidator::Copy(val);
    wxSilent::Copy(val);

    m_pFloat = val.m_pFloat;
    m_pDouble = val.m_pDouble;
    m_pUChar = val.m_pUChar;
    m_pUInt = val.m_pUInt;
    m_pULong = val.m_pULong;
    m_pString = val.m_pString;
    m_pFileName = val.m_pFileName;

    m_precision = val.m_precision;
    m_zeroAllowed = val.m_zeroAllowed;
    m_useDefault = val.m_useDefault;
    m_floatDefault = val.m_floatDefault;
    m_uintDefault = val.m_uintDefault;
    m_isDir = val.m_isDir;

    return true;
}

void wxExtendedValidator::Initialize() {
    m_pFloat = NULL;
    m_pDouble = NULL;
    m_pUChar = NULL;
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
    if (!CheckValidator()) {
        return false;
    }

    if (m_zeroAllowed && m_useDefault) {
        // Validation cannot fail
        return true;
    }

    wxString value = wxT("");

    if (dynamic_cast<wxTextCtrl*>(m_validatorWindow)) {
        value = (dynamic_cast<wxTextCtrl*>(m_validatorWindow))->GetValue();
    } else if (dynamic_cast<wxComboCtrl*>(m_validatorWindow)) {
        value = (dynamic_cast<wxComboCtrl*>(m_validatorWindow))->GetValue();
    } else if (dynamic_cast<wxComboBox*>(m_validatorWindow)) {
        value = (dynamic_cast<wxComboBox*>(m_validatorWindow))->GetValue();
    } else if (dynamic_cast<wxChoice*>(m_validatorWindow)) {
        value = (dynamic_cast<wxChoice*>(m_validatorWindow))->GetStringSelection();
    } else {
        return false;
    }

    if (m_pFloat || m_pDouble) {
        double test;

        if (!(value.ToDouble(&test))) {
            // Conversion failed
            if (!m_useDefault) {
                // Not using default, validation error
                if (!m_silent) {
                    m_validatorWindow->SetFocus();
                    ::wxMessageBox(wxT("'")+value+_("' is not a floating point number."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                }
                return false;
            }
        } else {
            if ((!m_zeroAllowed) && (test == 0.0)) {
                // Zero not allowed
                if (!m_silent) {
                    m_validatorWindow->SetFocus();
                    ::wxMessageBox(_("You may not enter zero here."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                }
                return false;
            }
        }
    } else if (m_pUChar || m_pUInt || m_pULong) {
        unsigned long test;

        if (!(value.ToULong(&test))) {
            // Conversion failed
            if (!m_useDefault) {
                // Not using default, validation error
                if (!m_silent) {
                    m_validatorWindow->SetFocus();
                    ::wxMessageBox(wxT("'")+value+_("' is not an integer number."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                }
                return false;
            }
        } else {
            if ((!m_zeroAllowed) && (test == 0)) {
                // Zero not allowed
                if (!m_silent) {
                    m_validatorWindow->SetFocus();
                    ::wxMessageBox(_("You may not enter zero here."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                }
                return false;
            }
        }
    } else if (m_pString) {
        if ((!m_zeroAllowed) && (value == wxT(""))) {
            // Empty not allowed
            if (!m_silent) {
                m_validatorWindow->SetFocus();
                ::wxMessageBox(_("This field may not be empty."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
            }
            return false;
        }
        // If we arrive here, we have a value or it's allowed to have none
        if (!m_useDefault) {
            // We check if the file exists, but only if the value is non-empty
            // If it's empty at this stage, we interpret it as "either a valid file or nothing"
            if (value != wxT("")) {
                if (!::wxFileExists(value)) {
                    // File does not exist.
                    if (!m_silent) {
                        m_validatorWindow->SetFocus();
                        ::wxMessageBox(_("This file does not exist."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                    }
                    return false;
                }
            }
        }
    } else if (m_pFileName) {
        wxFileName test;
        if (m_isDir) {
            test.SetPath(value);
        } else {
            test = value;
        }
        if ((!m_zeroAllowed) && (test == wxT(""))) {
            // Empty not allowed
            if (!m_silent) {
                m_validatorWindow->SetFocus();
                ::wxMessageBox(_("This field may not be empty."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
            }
            return false;
        }
        // If we arrive here, we have a value or it's allowed to have none
        if (!m_useDefault) {
            // We check if the file exists, but only if the value is non-empty
            // If it's empty at this stage, we interpret it as "either a valid file or nothing"
            if (test != wxT("")) {
                bool exists = false;
                if (m_isDir) {
                    exists = wxFileName::DirExists(test.GetPathWithSep());
                } else {
                    exists = wxFileName::FileExists(test.GetFullPath());
                }
                if (!exists) {
                    // File does not exist.
                    if (!m_silent) {
                        m_validatorWindow->SetFocus();
                        ::wxMessageBox(_("This file does not exist."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
                    }
                    return false;
                }
            }
        }
    }

    return true;
}

bool wxExtendedValidator::TransferToWindow(void) {
    if (!CheckValidator()) {
        return false;
    }

    if (dynamic_cast<wxTextCtrl*>(m_validatorWindow)) {
        wxTextCtrl *pControl = dynamic_cast<wxTextCtrl*>(m_validatorWindow);
        if ( m_pFloat || m_pDouble ) {
            pControl->SetValue(wxString::Format(wxT("%.*f"), m_precision, (m_pFloat)?(*m_pFloat):(*m_pDouble)));
        } else if (m_pUChar) {
            unsigned int t = *m_pUChar;
            pControl->SetValue(wxString::Format(wxT("%u"), t));
        } else if (m_pUInt) {
            pControl->SetValue(wxString::Format(wxT("%u"), *m_pUInt));
        } else if (m_pULong) {
            pControl->SetValue(wxString::Format(wxT("%lu"), *m_pULong));
        } else if (m_pString) {
            pControl->SetValue(*m_pString);
        } else if (m_pFileName) {
            if (m_isDir) {
                pControl->SetValue(m_pFileName->GetPathWithSep());
            } else {
                pControl->SetValue(m_pFileName->GetFullPath());
            }
        }
    } else if (dynamic_cast<wxComboCtrl*>(m_validatorWindow)) {
        wxComboCtrl *pControl = dynamic_cast<wxComboCtrl*>(m_validatorWindow);
        if ( m_pFloat || m_pDouble ) {
            pControl->SetValue(wxString::Format(wxT("%.*f"), m_precision, (m_pFloat)?(*m_pFloat):(*m_pDouble)));
        } else if (m_pUChar) {
            unsigned int t = *m_pUChar;
            pControl->SetValue(wxString::Format(wxT("%u"), t));
        } else if (m_pUInt) {
            pControl->SetValue(wxString::Format(wxT("%u"), *m_pUInt));
        } else if (m_pULong) {
            pControl->SetValue(wxString::Format(wxT("%lu"), *m_pULong));
        } else if (m_pString) {
            pControl->SetValue(*m_pString);
        } else if (m_pFileName) {
            if (m_isDir) {
                pControl->SetValue(m_pFileName->GetPathWithSep());
            } else {
                pControl->SetValue(m_pFileName->GetFullPath());
            }
        }
    } else if (dynamic_cast<wxComboBox*>(m_validatorWindow)) {
        wxComboBox *pControl = dynamic_cast<wxComboBox*>(m_validatorWindow);
        if ( m_pFloat || m_pDouble ) {
            pControl->SetValue(wxString::Format(wxT("%.*f"), m_precision, (m_pFloat)?(*m_pFloat):(*m_pDouble)));
        } else if (m_pUChar) {
            unsigned int t = *m_pUChar;
            pControl->SetValue(wxString::Format(wxT("%u"), t));
        } else if (m_pUInt) {
            pControl->SetValue(wxString::Format(wxT("%u"), *m_pUInt));
        } else if (m_pULong) {
            pControl->SetValue(wxString::Format(wxT("%lu"), *m_pULong));
        } else if (m_pString) {
            pControl->SetValue(*m_pString);
        } else if (m_pFileName) {
            if (m_isDir) {
                pControl->SetValue(m_pFileName->GetPathWithSep());
            } else {
                pControl->SetValue(m_pFileName->GetFullPath());
            }
        }
    } else if (dynamic_cast<wxChoice*>(m_validatorWindow)) {
        wxChoice *pControl = dynamic_cast<wxChoice*>(m_validatorWindow);
        if ( m_pFloat || m_pDouble ) {
            return pControl->SetStringSelection(wxString::Format(wxT("%.*f"), m_precision, (m_pFloat)?(*m_pFloat):(*m_pDouble)));
        } else if (m_pUChar) {
            unsigned int t = *m_pUChar;
            return pControl->SetStringSelection(wxString::Format(wxT("%u"), t));
        } else if (m_pUInt) {
            return pControl->SetStringSelection(wxString::Format(wxT("%u"), *m_pUInt));
        } else if (m_pULong) {
            return pControl->SetStringSelection(wxString::Format(wxT("%lu"), *m_pULong));
        } else if (m_pString) {
            return pControl->SetStringSelection(*m_pString);
        } else if (m_pFileName) {
            if (m_isDir) {
                return pControl->SetStringSelection(m_pFileName->GetPathWithSep());
            } else {
                return pControl->SetStringSelection(m_pFileName->GetFullPath());
            }
        }
    } else {
        return false;
    }

    return true;
}

bool wxExtendedValidator::TransferFromWindow(void) {
    if ( !CheckValidator() )
        return false;

    wxString value = wxT("");

    if (dynamic_cast<wxTextCtrl*>(m_validatorWindow)) {
        value = (dynamic_cast<wxTextCtrl*>(m_validatorWindow))->GetValue();
    } else if (dynamic_cast<wxComboCtrl*>(m_validatorWindow)) {
        value = (dynamic_cast<wxComboCtrl*>(m_validatorWindow))->GetValue();
    } else if (dynamic_cast<wxComboBox*>(m_validatorWindow)) {
        value = (dynamic_cast<wxComboBox*>(m_validatorWindow))->GetValue();
    } else if (dynamic_cast<wxChoice*>(m_validatorWindow)) {
        value = (dynamic_cast<wxChoice*>(m_validatorWindow))->GetStringSelection();
    } else {
        return false;
    }

    if ( m_pFloat || m_pDouble ) {
        double test;

        if (!(value.ToDouble(&test))) {
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
    } else if (m_pUChar || m_pUInt || m_pULong) {
        unsigned long test;

        if (!(value.ToULong(&test))) {
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
        if (m_pUChar) {
            *m_pUChar = test;
        } else if (m_pUInt) {
            *m_pUInt = test;
        } else {
            *m_pULong = test;
        }
    } else if (m_pString) {
        if ((!m_zeroAllowed) && (value == wxT(""))) {
            // Empty not allowed
            return false;
        }
        // If we arrive here, we have a value or it's allowed to have none
        if (!m_useDefault) {
            // We check if the file exists, but only if the value is non-empty
            // If it's empty at this stage, we interpret it as "either a valid file or nothing"
            if (value != wxT("")) {
                if (!::wxFileExists(value)) {
                    // File does not exist.
                    return false;
                }
            }
        }
        *m_pString = value;
    } else if (m_pFileName) {
        wxFileName test;
        if (m_isDir) {
            test.SetPath(value);
        } else {
            test = value;
        }
        if ((!m_zeroAllowed) && (test == wxT(""))) {
            // Empty not allowed
            return false;
        }
        // If we arrive here, we have a value or it's allowed to have none
        if (!m_useDefault) {
            // We check if the file exists, but only if the value is non-empty
            // If it's empty at this stage, we interpret it as "either a valid file or nothing"
            if (test != wxT("")) {
                bool exists = false;
                if (m_isDir) {
                    exists = wxFileName::DirExists(test.GetPathWithSep());
                } else {
                    exists = wxFileName::FileExists(test.GetFullPath());
                }
                if (!exists) {
                    // File does not exist.
                    return false;
                }
            }
        }
        *m_pFileName = test;
    }

    return true;
}

#endif
