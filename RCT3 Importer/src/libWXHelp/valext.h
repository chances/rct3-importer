/////////////////////////////////////////////////////////////////////////////
// Name:        valext.h
// Purpose:     wxExtendedValidator class
// Author:      Tobias Minich
// Modified by:
// Created:     Oct 28 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef VALEXT_H_INCLUDED
#define VALEXT_H_INCLUDED

#include "wx_pch.h"

#include <wx/validate.h>
#include <wx/filename.h>

class wxExtendedValidator : public wxValidator {
DECLARE_CLASS(wxExtendedValidator)
public:
    wxExtendedValidator():wxValidator(){Initialize();};
    wxExtendedValidator(float* val, unsigned int precision = 4, bool zero_allowed = true, bool use_default = true, float default_val = 0.0);
    wxExtendedValidator(double* val, unsigned int precision = 4, bool zero_allowed = true, bool use_default = true, double default_val = 0.0);
    wxExtendedValidator(unsigned char* val, bool zero_allowed = true, bool use_default = true, unsigned int default_val = 0);
    wxExtendedValidator(unsigned int* val, bool zero_allowed = true, bool use_default = true, unsigned int default_val = 0);
    wxExtendedValidator(unsigned long* val, bool zero_allowed = true, bool use_default = true, unsigned long default_val = 0);
    wxExtendedValidator(wxString* val, bool empty_allowed = true, bool check_exist = false);
    wxExtendedValidator(wxFileName* val, bool empty_allowed = true, bool check_exist = false);
    wxExtendedValidator(const wxExtendedValidator& copyFrom);

    virtual ~wxExtendedValidator(){}

    virtual wxObject *Clone() const { return new wxExtendedValidator(*this); }
    bool Copy(const wxExtendedValidator& val);

    virtual bool Validate(wxWindow* parent);

    virtual bool TransferToWindow();

    virtual bool TransferFromWindow();

protected:
    void Initialize();

    float*          m_pFloat;
    double*         m_pDouble;
    unsigned char*  m_pUChar;
    unsigned int*   m_pUInt;
    unsigned long*  m_pULong;
    wxString*       m_pString;
    wxFileName*     m_pFileName;

    unsigned int    m_precision;
    double          m_floatDefault;
    unsigned long   m_uintDefault;
    bool            m_useDefault;
    bool            m_zeroAllowed;

    bool CheckValidator() const
    {
        wxCHECK_MSG( m_validatorWindow, false,
                     _T("No window associated with validator") );
        wxCHECK_MSG( m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)), false,
                     _T("wxExtendedValidator is only for wxTextCtrl's") );

        return true;
    }

private:
    wxExtendedValidator& operator=(const wxExtendedValidator&);
};

#endif // WXFLOATVALIDATOR_H_INCLUDED
