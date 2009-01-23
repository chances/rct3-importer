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


#ifndef __car_h__
#define __car_h__

#include "sceneryvisual.h"

namespace r3 {

struct RideCar_V {
    char*	     	    name;			///< Possibly a language lookup, shown in error messages
    char*		        username;		///< No known effect in game
    uint8_t             seating;        ///< Seating type (@see RCT3_Seating)
    uint8_t             version;        ///< Structure version (0 Vanilla, 2 Soaked and 3 Wild)
    uint16_t		    unused;
    SceneryItemVisual_V* svd_ref;
    float_t		        inertia;	    ///< car body inertia.
                                        /**<
                                         * Is zero if there isn't really a main body (eg slides, suspended,
                                         * bobs).
                                         * otherwise in the range 0.1 - 5500. Never < 0.
                                         **/
    SceneryItemVisual_V* moving_svd_ref;    ///< If the car has a moving part (rotating mouse, multidimensional)
    uint32_t            axis_type;      ///< Rotation axis for moving part vs. base movement
                                        /**<
                                         * 0 none
                                         * 2 horizontal axis, crossing the track (y in right handed z up).
                                         *   Used for multidimensional and chair lift
                                         * 3 horizontal axis, along the track (x in rhzu).
                                         *   Used for bobs, slides and free hanging suspended. Many only use
                                         *   a virtual base to rotate against. Bobs and slides use this to achive
                                         *   the illusion of free movement in curves.
                                         * 4 vertical axis (z in rhzu)
                                         *   Spinnig and sliding cars.
                                         **/
    float_t             moving_inertia;	///< Inertia of seats. -1.0 for cars without rotation
                                        /**<
                                         * otherwise in the range 40 - 990
                                         **/
    float_t             axis_stability; ///< Distance of seat mass to rotation axis
                                        /**<
										 * Usually 4
										 * 
                                         * 0.1 chair lift cars
                                         * 0.2 - 0.8 on some free spinning cars, includes BallCoaster
                                         * 2 Suspendend and bobs, some slides, Xtend Drifting and Wild Mine
                                         **/
    float_t             axis_unk01;     ///< Probably something with swinging/sideways movement
                                        /**<
										 * Usually 0
										 * 
                                         * 0.2 - 12 for slides.
                                         * 4 for WildMine
                                         * 
                                         **/
    float_t             axis_unk02;     ///< Almost allways -1
                                        /**<
										 * Usually -1
										 * 
                                         * 3 for BallCoaster
                                         * 5 for River Raft cars
                                         **/
    float_t             axis_momentum;  ///< Momentum added by movement along the track
                                        /**<
										 * Usually 0.1
										 * 
                                         * 0.01 for Chair and Ski Lift
                                         * 0.2 for suspendeds, bobs, some slides
                                         * 0.3 for most spinnig cars (virginial reel cars, but also halfpipe and eurospinner)
                                         * 0.5-0.7 for Xtend Drifting, two slides, Wild Mine and BallCoaster
                                         **/
    float_t             axis_unk03;     ///< Probably related to left/right swinging for fixed situations
                                        /**<
										 * Usually 0
										 * 
                                         * 4000 for Xtend Drifting
                                         * 100000 for Wild Mine
                                         **/
    float_t             axis_unk04;     ///< Probably related to the left/right swinging/physics
                                        /**<
										 * Usually 0
										 * 
                                         * 0.1 for suspended and bobs
                                         * 0.5-0.9 for slides
                                         **/
    float_t             axis_maximum;   ///< Maximum rotational displacement
                                        /**<
										 * Almost always -1.0
										 * 
                                         * 0 for FrequentFaller
                                         * 0.2 for Wild Mine
                                         * 0.785 for Xtend Drifting
                                         * 0.9 for Super Soaker
                                         **/
    uint32_t            bobbing_flag;   ///< Activate bobbing
                                        /**<
                                         * Set on water cars
										 * 
										 * Following values are in in-game coordinate system
                                         **/
    float_t             bobbing_x;   	// Seen 0
    float_t             bobbing_y;   	// Seen 0
    float_t             bobbing_z;   	// Seen 0
    int32_t             anim_start;     		///< Runs when the car starts
    int32_t             anim_started_idle;		///< Runs when the car is on track
    int32_t             anim_stop;      		///< Runs when the car stops
    int32_t             anim_stopped_idle;  	///< Runs while the car is in a station
    int32_t             anim_belt_open; 		///< Runs to open belts
    int32_t             anim_belt_open_idle;	///< Runs while belts are open
    int32_t             anim_belt_close;        ///< Runs to close belts
    int32_t             anim_belt_closed_idle;	///< Runs while belts are closed
    int32_t             anim_doors_open;		///< Runs to open doors
    int32_t             anim_doors_open_idle;   ///< Runs while doors are open
    int32_t             anim_doors_close;		///< Runs to close doors
    int32_t             anim_doors_closed_idle; ///< Runs while doors are closed
    int32_t             anim_row_both;
    int32_t             anim_row_left;
    int32_t             anim_row_right;
    int32_t             anim_canoe_station_idle;
    int32_t             anim_canoe_idle_front;
    int32_t             anim_canoe_idle_back;
    int32_t             anim_canoe_row_idle;
    int32_t             anim_canoe_row;
    uint32_t            seat_type_count;		///< Alternate seat postitions for peeps
    uint32_t*           seat_types;
    SceneryItemVisual_V* wheels_front_right_ref;	///< SVD for front right wheel
    uint32_t            wheels_front_right_flipped; ///< Flip front right wheel (to reuse left)
    SceneryItemVisual_V* wheels_front_left_ref;
    uint32_t            wheels_front_left_flipped;
    SceneryItemVisual_V* wheels_back_right_ref;
    uint32_t            wheels_back_right_flipped;
    SceneryItemVisual_V* wheels_back_left_ref;
    uint32_t            wheels_back_left_flipped;
    SceneryItemVisual_V* axel_front_ref;	///< SVD for front axel
    SceneryItemVisual_V* axel_rear_ref;		///< SVD for rear axel
    uint32_t            axel_front_type;	///< Probably rotation axis or type for front axel
											/**<
											 * 1 Vertical? (eg Arrows car)
											 * 2 Only occurs on rear axel.
											 * 3 Horizontal? (eg Heartline car)
											 * 4 No axel (no svd)
											 **/
    uint32_t            axel_rear_type;		///< Probably rotation axis or type for rear axel
    uint32_t            unk54;          ///< Always 0, never symref
    float_t             vs_unk01;       // Seen -1.0, 0
										/**<
										 * Vanilla: always -1.0
										 * Soaked/Wild: always 0
										 **/
    int32_t             skyslide_unk01; // Seen -1, 0, 4
										/**<
										 * Vanilla: always -1
										 * Soaked/Wild:
										 *   Usually 0
										 *   Skyrider, BodySlide, MatSlide, RaftSlide, RingSlide 4
										 **/ 
    float_t             skyslide_unk02; // Seen -1.0, rarely 40, 150 or 400 on Slides and SkyRider
										/**<
										 * Usually -1
										 * Skyrider 400
										 * BodySlide, MatSlide 40
										 * RaftSlide, RingSlide 150
										 **/
    float_t             skyslide_unk03; // Seen -1.0, 4.0, rarely 0.18, 0.2,
										/**<
										 * Vanilla: always -1
										 * Soaked/Wild:
										 *    Usually 4
										 *    Skyrider, BodySlide, MatSlide, RingSlide 0.2
										 *    Raftslide 0.18
										 **/
    float_t             vs_unk02;       // Seen -1.0, 0.0
										/**<
										 * Vanilla: always -1.0
										 * Soaked/Wild: always 0
										 **/
    float_t             unk60;          ///< always -1.0
};

struct RideCar_Sext {
    float_t             skyslide_unk04; // Seen 0.1
										/**<
										 * Usually 0.1
										 * 
										 * 0.25 RaftSlide
										 * 0.3  Skyrider, BodySlide, MatSlide, RingSlide
										 **/
    uint32_t            unk62;			///< Always 0, never symref
    uint32_t            unk63;			///< Always 0, never symref
    float_t             unk64;          ///< Always -1.0
    uint32_t            slide_unk01;	///< Seen 0, 1
										/**<
										 * Usually 0
										 * 
										 * BodySlide, MatSlide, RaftSlide, RingSlide 1
										 * 
										 * Flag?
										 **/
    uint32_t            skyslide_unk05; ///< Seen 1, 4
										/**<
										 * Usually 1
										 * 
										 * Skyrider, RaftSlide, RingSlide 4
										 **/
    float_t             supersoaker_unk01;	///< Seen 0, 4
										/**<
										 * Usually 0
										 * 
										 * SuperSoaker 4
										 **/
    uint32_t            supersoaker_unk02;	///< Seen 0, 1
										/**<
										 * Usually 0
										 * 
										 * SuperSoaker 1
										 **/
    uint32_t            unstable_unk01;	///< Occurs on bobs and slides, looks like flags
										/**<
										 *   1 0x0001 Usually
										 * 
										 *   9 0x0009 WindSurfer
										 *  49 0x0031 LargeRiverRaft, RapidRiver
										 * 112 0x0070 MasterBlaster
										 * 624 0x0270 ProBowlRubberRing, RaftSlide, RingSlide
										 * 625 0x0271 Bobsleigh, Dingy, PenguinBobsleigh
										 * 626 0x0272 MatSlide
										 * 630 0x0276 BodySlide
										 **/
    uint32_t            unk70;			///< Always 0, never symref
    float_t             slide_unk02;	///< Seen 0, 1, 3.1
										/**<
										 * Usually 0
										 * 
										 * 1   MasterBlaster
										 * 3.1 RaftSlide, RingSlide
										 **/
    uint32_t            unk72;          ///< always 0, never symref
    float_t             slide_unk03;    ///< Seen -9999.9 (0xc61c3f9a), 0.2, 0.3
										/**<
										 * Usually -9999.9
										 * 
										 * 0.2 RaftSlide
										 * 0.3 RingSlide
										 **/
};

struct RideCar_S {
    RideCar_V           v;
    RideCar_Sext        s;
};

struct RideCar_Wext {
    uint32_t            flip_unk;		///< Seen 1, 2
										/**<
										 * Usually 1
										 * Siezmic 2
										 **/
    SceneryItemVisual_W* svd_flipped_ref;        	///< Flipped body svd for splitting (siezmic)
    SceneryItemVisual_W* moving_svd_flipped_ref;    ///< Flipped seat svd for splitting (siezmic)
    uint32_t            unk77;          ///< Always 0, never symref
    float_t             drifting_unk;   ///< Seen 0, 1.0
										/**<
										 * Usually 1
										 * XtendedDrifting 0
										 **/
    float_t             unk79;          ///< Always 1.0
    void*               was_ref;        ///< Symbol reference to a was (WildAnimalSpecies)
                                        /**<
                                         * Used for Elephant transport
                                         */
    float_t             paddle_steamer_unk01;	// Seen 4.1, 0.05
										/**<
										 * Usually 4.1
										 * PaddleSteamer 0.05
										 **/
    uint32_t            paddle_steamer_unk02;   // Seen 0, 1
										/**<
										 * Usually 0
										 * PaddleSteamer 1
										 **/
    int32_t             anim_rudder;    ///< Rudder/fan animation, only used on AirBoat
    uint32_t            ball_coaster_unk;	///< Seen 0, 1
										/**<
										 * Usually 0
										 * BallCoaster 1
										 **/
};

struct RideCar_W {
    RideCar_V           v;
    RideCar_Sext        s;
    RideCar_Wext        w;
};

};

#endif
