///////////////////////////////////////////////////////////////////////////////
//
// Light guid class built aroud cryptopp
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
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CRYPTOGUID_H
#define CRYPTOGUID_H

#include <boost/format.hpp>
#include <cryptopp/osrng.h>
#include <string>

class cryptoGUID {
public:
	typedef short rawguid[8];
private:
	rawguid m_guid;
	
public:
	cryptoGUID() {
		init();
	}
	cryptoGUID(bool ignored) {
		seed();
	}
	void init() {
		memset(&m_guid, 0, sizeof(m_guid));		
	}
	void seed() {
		CryptoPP::AutoSeededRandomPool rp;
		rp.GenerateBlock(reinterpret_cast<byte*>(&m_guid), 16);		
	}
	rawguid& raw() {
		return m_guid;
	}
	const rawguid& raw() const {
		return m_guid;
	}
	std::string str() {
		return boost::str(boost::format("%04hX%04hX-%04hX-%04hX-%04hX-%04hX%04hX%04hX") 
					% m_guid[0] % m_guid[1] % m_guid[2] % m_guid[3] % m_guid[4] % m_guid[5] % m_guid[6] % m_guid[7]);
	}
	bool operator==(const cryptoGUID& other) {
		return m_guid == other.m_guid;
	}
};

#endif
