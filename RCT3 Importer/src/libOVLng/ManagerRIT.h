///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for RIT structures
// Copyright (C) 2009 Tobias Minch
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


#ifndef MANAGERRIT_H_INCLUDED
#define MANAGERRIT_H_INCLUDED

#include "train.h"

#include <map>
#include <string>
#include <vector>

#include "ManagerOVL.h"
#include "ManagerCommon.h"

class cRideTrain {
public:
    typedef std::pair<std::string, cRideTrain> mapentry;

    std::string name;
    std::string internalname;
    std::string internaldescription;
    unsigned long   version;
	unsigned long	overtake;
	std::string station;
	struct Cars {
		std::string	front;
		std::string	second;
		std::string	mid;
		std::string	penultimate;
		std::string	rear;
		std::string	link;
		unsigned long count_min;
		unsigned long count_max;
		unsigned long count_def;

		// Wild
		std::string	unknown;
		
		Cars(): count_min(1), count_max(1), count_def(1) {}

		void Fill(r3::RideTrain_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} cars;
	struct Speed {
		float     unk01;
		float     unk02; 
		float     unk03;
		Speed(): unk01(0.04), unk02(0.06),  unk03(0.02) {}
				
		void Fill(r3::RideTrain_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} speed;
	struct Camera {
		float             lookahead;
		float             g_displacement_mult;
		float             g_displacement_max;
		float             g_displacement_smoothing;
		float             shake_min_speed;
		float             shake_factor;
		float             shake_h_max;
		float             shake_v_max;
		float             chain_shake_factor;
		float             chain_shake_h_max;
		float             chain_shake_v_max;
		float             smoothing;
		float             la_tilt_factor;
		Camera(): lookahead(8.0), g_displacement_mult(0.5), g_displacement_max(0.4), g_displacement_smoothing(2.0), 
			shake_min_speed(1.0), shake_factor(0.00001), shake_h_max(0.1), shake_v_max(0.1),
			chain_shake_factor(0.001), chain_shake_h_max(0.0), chain_shake_v_max(0.004),
			smoothing(0.1), la_tilt_factor(0.5)
			{}
		void Fill(r3::RideTrain_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} camera;
	struct Water {
		float     unk01;
		float     unk02;
		float     unk03;
		float     unk04;
		float     unk05;
		float     unk06;
		float     free_roam_curve_angle;
		float     unk07;
		Water(): unk01(1.0), unk02(2.0),  unk03(2.0), unk04(0.5), unk05(10.0),  unk06(0.5),
			free_roam_curve_angle(0.0), unk07(0.0)
			{}
				
		void Fill(r3::RideTrain_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} water;
	struct LiftSplines {
		std::string left;
		std::string right;
	} liftsplines;
	struct Unknowns {
		float             unk37;
		float             unk38;
		float             unk39;
		float             unk40;
		float             unk41;
		float             unk42;
		float             unk43;
		float             unk44;
		Unknowns(): unk37(5.0), unk38(15.0), unk39(5.0), unk40(7.0), unk41(9.0), unk42(11.0), unk43(0.8), unk44(0.32) {}

		void Fill(r3::RideTrain_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;

	} unknowns;
	struct Soaked {
		unsigned long            icon;
		unsigned long            unk51;
		Soaked(): icon(0), unk51(0) {}
		
		void Fill(r3::RideTrain_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} soaked;
	struct Wild {
		float    	    	airboat_unk01;      // Usually 0, Airboat 0.02
		float    	    	airboat_unk02;      // Usually 0, Airboat 0.15
		float    	        airboat_unk03;      // Usually -1, Airboat 4
		float    	        airboat_unk04;      // Usually -1, Airboat 3
		float    	        airboat_unk05;      // Usually -1, Airboat 0.75
		float    	        airboat_unk06;      // Usually 1, Airboat 4, PaddleSteamer 11
		unsigned long  	    unk59;              // Always 0, never symref
		unsigned long 	    unk60;              // Always 0, never symref
		float           	ffsiez_unk;         // Seen 0.0, 90.0 on FrequentFaller, -90.0 on Siezmic
		Wild(): airboat_unk01(0.0), airboat_unk02(0.0), airboat_unk03(-1.0), airboat_unk04(-1.0),
			airboat_unk05(-1.0), airboat_unk06(1.0), unk59(0), unk60(0), ffsiez_unk(0.0)
			{}
		void Fill(r3::RideTrain_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} wild;
	
	cRideTrain(): name(""), internalname(""), internaldescription(""), version(0), overtake(0), station("") {}

    void Fill(r3::RideTrain_W* rc, ovlStringTable* tab, ovlRelocationManager* rel) const;

    unsigned long GetUniqueSize() const;
};

class ovlRITManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cRideTrain>  m_items;

public:
    ovlRITManager(): ovlOVLManager() {
    };

    void AddItem(const cRideTrain& item);

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
    virtual const char* Loader() const {
        return LOADER;
    };
};

#endif

