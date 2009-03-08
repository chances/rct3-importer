///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for GSI structures
// Copyright (C) 2007 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
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
// Based on libOVL by Jonathan Wilson
//
///////////////////////////////////////////////////////////////////////////////


#ifndef MANAGERGSI_H_INCLUDED
#define MANAGERGSI_H_INCLUDED

#include <string>
#include <vector>

#include "ManagerOVL.h"

#include "guiicon.h"

struct cGUISkinItem {
    typedef std::pair<std::string, cGUISkinItem> mapentry;
	std::string name;
	unsigned long gsi_type;
	std::string texture;
	std::vector<unsigned long> values;
	unsigned long unk2;
	
	cGUISkinItem() {
		gsi_type = 0;
		unk2 = 0;
	}
	
    unsigned long GetCommonSize() const;
    unsigned long GetUniqueSize() const;
	void Check() const;
};

class ovlGSIManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cGUISkinItem>  m_items;

    unsigned long m_commonsize;
public:
    ovlGSIManager(): ovlOVLManager() {
        m_commonsize = 0;
    };
    virtual ~ovlGSIManager(){};

    void AddItem(const cGUISkinItem& item);

    virtual void Make(cOvlInfo* info);

	virtual int GetCount(r3::cOvlType type) const {
		if (type == r3::OVLT_COMMON)
			return 0;
		else
			return m_items.size();
	}

    virtual const char* Loader() const {
        return LOADER;
    };
    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};

#endif
