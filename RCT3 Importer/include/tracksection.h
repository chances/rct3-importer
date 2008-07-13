/*
    Originally appeared in
    RCT3 File Dumper
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 File Dumper
	The RCT3 File Dumper Program is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#ifndef TRACKSECTION_H_INCLUDED
#define TRACKSECTION_H_INCLUDED

#include "rct3basetypes.h"

#include "sceneryrevised.h"
#include "sharedride.h"
#include "spline.h"

namespace r3 {

struct TrackSectionSpeedStruct_V {
    float_t             unk1;
    float_t             unk2;
    float_t             unk3;
};

struct TrackSectionSpeedStruct_Sext {
    float_t             unk4;
    float_t             unk5;
};

struct TrackSectionSpeedStruct_SW {
    TrackSectionSpeedStruct_V       v;
    TrackSectionSpeedStruct_Sext    s;
};

struct TrackSectionAnimation_V {
    int32_t         stopped;                ///< Plays while the ride is stopped
    int32_t         starting;               ///< Plays once when the ride is started
    int32_t         running;                ///< Plays while the ride is running
    int32_t         stopping;               ///< If starting is -1, this is -1 too, plobably depends on starting and/or running
    int32_t         hold_after_train_stop;  ///< On holding sections, plays once after train stops
    int32_t         hold_loop;              ///< On holding sections, plays while the train is held
    int32_t         hold_before_release;    ///< On holding sections, plays once before the train is released
    int32_t         hold_leaving;           ///< On holding sections, plays once or while the train is leaving
    int32_t         hold_after_train_left;  ///< On holding sections, probably plays once after train has left. Seems to depend on hold_leaving?
};

struct TrackSectionAnimation_Sext {
    int32_t         pre_station_leave;      ///< Used for filling water in Roller Soaker
};

struct TrackSectionAnimation_S {
    TrackSectionAnimation_V     v;
    TrackSectionAnimation_Sext  s;
};

struct TrackSectionAnimation_Wext {
    int32_t         rotating_tower_idle;    ///< No rotation animation on rotating tower caps
};

struct TrackSectionAnimation_W {
    TrackSectionAnimation_V     v;
    TrackSectionAnimation_Sext  s;
    TrackSectionAnimation_Wext  w;
};

struct TrackSectionSpeedSplines {
    float_t         speed_selector; ///< In the array the first elemant has 0.0, the last 15.0 for the slides. The others are distributed equally
    Spline*         left_ref;       ///< First one duplicates the normal track splines
    Spline*         right_ref;
};

struct TrackSection_V {
    char*           internalname;
    SceneryItem_V*  sid_ref;
    uint32_t        entry_curve;	    ///< Probably curve related
    uint32_t        exit_curve;	        ///< Probably curve related
    uint32_t        special_curves;	    ///< Special curve types
    uint32_t        direction;	        ///< 0 straight, 1 left, 2 right
    uint32_t        entry_flags; 		///< Bitflags
    uint32_t        exit_flags; 		///< Usually identical to above.
    Spline*	        spline_left_ref;
    Spline*	        spline_right_ref;
    Spline*	        join_spline_left_ref;
    Spline*	        join_spline_right_ref;
    Spline*	        extra_spline_left_ref;
    Spline*         extra_spline_right_ref;
    uint32_t        unk15;              ///< Allways 0, never a symref
    uint32_t        unk16;              ///< Allways 0, never a symref
    uint32_t        unk17;              ///< Allways 0, never a symref
    uint32_t        unk18;		        ///< Set to either 0 or 25 (0x19)
    uint32_t        entry_slope; 	    /**< Slope
                                         * 0: flat
                                         * 1: medium up
                                         * 2: medium down
                                         * 3: steep up
                                         * 4: steep down
                                         * 5: vertical
                                         **/
    uint32_t        entry_bank;
                // 0: flat
                // 1: bank left
                // 2: bank full left
                // 3: inverted bank left
                // 4: inverted
                // 5: inverted bank right
                // 6: bank full right
                // 7: bank right
    char*           entry_trackgroup;
    uint32_t        unk22;              ///< Allways 0, never a symref
    uint32_t        unk23;              ///< Allways 0, never a symref
    uint32_t        unk24;              ///< Allways 0, never a symref
    uint32_t        unk25;		// Corresponds with unk18 in all known cases
    uint32_t        exit_slope;	// see entry_slope
    uint32_t        exit_bank;	// see entry_bank
    char*           exit_trackgroup;
    uint32_t        speed_count;
    union {
        TrackSectionSpeedStruct_V*  speeds; 		///< Somehow modifies speed.
        TrackSectionSpeedStruct_SW* speeds_sw;
    };
    uint32_t        tower_ride_base_flag;   ///< Usually 0, set to 1 on tower ride bases and elevator top
    float_t         tower_unkf01;           ///< Usually 0.0, some tower ride pieces have 0 < x < 1
    float_t         water_splash01;         ///< Usually 0.0, water splash has 5.1
    float_t         water_splash02;         ///< Usually 0.0, water splash has 15.3
    float_t         reverser_val;           ///< Usually 0.0, reverser pieces have 0.75
/*
    union {
        int32_t     unk36;              ///< -1 or for _V
        int32_t     long_count_sw; 		///< entries in the following array for _S and _W. ??? If -1 then the structure ends here. ???
    };
    union {
        int32_t     unk37;
        int32_t*    longs_sw;
    };
    uint32_t        unk38;
    uint32_t        unk39;
    uint32_t        unk40;
    uint32_t        unk41;
    uint32_t        unk42;
    uint32_t        unk43;
    uint32_t        unk44;
*/
    union {
        TrackSectionAnimation_V anim;
        struct {
            int32_t     anim_count;         ///< 10 for soaked, 11 for wild
            union {
                TrackSectionAnimation_S* anim_ex_s;
                TrackSectionAnimation_W* anim_ex_w;
            };
        };
    };
    uint32_t        unk45;                  ///< Always 0, never a symref
    float_t         elevator_top_val;       ///< Usually 0.0, elevator top has 4.1
    uint32_t        unk47;                  ///< Always 0, never a symref
    float_t         rapids01;               ///< Usually 1.0
                                            /**<
                                             * 0.1 White Water Rapids Station
                                             * 2.0 Rapdis Waterfall and Whirlpool
                                             * 6.0 Rapids Rapids; White Water Rapids Rapids and Waterfall
                                             **/
    float_t         rapids02;               ///< Usually identical to rapids01
                                            /**<
                                             * 1.2 Rapdis Whirlpool
                                             * 8.0 Rapids Rapids; White Water Rapids Rapids and Waterfall
                                             **/
    float_t         rapids03;               ///< Identical to rapids01
    float_t         whirlpool01;            ///< Usually 0.0, 3.0 for Rapids Whirlpool. Spins compatible cars. Pos = clockwise
    float_t         whirlpool02;            ///< Usually 0.0, 3.0 for Rapids Whirlpool. Identical to above, both seem necessary to make it work
    uint32_t        unk53;                  ///< Always 0, never a symref
    uint32_t        water_spline_flag;      ///< 0: no water splines, 1 water splines
    uint32_t        water_spline_left_ref;
    uint32_t        water_spline_right_ref;
    char*           chair_lift_station_end; ///< Usually not relocated, chair lift station end has "Alt"
};

struct TrackSection_Sext {
    uint32_t                version;                        ///< Maybe structure version (soaked 2, wild 3)
    Spline*                 loop_spline_ref;
    uint32_t                path_count;
    Spline**                paths_ref;
    uint32_t                ride_station_limit_count;
    RideStationLimit*       ride_station_limits;            ///< Present in eg Insect House, Giant Slide
    float_t                 tower_unkf2;                    ///< Usually -1.0, Tower pieces can have: 0.1, 0.3
    float_t                 aquarium_val;                   ///< Usually -1.0, Aquarium pieces have -2.0
    char*                   auto_group;                     ///< Group for auto/mid/end selection
    int32_t                 giant_flume_val;                ///< Usually -1, All Giant Flume pieces except station have 2
    uint32_t                unk68;                          ///< Always 0, never a symref
    uint32_t                unk69;                          ///< Always 0, never a symref
    uint32_t                entry_wide_flag;                ///< Usually 0, 1 for pieces that start wide
    uint32_t                exit_wide_flag;                 ///< Usually 0, 1 for pieces that end wide
    uint32_t                speed_spline_count;              ///< Usually 0, 4 on slide bowl, 6 on slide funnel
    TrackSectionSpeedSplines* speed_splines;
    float_t                 slide_end_to_lifthill_val;      ///< Usually -1.0, Slide end to lifthills have: 7.7, 10
    uint32_t                soaked_options;                 ///< Spline Options?
                                                            /** Vals:
                                                             * 0 Most, including elevator base
                                                             * 24 Animal Houses, most Tower Ride bases 16+8
                                                             * 16 Free Fall Base
                                                             * 3 Slide station transition 1+2
                                                             * 36 Slide to lift end 32+4
                                                             * 6 Bowl and most Funnel 2+4
                                                             * 4 Funnel right capped 4
                                                             */
    float_t                 speed_spline_val;                ///< Usually -1.0, parts with speed splines have 5
    float_t                 unk77;                          ///< Always -1.0
    float_t                 unk78;                          ///< Always -1.0
    float_t                 unk79;                          ///< Always -1.0
    uint32_t                group_is_at_entry_count;
    char**                  groups_is_at_entry;             ///< Defines the entry groups for this part (does by itself not force anything)
                                                            /** Used:
                                                             * Glider spiral lift hill bottom (SpiralLiftHill)
                                                             * Rotating tower base down (Tower)
                                                             * Rotating tower both rotators and both vertical (Tower)
                                                             */
    uint32_t                group_is_at_exit_count;
    char**                  groups_is_at_exit;              ///< Defines the exit groups for this part (does by itself not force anything)
                                                            /** Used:
                                                             * LogFlume (45)medslope2straight (TopOfLiftHill)
                                                             * Rotating tower base (Tower)
                                                             * Rotating tower both rotators and both vertical (Tower)
                                                             */
    uint32_t                group_must_have_at_entry_count;
    char**                  groups_must_have_at_entry;      ///< Part before must have one (all?) of these as exit group(s)
                                                            /** Used:
                                                             * Rotating tower base down (Tower)
                                                             * Rotating tower both rotators and both vertical (Tower)
                                                             */
    uint32_t                group_must_have_at_exit_count;
    char**                  groups_must_have_at_exit;       ///< Part after must have one (all?) of these as entry group(s)
                                                            /** Used:
                                                             * Rotating tower base (Tower)
                                                             * Rotating tower both rotators and both vertical (Tower)
                                                             */
    uint32_t                group_must_not_be_at_entry_count;
    char**                  groups_must_not_be_at_entry;    ///< A part that has one (all?) of these exit groups may not come directly before
                                                            /** Used:
                                                             * LogFlume Reverser (TopOfLiftHill)
                                                             */
    uint32_t                group_must_not_be_at_exit_count;
    char**                  groups_must_not_be_at_exit;     ///< A part that has one (all?) of these entry groups may not come directly after
                                                            /** Used:
                                                             * Glider station and block breaks (SpiralLiftHill)
                                                             */
};

struct TrackSection_S {
    TrackSection_V      v;
    TrackSection_Sext   s;
};

struct TrackSection_W;
struct TrackSection_Wext {
    uint32_t                splitter_half;                  ///< Which half of the track for the Splitting coaster
    TrackSection_W*         splitter_joined_other_ref;      ///< For joned sections a symref to the respective other part.
    uint32_t                rotator_type;                   ///< Type for tower coaster rotator
    float_t                 animal_house;                   ///< Usually -1, 1 for Insect an Reptile, 0.1 for Nocturnal House
    char*                   alternate_text_lookup;
    float_t                 tower_cap01;                    ///< Usually 0, -1 on tower cap
    float_t                 tower_cap02;                    ///< Usually -1, 14.75 on tower cap
};

struct TrackSection_W {
    TrackSection_V      v;
    TrackSection_Sext   s;
    TrackSection_Wext   w;
};

};

#endif // TRACKSECTION_H_INCLUDED
