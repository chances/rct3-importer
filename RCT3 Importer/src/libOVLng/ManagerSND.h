///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for SND structures
// Copyright (C) 2008 Tobias Minch
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


#ifndef MANAGERSND_H_INCLUDED
#define MANAGERSND_H_INCLUDED

#include <string>
#include <vector>
#include <boost/shared_array.hpp>

#include "sceneryrevised.h"
#include "vertex.h"
#include "ManagerOVL.h"
#include "rct3log.h"

class cSound {
public:
    typedef std::pair<std::string, cSound> mapentry;
    std::string     name;
	WAVEFORMATEX    format;
	r3::int32_t         unk6; 		// always 0x00000000 could also be a float
	r3::float_t         unk7; 		// always 0.050000001
	r3::float_t         unk8; 		// always 3.9999999e-005
	r3::float_t         unk9; 		// always 1.0
	r3::float_t         unk10; 		// always 0.0
	r3::float_t         unk11; 		// always 15000000.0
	r3::float_t         unk12; 		// always 0.0
	r3::float_t         unk13; 		// always 0.050000001
	r3::float_t         unk14; 		// always 2.0
	r3::float_t         unk15; 		// always 30.0
	r3::int32_t         loop; 		// Usually 0, also seen 1. If 1, the sound is a loop and continues playing till stopped
	boost::shared_array<int16_t> channel1; 	// shorts
	r3::int32_t         channel1_size; 	// size of the above data
	boost::shared_array<int16_t> channel2; 	// shorts, NULL in no channel2
	r3::int32_t         channel2_size; 	// 0 if not channel2

    cSound() {
        format.wFormatTag = 1;
        format.nChannels = 1;
        format.nSamplesPerSec = 22050;
        format.nAvgBytesPerSec = 44100;
        format.nBlockAlign = 2;
        format.wBitsPerSample = 16;
        format.cbSize = 0;
        unk6 = 0;
        unk7 = 0.05;
        unk8 = 0.00004;
        unk9 = 1.0;
        unk10 = 0.0;
        unk11 = 1500000.0;
        unk12 = 0.0;
        unk13 = 0.05;
        unk14 = 2.0;
        unk15 = 30.0;
        loop = 0;
        channel1_size = 0;
        channel2_size = 0;
    }
    void Fill(r3::Sound& snd) const;
};

class ovlSNDManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cSound> m_items;
public:
    ovlSNDManager(): ovlOVLManager() {};

    void AddItem(const cSound& item);

    virtual void Make(cOvlInfo* info);

	virtual int GetCount(r3::cOvlType type) const {
		if (type == r3::OVLT_COMMON)
			return m_items.size();
		else
			return 0;
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
