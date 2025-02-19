///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for TEX structures
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


#ifndef MANAGERTEX_H_INCLUDED
#define MANAGERTEX_H_INCLUDED

#include <map>
#include <string>

#include "ManagerBTBL.h"
#include "ManagerOVL.h"

#include "icontexture.h"

/// Encapsulates a TextureStruct.
/**
 *  @see TextureStruct
 */
class cTextureStruct {
public:
	unsigned long unk1;
	unsigned long unk2;
	unsigned long unk3;
	unsigned long unk4;
	unsigned long unk5;
	unsigned long unk6;
	unsigned long unk7;
	unsigned long unk8;
	unsigned long unk9;
	unsigned long unk10;
	unsigned long unk11;
	std::string texturestyle;
	unsigned long unk12;

	cTexture texture;

	cTextureStruct(): unk1(0x70007), unk2(0x70007), unk3(0x70007), unk4(0x70007),
	                  unk5(0x70007), unk6(0x70007), unk7(0x70007), unk8(0x70007),
	                  unk9(1), unk10(8), unk11(0x10), texturestyle("GUIIcon"), unk12(1) {}

    void Fill(TextureStruct* ts);
};

class ovlFLICManager;
class ovlTEXManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
    static const unsigned long TYPE;
private:
    std::map<std::string, cTextureStruct> m_items;

    ovlFLICManager* m_flicman;
public:
    ovlTEXManager(): ovlOVLManager() {};

    virtual void Init(cOvl* ovl);

    void AddTexture(const cTextureStruct& item);

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
    virtual const unsigned long Type() const {
        return TYPE;
    };
};

#endif
