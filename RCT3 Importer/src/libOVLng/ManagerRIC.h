///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for RIC structures
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


#ifndef MANAGERRIC_H_INCLUDED
#define MANAGERRIC_H_INCLUDED

#include "car.h"

#include <map>
#include <string>
#include <vector>

#include "ManagerOVL.h"
#include "ManagerCommon.h"

struct cRideCarTypeSVD {
	std::string svd;
	unsigned long type;
	cRideCarTypeSVD(const std::string& _svd = "", unsigned long _type = 0): svd(_svd), type(_type) {}
};

class cRideCar {
public:
    typedef std::pair<std::string, cRideCar> mapentry;

    std::string name;
    std::string internalname;
    std::string otherinternalname;
    unsigned long     version;
    std::string svd_ref;
    float		inertia;
	std::vector<unsigned long> seatings;
	struct Moving {
		std::string	svd_ref;
		float     	inertia;
		Moving(): svd_ref(""), inertia(-1.0) {}
	} moving;
	struct Axis {
		unsigned long type;
		float     stability; ///< Distance of seat mass to rotation axis
		float     unk01;     ///< Probably something with swinging/sideways movement
		float     unk02;     ///< Almost allways -1
		float     momentum;  ///< Momentum added by movement along the track
		float     unk03;     ///< Probably related to left/right swinging for fixed situations
		float     unk04;     ///< Probably related to the left/right swinging/physics
		float     maximum;       ///< Maximum rotational displacement
		Axis(): type(0), stability(4.0), unk01(0.0), unk02(-1.0), momentum(0.1), unk03(0.0), 
				unk04(0.0), maximum(-1.0) {}
		void Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} axis;
	struct Bobbing {
		unsigned long flag;   ///< Activate bobbing
		float             x;   	// Seen 0
		float             y;   	// Seen 0
		float             z;   	// Seen 0
		Bobbing(): flag(0), x(0.0), y(0.0), z(0.0) {}
		void Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} bobbing;
	struct Animations {
		long             start;     		///< Runs when the car starts
		long             started_idle;		///< Runs when the car is on track
		long             stop;      		///< Runs when the car stops
		long             stopped_idle;  	///< Runs while the car is in a station
		long             belt_open; 		///< Runs to open belts
		long             belt_open_idle;	///< Runs while belts are open
		long             belt_close;        ///< Runs to close belts
		long             belt_closed_idle;	///< Runs while belts are closed
		long             doors_open;		///< Runs to open doors
		long             doors_open_idle;   ///< Runs while doors are open
		long             doors_close;		///< Runs to close doors
		long             doors_closed_idle; ///< Runs while doors are closed
		long             row_both;
		long             row_left;
		long             row_right;
		long             canoe_station_idle;
		long             canoe_idle_front;
		long             canoe_idle_back;
		long             canoe_row_idle;
		long             canoe_row;
		
		// Wild
		long             rudder;    ///< Rudder/fan animation, only used on AirBoat
		Animations(): start(-1), started_idle(-1), stop(-1), stopped_idle(-1), belt_open(-1), belt_open_idle(-1), 
					 belt_close(-1), belt_closed_idle(-1), doors_open(-1), doors_open_idle(-1), doors_close(-1),
					 doors_closed_idle(-1), row_both(-1), row_left(-1), row_right(-1), canoe_station_idle(-1),
					 canoe_idle_front(-1), canoe_idle_back(-1), canoe_row_idle(-1), canoe_row(-1), rudder(-1) {}
		void Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
		void Fill(r3::RideCar_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} animations;
	struct Wheels {
		cRideCarTypeSVD	front_right;
		cRideCarTypeSVD	front_left;
		cRideCarTypeSVD	back_right;
		cRideCarTypeSVD	back_left;
		void Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} wheels;
	struct Axels {
		cRideCarTypeSVD front;
		cRideCarTypeSVD rear;
		Axels():front("", 4), rear("", 4) {}
		void Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} axels;
	struct Unknowns {
		unsigned long       unk54;          ///< Always 0, never symref
		float             vs_unk01;
		float             vs_unk02;
		float             unk60;
		Unknowns(): unk54(0), vs_unk01(-1.0), vs_unk02(-1.0), unk60(-1.0) {}

		void Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;

		void initSoaked() {
			vs_unk01 = 0.0;
			vs_unk02 = 0.0;
		}
	} unknowns;
	struct Sliding {
		long              skyslide_unk01; // Seen -1, 0, 4
		float             skyslide_unk02; // Seen -1.0, rarely 40, 150 or 400 on Slides and SkyRider
		float             skyslide_unk03; // Seen -1.0, 4.0, rarely 0.18, 0.2,
		
		// Soaked:
		float             skyslide_unk04; // Seen 0.1
		unsigned long     slide_unk01;	///< Seen 0, 1
		unsigned long     skyslide_unk05; ///< Seen 1, 4
		unsigned long     unstable_unk01;	///< Occurs on bobs and slides, looks like flags
		float             slide_unk02;	///< Seen 0, 1, 3.1
		float             slide_unk03;    ///< Seen -9999.9 (0xc61c3f9a), 0.2, 0.3
		
		Sliding(): skyslide_unk01(-1), skyslide_unk02(-1.0), skyslide_unk03(-1.0), skyslide_unk04(0.1),
				   slide_unk01(0), skyslide_unk05(1), unstable_unk01(1), slide_unk02(0), slide_unk03(-9999.900391) {}
				   
		void Fill(r3::RideCar_V& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
		void Fill(r3::RideCar_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
		
		void initSoaked() {
			skyslide_unk01 = 0.0;
			skyslide_unk03 = 4.0;
		}
	} sliding;
	struct Soaked {
		unsigned long            unk62;			///< Always 0, never symref
		unsigned long            unk63;			///< Always 0, never symref
		float             		 unk64;          ///< Always -1.0
		float             		 supersoaker_unk01;	///< Seen 0, 4
		unsigned long            supersoaker_unk02;	///< Seen 0, 1
		unsigned long            unk70;			///< Always 0, never symref
		unsigned long            unk72;          ///< always 0, never symref
		Soaked(): unk62(0), unk63(0), unk64(-1.0), supersoaker_unk01(0.0), supersoaker_unk02(0), unk70(0), unk72(0) {}
		void Fill(r3::RideCar_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} soaked;
	struct Wild {
		unsigned long     flip_unk;		///< Seen 1, 2
		std::string       svd_flipped_ref;        	///< Flipped body svd for splitting (siezmic)
		std::string 	  moving_svd_flipped_ref;    ///< Flipped seat svd for splitting (siezmic)
		unsigned long     unk77;          ///< Always 0, never symref
		float             drifting_unk;   ///< Seen 0, 1.0
		float             unk79;          ///< Always 1.0
		std::string       was_ref;        ///< Symbol reference to a was (WildAnimalSpecies)
		float             paddle_steamer_unk01;	// Seen 4.1, 0.05
		unsigned long     paddle_steamer_unk02;   // Seen 0, 1
		unsigned long     ball_coaster_unk;	///< Seen 0, 1
		Wild(): flip_unk(1), svd_flipped_ref(""), moving_svd_flipped_ref(""), unk77(0), drifting_unk(1.0), unk79(1.0),
		        was_ref(""), paddle_steamer_unk01(4.1), paddle_steamer_unk02(0), ball_coaster_unk(0) {}
		void Fill(r3::RideCar_W& rc, ovlStringTable* tab, ovlRelocationManager* rel) const;
	} wild;
	
	cRideCar(): name(""), internalname(""), otherinternalname(""), version(0), svd_ref(""), inertia(500.0) {}

    void Fill(r3::RideCar_W* rc, ovlStringTable* tab, ovlRelocationManager* rel) const;

	unsigned long GetCommonSize() const;
    unsigned long GetUniqueSize() const;
};

class ovlRICManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cRideCar>  m_items;
    unsigned long m_commonsize;

public:
    ovlRICManager(): ovlOVLManager() {
        m_commonsize = 0;
    };

    void AddItem(const cRideCar& item);

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

