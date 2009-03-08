///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for TKS structures
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


#ifndef MANAGERTKS_H_INCLUDED
#define MANAGERTKS_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "tracksection.h"
#include "ManagerOVL.h"
#include "ManagerCommon.h"

struct cSplinePair {
    std::string left;
    std::string right;
};

struct cTrackSectionSpeed {
    float unk01;
    float unk02;
    float unk03;
    float unk04;
    float unk05;

    cTrackSectionSpeed(): unk01(-99999.0), unk02(8.0), unk03(30000.0), unk04(-1.0), unk05(-1.0) {}
    void Fill(r3::TrackSectionSpeedStruct_V& tkss) const;
    void Fill(r3::TrackSectionSpeedStruct_SW& tkss) const;
};

struct cTrackSectionSpeedSplines {
    float speed;
    cSplinePair splines;
    struct Compare {
        bool operator() (const cTrackSectionSpeedSplines& lhs, const cTrackSectionSpeedSplines& rhs) const {
            return lhs.speed < rhs.speed;
        }
    };

    cTrackSectionSpeedSplines(): speed(0.0) {}
    void Fill(r3::TrackSectionSpeedSplines& tkss) const;
};

struct cTrackSectionEntryExit {
    unsigned long curve; // Maybe icon? Values are 0-7
    unsigned long flags;
    unsigned long unknown; // 0 or 0x25
    unsigned long slope;
    unsigned long bank;
    std::string group;
    cTrackSectionEntryExit(): curve(0), flags(0), unknown(0), slope(0), bank(0) {}
};

class cTrackSection {
public:
    typedef std::pair<std::string, cTrackSection> mapentry;

    std::string name;
    std::string internalname;
    std::string sid;
    unsigned long version; ///< 0,2,3 scheme
    std::vector<cTrackSectionSpeed> speeds;

    struct Basic {
        cTrackSectionEntryExit entry;
        cTrackSectionEntryExit exit;
        unsigned long special;
        unsigned long direction;
        Basic(): special(0), direction(0) {}
        void Fill(r3::TrackSection_V& tva, ovlStringTable* tab, ovlRelocationManager* rel) const;
    } basic;
    struct Splines {
        cSplinePair car;
        cSplinePair join;
        cSplinePair extra;
        cSplinePair water;
        Splines() {}
        void Fill(r3::TrackSection_V& tva) const;
    } splines;
    struct Animations {
        int32_t         stopped;                ///< Plays while the ride is stopped
        int32_t         starting;               ///< Plays once when the ride is started
        int32_t         running;                ///< Plays while the ride is running
        int32_t         stopping;               ///< Usually a running animation except for the chair lift station end. If starting is -1, this is -1 too
        int32_t         hold_after_train_stop;  ///< On holding sections, plays once after train stops
        int32_t         hold_loop;              ///< On holding sections, plays while the train is held
        int32_t         hold_before_release;    ///< On holding sections, plays once before the train is released
        int32_t         hold_leaving;           ///< On holding sections, plays once or while the train is leaving
        int32_t         hold_after_train_left;  ///< On holding sections, probably plays once after train has left. Seems to depend on hold_leaving?
        int32_t         pre_station_leave;      ///< Used for filling water in Roller Soaker
        int32_t         rotating_tower_idle;    ///< No rotation animation on rotating tower caps
        Animations(): stopped(-1), starting(-1), running(-1), stopping(-1), hold_after_train_stop(-1), hold_loop(-1),
            hold_before_release(-1), hold_leaving(-1), hold_after_train_left(-1), pre_station_leave(-1), rotating_tower_idle(-1) {}
        void Fill(r3::TrackSectionAnimation_V& tva) const;
        void Fill(r3::TrackSectionAnimation_S& tva) const;
        void Fill(r3::TrackSectionAnimation_W& tva) const;
    } animations;
    struct Unknowns {
        uint32_t        unk15;              ///< Allways 0, never a symref
        uint32_t        unk16;              ///< Allways 0, never a symref
        uint32_t        unk17;              ///< Allways 0, never a symref
        uint32_t        unk22;              ///< Allways 0, never a symref
        uint32_t        unk23;              ///< Allways 0, never a symref
        uint32_t        unk24;              ///< Allways 0, never a symref
        uint32_t        unk45;                  ///< Always 0, never a symref
        uint32_t        unk47;                  ///< Always 0, never a symref
        uint32_t        unk53;                  ///< Always 0, never a symref
        // Soaked
        uint32_t                unk68;                          ///< Always 0, never a symref
        uint32_t                unk69;                          ///< Always 0, never a symref
        float                   unk77;                          ///< Always -1.0
        float                   unk78;                          ///< Always -1.0
        float                   unk79;                          ///< Always -1.0
        Unknowns(): unk15(0), unk16(0), unk17(0), unk22(0), unk23(0), unk24(0), unk45(0), unk47(0), unk53(0),
            unk68(0), unk69(0), unk77(-1.0), unk78(-1.0), unk79(-1.0) {}
        void Fill(r3::TrackSection_V& tva) const;
        void Fill(r3::TrackSection_S& tva) const;
    } unknowns;
    struct Options {
        uint32_t        tower_ride_base_flag;   ///< Usually 0, set to 1 on tower ride bases and elevator top
        float         tower_unkf01;           ///< Usually 0.0, some tower ride pieces have 0 < x < 1
        float         water_splash01;         ///< Usually 0.0, water splash has 5.1
        float         water_splash02;         ///< Usually 0.0, water splash has 15.3
        float         reverser_val;           ///< Usually 0.0, reverser pieces have 0.75
        float         elevator_top_val;       ///< Usually 0.0, elevator top has 4.1
        float         rapids01;               ///< Usually 1.0
        float         rapids02;               ///< Usually identical to rapids01
        float         rapids03;               ///< Identical to rapids01
        float         whirlpool01;            ///< Usually 0.0, 3.0 for Rapids Whirlpool
        float         whirlpool02;            ///< Usually 0.0, 3.0 for Rapids Whirlpool
        std::string     chair_lift_station_end; ///< Usually not relocated, chair lift station end has "Alt"
        Options(): tower_ride_base_flag(0), tower_unkf01(0.0), water_splash01(0.0), water_splash02(0.0), reverser_val(0.0),
            elevator_top_val(0.0), rapids01(1.0), rapids02(1.0), rapids03(1.0), whirlpool01(0.0), whirlpool02(0.0) {}
        void Fill(r3::TrackSection_V& tva, ovlStringTable* tab, ovlRelocationManager* rel) const;
    } options;
    struct SoakedOptions {
        std::string             loop_path;
        std::vector<std::string> paths;
        std::vector<cRideStationLimit> ride_station_limits;
        float                 tower_unkf2;                    ///< Usually -1.0, Tower pieces can have: 0.1, 0.3
        float                 aquarium_val;                   ///< Usually -1.0, Aquarium pieces have -2.0
        std::string             auto_group;                     ///< Group for auto/mid/end selection
        int32_t                 giant_flume_val;                ///< Usually -1, All Giant Flume pieces except station have 2
        uint32_t                entry_wide_flag;                ///< Usually 0, 1 for pieces that start wide
        uint32_t                exit_wide_flag;                 ///< Usually 0, 1 for pieces that end wide
        std::vector<cTrackSectionSpeedSplines> speeds;
        float                 speed_spline_val;                ///< Usually -1.0, parts with speed splines have 5
        float                 slide_end_to_lifthill_val;      ///< Usually -1.0, Slide end to lifthills have: 7.7, 10
        uint32_t                soaked_options;                 ///< Spline Options?
        std::vector<std::string> groups_is_at_entry;             ///< Defines the entry groups for this part (does by itself not force anything)
        std::vector<std::string> groups_is_at_exit;              ///< Defines the exit groups for this part (does by itself not force anything)
        std::vector<std::string> groups_must_have_at_entry;      ///< Part before must have one (all?) of these as exit group(s)
        std::vector<std::string> groups_must_have_at_exit;       ///< Part after must have one (all?) of these as entry group(s)
        std::vector<std::string> groups_must_not_be_at_entry;    ///< A part that has one (all?) of these exit groups may not come directly before
        std::vector<std::string> groups_must_not_be_at_exit;     ///< A part that has one (all?) of these entry groups may not come directly after
        SoakedOptions(): tower_unkf2(-1.0), aquarium_val(-1.0), giant_flume_val(-1), entry_wide_flag(0), exit_wide_flag(0),
            speed_spline_val(-1.0), slide_end_to_lifthill_val(-1.0), soaked_options(0) {}
        void Fill(r3::TrackSection_S& tva, ovlStringTable* tab, ovlRelocationManager* rel) const;
    } soaked;

    struct WildOptions {
        uint32_t                splitter_half;                  ///< Which half of the track for the Splitting coaster
        std::string             splitter_joined_other_ref;      ///< For joned sections a symref to the respective other part.
        uint32_t                rotator_type;                   ///< Type for tower coaster rotator
        float                 animal_house;                   ///< Usually -1, 1 for Insect an Reptile, 0.1 for Nocturnal House
        std::string             alternate_text_lookup;
        float                 tower_cap01;                    ///< Usually 0, -1 on tower cap
        float                 tower_cap02;                    ///< Usually -1, 14.75 on tower cap
        WildOptions(): splitter_half(0), rotator_type(0), animal_house(-1.0), tower_cap01(0.0), tower_cap02(-1.0) {}
        void Fill(r3::TrackSection_W& tva, ovlStringTable* tab, ovlRelocationManager* rel) const;
    } wild;

    cTrackSection() {
        version = 0;
    };
    void Fill(r3::TrackSection_V* tks, ovlStringTable* tab, ovlRelocationManager* rel) const;
    void Fill(r3::TrackSection_S* tks, ovlStringTable* tab, ovlRelocationManager* rel) const;

    unsigned long GetCommonSize() const;
    unsigned long GetUniqueSize() const;
};

class ovlTKSManager: public ovlOVLManager {
public:
    static const char* LOADER;
    static const char* NAME;
    static const char* TAG;
private:
    std::map<std::string, cTrackSection>  m_items;
    unsigned long m_commonsize;

public:
    ovlTKSManager(): ovlOVLManager() {
        m_commonsize = 0;
    };

    void AddSection(const cTrackSection& item);

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
