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


#ifndef __SPLINE_H__
#define __SPLINE_H__

#include "vertex.h"

/// Defines a spline node
struct SplineNode {
    VECTOR       pos;               ///< Location of node
    VECTOR       cp1;            ///< Control point 1
    VECTOR       cp2;            ///< Control point 2
};

struct SplineData {
    unsigned char data[14];          ///< The data somehow encodes how things travel along a spline segmont.
};

/*
struct SplineNode {
    float        x;               ///< Location of node
    float        y;
    float        z;
    float        cp1x;            ///< Control point 1
    float        cp1y;
    float        cp1z;
    float        cp2x;            ///< Control point 2
    float        cp2y;
    float        cp2z;
};
*/

/// Defines a spline
/**
 * The exact spline type is so far unknown
 */
struct Spline {
    unsigned long    nodecount;       ///< A count
    SplineNode*      nodes;           ///< Pointer to a list of SplineNode structures.
    unsigned long    cyclic;          ///< 0 for open spliens, 1 for cyclic splines
    float            totallength;
    float            inv_totallength; ///< Inverse of total length
    float*           lengths;         ///< The distance between each spline node point.
                                      /**
                                       * number of lengths is nodecount - 1 for open and nodecount for cyclic splines
                                       */
    SplineData*      datas;               ///< Pointer to SplineData, 14 bytes/length
    float            max_y;
};


#endif
