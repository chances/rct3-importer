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

struct RideCar
{
    char*	     	    name;			// Possibly a language lookup
    char*		        username;
    unsigned long		flags;			// Possibly boolean flags. Second byte seems to signify stricture version
    SceneryItemVisual*  svd;
    float		        weight1;	    // Probably weight. Can be zero (rot mouse)
    SceneryItemVisual*  seatsvd;        // If seats move in some way (rotating mouse, multidimensional)
    unsigned long       unk7;           // Seen 2 (cuttlefish multidim), 4 (rot mouse, EuroSpinner). 0 if not rotating
                                        // Probably 2 = horizontal axis, 4 = vertical axis
    float               weight2;        // Probably weight of seats. -1.0 for cars withot rotation
    float               unk9;           // Seen 0.3, 4.0
    unsigned long       unk10;          // Seen 0
    float               unk11;          // Seen -1
    float               unk12;          // Seen 0.1, 0.3
    unsigned long       unk13;          // Seen 0
    unsigned long       unk14;          // Seen 0
    float               unk15;          // Seen -1.0
    unsigned long       unk16;          // Seen 0
    unsigned long       unk17;          // Seen 0
    unsigned long       unk18;          // Seen 0
    unsigned long       unk19;          // Seen 0
    long                unk20;          // Seen -1
    long                unk21;          // Seen -1
    long                unk22;          // Seen -1
    long                unk23;          // Seen -1
    unsigned long       unk24;          // Seen 0
    unsigned long       unk25;          // Seen 1
    unsigned long       unk26;          // Seen 2
    long                unk27;          // Seen -1
    long                unk28;          // Seen -1
    long                unk29;          // Seen -1
    long                unk30;          // Seen -1
    long                unk31;          // Seen -1
    long                unk32;          // Seen -1
    long                unk33;          // Seen -1
    long                unk34;          // Seen -1
    long                unk35;          // Seen -1
    long                unk36;          // Seen -1
    long                unk37;          // Seen -1
    long                unk38;          // Seen -1
    long                unk39;          // Seen -1
    unsigned long       unk40;
    unsigned long       unk41;
    unsigned long       unk42;
    unsigned long       unk43;
    unsigned long       unk44;
    unsigned long       unk45;
    unsigned long       unk46;
    unsigned long       unk47;
    unsigned long       unk48;
    unsigned long       unk49;
    unsigned long       unk50;
    unsigned long       unk51;
    unsigned long       unk52;          // Seen 4
    unsigned long       unk53;          // Seen 4
    unsigned long       unk54;
    float               unk55;          // Seen -1.0
    long                unk56;          // Seen -1
    float               unk57;          // Seen -1.0
    float               unk58;          // Seen -1.0
    float               unk59;          // Seen -1.0
    float               unk60;          // Seen -1.0
};

struct RideCar2
{
    char*	     	    name;			// Possibly a language lookup
    char*		        username;
    unsigned long		flags;			// Possibly boolean flags. Second byte seems to signify stricture version
    SceneryItemVisual*  svd;
    float		        weight1;	    // Probably weight. Can be zero (rot mouse)
    SceneryItemVisual*  seatsvd;        // If seats move in some way (rotating mouse, multidimensional)
    unsigned long       unk7;           // Seen 2 (cuttlefish multidim), 4 (rot mouse, EuroSpinner). 0 if not rotating
                                        // Probably 2 = horizontal axis, 4 = vertical axis
    float               weight2;        // Probably weight of seats. -1.0 for cars withot rotation
    float               unk9;           // Seen 0.3, 4.0
    unsigned long       unk10;          // Seen 0
    float               unk11;          // Seen -1
    float               unk12;          // Seen 0.1, 0.3
    unsigned long       unk13;          // Seen 0
    unsigned long       unk14;          // Seen 0
    float               unk15;          // Seen -1.0
    unsigned long       unk16;          // Seen 0
    unsigned long       unk17;          // Seen 0
    unsigned long       unk18;          // Seen 0
    unsigned long       unk19;          // Seen 0
    long                unk20;          // Seen -1
    long                unk21;          // Seen -1
    long                unk22;          // Seen -1
    long                unk23;          // Seen -1
    unsigned long       unk24;          // Seen 0
    unsigned long       unk25;          // Seen 1
    unsigned long       unk26;          // Seen 2
    long                unk27;          // Seen -1
    long                unk28;          // Seen -1
    long                unk29;          // Seen -1
    long                unk30;          // Seen -1
    long                unk31;          // Seen -1
    long                unk32;          // Seen -1
    long                unk33;          // Seen -1
    long                unk34;          // Seen -1
    long                unk35;          // Seen -1
    long                unk36;          // Seen -1
    long                unk37;          // Seen -1
    long                unk38;          // Seen -1
    long                unk39;          // Seen -1
    unsigned long       unk40;
    unsigned long       unk41;
    unsigned long       unk42;
    unsigned long       unk43;
    unsigned long       unk44;
    unsigned long       unk45;
    unsigned long       unk46;
    unsigned long       unk47;
    unsigned long       unk48;
    unsigned long       unk49;
    SceneryItemVisual*  axelsvd;
    unsigned long       unk51;
    unsigned long       unk52;          // Seen 1, 4
    unsigned long       unk53;          // Seen 4, 1 (for rear car)
    unsigned long       unk54;
    float               unk55;          // Seen 0
    long                unk56;          // Seen 0
    float               unk57;          // Seen -1.0
    float               unk58;          // Seen 4.0
    float               unk59;          // Seen 0
    float               unk60;          // Seen -1.0
    float               unk61;          // Seen 0.1
    unsigned long       unk62;
    unsigned long       unk63;
    float               unk64;          // Seen -1.0
    unsigned long       unk65;
    unsigned long       unk66;          // Seen 1
    unsigned long       unk67;
    unsigned long       unk68;
    unsigned long       unk69;          // Seen 1
    unsigned long       unk70;
    unsigned long       unk71;
    unsigned long       unk72;          // Seen 0
    float               unk73;          // Seen -9999.9
};

struct RideCar3
{
    char*	     	    name;			// Possibly a language lookup
    char*		        username;
    unsigned long		flags;			// Possibly boolean flags. Second byte seems to signify stricture version
    SceneryItemVisual*  svd;
    float		        weight1;	    // Probably weight. Can be zero (rot mouse)
    SceneryItemVisual*  seatsvd;        // If seats move in some way (rotating mouse, multidimensional)
    unsigned long       unk7;           // Seen 2 (cuttlefish multidim), 4 (rot mouse, EuroSpinner). 0 if not rotating
                                        // Probably 2 = horizontal axis, 4 = vertical axis
    float               weight2;        // Probably weight of seats. -1.0 for cars withot rotation
    float               unk9;           // Seen 0.3, 4.0
    unsigned long       unk10;          // Seen 0
    float               unk11;          // Seen -1
    float               unk12;          // Seen 0.1, 0.3
    unsigned long       unk13;          // Seen 0
    unsigned long       unk14;          // Seen 0
    float               unk15;          // Seen -1.0
    unsigned long       unk16;          // Seen 0
    unsigned long       unk17;          // Seen 0
    unsigned long       unk18;          // Seen 0
    unsigned long       unk19;          // Seen 0
    long                unk20;          // Seen -1
    long                unk21;          // Seen -1
    long                unk22;          // Seen -1
    long                unk23;          // Seen -1
    unsigned long       unk24;          // Seen 0
    unsigned long       unk25;          // Seen 1
    unsigned long       unk26;          // Seen 2
    long                unk27;          // Seen -1
    long                unk28;          // Seen -1
    long                unk29;          // Seen -1
    long                unk30;          // Seen -1
    long                unk31;          // Seen -1
    long                unk32;          // Seen -1
    long                unk33;          // Seen -1
    long                unk34;          // Seen -1
    long                unk35;          // Seen -1
    long                unk36;          // Seen -1
    long                unk37;          // Seen -1
    long                unk38;          // Seen -1
    long                unk39;          // Seen -1
    unsigned long       unk40;
    unsigned long       unk41;
    unsigned long       unk42;
    unsigned long       unk43;
    unsigned long       unk44;
    unsigned long       unk45;
    unsigned long       unk46;
    unsigned long       unk47;
    unsigned long       unk48;
    unsigned long       unk49;
    unsigned long       unk50;
    unsigned long       unk51;
    unsigned long       unk52;          // Seen 4
    unsigned long       unk53;          // Seen 4
    unsigned long       unk54;
    float               unk55;          // Seen -1.0
    long                unk56;          // Seen -1
    float               unk57;          // Seen -1.0
    float               unk58;          // Seen -1.0
    float               unk59;          // Seen -1.0
    float               unk60;          // Seen -1.0
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
