///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for MAM structures
// Copyright (C) 2010 Tobias Minch
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


#ifndef MANAGERMAM_H_INCLUDED
#define MANAGERMAM_H_INCLUDED

#include "ManagerOVL.h"
#include "manifoldmesh.h"
#include "vertex.h"

class cManifoldMesh {
public:
    typedef std::pair<std::string, cManifoldMesh> mapentry;
	
    std::string name;
    r3::VECTOR bbox1;
    r3::VECTOR bbox2;
	std::vector<r3::ManifoldMeshVertex> vertices;
	std::vector<uint16_t> indices;

	cManifoldMesh() {};
	void Fill(r3::ManifoldMesh* mam) const;
};


class ovlMAMManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cManifoldMesh> m_items;
public:
    ovlMAMManager(): ovlOVLManager() {};

    void AddMesh(const cManifoldMesh& item);

    virtual void Make(cOvlInfo* info);

	virtual int GetCount(r3::cOvlType type) const {
		if (type == r3::OVLT_COMMON)
			return m_items.size();
		else
			return 0;
	}

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};



#endif
