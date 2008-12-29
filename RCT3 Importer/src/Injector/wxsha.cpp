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

#include "wxsha.h"

#include <boost/shared_array.hpp>
#include <wx/file.h>

wxSHA::wxSHA():m_calculated(false) {
	for (int i = 0; i < 32; ++i)
		m_digest[i] = 0;
}

wxFileOffset wxSHA::Input(const wxString& file) {
    wxFileOffset res = 0;
    wxFile f(file, wxFile::read);
    res = f.Length();
	m_calculated = false;

    if (res) {
		m_sha.Restart();
        boost::shared_array<unsigned char> buf(new unsigned char[res]);
        f.Read(buf.get(), res);
        m_sha.CalculateDigest(reinterpret_cast<byte*>(&m_digest), buf.get(), res);
		m_calculated = true;
    }

    return res;
}
wxString wxSHA::getDigestString() {
	wxString res;
	if (m_calculated) {
		for (int i = 0; i < 32; ++i)
			res += wxString::Format(wxT("%02x"), m_digest[i]);
	}
	return res;
}

