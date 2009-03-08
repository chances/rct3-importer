///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SHS structures
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


#ifndef MANAGERSHS_H_INCLUDED
#define MANAGERSHS_H_INCLUDED

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "staticshape.h"
#include "ManagerCommon.h"
#include "ManagerOVL.h"

class cEffectStruct {
public:
    std::string name;
	r3::MATRIX pos;

    cEffectStruct() {};
    void Fill(r3::MATRIX* m) {
        *m = pos;
    }
};

class cStaticShape2 {
public:
    long support;
	std::string fts; //is 0 in disk files
	std::string texturestyle; //is 0 in disk files
	unsigned long placetexturing; //0 = dont see texturing on things when being placed, 1 = see texturing on things when being placed
	unsigned long textureflags; //always 0
	unsigned long sides; //seen values of 3 or 1 for this, 3 is more common
	std::vector<r3::VERTEX> vertices;
	std::vector<unsigned long> indices;

#ifndef GLASS_OLD
	cTriangleSortAlgorithm::Algorithm algo_x;
	cTriangleSortAlgorithm::Algorithm algo_y;
	cTriangleSortAlgorithm::Algorithm algo_z;
#endif

	cStaticShape2() {
	    placetexturing = 0;
	    textureflags = 0;
	    sides = 3;
	    support = r3::Constants::Mesh::SupportType::None;
#ifndef GLASS_OLD
	    algo_x = cTriangleSortAlgorithm::DEFAULT;
	    algo_y = cTriangleSortAlgorithm::DEFAULT;
	    algo_z = cTriangleSortAlgorithm::DEFAULT;
#endif
	}
	void Fill(r3::StaticShapeMesh* ss2, r3::uint32_t* vert, r3::uint32_t* ind);
	bool IsAlgoNone() const {
	    return ((algo_x == cTriangleSortAlgorithm::NONE) || (algo_y == cTriangleSortAlgorithm::NONE) || (algo_z == cTriangleSortAlgorithm::NONE));
	}
};

class cStaticShape1 {
public:
    std::string name;
    r3::VECTOR bbox1;
    r3::VECTOR bbox2;
	std::vector<cStaticShape2> meshes;
    std::vector<cEffectStruct> effects;

	cStaticShape1() {};
	void Fill(r3::StaticShape* ss1);
};

class ovlSHSManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cStaticShape1> m_items;
public:
    ovlSHSManager(): ovlOVLManager() {};

    void AddModel(const cStaticShape1& item);

    virtual void Make(cOvlInfo* info);

	virtual int GetCount(r3::cOvlType type) const {
		if (type == r3::OVLT_COMMON)
			return 0;
		else
			return m_items.size();
	}

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};

#endif
