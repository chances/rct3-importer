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


#ifndef MANAGERFTX_H_INCLUDED
#define MANAGERFTX_H_INCLUDED

#include <string>
#include <vector>

#include "counted_array_ptr.h"

#include "flexitexture.h"
#include "vertex.h"
#include "ManagerOVL.h"
#include "rct3log.h"

class cFlexiTextureStruct {
public:
    unsigned long dimension;
    unsigned long recolourable;
    counted_array_ptr<unsigned char> palette;
    counted_array_ptr<unsigned char> texture;
    counted_array_ptr<unsigned char> alpha;

    cFlexiTextureStruct() {
        dimension = 0;
    }
    void Fill(FlexiTextureStruct* fts);
};

class cFlexiTextureInfoStruct {
public:
    std::string name;
    unsigned long dimension;
    unsigned long fps;
    unsigned long recolourable;
    std::vector<unsigned long> animation;
    std::vector<cFlexiTextureStruct> frames;

    cFlexiTextureInfoStruct() {
        dimension = 0;
        fps = 0;
        recolourable = 0;
    }
    void Fill(FlexiTextureInfoStruct* fti);
};

class ovlFTXManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cFlexiTextureInfoStruct> m_items;
public:
    ovlFTXManager(): ovlOVLManager() {};

    void AddTexture(const cFlexiTextureInfoStruct& item);

    virtual void Make(cOvlInfo* info);

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

/*
class ovlFTXManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    vector<FlexiTextureInfoStruct*> m_ftxlist;
    vector<string> m_ftxnames;

    FlexiTextureInfoStruct* m_cftis;
    FlexiTextureStruct* m_cfts;
    long m_ftscount;
public:
    ovlFTXManager(): ovlOVLManager() {
        m_cftis = NULL;
        m_cfts = NULL;
        m_ftscount = 0;
    };
    virtual ~ovlFTXManager();

    void AddTexture(const char* name, unsigned long dimension, unsigned long fps, unsigned long recol,
                    unsigned long animationcount, unsigned long* animation, unsigned long framecount);
    void AddTextureFrame(unsigned long dimension, unsigned long recol,
                         unsigned char* palette, unsigned char* texture, unsigned char* alpha);

    virtual void Make(cOvlInfo* info);

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
*/
#endif
