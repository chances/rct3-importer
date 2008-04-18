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
    char*	     	    name;			// Possibly a language lookup
    char*		        username;
    uint8_t             seating;        ///< Seating type (@see RCT3_Seating)
    uint8_t             version;        ///< Structure version (0 Vanilla, 2 Soaked and 3 Wild)
    uint16_t		    unused;
    SceneryItemVisual_V* svd_ref;
    float_t		        weight1;	    ///< car body weight.
                                        /**<
                                         * Is zero if there isn't really a main body (eg slides, suspended,
                                         * bobs).
                                         * otherwise in the range 0.1 - 5500. Never < 0.
                                         **/
    SceneryItemVisual_V* seatsvd_ref;    ///< If seats move in some way (rotating mouse, multidimensional)
    uint32_t            free_axis;      ///< Rotation axis for seat vs. base movement
                                        /**<
                                         * 0 none
                                         * 2 horizontal axis, crossing the track (y in right handed z up).
                                         *   Used for multidimensional and chair lift
                                         * 3 horizontal axis, along the track (x in rhzu).
                                         *   Used for bobs, slides and free hanging suspended. Many only use
                                         *   a virtual base to rotate against. Bobs and slides use this to achive
                                         *   the illusion of free movement in curves.
                                         * 4 horizontal axis, along the track (x in rhzu)
                                         *   Spinnig and sliding cars.
                                         **/
    float_t             weight2;        ///< Probably weight of seats. -1.0 for cars withot rotation
                                        /**<
                                         * otherwise in the range 40 - 990
                                         **/
    float_t             unk9;           ///< Mostly 4
                                        /**<
                                         * 0.1 chair lift cars
                                         * 0.2 - 0.8 mostly free spinning cars, includes BallCoaster
                                         * 2 Suspendend and bobs, some slides, Xtend Drifting and Wild Mine
                                         **/
    float_t             unk10;          ///< Almost allways 0
                                        /**<
                                         * 0.2 - 12 for slides.
                                         * 4 for WildMine
                                         * Probably something with swinging/sideways movement
                                         **/
    float_t             unk11;          ///< Almost allways -1
                                        /**<
                                         * 3 for BallCoaster
                                         * 5 for River Raft cars
                                         **/
    float_t             unk12;          ///< Mostly 0.1
                                        /**<
                                         * 0.01 for Chair and Ski Lift
                                         * 0.2 for suspendeds, bobs, some slides
                                         * 0.3 for most spinnig cars (virginial reel cars, but also halfpipe and eurospinner)
                                         * 0.5-0.7 for Xtend Drifting, two slides, Wild Mine and BallCoaster
                                         **/
    float_t             unk13;          ///< Almost always 0
                                        /**<
                                         * 4000 for Xtend Drifting
                                         * 10000 for Wild Mine
                                         * Probably related to left/right swinging for fixed situations
                                         **/
    float_t             unk14;          ///< Mostly 0
                                        /**<
                                         * 0.1 for suspended and bobs
                                         * 0.5-0.9 for slides
                                         * Probably related to the left/right swinging/physics
                                         **/
    float_t             unk15;          ///< Almost always -1.0
                                        /**<
                                         * 0 for FrequentFaller
                                         * 0.2 for Wild Mine
                                         * 0.785 for Xtend Drifting
                                         * 0.9 for Super Soaker
                                         **/
    uint32_t            water_car;      ///< 1 for water cars, 0 otherwise.
                                        /**<
                                         * maybe floating, bobbing?
                                         **/
    float_t             water_float1;   // Seen 0
    float_t             water_float2;   // Seen 0
    float_t             water_float3;   // Seen 0
    int32_t             unk20;          ///< -1, 1 or 3.
                                        /**<
                                         * -1 Most common
                                         * 1 Several. Among them are eg the tour boats, but also others. No idea
                                         * 3 for rotating tower cars (observation towers and rotodrop)
                                         **/
    int32_t             unk21;          // Seen -1
    int32_t             unk22;          // Seen -1
    int32_t             unk23;          // Seen -1
    uint32_t            unk24;          // Seen 0
    uint32_t            unk25;          // Seen 1
    uint32_t            unk26;          // Seen 2
    int32_t             unk27;          // Seen -1
    int32_t             unk28;          // Seen -1
    int32_t             unk29;          // Seen -1
    int32_t             unk30;          // Seen -1
    int32_t             unk31;          // Seen -1
    int32_t             unk32;          // Seen -1
    int32_t             unk33;          // Seen -1
    int32_t             unk34;          // Seen -1
    int32_t             unk35;          // Seen -1
    int32_t             unk36;          // Seen -1
    int32_t             unk37;          // Seen -1
    int32_t             unk38;          // Seen -1
    int32_t             unk39;          // Seen -1
    uint32_t            unk40;
    uint32_t            unk41;
    uint32_t            unk42;
    uint32_t            unk43;
    uint32_t            unk44;
    uint32_t            unk45;
    uint32_t            unk46;
    uint32_t            unk47;
    uint32_t            unk48;
    uint32_t            unk49;
    SceneryItemVisual_V* axelsvd_ref;
    uint32_t            unk51;
    uint32_t            unk52;          // Seen 4
    uint32_t            unk53;          // Seen 4
    uint32_t            unk54;
    float_t             unk55;          // Seen -1.0
    int32_t             unk56;          // Seen -1
    float_t             unk57;          // Seen -1.0
    float_t             unk58;          // Seen -1.0
    float_t             unk59;          // Seen -1.0
    float_t             unk60;          // Seen -1.0
};

struct RideCar_Sext {
    float_t             unk61;          // Seen 0.1
    uint32_t            unk62;
    uint32_t            unk63;
    float_t             unk64;          // Seen -1.0
    uint32_t            unk65;
    uint32_t            unk66;          // Seen 1
    uint32_t            unk67;
    uint32_t            unk68;
    uint32_t            unk69;          // Seen 1
    uint32_t            unk70;
    uint32_t            unk71;
    uint32_t            unk72;          // Seen 0
    float_t             unk73;          // Seen -9999.9 (0xc61c3f9a)
};

struct RideCar_S {
    RideCar_V           v;
    RideCar_Sext        s;
};

struct RideCar_Wext {
    uint32_t            unk74;          // Seen 1
    SceneryItemVisual_W* svd_flipped_ref;        ///< Flipped body svd for splitting (siezmic)
    SceneryItemVisual_W* seatsvd_flipped_ref;    ///< Flipped seat svd for splitting (siezmic)
    uint32_t            unk77;          // Seen 0
    float_t             unk78;          // Seen 1.0
    float_t             unk79;          // Seen 1.0
    void*               was_ref;        ///< Symbol reference to a was (WildAnimalSpecies)
                                        /**
                                         * Used for Elephant transport
                                         */
    float_t             unk81;          // Seen 4.1
    uint32_t            unk82;          // Seen 0
    int32_t             unk83;          // Seen -1
    uint32_t            unk84;          // Seen 0
};

struct RideCar_W {
    RideCar_V           v;
    RideCar_Sext        s;
    RideCar_Wext        w;
};

struct RideTrain_V {
    char*           	name;              	///< txt for train name, not a symref. 0xFFFFFFFF for RideTrainB/C
    char*            	description;        ///< txt for train description, not a symref
                                            /**<
                                             * In Soaked and Wild this could be user visible text, it contains spaces
                                             * and stuff usually not present for an internal name.
                                             **/
    RideCar_V*         	frontcar_ref;
    RideCar_V*         	secondcar_ref;
    RideCar_V*         	midcar_ref;
    RideCar_V*         	penultimatecar_ref;
    RideCar_V*         	rearcar_ref;
    RideCar_V*         	link_ref;	        /// Pointer to the object that links cars together.
    uint32_t    	    minimum_cars;       /// the minimum number of cars for this train
    uint32_t    	    maximum_cars;		/// the maximum number of cars for this train
    uint32_t    	    initial_cars;		/// the initial number of cars for this train when the ride is created
    float_t	     	    unkfloats1[35];		/// 35 unknown floats, unk12 - unk46
    char*	     	    configuration;      /// Platform? Seen "NoPlatform"
};

struct RideTrain_Sext {
    uint32_t    	    unk48;			    ///< Seen 2
    char*	     	    carname;            ///< Probably equivalent to name in V structure
    uint32_t    	    unk50;              ///< Seen 0, 1, 2
    uint32_t    	    unk51;              ///< Seen 0
};

struct RideTrain_S {
    RideTrain_V         v;
    RideTrain_Sext      s;
};

struct RideTrain_Wext {
    uint32_t    	    unk52;              // Seen 0
    uint32_t    	    unk53;              // Seen 0
    uint32_t    	    unk54;              // Seen 0
    float_t    	        unk55;              // Seen -1.0
    float_t    	        unk56;              // Seen -1.0
    float_t    	        unk57;              // Seen -1.0
    float_t    	        unk58;              // Seen 1.0
    uint32_t    	    unk59;              // Seen 0
    uint32_t    	    unk60;              // Seen 0
    float_t           	unk61;              // Seen 0.0, 90.0 on FrequentFaller
};

struct RideTrain_W {
    RideTrain_V         v;
    RideTrain_Sext      s;
    RideTrain_Wext      w;
};


};

#endif
