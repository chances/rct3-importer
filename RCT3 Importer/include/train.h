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

namespace r3 {

struct RideTrain_V {
    char*           	name;              	///< txt for train name, not a symref. 0xFFFFFFFF for RideTrainB/C
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
    uint32_t    	    minimum_cars;       ///< the minimum number of cars for this train
    uint32_t    	    maximum_cars;		///< the maximum number of cars for this train
    uint32_t    	    initial_cars;		///< the initial number of cars for this train when the ride is created
    //float_t	     	    unkfloats1[33];		/// 35 unknown floats, unk12 - unk44
	float_t				speed_unk01;		///< Usually 0.04
											/**<
											 * Bat 0.06, Dingy 0.06, CrocLog 0.25, RapidRiver 0.01
											 * TowerRides usually 0.06 (LauncherFF 0.05)
											 * Markus: something about speedloss/friction after start
											 **/
	float_t				unk13;				///< Usually 0.06
											/**<
											 * Bat 0.04, Dingy 0.04
											 * TowerRides usually 0.04 (LauncherFF 0.00)
											 **/
	float_t				speed_unk02;		///< Usually 0.02
											/**<
											 * Bat 0.00, Dingy 0.00
											 * TowerRides usually 0.005 (LauncherFF 13.0)
											 * Markus: something about speedloss/friction after start
											 **/
	float_t				camera_lookahead;   ///< Usually 8
											/**<
											 * Kart 4.00 
											 * Heartline, Alien, SpinningWM, VirginiaReel, MultiDim, SkiLift 0.00
											 **/
	float_t				camera_g_displacement_mult;
	float_t				camera_g_displacement_max;
	float_t				camera_g_displacement_smoothing;
	float_t				camera_shake_min_speed;
	float_t				camera_shake_factor;		///< Somewhat random camera jittering, Usually 0.00001
	float_t				camera_shake_h_max;
	float_t				camera_shake_v_max;
	float_t				camera_chain_shake_factor;
	float_t				camera_chain_shake_h_max;
	float_t				camera_chain_shake_v_max;
	float_t				camera_smoothing;   ///< Determines how sluggish the camera moves
											/**<
											 * Affects speed of interpolation between camera orientations
											 * 
											 * If set too high, this will cause artefacts as the camera
											 * cannot follow its "orientation waypoints" fast enough
											 **/
	float_t				camera_la_tilt_factor; ///< Determines how much the front camera "goes into curves"
											/**<
											 * that means rotation along the horizontal view axis of the camera
											 * (normally along the track)
											 **/
	float_t				unk28;
	float_t				camera_unk02;
	float_t				water_unk01;
	float_t				water_unk02;
	float_t				jetski_unk01;
	float_t				water_unk03;
	float_t				water_free_roam_curve_angle;
	float_t				water_unk04;
	uint32_t			overtake_flag;
	float_t				unk37;
	float_t				unk38;
	float_t				unk39;
	float_t				unk40;
	float_t				unk41;
	float_t				unk42;
	float_t				unk43;
	float_t				unk44;
    Spline*             liftcar_left_ref;
    Spline*             liftcar_right_ref;
    char*	     	    station;      		/// Supercedes station named in TRR
};

struct RideTrain_Sext {
    uint32_t    	    version;			///< 2 soaked, 3 wild
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
