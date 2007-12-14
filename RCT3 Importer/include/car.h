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


#ifndef __ovlstructs_h__
#define __ovlstructs_h__

struct RideCar
{
    char*	     	    name;			// Possibly a language lookup
    char*		        username;
    unsigned long		unk1;			// Possibly boolean flags.
    SceneryItemVisual*  svd;
    float		        unk2;			// Probably weight
    float		        unk3[12];
    float		        unk4;			// Always 0
};

struct RideTrain3 // ?
{
    unsigned long    	unk1;              	// -1 on the structures that I've looked at.
    char*            	name;              	//  probably user visible text, ie. spaces and stuff rather than an internal name
    RideCar*         	frontcar;
    RideCar*         	secondcar;
    RideCar*         	midcar;
    RideCar*         	penultimatecar;
    RideCar*         	rearcar;
    RideCar*         	link;	        	// Pointer to the object that links cars together.
    unsigned long    	minimumcars;       	// the minimum number of cars for this train
    unsigned long    	maximumcars;		// the maximum number of cars for this train
    unsigned long    	initialcars;		// the initial number of cars for this train when the ride is created
    float	     	    unkfloats1[35];		// 35 unknown floats
    char*	     	    configuration;
    unsigned long    	unk10;			    // Seen 3
    char*	     	    carname;
    unsigned long    	unk11;              // Seen 2, 0 on HyperTwister|Looping
    unsigned long    	unk12;              // Seen 0
    unsigned long    	unk13;              // Seen 0
    unsigned long    	unk14;              // Seen 0
    unsigned long    	unk15;              // Seen 0
    float    	        unk16;              // Seen -1.0
    float    	        unk17;              // Seen -1.0
    float    	        unk18;              // Seen -1.0
    float    	        unk19;              // Seen 1.0
    unsigned long    	unk20;              // Seen 0
    unsigned long    	unk21;              // Seen 0
    float           	unk22;              // Seen 0.0, 90.0 on FrequentFaller
};

struct RideTrain2
{
    unsigned long    	unk1;              	// -1 on the structures that I've looked at.
    char*            	name;              	//  probably user visible text, ie. spaces and stuff rather than an internal name
    RideCar*         	frontcar;
    RideCar*         	secondcar;
    RideCar*         	midcar;
    RideCar*         	penultimatecar;
    RideCar*         	rearcar;
    RideCar*         	link;	        	// Pointer to the object that links cars together.
    unsigned long    	minimumcars;       	// the minimum number of cars for this train
    unsigned long    	maximumcars;		// the maximum number of cars for this train
    unsigned long    	initialcars;		// the initial number of cars for this train when the ride is created
    float	     	    unkfloats1[35];		// 35 unknown floats
    char*	     	    configuration;
    unsigned long    	unk10;			    // Seen 2
    char*	     	    carname;
    unsigned long    	unk11;              // Seen 0, 1
    unsigned long    	unk12;              // Seen 0
};

struct RideTrain1
{
    char*           	name;              	// txt for train name. 0xFFFFFFFF for RideTrain2
    char*            	description;        // txt for train description.
    RideCar*         	frontcar;
    RideCar*         	secondcar;
    RideCar*         	midcar;
    RideCar*         	penultimatecar;
    RideCar*         	rearcar;
    RideCar*         	link;	        	// Pointer to the object that links cars together.
    unsigned long    	minimumcars;       	// the minimum number of cars for this train
    unsigned long    	maximumcars;		// the maximum number of cars for this train
    unsigned long    	initialcars;		// the initial number of cars for this train when the ride is created
    float	     	unkfloats1[35];		// 35 unknown floats
    char*	     	configuration;      // Platform? Seen "NoPlatform"
};

#endif
