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

#include "impValidator.h"

#include "impControl.h"

impValidator::impValidator(bool, const impValidator& nextval): m_control(NULL) {
    m_next.reset(nextval.clone());
}

impValidator::STATUS impValidator::operator()() {
    if (!m_control) {
        wxLogError(wxT("PANIC! Validator called without control."));
        return STATUS_ERROR;
    }
    return operator()(m_control->value());
}

impValidator::STATUS impValidator::operator()(const wxString& check) {
    return get_next(check);
}

impValidator* impValidator::clone() const {
    return new impValidator(*this);
}

impValidator::STATUS impValidator::get_next(const wxString& check) {
    m_reason = wxT("");
    if (!m_next) {
        return STATUS_OK;
    }
    STATUS next = (*m_next)(check);
    if (next) {
        m_reason = m_next->reason();
    }
    return next;
}

impValidator::STATUS impValidatorAscii::operator()(const wxString& check) {
    STATUS next = get_next(check);

    if (next == STATUS_ERROR)
        return STATUS_ERROR;

    if (check.IsAscii()) {
        return next;
    } else {
        if (next == STATUS_WARNING)
            m_reason += wxT(", ");
        if (!m_warn)
            m_reason = wxT("");
        m_reason += _("Value needs to be AscII");
        return m_warn?STATUS_WARNING:STATUS_ERROR;
    }
}

impValidator* impValidatorAscii::clone() const {
    return new impValidatorAscii(*this);
}

