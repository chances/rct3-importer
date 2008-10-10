/////////////////////////////////////////////////////////////////////////////
// Name:        valchb.h
// Purpose:     wxCheckBoxValidator class
// Author:      Tobias Minich
// Modified by:
// Created:     Jun 02 2008
// RCS-ID:
// Copyright:   (c) 2008 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef VALCHB_H_INCLUDED
#define VALCHB_H_INCLUDED

#include <wx/validate.h>

#include "silence.h"

template<class T>
struct wxCheckBoxValidatorValue {
    const T operator() (bool val) {
        return val;
    }
};

template<class T>
class wxCheckBoxValidator: public wxValidator {
//DECLARE_CLASS(wxCheckBoxValidator<T>)
public:

    wxCheckBoxValidator(): wxValidator() {m_val = NULL;}
    wxCheckBoxValidator(T* val, const T& checkedval = wxCheckBoxValidatorValue<T>()(true), const T& uncheckedval = wxCheckBoxValidatorValue<T>()(false)):
        wxValidator(),m_val(val),m_false(uncheckedval),m_true(checkedval) {}
    wxCheckBoxValidator(const wxCheckBoxValidator<T>& copyFrom) {
        Copy(copyFrom);
    }

    virtual ~wxCheckBoxValidator(){}

    virtual wxObject *Clone() const { return new wxCheckBoxValidator<T>(*this); }
    bool Copy(const wxCheckBoxValidator<T>& val) {
        wxValidator::Copy(val);

        m_val = val.m_val;
        m_false = val.m_false;
        m_true = val.m_true;
    }

    virtual bool Validate(wxWindow* parent) {
        if (!CheckValidator()) {
            return false;
        }
        // Well, how should it not validate?!?
        return true;
    }

    virtual bool TransferToWindow() {
        if (!CheckValidator()) {
            return false;
        }
        wxCheckBox* cb = dynamic_cast<wxCheckBox*>(m_validatorWindow);
        if (*m_val == m_true) {
            cb->SetValue(true);
        } else if (*m_val == m_false) {
            cb->SetValue(false);
        } else
            return false;
        return true;
    }

    virtual bool TransferFromWindow() {
        if (!CheckValidator()) {
            return false;
        }
        bool val = dynamic_cast<wxCheckBox*>(m_validatorWindow)->GetValue();
        if (val)
            *m_val = m_true;
        else
            *m_val = m_false;
        return true;
    }

protected:
    T*  m_val;
    T   m_false;
    T   m_true;

    bool CheckValidator() const {
        wxCHECK_MSG( m_validatorWindow, false,
                     _("No window associated with validator") );
        wxCHECK_MSG( (m_validatorWindow->IsKindOf(CLASSINFO(wxCheckBox)))
                     , false, _("wxCheckBoxValidator is only for wxCheckBoxes") );

        return true;
    }

private:
    wxCheckBoxValidator<T>& operator=(const wxCheckBoxValidator<T>&);
};

#endif // VALCHB_H_INCLUDED
