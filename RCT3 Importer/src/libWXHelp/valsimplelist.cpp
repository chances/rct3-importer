/////////////////////////////////////////////////////////////////////////////
// Name:        valsimplelist.cpp
// Purpose:     wxSimpleListValidator class
// Author:      Tobias Minich
// Modified by:
// Created:     Dec 02 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXHELP_BASE

#include "valsimplelist.h"

IMPLEMENT_CLASS(wxSimpleListValidator, wxValidator)

wxSimpleListValidator::wxSimpleListValidator(char* val):wxSilent() {
    Initialize();
    m_kind = Val_Char;
    m_pChar = val;
}

wxSimpleListValidator::wxSimpleListValidator(unsigned char* val):wxSilent() {
    Initialize();
    m_kind = Val_UChar;
    m_pUChar = val;
}

wxSimpleListValidator::wxSimpleListValidator(int* val):wxSilent() {
    Initialize();
    m_kind = Val_Int;
    m_pInt = val;
}

wxSimpleListValidator::wxSimpleListValidator(unsigned int* val):wxSilent() {
    Initialize();
    m_kind = Val_UInt;
    m_pUInt = val;
}

wxSimpleListValidator::wxSimpleListValidator(long* val):wxSilent() {
    Initialize();
    m_kind = Val_Long;
    m_pLong = val;
}

wxSimpleListValidator::wxSimpleListValidator(unsigned long* val):wxSilent() {
    Initialize();
    m_kind = Val_ULong;
    m_pULong = val;
}

wxSimpleListValidator::wxSimpleListValidator(wxString* val):wxSilent() {
    Initialize();
    m_kind = Val_String;
    m_pString = val;
}

wxSimpleListValidator::wxSimpleListValidator(const wxSimpleListValidator& copyFrom):wxValidator(),wxSilent() {
    Copy(copyFrom);
}

bool wxSimpleListValidator::Copy(const wxSimpleListValidator& val) {
    wxValidator::Copy(val);

    m_pChar = val.m_pChar;
    m_kind = val.m_kind;

    return true;
}

void wxSimpleListValidator::Initialize() {
    m_pChar = NULL;
    m_kind = Val_Char;
}

bool wxSimpleListValidator::Validate(wxWindow* parent) {
    if ( !CheckValidator() )
        return false;

    bool valid = false;
    if (dynamic_cast<wxControlWithItems*>(m_validatorWindow)) {
        valid = (dynamic_cast<wxControlWithItems*>(m_validatorWindow)->GetSelection()!=wxNOT_FOUND);
    } else if (dynamic_cast<wxVListBox*>(m_validatorWindow)) {
        valid = (dynamic_cast<wxVListBox*>(m_validatorWindow)->GetSelection()!=wxNOT_FOUND);
    }

    if ((!valid) && (!m_silent)) {
        m_validatorWindow->SetFocus();
        ::wxMessageBox(_("You need to select something in this list."), _("Validation conflict"), wxOK|wxICON_ERROR, parent);
    }

    return valid;
}

bool wxSimpleListValidator::TransferToWindow(void) {
    if ( !CheckValidator() )
        return false;

    if (!m_pChar)
        return false;

    int sel = wxNOT_FOUND;

    if (m_pChar) {
        switch (m_kind) {
            case Val_Char:
                sel = *m_pChar;
                break;
            case Val_Int:
                sel = *m_pInt;
                break;
            case Val_Long:
                sel = *m_pLong;
                break;
            case Val_UChar:
                sel = *m_pUChar;
                break;
            case Val_UInt:
                sel = *m_pUInt;
                break;
            case Val_ULong:
                sel = *m_pULong;
                break;
            default:
                sel = wxNOT_FOUND;
        }
    }

    if (dynamic_cast<wxControlWithItems*>(m_validatorWindow)) {
        if ((m_kind == Val_String) && m_pString) {
            sel = dynamic_cast<wxControlWithItems*>(m_validatorWindow)->FindString(*m_pString);
        }
        dynamic_cast<wxControlWithItems*>(m_validatorWindow)->SetSelection(sel);
    } else if (dynamic_cast<wxVListBox*>(m_validatorWindow)) {
        dynamic_cast<wxVListBox*>(m_validatorWindow)->SetSelection(sel);
    } else {
        return false;
    }

    return true;
}

bool wxSimpleListValidator::TransferFromWindow(void) {
    if ( !CheckValidator() )
        return false;

    int sel = wxNOT_FOUND;

    if (dynamic_cast<wxControlWithItems*>(m_validatorWindow)) {
        if ((m_kind == Val_String) && m_pString) {
            *m_pString = dynamic_cast<wxControlWithItems*>(m_validatorWindow)->GetStringSelection();
        }
        sel = dynamic_cast<wxControlWithItems*>(m_validatorWindow)->GetSelection();
    } else if (dynamic_cast<wxVListBox*>(m_validatorWindow)) {
        sel = dynamic_cast<wxVListBox*>(m_validatorWindow)->GetSelection();
    }

    if (sel == wxNOT_FOUND) {
        return false;
    }
    if (m_pChar) {
        switch (m_kind) {
            case Val_Char:
                *m_pChar = sel;
                break;
            case Val_Int:
                *m_pInt = sel;
                break;
            case Val_Long:
                *m_pLong = sel;
                break;
            case Val_UChar:
                *m_pUChar = sel;
                break;
            case Val_UInt:
                *m_pUInt = sel;
                break;
            case Val_ULong:
                *m_pULong = sel;
                break;
            case Val_String:
                break;
            default:
                return false;
        }
    } else {
        return false;
    }

    return true;
}

#endif
