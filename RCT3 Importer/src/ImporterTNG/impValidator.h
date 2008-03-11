///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Importer TNG
// Custom object importer for Ataris Roller Coaster Tycoon 3
// Copyright (C) 2008 Belgabor (Tobias Minich)
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
//   Belgabor (Tobias Minich) - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#ifndef IMPVALIDATOR_H_INCLUDED
#define IMPVALIDATOR_H_INCLUDED

#include <boost/shared_ptr.hpp>
#include <wx/string.h>

class impControl;
class impValidator {
public:
    enum STATUS {
        STATUS_OK = 0,
        STATUS_WARNING = 1,
        STATUS_ERROR = 2
    };

    impValidator(): m_control(NULL) {}
    impValidator(bool, const impValidator& nextval);
    virtual ~impValidator() {}

    virtual STATUS operator()();
    virtual STATUS operator()(const wxString& check);
    virtual impValidator* clone() const;
    inline void setControl(impControl* control) { m_control = control; }
    inline const wxString& reason() { return m_reason; }

protected:
    boost::shared_ptr<impValidator> m_next;
    impControl* m_control;
    wxString m_reason;

    STATUS get_next(const wxString& check);
};

class impValidatorAscii: public impValidator {
public:
    impValidatorAscii(bool warn = true): impValidator(), m_warn(warn) {}
    impValidatorAscii(bool warn, const impValidator& nextval): impValidator(true, nextval), m_warn(warn) {}

    virtual STATUS operator()(const wxString& check);
    virtual impValidator* clone() const;
private:
    bool m_warn;
};

#endif // IMPVALIDATOR_H_INCLUDED
