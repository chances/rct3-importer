
#ifndef __SPLINE_H__
#define __SPLINE_H__

struct SplineNode
{
    float        x;            // Location of node
    float        y;
    float        z;
    float        cp1x;            // Control point 1
    float        cp1y;
    float        cp1z;
    float        cp2x;            // Control point 2
    float        cp2y;
    float        cp2z;
};
struct Spline
{
    unsigned long    count;           // A count
    SplineNode*      nodes;           // Pointer to a list of SplineNode structures.
    unsigned long    cyclic;          // 0 for open spliens, 1 for cyclic splines
    float            totallength;
    float            unk3;
    float*           lengths;         // The distance between each Spline2 point.
                                      // number of lengths is count - 1 for open and count for cyclic splines
    unsigned char*   unk5;            // Pointer to some data. 14 bytes/length
    float            unk6;
};


#endif
