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


#ifndef __train_h__
#define __train_h__

#include "car.h"
#include "spline.h"

namespace r3 {

struct RideTrain_V {
	union {
		char*           name;           ///< txt for train name, not a symref. 0xFFFFFFFF for RideTrainB/C
		int32_t			v2;
	};
    char*            	description;        ///< txt for train description, not a symref
                                            /**<
                                             * In Soaked and Wild this could be user visible text, it contains spaces
                                             * and stuff usually not present for an internal name.
											 * 
											 * No known game effect
                                             **/
    RideCar_V*         	frontcar_ref;
    RideCar_V*         	secondcar_ref;
    RideCar_V*         	midcar_ref;
    RideCar_V*         	penultimatecar_ref;
    RideCar_V*         	rearcar_ref;
    RideCar_V*         	link_ref;	        ///< Pointer to the object that links cars together.
    uint32_t    	    car_count_min;       ///< the minimum number of cars for this train
    uint32_t    	    car_count_max;		///< the maximum number of cars for this train
    uint32_t    	    car_count_def;		///< the initial number of cars for this train when the ride is created
    //float_t	     	    unkfloats1[33];		/// 35 unknown floats, unk12 - unk44
	float_t				speed_unk01;		///< Usually 0.04
	float_t				speed_unk02;	    ///< Usually 0.06
	float_t				speed_unk03;		///< Usually 0.02
											/**<
											 * Two default cases:
											 * 1. mostly on "normal" trains
											 *    unk01 0.04
											 *    unk02 0.06
											 *    unk03 0.02
											 * 2. mostly on suspended an functionally suspended (bob) trains
											 *    unk01 0.06
											 *    unk02 0.04
											 *    unk03 0.01
											 * 3. cable lifts and thrill lift, elephant
											 *    unk01-unk03 0.0
											 * 
											 * Special cases:
											 * - CrocLog, LogFlume (1, but unk01 0.25)
											 * - GoldPanningRapids, RapidRiver, WitchesCauldron (1, but unk01 0.01)
											 * - LauncherFFSeats (0.05/0.0/13)
											 **/
	float_t				camera_lookahead;   ///< Distance on track the front camera looks at
											/**<
											 * Usually 8
											 * Kart, ... 4.00 
											 * Heartline, Alien, SpinningWM, VirginiaReel, MultiDim, SkiLift, ... 0.00
											 * Elephant -1
											 **/
	float_t				camera_g_displacement_mult;			///< Always 0.5
	float_t				camera_g_displacement_max;			///< Always 0.4
	float_t				camera_g_displacement_smoothing;	///< Always 2
	float_t				camera_shake_min_speed;				///< Always 1
	float_t				camera_shake_factor;				///< Somewhat random camera jittering, Usually 0.00001, tower rides + elevator 0.0
	float_t				camera_shake_h_max;					///< Always 0.1
	float_t				camera_shake_v_max;					///< Always 0.1
	float_t				camera_chain_shake_factor;			///< Usually 0.001, tower rides, elevator, tram + monorail 0.0, Alpine, bobs + haunted mansion 0.00025
	float_t				camera_chain_shake_h_max;			///< Always 0.0
	float_t				camera_chain_shake_v_max;			///< Usually 0.004, Tram + monorail 0.0
	float_t				camera_smoothing;   ///< Determines how sluggish the camera moves
											/**<
											 * Affects speed of interpolation between camera orientations
											 * 
											 * If set too high, this will cause artefacts as the camera
											 * cannot follow its "orientation waypoints" fast enough
											 * 
											 * Always 0.1
											 **/
	float_t				camera_la_tilt_factor; ///< Determines how much the front camera "goes into curves"
											/**<
											 * that means rotation along the horizontal view axis of the camera
											 * (normally along the track)
											 * 
											 * Usually 0.5
											 * Alpine, bobs, Reverser, WildMine, tower rides, elevator, haunted mansion, orca whale, submarine 0.0
											 **/
	float_t				water_unk01;
											/**<
											 * Usually 1.0
											 * AirBoat 4.0
											 * DolphinTourBoat, Hovercraft, PaddleSteamer, TourBoat, TurtleHovercraft 0.5
											 * JetSki 2.0
											 **/
	float_t				water_unk02;
											/**<
											 * Usually 2.0
											 * AirBoat 4.0
											 * AquaCycle, Canoe, DolphinTourBoat, Hovercraft, RowingBoat, ShipPedallo, 
											 *   SwanRidePedallo, TourBoat, TurtleHovercraft, Windsurfer 1.0
											 * JetSki 3.0
											 * PaddleSteamer 0.4
											 **/
	float_t				water_unk03;
											/**<
											 * Usually 2.0
											 * AirBoat 6.0
											 * AquaCycle, Canoe, RowingBoat, ShipPedallo, SwanRidePedallo, Windsurfer 1.0
											 **/
	float_t				water_unk04;
											/**<
											 * Usually 0.5
											 * AquaCycle, Canoe, RowingBoat, ShipPedallo, SwanRidePedallo, Windsurfer 0.1
											 **/
	float_t				water_unk05;
											/**<
											 * Usually 10.0
											 * AirBoat 12.0
											 * DolphinTourBoat, Hovercraft, PaddleSteamer, TourBoat, TurtleHovercraft 2.0
											 * JetSki 30.0
											 **/
	float_t				water_unk06;
											/**<
											 * Usually 0.5
											 * AirBoat, DolphinTourBoat, Hovercraft, JetSki, PaddleSteamer, TourBoat, 
											 *   TurtleHovercraft 5.0
											 * AquaCycle, Canoe, RowingBoat, ShipPedallo, SwanRidePedallo, Windsurfer 3.0
											 **/
	float_t				water_free_roam_curve_angle;
											/**<
											 * Usually 0.0
											 * AquaCycle, Canoe, RowingBoat, ShipPedallo, SwanRidePedallo, Windsurfer 0.4
											 * JetSki 0.9
											 **/
	float_t				water_unk07;
											/**<
											 * Usually 0.0
											 * AquaCycle, Canoe, RowingBoat, ShipPedallo, SwanRidePedallo, Windsurfer 0.2
											 * JetSki 0.3
											 **/
	uint32_t			overtake_flag;
	float_t				unk37;				///< Always 5.0
	float_t				unk38;				///< Always 15.0
	float_t				unk39;				///< Always 5.0
	float_t				unk40;				///< Always 7.0
	float_t				unk41;				///< Always 9.0
	float_t				unk42;				///< Always 11.0
	float_t				unk43;				///< Always 0.8
	float_t				unk44;				///< Always 0.32
    Spline*             liftcar_left_ref;
    Spline*             liftcar_right_ref;
    char*	     	    station;      		/// Supercedes station named in TRR
};

struct RideTrain_Sext {
    uint32_t    	    version;			///< 2 soaked, 3 wild
    char*	     	    name;            ///< Probably equivalent to name in V structure
    uint32_t    	    icon;               ///< Show no(0), soaked(1) or wild(2) icon
    uint32_t    	    unk51;              ///< Always 0, never symref
};

struct RideTrain_S {
    RideTrain_V         v;
    RideTrain_Sext      s;
};

struct RideTrain_Wext {
    RideCar_V*    	    unknown_ric_ref;    // On TigerCoaster references front car, unknown purpose
    float_t    	    	airboat_unk01;      // Usually 0, Airboat 0.02
    float_t    	    	airboat_unk02;      // Usually 0, Airboat 0.15
    float_t    	        airboat_unk03;      // Usually -1, Airboat 4
    float_t    	        airboat_unk04;      // Usually -1, Airboat 3
    float_t    	        airboat_unk05;      // Usually -1, Airboat 0.75
    float_t    	        airboat_unk06;      // Usually 1, Airboat 4, PaddleSteamer 11
    uint32_t    	    unk59;              // Always 0, never symref
    uint32_t    	    unk60;              // Always 0, never symref
    float_t           	ffsiez_unk;         // Seen 0.0, 90.0 on FrequentFaller, -90.0 on Siezmic
};

struct RideTrain_W {
    RideTrain_V         v;
    RideTrain_Sext      s;
    RideTrain_Wext      w;
};
	
};

#endif
