/////////////////////////////////////////////////////////////////////////////
// Name:        RCT3Exception.h
// Purpose:     RCT3Exception class
// Author:      Tobias Minich
// Modified by:
// Created:     2007-11-14
// RCS-ID:
// Copyright:   (c) Tobias Minich
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef RCT3EXCEPTION_H_INCLUDED
#define RCT3EXCEPTION_H_INCLUDED

#include "wx_pch.h"

#include <exception>

class RCT3Exception: public std::exception {
public:
    RCT3Exception(const wxString& message);
    virtual const char* what() const throw() {
        return m_message.c_str();
    }
    virtual const wxString& wxwhat() const throw() {
        return m_message;
    }
    ~RCT3Exception() throw() {};
protected:
    wxString m_message;
};

#endif
