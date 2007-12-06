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

#define WIN32_MEAN_AND_LEAN
#include <windows.h>

#include "OVLException.h"
#include "rct3log.h"

const char* ovlFTXManager::LOADER = "FGDK";
const char* ovlFTXManager::NAME = "FlexiTexture";
const char* ovlFTXManager::TAG = "ftx";
const char* ovlTXSManager::TAG = "txs";

ovlFTXManager::~ovlFTXManager() {
    for (vector<FlexiTextureInfoStruct*>::iterator it = m_ftxlist.begin(); it != m_ftxlist.end(); ++it) {
        delete[] (*it)->offset1;
        for (unsigned long i = 0; i < (*it)->fts2Count; ++i) {
            delete[] (*it)->fts2[i].palette;
            delete[] (*it)->fts2[i].texture;
            if ((*it)->fts2[i].alpha)
                delete[] (*it)->fts2[i].alpha;
        }
        delete[] (*it)->fts2;
        delete (*it);
    }
}

void ovlFTXManager::AddTexture(const char* name, unsigned long dimension, unsigned long fps, unsigned long recol,
                            unsigned long animationcount, unsigned long* animation, unsigned long framecount) {
    Check("ovlFTXManager::AddTexture");
    if (m_ftscount)
        throw EOvl("ovlFTXManager::AddTexture called but last texture misses frames");

    FlexiTextureInfoStruct* ftis = new FlexiTextureInfoStruct;
    ftis->scale = local_log2(dimension);
    ftis->width = dimension;
    ftis->height = dimension;
    ftis->fps = fps;
    ftis->Recolorable = recol;
    ftis->offsetCount = animationcount;
    if (ftis->offsetCount) {
        ftis->offset1 = new unsigned long[ftis->offsetCount];
        memcpy(ftis->offset1, animation, ftis->offsetCount*4);
    } else {
        ftis->offset1 = NULL;
    }
    ftis->fts2Count = framecount;
    ftis->fts2 = new FlexiTextureStruct[ftis->fts2Count];

    // InfoStruct
    m_size += sizeof(FlexiTextureInfoStruct);
    // Offsets
    m_size += 4 * ftis->offsetCount;
    // Textures
    m_size += sizeof(FlexiTextureStruct) * ftis->fts2Count;

    m_ftxlist.push_back(ftis);
    m_lsrman->AddLoader(OVLT_COMMON);
    m_lsrman->AddSymbol(OVLT_COMMON);
    m_ftxnames.push_back(string(name));
    m_stable->AddSymbolString(name, Tag());

    m_cftis = ftis;
    m_cfts = ftis->fts2;
    m_ftscount = ftis->fts2Count;
}

void ovlFTXManager::AddTextureFrame(unsigned long dimension, unsigned long recol,
                                    unsigned char* palette, unsigned char* texture, unsigned char* alpha) {
    Check("ovlFTXManager::AddTextureFrame");
    if (!m_ftscount)
        throw EOvl("ovlFTXManager::AddTextureFrame called but no frames available");
    if (!m_cfts)
        throw EOvl("ovlFTXManager::AddTextureFrame called but no frame pointer available");
    m_cfts->scale = local_log2(dimension);
    m_cfts->width = dimension;
    m_cfts->height = dimension;
    m_cfts->Recolorable = recol;
    m_cfts->palette = new unsigned char[256*sizeof(RGBQUAD)];
    memcpy(m_cfts->palette, palette, 256*sizeof(RGBQUAD));
    m_cfts->texture = new unsigned char[m_cfts->width*m_cfts->height];
    memcpy(m_cfts->texture, texture, m_cfts->width*m_cfts->height);
    if (alpha) {
        m_cfts->alpha = new unsigned char[m_cfts->width*m_cfts->height];
        memcpy(m_cfts->alpha, alpha, m_cfts->width*m_cfts->height);
    } else {
        m_cfts->alpha = NULL;
    }

    // Palette
    m_size += 256*sizeof(RGBQUAD);
    // Texture
    //m_size += m_cfts->width*m_cfts->height;
    m_blobs[m_cfts->texture] = cOvlMemBlob(OVLT_COMMON, 0, m_cfts->width*m_cfts->height);
    // Alpha
    if (alpha) {
        //m_size += m_cfts->width*m_cfts->height;
        m_blobs[m_cfts->alpha] = cOvlMemBlob(OVLT_COMMON, 0, m_cfts->width*m_cfts->height);
    }

    m_ftscount--;
    m_cfts++;
}

unsigned char* ovlFTXManager::Make(cOvlInfo* info) {
    Check("ovlFTXManager::Make");
    if (!info)
        throw EOvl("ovlFTXManager::Make called without valid info");
    if (m_ftscount)
        throw EOvl("ovlFTXManager::Make called but last texture misses frames");

    m_blobs[0] = cOvlMemBlob(OVLT_COMMON, 2, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[0].data;

    for (unsigned long t = 0; t < m_ftxlist.size(); ++t) {
        // Data Transfer, FlexiTextureInfoStruct
        FlexiTextureInfoStruct* c_dftis = reinterpret_cast<FlexiTextureInfoStruct*>(c_data);
        c_data += sizeof(FlexiTextureInfoStruct);
        memcpy(c_dftis, m_ftxlist[t], sizeof(FlexiTextureInfoStruct));

        // Data Transfer, Animation
        if (c_dftis->offsetCount) {
            c_dftis->offset1 = reinterpret_cast<unsigned long*>(c_data);
            c_data += 4 * c_dftis->offsetCount;
            memcpy(c_dftis->offset1, m_ftxlist[t]->offset1, 4*c_dftis->offsetCount);
            m_relman->AddRelocation((unsigned long *)&c_dftis->offset1);
        }

        // Data Transfer, FlexiTextureStructs
        c_dftis->fts2 = reinterpret_cast<FlexiTextureStruct*>(c_data);
        c_data += sizeof(FlexiTextureStruct) * c_dftis->fts2Count;
        memcpy(c_dftis->fts2, m_ftxlist[t]->fts2, sizeof(FlexiTextureStruct) * c_dftis->fts2Count);
        m_relman->AddRelocation((unsigned long *)&c_dftis->fts2);

        // Data Transfer, Texture Data
        for (unsigned long i = 0; i < m_ftxlist[t]->fts2Count; ++i) {
            // Palette
            c_dftis->fts2[i].palette = c_data;
            c_data += 256*sizeof(RGBQUAD);
            memcpy(c_dftis->fts2[i].palette, m_ftxlist[t]->fts2[i].palette, 256*sizeof(RGBQUAD));
            m_relman->AddRelocation((unsigned long*)&c_dftis->fts2[i].palette);

            // Texture
//            c_dftis->fts2[i].texture = c_data;
//            c_data += c_dftis->fts2[i].width * c_dftis->fts2[i].height;
//            memcpy(c_dftis->fts2[i].texture, m_ftxlist[t]->fts2[i].texture, c_dftis->fts2[i].width * c_dftis->fts2[i].height);
//            m_relman->AddRelocation((unsigned long*)&c_dftis->fts2[i].texture);
            c_dftis->fts2[i].texture = m_blobs[m_ftxlist[t]->fts2[i].texture].data;
            memcpy(c_dftis->fts2[i].texture, m_ftxlist[t]->fts2[i].texture, c_dftis->fts2[i].width * c_dftis->fts2[i].height);
            m_relman->AddRelocation((unsigned long*)&c_dftis->fts2[i].texture);

            // Alpha
            if (m_ftxlist[t]->fts2[i].alpha) {
//                c_dftis->fts2[i].alpha = c_data;
//                c_data += c_dftis->fts2[i].width * c_dftis->fts2[i].height;
//                memcpy(c_dftis->fts2[i].alpha, m_ftxlist[t]->fts2[i].alpha, c_dftis->fts2[i].width * c_dftis->fts2[i].height);
//                m_relman->AddRelocation((unsigned long*)&c_dftis->fts2[i].alpha);
                c_dftis->fts2[i].alpha = m_blobs[m_ftxlist[t]->fts2[i].alpha].data;
                memcpy(c_dftis->fts2[i].alpha, m_ftxlist[t]->fts2[i].alpha, c_dftis->fts2[i].width * c_dftis->fts2[i].height);
                m_relman->AddRelocation((unsigned long*)&c_dftis->fts2[i].alpha);
          }
        }

        SymbolStruct* s_ftx = m_lsrman->MakeSymbol(OVLT_COMMON, m_stable->FindSymbolString(m_ftxnames[t].c_str(), Tag()), reinterpret_cast<unsigned long*>(c_dftis), true);
        m_lsrman->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(c_dftis), false, s_ftx);
        m_lsrman->CloseLoader(OVLT_COMMON);
    }

    return m_data;
}
