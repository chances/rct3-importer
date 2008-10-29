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

#include <assert.h>
#include <math.h>

#include "pretty.h"

#include "OVLException.h"
#include "ManagerGSI.h"
#include "ManagerSPL.h"
#include "ManagerTXT.h"

#ifdef __BORLANDC__
#define fabsf fabs
#endif

using namespace r3;
using namespace std;

const char* ovlTXSManager::TAG = "txs";

////////////////////////////////////////////////////////////////////////
//
//  Triangle Sort Algorithms
//
////////////////////////////////////////////////////////////////////////

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
        default:
            assert(0);
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
        default:
            assert(0);
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
        default:
            assert(0);
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
#ifndef __BORLANDC__
		if (!strcasecmp(algoname, algonames[i])) {
			return i;
		}
#else
		if (!stricmp(algoname, algonames[i])) {
			return i;
		}
#endif
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


////////////////////////////////////////////////////////////////////////
//
//  cAttraction
//
////////////////////////////////////////////////////////////////////////

void cAttraction::Fill(Attraction_V* attv) const {
    Attraction_W* att = reinterpret_cast<Attraction_W*>(attv);
    att->v.type = type;
    att->v.name_ref = NULL;
    att->v.description_ref = NULL;
    att->v.icon_ref = NULL;
    att->v.unk2 = unk2;
    att->v.unk3 = unk3;
    att->v.unk4 = unk4;
    att->v.unk5 = unk5;
    att->v.base_upkeep = unk6;
    att->v.spline_ref = NULL;
    att->v.path_count = splines.size();
    if (!splines.size())
        att->v.paths_ref = NULL;
    att->v.flags = unk9;
    att->v.max_height = unk10;
    if (type & r3::Constants::Addon::Soaked_Hi) {
        att->s.addonascn = addonascn;
        att->s.unk12 = unk12;
    }
    if ((type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
        att->w.ui_deactivation = ui_deactivation;
    }
/*
    if ((type & r3::Constants::Addon::Wild_Hi) == r3::Constants::Addon::Wild_Hi) {
        Attraction_W* att2 = reinterpret_cast<Attraction_W*>(att);
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
        att2->max_height = unk10;
        att2->addonascn = addonascn;
        att2->unk12 = unk12;
        att2->unk13 = unk13;
    } else if ((type & r3::Constants::Addon::Soaked_Hi) == r3::Constants::Addon::Soaked_Hi) {
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
        att->max_height = unk10;
        att->addonascn = addonascn;
        att->unk12 = unk12;
    } else {
        throw EOvl("cAttractionType::Fill, cannot determine structure type");
    }
*/
}

void cAttraction::Fill(r3old::StallA* sta) const {
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

void cAttraction::Fill(r3old::SpecialAttractionA* sp) const {
    Fill(reinterpret_cast<r3old::StallA*>(sp));
}

/** @brief DoAdd
  *
  * @todo: document this function
  */
void cAttraction::DoAdd(ovlStringTable* st, ovlLodSymRefManager* lsr) const {
    lsr->AddSymRef(OVLT_UNIQUE);
    st->AddSymbolString(name, ovlTXTManager::TAG);
    lsr->AddSymRef(OVLT_UNIQUE);
    st->AddSymbolString(description, ovlTXTManager::TAG);
    // The following is not an error, if not set these are symref'd to :gsi and :spl
    lsr->AddSymRef(OVLT_UNIQUE);
    st->AddSymbolString(icon, ovlGSIManager::TAG);
    lsr->AddSymRef(OVLT_UNIQUE);
    st->AddSymbolString(spline, ovlSPLManager::TAG);

    foreach(const string& spl, splines) {
        lsr->AddSymRef(OVLT_UNIQUE);
        st->AddSymbolString(spl, ovlSPLManager::TAG);
    }
}

/** @brief GetUniqueSize
  *
  * @todo: document this function
  */
unsigned long cAttraction::GetUniqueSize() const {
    int usize = 0;
    if (soaked())
        usize = sizeof(Attraction_S);
    else if (wild())
        usize = sizeof(Attraction_W);
    // Spline pointers
    usize += splines.size() * 4;
    return usize;
}

/** @brief GetCommonSize
  *
  * @todo: document this function
  */
unsigned long cAttraction::GetCommonSize() const {
    return 0;
}


/** @brief MakeSymRefs
  *
  * @todo: document this function
  */
void cAttraction::MakeSymRefs(r3::Attraction_V* ptr, ovlLodSymRefManager* lsr, ovlStringTable* st) const {
    lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(name, ovlTXTManager::TAG), reinterpret_cast<unsigned long*>(&ptr->name_ref));
    lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(description, ovlTXTManager::TAG), reinterpret_cast<unsigned long*>(&ptr->description_ref));
    lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(icon, ovlGSIManager::TAG), reinterpret_cast<unsigned long*>(&ptr->icon_ref));
    lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(spline, ovlSPLManager::TAG), reinterpret_cast<unsigned long*>(&ptr->spline_ref));

    for (int i = 0; i < splines.size(); ++i) {
        lsr->MakeSymRef(OVLT_UNIQUE, st->FindSymbolString(splines[i], ovlSPLManager::TAG),
                             reinterpret_cast<unsigned long*>(&ptr->paths_ref[i]));
    }
}

/** @brief MakeSymRefs
  *
  * @todo: document this function
  */
void cAttraction::MakeSymRefs(r3::Attraction_S* ptr, ovlLodSymRefManager* lsr, ovlStringTable* st) const {
    MakeSymRefs(&ptr->v, lsr, st);
}

/** @brief MakePointers
  *
  * @todo: document this function
  */
void cAttraction::MakePointers(Attraction_S** ptr, unsigned char*& uni_ptr, unsigned char*& com_ptr, ovlRelocationManager* rel) const {
    *ptr = reinterpret_cast<Attraction_S*>(uni_ptr);
    if (soaked()) {
        uni_ptr += sizeof(Attraction_S);
    } else {
        uni_ptr += sizeof(Attraction_W);
    }
    rel->AddRelocation(reinterpret_cast<unsigned long*>(ptr));

    if (splines.size()) {
        (*ptr)->v.paths_ref = reinterpret_cast<Spline**>(uni_ptr);
        uni_ptr += splines.size() * 4;
        rel->AddRelocation(reinterpret_cast<unsigned long*>(&(*ptr)->v.paths_ref));
    }
}

/** @brief MakePointers
  *
  * @todo: document this function
  */
void cAttraction::MakePointers(Attraction_V* ptr, unsigned char*& uni_ptr, unsigned char*& com_ptr, ovlRelocationManager* rel) const {
    if (splines.size()) {
        ptr->paths_ref = reinterpret_cast<Spline**>(uni_ptr);
        uni_ptr += splines.size() * 4;
        rel->AddRelocation(reinterpret_cast<unsigned long*>(&ptr->paths_ref));
    }
}


////////////////////////////////////////////////////////////////////////
//
//  cRideOption
//
////////////////////////////////////////////////////////////////////////


/** @brief GetSize
  *
  * @todo: document this function
  */
unsigned long cRideOption::GetSize() const {
    switch (type) {
        case 0: return 4;
        case 1: return 2*4;
        case 2: return 3*4;
        case 3: return 3*4;
        case 4: return 3*4;
        case 5: return 3*4;
        case 6: return 3*4;
        case 7: return 3*4;
        case 8: return 6*4;
        case 9: return 4*4;
        case 10: return 3*4;
        case 11: return 3*4;
        case 12: return 4*4;
        default:
            throw EOvl("cRideOption::GetSize, unknown option type");
    }
}

/** @brief SetType
  *
  * Sets sensible defaults for a certain option
  */
void cRideOption::SetType(unsigned long t) {
    type = t;
    switch (type) {
        case 0: break;
        case 1: return;
        case 2: return;
        case 3: return;
        case 4: return;
        case 5: return;
        case 6: return;
        case 7: return;
        case 8: {
                param[3] = 1.0;
            }
            break;
        case 9: return;
        case 10: {
                param[0] = 22.0;
            }
            break;
        case 11: return;
        case 12: {
                param[0] = 50.0;
                param[1] = 10.0;
            }
            break;
        default:
            throw EOvl("cRideOption::GetSize, unknown option type");
    }
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cRideOption::Fill(r3::RideOption* r) const {
    r->type = type;
    switch (type) {
        case 0:
            break;
        case 1: {
                r->type_01.option = option;
            }
            break;
        case 2: {
                r->type_02.option = option;
                r->type_02.unk2 = param[0];
            }
            break;
        case 3: {
                r->type_03.option = option;
                r->type_03.unk2 = param[0];
            }
            break;
        case 4: {
                r->type_04.option = option;
                r->type_04.unk2 = param[0];
            }
            break;
        case 5: {
                r->type_05.option = option;
                r->type_05.unk2 = param[0];
            }
            break;
        case 6: {
                r->type_06.option = option;
                r->type_06.unk2 = param[0];
            }
            break;
        case 7: {
                r->type_07.option = option;
                r->type_07.unk2 = param[0];
            }
            break;
        case 8: {
                r->type_08.excitement = param[0];
                r->type_08.intensity = param[1];
                r->type_08.nausea = param[2];
                r->type_08.option = option;
                r->type_08.factor = param[3];
            }
            break;
        case 9: {
                r->type_09.unk1 = param[0];
                r->type_09.unk2 = param[1];
                r->type_09.unk3 = param[2];
            }
            break;
        case 10: {
                r->type_10.unk1 = param[0];
                r->type_10.option = option;
            }
            break;
        case 11: {
                r->type_11.unk1 = param[0];
                r->type_11.option = option;
            }
            break;
        case 12: {
                r->type_12.unk1 = param[0];
                r->type_12.unk2 = param[1];
                r->type_12.unk3 = param[2];
            }
            break;
        default:
            throw EOvl("cRideOption::Fill, unknown option type");
    }
}



////////////////////////////////////////////////////////////////////////
//
//  cRideStationLimit
//
////////////////////////////////////////////////////////////////////////

/** @brief Fill
  *
  * @todo: document this function
  */
void cRideStationLimit::Fill(r3::RideStationLimit* r) const {
    r->x = x_pos;
    r->z = z_pos;
    r->height = height;
    r->flags = flags;
}



////////////////////////////////////////////////////////////////////////
//
//  cRide
//
////////////////////////////////////////////////////////////////////////

/** @brief DoAdd
  *
  * @todo: document this function
  */
void cRide::DoAdd(ovlStringTable* st, ovlLodSymRefManager* lsr) const {
    return;
}

/** @brief MakePointers
  *
  * @todo: document this function
  */
void cRide::MakePointers(Ride_S** ptr, unsigned char*& uni_ptr, unsigned char*& com_ptr, const cAttraction& att, ovlRelocationManager* rel) const {
    *ptr = reinterpret_cast<Ride_S*>(uni_ptr);
    if (att.soaked()) {
        uni_ptr += sizeof(Ride_S);
    } else {
        uni_ptr += sizeof(Ride_W);
    }
    rel->AddRelocation(reinterpret_cast<unsigned long*>(ptr));

    att.MakePointers(&(*ptr)->s.att, uni_ptr, com_ptr, rel);

    (*ptr)->v.options = reinterpret_cast<RideOption**>(com_ptr);
    com_ptr += (options.size()+1) * 4;
    rel->AddRelocation(reinterpret_cast<unsigned long*>(&(*ptr)->v.options));
    for (int i = 0; i < options.size(); ++i) {
        (*ptr)->v.options[i] = reinterpret_cast<RideOption*>(com_ptr);
        com_ptr += options[i].GetSize();
        rel->AddRelocation(reinterpret_cast<unsigned long*>(&(*ptr)->v.options[i]));
    }
    (*ptr)->v.options[options.size()] = NULL;

    if (stationlimits.size()) {
        (*ptr)->s.extras = reinterpret_cast<RideStationLimit*>(com_ptr);
        com_ptr += stationlimits.size() * sizeof(RideStationLimit);
        rel->AddRelocation(reinterpret_cast<unsigned long*>(&(*ptr)->s.extras));
    } else {
        (*ptr)->s.extras = NULL;
    }
}

/** @brief MakePointers
  *
  * @todo: document this function
  */
void cRide::MakePointers(Ride_V* ptr, unsigned char*& uni_ptr, unsigned char*& com_ptr, const cAttraction& att, ovlRelocationManager* rel) const {

    ptr->options = reinterpret_cast<RideOption**>(com_ptr);
    com_ptr += (options.size()+1) * 4;
    rel->AddRelocation(reinterpret_cast<unsigned long*>(&ptr->options));
    for (int i = 0; i < options.size(); ++i) {
        ptr->options[i] = reinterpret_cast<RideOption*>(com_ptr);
        com_ptr += options[i].GetSize();
        rel->AddRelocation(reinterpret_cast<unsigned long*>(&ptr->options[i]));
    }
    ptr->options[options.size()] = NULL;
}

/** @brief GetUniqueSize
  *
  * @todo: document this function
  */
unsigned long cRide::GetUniqueSize(const cAttraction& att) const {
    if (att.soaked())
        return sizeof(Ride_S);
    if (att.wild())
        return sizeof(Ride_W);
    return 0;
}

/** @brief GetCommonSize
  *
  * @todo: document this function
  */
unsigned long cRide::GetCommonSize(const cAttraction& att) const {
    // Options
    int csize = 0;
    csize += (options.size()+1) * 4;
    csize += GetOptionsSize();
    if (att.isnew()) {
        csize += stationlimits.size() * sizeof(RideStationLimit);
    }
    return csize;
}

/** @brief MakeSymRefs
  *
  * @todo: document this function
  */
void cRide::MakeSymRefs(r3::Ride_V* ptr, const cAttraction& att, ovlLodSymRefManager* lsr, ovlStringTable* st) const {
    return;
}

/** @brief MakeSymRefs
  *
  * @todo: document this function
  */
void cRide::MakeSymRefs(r3::Ride_S* ptr, const cAttraction& att, ovlLodSymRefManager* lsr, ovlStringTable* st) const {
    att.MakeSymRefs(ptr->s.att, lsr, st);
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cRide::Fill(r3::Ride_V* r) const {
    r->attractivity_v = attractivity;
    r->seating = seating;
    r->entry_fee = entry_fee;
    r->min_circuits = min_circuits;
    r->max_circuits = max_circuits;
    for (int i = 0; i < options.size(); ++i) {
        options[i].Fill(r->options[i]);
    }
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cRide::Fill(r3::Ride_S* r) const {
    Fill(&r->v);
    r->v.attractivity_v = 0xFFFFFFFF;
    r->s.attractivity_sw = attractivity;
    r->s.extra_count = stationlimits.size();
    for (int i = 0; i < stationlimits.size(); ++i) {
        stationlimits[i].Fill(&r->s.extras[i]);
    }
    r->s.unk11 = unk11;
    r->s.unk12 = unk12;
    r->s.unk13 = unk13;
    r->s.unk14 = unk14;
    r->s.unk15 = unk15;
}

/** @brief Fill
  *
  * @todo: document this function
  */
void cRide::Fill(r3::Ride_W* r) const {
    Fill(reinterpret_cast<Ride_S*>(r));
    r->w.unk16 = unk16;
    r->w.unk17 = unk17;
}

/** @brief GetOptionsSize
  *
  * @todo: document this function
  */
unsigned long cRide::GetOptionsSize() const {
    unsigned long s = 0;
    for (vector<cRideOption>::const_iterator it = options.begin(); it != options.end(); ++it) {
        s += it->GetSize();
    }
    return s;
}


