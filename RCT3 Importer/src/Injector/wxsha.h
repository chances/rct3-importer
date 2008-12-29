///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Injector
// Copyright (C) 2008 Tobias Minch
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
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WXSHA_H_INCLUDED
#define WXSHA_H_INCLUDED

#include <cryptopp/sha.h>
#include <wx/filename.h>
#include <wx/string.h>

class wxSHA {
public:
	typedef unsigned char DIGEST[32];
private:
	CryptoPP::SHA256 m_sha;
	DIGEST m_digest;
	bool m_calculated;
public:
    wxSHA();

    wxString getDigestString();
	inline const DIGEST& getDigest() const {return m_digest;};
    wxFileOffset Input(const wxString& file);
    wxFileOffset Input(const wxFileName& file) {
        return Input(file.GetFullPath());
    }
};

#endif // WXSHA_H_INCLUDED
