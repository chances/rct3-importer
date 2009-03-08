///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for FTX structures
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


#include "ManagerFTX.h"


#include "ManagerCommon.h"
#include "OVLException.h"

#include <iostream>
#include <sstream>

using namespace r3;
using namespace std;

const char* ovlFTXManager::LOADER = "FGDK";
const char* ovlFTXManager::NAME = "FlexiTexture";
const char* ovlFTXManager::TAG = "ftx";

void cFlexiTextureStruct::Fill(FlexiTextureStruct* fts) {
    fts->scale = local_log2(dimension);
    fts->width = dimension;
    fts->height = dimension;
    fts->Recolorable = recolourable;
    memcpy(fts->palette, palette.get(), 256 * sizeof(r3::COLOURQUAD));
    memcpy(fts->texture, texture.get(), dimension * dimension);
    if (alpha.get()) {
        memcpy(fts->alpha, alpha.get(), dimension * dimension);
    } else {
        fts->alpha = NULL;
    }
}

void cFlexiTextureInfoStruct::Fill(FlexiTextureInfoStruct* fti) {
    fti->scale = local_log2(dimension);
    fti->width = dimension;
    fti->height = dimension;
    fti->fps = fps;
    fti->Recolorable = recolourable;
    fti->offsetCount = animation.size();
    for (unsigned long i = 0; i < animation.size(); ++i) {
        fti->offset1[i] = animation[i];
    }
    fti->fts2Count = frames.size();
    for (unsigned long i = 0; i < frames.size(); ++i) {
        frames[i].Fill(&fti->fts2[i]);
    }
}

void ovlFTXManager::AddTexture(const cFlexiTextureInfoStruct& item) {
    Check("ovlFTXManager::AddTexture");
    if (item.name == "")
        BOOST_THROW_EXCEPTION(EOvl("ovlFTXManager::AddTexture called without name"));
    if (m_items.find(item.name) != m_items.end())
        BOOST_THROW_EXCEPTION(EOvl("ovlFTXManager::AddTexture: Item with name '"+item.name+"' already exists"));

    m_items[item.name] = item;

    // FlexiTextureInfoStruct
    m_size += sizeof(FlexiTextureInfoStruct);
    // Animations
    m_size += item.animation.size() * sizeof(unsigned long);
    // Frames
    m_size += item.frames.size() * sizeof(FlexiTextureStruct);

    int i = 0;
    for (vector<cFlexiTextureStruct>::const_iterator it = item.frames.begin(); it != item.frames.end(); ++it) {
        m_size += 256*sizeof(COLOURQUAD);
        stringstream s;
        s << item.name << i;
        // Texture
        m_blobs[s.str() + "_t"] = cOvlMemBlob(OVLT_COMMON, 0, it->dimension * it->dimension);
        // Alpha
        if (it->alpha.get()) {
            m_blobs[s.str() + "_a"] = cOvlMemBlob(OVLT_COMMON, 0, it->dimension * it->dimension);
        }
        i++;
    }

	GetLSRManager()->reserveIndexElement(OVLT_COMMON, item.name, ovlFTXManager::TAG);
	/*
    GetLSRManager()->AddSymbol(OVLT_COMMON);
    GetLSRManager()->AddLoader(OVLT_COMMON);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlFTXManager::TAG);
	 */
}

void ovlFTXManager::Make(cOvlInfo* info) {
    Check("ovlFTXManager::Make");
    if (!info)
        BOOST_THROW_EXCEPTION(EOvl("ovlFTXManager::Make called without valid info"));

    m_blobs[""] = cOvlMemBlob(OVLT_COMMON, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[""].data;

    for (map<string, cFlexiTextureInfoStruct>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        FlexiTextureInfoStruct* c_dftis = reinterpret_cast<FlexiTextureInfoStruct*>(c_data);
        c_data += sizeof(FlexiTextureInfoStruct);

        if (it->second.animation.size()) {
            c_dftis->offset1 = reinterpret_cast<unsigned long*>(c_data);
            c_data += 4 * it->second.animation.size();
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_dftis->offset1));
        }

        c_dftis->fts2 = reinterpret_cast<FlexiTextureStruct*>(c_data);
        c_data += sizeof(FlexiTextureStruct) * it->second.frames.size();
        GetRelocationManager()->AddRelocation((unsigned long *)&c_dftis->fts2);

        for (unsigned long i = 0; i < it->second.frames.size(); ++i) {
            stringstream s;
            s << it->first << i;

            // Palette
            c_dftis->fts2[i].palette = c_data;
            c_data += 256*sizeof(COLOURQUAD);
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_dftis->fts2[i].palette));

            // Texture
            c_dftis->fts2[i].texture = m_blobs[s.str() + "_t"].data;
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_dftis->fts2[i].texture));

            // Alpha
            if (it->second.frames[i].alpha.get()) {
                c_dftis->fts2[i].alpha = m_blobs[s.str() + "_a"].data;
                GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long*>(&c_dftis->fts2[i].alpha));
            }
        }
        it->second.Fill(c_dftis);

		GetLSRManager()->assignIndexElement(OVLT_COMMON, it->first, ovlFTXManager::TAG, c_dftis);
		/*
        SymbolStruct* s_ftx = GetLSRManager()->MakeSymbol(OVLT_COMMON, GetStringTable()->FindSymbolString(it->first.c_str(), Tag()), reinterpret_cast<unsigned long*>(c_dftis));
        GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(c_dftis), false, s_ftx);
        GetLSRManager()->CloseLoader(OVLT_COMMON);
		 */
    }

}
