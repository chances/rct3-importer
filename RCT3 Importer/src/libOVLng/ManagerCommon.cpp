///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Shared tranfer classes for managers
// Copyright (C) 2007 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
// Based on libOVL by Jonathan Wilson
//
///////////////////////////////////////////////////////////////////////////////

#include "OVLDebug.h"

#include "ManagerCommon.h"

#include <math.h>

#include "OVLException.h"

const char* ovlTXSManager::TAG = "txs";

const cTriangleSortAlgorithm::Algorithm cTriangleSortAlgorithm::DEFAULT = cTriangleSortAlgorithm::MINMAX;

const char* algonames[cTriangleSortAlgorithm::EnumSize+1] = {
    "min",
    "max",
    "mean",
    "minmax",
    "maxmin",
    "angle45",
    "minXZbyY",
    "minXZbyOther",
    "minYbyX",
    "minYbyZ",
    "mininv",
    "minmaxinv",
    "none",
    "UNKNOWN!"
};

inline float TriMin(const VECTOR& a, const VECTOR& b, const VECTOR& c, cTriangleSortAlgorithm::Axis dir) {
    switch (dir) {
        case cTriangleSortAlgorithm::BY_X:
            return TriMin(a.x, b.x, c.x);
        case cTriangleSortAlgorithm::BY_Y:
            return TriMin(a.y, b.y, c.y);
        case cTriangleSortAlgorithm::BY_Z:
            return TriMin(a.z, b.z, c.z);
    }
}

inline float TriMax(const VECTOR& a, const VECTOR& b, const VECTOR& c, cTriangleSortAlgorithm::Axis dir) {
    switch (dir) {
        case cTriangleSortAlgorithm::BY_X:
            return TriMax(a.x, b.x, c.x);
        case cTriangleSortAlgorithm::BY_Y:
            return TriMax(a.y, b.y, c.y);
        case cTriangleSortAlgorithm::BY_Z:
            return TriMax(a.z, b.z, c.z);
    }
}

inline float TriSum(const VECTOR& a, const VECTOR& b, const VECTOR& c, cTriangleSortAlgorithm::Axis dir) {
    switch (dir) {
        case cTriangleSortAlgorithm::BY_X:
            return a.x + b.x + c.x;
        case cTriangleSortAlgorithm::BY_Y:
            return a.y + b.y + c.y;
        case cTriangleSortAlgorithm::BY_Z:
            return a.z + b.z + c.z;
    }
}

inline float MinDist(const VECTOR& a, const VECTOR& b, const VECTOR& c, const VECTOR& v) {
    return TriMin(fabsf(v.x - a.x) + fabsf(v.y - a.y) + fabsf (v.z - a.z),
                  fabsf(v.x - b.x) + fabsf(v.y - b.y) + fabsf (v.z - b.z),
                  fabsf(v.x - c.x) + fabsf(v.y - c.y) + fabsf (v.z - c.z));
}

const char* cTriangleSortAlgorithm::GetDefaultName() {
    return algonames[GetDefault()];
}

const char* cTriangleSortAlgorithm::GetAlgorithmName(Algorithm algo) {
    return algonames[algo];
}

cTriangleSortAlgorithm::Algorithm cTriangleSortAlgorithm::GetAlgo(const char* algoname) {
    for (Algorithm i = MIN; i < EnumSize; i = Algorithm(i+1)) {
        if (!stricmp(algoname, algonames[i])) {
            return i;
        }
    }
    return EnumSize;
}

bool cTriangleSortAlgorithm::operator() (const VECTOR& a1, const VECTOR& a2, const VECTOR& a3, const VECTOR& b1, const VECTOR& b2, const VECTOR& b3) const {
    switch(algo) {
        case MIN:
            return TriMin(a1, a2, a3, dir) < TriMin(b1, b2, b3, dir);
        case MAX:
            return TriMax(a1, a2, a3, dir) < TriMax(b1, b2, b3, dir);
        case MEAN:
            return TriSum(a1, a2, a3, dir) < TriSum(b1, b2, b3, dir);
        case MINMAX: {
                float a = TriMin(a1, a2, a3, dir);
                float b = TriMin(b1, b2, b3, dir);
                if (a == b) {
                    return TriMax(a1, a2, a3, dir) < TriMax(b1, b2, b3, dir);
                } else {
                    return a < b;
                }
            }
        case MAXMIN: {
                float a = TriMax(a1, a2, a3, dir);
                float b = TriMax(b1, b2, b3, dir);
                if (a == b) {
                    return TriMin(a1, a2, a3, dir) < TriMin(b1, b2, b3, dir);
                } else {
                    return a < b;
                }
            }
        case ANGLE45: {
                switch (dir) {
                    case BY_X: {
                        VECTOR v;
                        v.x = -10000.0;
                        v.y = 10000.0;
                        v.z = 0;
                        return  MinDist(a1, a2, a3, v) < MinDist(b1, b2, b3, v);
                    }
                    case BY_Y: {
                        float a = TriMax(a1, a2, a3, dir);
                        float b = TriMax(b1, b2, b3, dir);
                        if (a == b) {
                            return TriMin(a1, a2, a3, dir) < TriMin(b1, b2, b3, dir);
                        } else {
                            return a < b;
                        }
                    }
                    case BY_Z: {
                        VECTOR v;
                        v.x = 0;
                        v.y = 10000.0;
                        v.z = -10000.0;
                        return  MinDist(a1, a2, a3, v) < MinDist(b1, b2, b3, v);
                    }
                }
            }
        case MINXZBYY: {
                switch (dir) {
                    case BY_X:
                    case BY_Z: {
                        float a = TriMin(a1, a2, a3, dir);
                        float b = TriMin(b1, b2, b3, dir);
                        if (a == b) {
                            return TriMin(a1, a2, a3, BY_Y) < TriMin(b1, b2, b3, BY_Y);
                        } else {
                            return a < b;
                        }
                    }
                    default: {
                        return TriMin(a1, a2, a3, dir) < TriMin(b1, b2, b3, dir);
                    }
                }
            }
        case MINXZBYOTHER: {
                switch (dir) {
                    case BY_X:
                    case BY_Z: {
                        float a = TriMin(a1, a2, a3, dir);
                        float b = TriMin(b1, b2, b3, dir);
                        if (a == b) {
                            return TriMin(a1, a2, a3, (dir==BY_X)?BY_Z:BY_X) < TriMin(b1, b2, b3, (dir==BY_X)?BY_Z:BY_X);
                        } else {
                            return a < b;
                        }
                    }
                    default: {
                        return TriMin(a1, a2, a3, dir) < TriMin(b1, b2, b3, dir);
                    }
                }
            }
        case MINYBYX: {
                switch (dir) {
                    case BY_Y: {
                        float a = TriMin(a1, a2, a3, dir);
                        float b = TriMin(b1, b2, b3, dir);
                        if (a == b) {
                            return TriMin(a1, a2, a3, BY_X) < TriMin(b1, b2, b3, BY_X);
                        } else {
                            return a < b;
                        }
                    }
                    default: {
                        return TriMin(a1, a2, a3, dir) < TriMin(b1, b2, b3, dir);
                    }
                }
            }
        case MINYBYZ: {
                switch (dir) {
                    case BY_Y: {
                        float a = TriMin(a1, a2, a3, dir);
                        float b = TriMin(b1, b2, b3, dir);
                        if (a == b) {
                            return TriMin(a1, a2, a3, BY_Z) < TriMin(b1, b2, b3, BY_Z);
                        } else {
                            return a < b;
                        }
                    }
                    default: {
                        return TriMin(a1, a2, a3, dir) < TriMin(b1, b2, b3, dir);
                    }
                }
            }
        case MININV:
            return TriMin(a1, a2, a3, dir) > TriMin(b1, b2, b3, dir);
        case MINMAXINV: {
                float a = TriMin(a1, a2, a3, dir);
                float b = TriMin(b1, b2, b3, dir);
                if (a == b) {
                    return TriMax(a1, a2, a3, dir) > TriMax(b1, b2, b3, dir);
                } else {
                    return a > b;
                }
            }
        default:
            DUMP_LOG("Sort algorithm called with illegal algo");
            return false;
    }
}



void cAttraction::Fill(AttractionA* att) {
    if ((type & ATTRACTION_TYPE_Wild) == ATTRACTION_TYPE_Wild) {
        AttractionB* att2 = reinterpret_cast<AttractionB*>(att);
        att2->type = type;
        att2->Name = NULL;
        att2->Description = NULL;
        att2->GSI = NULL;
        att2->unk2 = unk2;
        att2->unk3 = unk3;
        att2->unk4 = unk4;
        att2->unk5 = unk5;
        att2->unk6 = unk6;
        att2->spline = NULL;
        att2->pathcount = splines.size();
        att2->paths = NULL;
        att2->unk9 = unk9;
        att2->unk10 = unk10;
        att2->unk11 = unk11;
        att2->unk12 = unk12;
        att2->unk13 = unk13;
    } else if ((type & ATTRACTION_TYPE_Soaked) == ATTRACTION_TYPE_Soaked) {
        att->type = type;
        att->Name = NULL;
        att->Description = NULL;
        att->GSI = NULL;
        att->unk2 = unk2;
        att->unk3 = unk3;
        att->unk4 = unk4;
        att->unk5 = unk5;
        att->unk6 = unk6;
        att->spline = NULL;
        att->pathcount = splines.size();
        att->paths = NULL;
        att->unk9 = unk9;
        att->unk10 = unk10;
        att->unk11 = unk11;
        att->unk12 = unk12;
    } else {
        throw EOvl("cAttractionType::Fill, cannot determine structure type.");
    }
}

void cAttraction::Fill(StallA* sta) {
    sta->type = type & 0xFF; // low byte only
    sta->unk2 = unk2;
    sta->unk3 = unk3;
    sta->unk4 = unk4;
    sta->unk5 = unk5;
    sta->unk6 = unk6;
    sta->unk7 = 0;
    sta->unk8 = 0;
    sta->unk9 = unk9;
    sta->unk10 = unk10;
}

void cAttraction::Fill(SpecialAttractionA* sp) {
    Fill(reinterpret_cast<StallA*>(sp));
}
