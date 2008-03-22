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

namespace r3 {

/// Defines a spline node
struct SplineNode {
    VECTOR       pos;            ///< Location of node
    VECTOR       cp1;            ///< Control point 1, relative to pos. Points to the previous node.
    VECTOR       cp2;            ///< Control point 2, relative to pos. Points to the next node.
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

struct SplineData {
    uint8_t data[14];          ///< The data encodes how things travel along a spline segment.
};

/// Defines a spline
/**
 * The exact spline type is so far unknown
 */
struct Spline {
    uint32_t         nodecount;       ///< A count
    SplineNode*      nodes;           ///< Pointer to a list of SplineNode structures.
    uint32_t         cyclic;          ///< 0 for open splines, 1 for cyclic splines
    float_t          totallength;
    float_t          inv_totallength; ///< Inverse of total length
    float_t*         lengths;         ///< The distance between each spline node point.
                                      /**
                                       * number of lengths is nodecount - 1 for open and nodecount for cyclic splines
                                       */
    SplineData*      datas;               ///< Pointer to SplineData, 14 bytes/length
    float_t          max_y;
};

}

#endif
