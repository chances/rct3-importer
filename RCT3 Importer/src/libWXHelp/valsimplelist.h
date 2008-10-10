/////////////////////////////////////////////////////////////////////////////
// Name:        valsimplelist.h
// Purpose:     wxSimpleListValidator class
// Author:      Tobias Minich
// Modified by:
// Created:     Dec 02 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef VALSIMPLELIST_H_INCLUDED
#define VALSIMPLELIST_H_INCLUDED

#include <wx/ctrlsub.h>
#include <wx/validate.h>
#include <wx/vlbox.h>

#include "silence.h"

class wxSimpleListValidator : public wxValidator, public wxSilent {
DECLARE_CLASS(wxSimpleListValidator)
public:
    wxSimpleListValidator():wxValidator(),wxSilent(){Initialize();};
    wxSimpleListValidator(char* val);
    wxSimpleListValidator(int* val);
    wxSimpleListValidator(long* val);
    wxSimpleListValidator(unsigned char* val);
    wxSimpleListValidator(unsigned int* val);
    wxSimpleListValidator(unsigned long* val);
    wxSimpleListValidator(wxString* val);
    wxSimpleListValidator(const wxSimpleListValidator& copyFrom);

    virtual ~wxSimpleListValidator(){}

    virtual wxObject *Clone() const { return new wxSimpleListValidator(*this); }
    bool Copy(const wxSimpleListValidator& val);

    virtual bool Validate(wxWindow* parent);

    virtual bool TransferToWindow();

    virtual bool TransferFromWindow();

protected:
    void Initialize();

    enum {
        Val_Char,
        Val_Int,
        Val_Long,
        Val_UChar,
        Val_UInt,
        Val_ULong,
        Val_String
    } m_kind;
    union {
        char*  m_pChar;
        int*   m_pInt;
        long*  m_pLong;
        unsigned char*  m_pUChar;
        unsigned int*   m_pUInt;
        unsigned long*  m_pULong;
        wxString*  m_pString;
    };

    bool CheckValidator() const
    {
        wxCHECK_MSG( m_validatorWindow, false,
                     _T("No window associated with validator") );
        // IsKindOf for some odd reason doesn't work.
        wxCHECK_MSG( dynamic_cast<wxControlWithItems*>(m_validatorWindow) || dynamic_cast<wxVListBox*>(m_validatorWindow), false,
                     _T("wxSimpleListValidator is only for wxControlWithItems and wxVListBox derived classes") );
        wxCHECK_MSG( (!(m_kind == Val_String)) ||  dynamic_cast<wxControlWithItems*>(m_validatorWindow), false,
                     _T("wxSimpleListValidator for a wxVListBox with a wxString value is not supported.") );

        return true;
    }

private:
    wxSimpleListValidator& operator=(const wxSimpleListValidator&);
};


#endif // VALSIMPLELIST_H_INCLUDED
