///////////////////////////////////////////////////////////////////////////////
//
// ovlspy
// Command line utility to extract information from ovls
// Copyright (C) 2008 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, <http://www.gnu.org/licenses/>.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
// Idea stolen from ovlcompiler by The_Cook
//
///////////////////////////////////////////////////////////////////////////////

#include <wx/wxprec.h>

#include <wx/cmdline.h>
#include <wx/filename.h>
#include <wx/init.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif


#include <exception>
#include <stdlib.h>

#include <boost/format.hpp>

#include "animatedride.h"
#include "rct3constants.h"
#include "sceneryrevised.h"
#include "stall.h"

#include "version.h"

//#define LIBXMLTEST

#include "OVLDException.h"
#include "OVLDump.h"
#include "pretty.h"

#ifdef UNICODE
#define UNIPTR(s) s.mb_str(wxConvLocal).data()
#else
#define UNIPTR(s) s.c_str()
#endif

using namespace pretty;
using namespace r3;
using namespace std;

inline void maximize(int& domax, const int withmax) {
    if (withmax>domax)
        domax = withmax;
}

void printVersion() {
    wxString strdate = _("Built on ") + wxString(AutoVersion::DATE, wxConvLocal) + wxT(".")
                          + wxString(AutoVersion::MONTH, wxConvLocal) + wxT(".")
                          + wxString(AutoVersion::YEAR, wxConvLocal);
    wxString strversion = wxString::Format(wxT("ovlspy v%ld.%ld, Build %ld, svn %s, "), AutoVersion::MAJOR, AutoVersion::MINOR, AutoVersion::BUILD, AutoVersion::SVN_REVISION)+ wxString(AutoVersion::STATUS, wxConvLocal);
    wxString compversion("Compiler: ");
#ifdef __GNUC__
    compversion += "GCC ";
#else
    compversion += "Non-GCC ";
#endif
    compversion += wxString(__VERSION__);
#ifdef UNICODE
    strdate += wxT(", Unicode");
#else
    strdate += wxT(", ANSI");
#endif
#ifdef PUBLICDEBUG
    strdate += wxT(" Debug");
#endif
    strdate += _(" version");
    int linelen = strdate.size();
    maximize(linelen, strversion.size());
    maximize(linelen, compversion.size());
    wxString line(wxT('-'), linelen);
    fprintf(stderr, "\n%s\n", UNIPTR(line));
    fprintf(stderr, "%s\n", UNIPTR(strversion));
    fprintf(stderr, "%s\n", UNIPTR(strdate));
    fprintf(stderr, "%s", UNIPTR(compversion));
    fprintf(stderr, "\nCopyright (C) 2008 Belgabor");
    fprintf(stderr, "\n%s\n\n", UNIPTR(line));
    fprintf(stderr, "This program comes with ABSOLUTELY NO WARRANTY.\n");
    fprintf(stderr, "This is free software, and you are welcome to redistribute it\n");
    fprintf(stderr, "under certain conditions; see License.txt for details.\n\n");
}

wxString FindText(vector<cOVLDump>& dovls, const string& ref) {
    foreach(cOVLDump& dovl, dovls) {
        map<string, map<string, OvlRelocation*> >::const_iterator me;
        me = dovl.GetStructures(OVLT_COMMON).find("txt");
        if (me != dovl.GetStructures(OVLT_COMMON).end()) {
            map<string, OvlRelocation*>::const_iterator txt = me->second.find(ref);
            if (txt != me->second.end()) {
                wchar_t* tx = reinterpret_cast<wchar_t*>(txt->second->target);
                return wxString(tx);
            }
        }
        me = dovl.GetStructures(OVLT_UNIQUE).find("txt");
        if (me != dovl.GetStructures(OVLT_UNIQUE).end()) {
            map<string, OvlRelocation*>::const_iterator txt = me->second.find(ref);
            if (txt != me->second.end()) {
                wchar_t* tx = reinterpret_cast<wchar_t*>(txt->second->target);
                return wxString(tx);
            }
        }
    }
    return wxString::Format("[%s]", wxString(ref.c_str(), wxConvLocal).c_str());
}

void LoadRecursive(vector<cOVLDump>& dovls, const cOVLDump& dovl) {
    wxString dovlpath = wxFileName(wxString(dovl.GetFilename(OVLT_COMMON).c_str(), wxConvLocal)).GetPathWithSep();
    for (int i = 0; i < 2; ++i) {
        foreach(const string& ref, dovl.GetReferences(static_cast<cOvlType>(i))) {
            wxFileName reffn(wxString(ref.c_str(), wxConvLocal));
            reffn.SetExt("common.ovl");
            reffn.MakeAbsolute(dovlpath);
            if (reffn.IsFileReadable()) {
                try {
                    cOVLDump dovlr;
                    dovlr.Load(reffn.GetFullPath().mb_str(wxConvLocal));
                    dovls.push_back(dovlr);
                    LoadRecursive(dovls, dovlr);
                } catch (exception& e) {
                    wxLogWarning("Referenced ovl '%s' could not be parsed: %s", reffn.GetFullName().c_str(), wxString(e.what(), wxConvLocal).c_str());
                }
            } else {
                wxLogWarning("Referenced ovl '%s' not found.", reffn.GetFullName().c_str());
            }
        }
    }
}

wxString GetTypeString(uint32_t type, bool numeric) {
    if (numeric)
        return wxString::Format("%ld", type);
    switch (type) {
        case r3::Constants::SID::Type::Tree:
            return "Tree (Scenery -> Foliage)";
        case r3::Constants::SID::Type::Plant:
            return "Plant (Scenery -> Foliage)";
        case r3::Constants::SID::Type::Shrub:
            return "Shrub (Scenery -> Foliage)";
        case r3::Constants::SID::Type::Flowers:
            return "Flowers (Scenery -> Foliage)";
        case r3::Constants::SID::Type::Fence:
            return "Fence (Scenery -> Fences)";
        case r3::Constants::SID::Type::Wall_Misc:
            return "Misc. Wall (Scenery -> Walls, Roofs and Buildings)";
        case r3::Constants::SID::Type::Path_Lamp:
            return "Path Lamp (Scenery -> Path Extras)";
        case r3::Constants::SID::Type::Scenery_Small:
            return "Small Scenery (Scenery -> Scenery Items)";
        case r3::Constants::SID::Type::Scenery_Medium:
            return "Medium Scenery (Scenery -> Scenery Items)";
        case r3::Constants::SID::Type::Scenery_Large:
            return "Large Scenery (Scenery -> Scenery Items)";
        case r3::Constants::SID::Type::Scenery_Anamatronic:
            return "Animatronic Scenery (Scenery -> Scenery Items)";
        case r3::Constants::SID::Type::Scenery_Misc:
            return "Misc. Scenery (Scenery -> Scenery Items)";
        case r3::Constants::SID::Type::Support_Middle:
            return "Middle Support (Nowhere)";
        case r3::Constants::SID::Type::Support_Top:
            return "Top Support (Nowhere)";
        case r3::Constants::SID::Type::Support_Bottom:
            return "Bottom Support (Nowhere)";
        case r3::Constants::SID::Type::Support_Bottom_Extra:
            return "Bottom Extra Support (Nowhere)";
        case r3::Constants::SID::Type::Support_Girder:
            return "Girder Support (Nowhere)";
        case r3::Constants::SID::Type::Support_Cap:
            return "Support Cap (Nowhere)";
        case r3::Constants::SID::Type::Ride_Track:
            return "Ride Track (Nowhere)";
        case r3::Constants::SID::Type::Path:
            return "Path (Nowhere)";
        case r3::Constants::SID::Type::Park_Entrance:
            return "Middle Support (Scenery -> Park entrances)";
        case r3::Constants::SID::Type::Litter:
            return "Litter (Nowhere)";
        case r3::Constants::SID::Type::Guest_Inject:
            return "Guest Injection Point (Scenery -> Park Entrances)";
        case r3::Constants::SID::Type::Ride:
            return "Ride (%s)";
        case r3::Constants::SID::Type::Ride_Entrance:
            return "Ride Entrance (Ride Entrances)";
        case r3::Constants::SID::Type::Ride_Exit:
            return "Ride Exit (Ride Exits)";
        case r3::Constants::SID::Type::Keep_Clear_Fence:
            return "Keep Clear Fence (No idea, maybe in the park options)";
        case r3::Constants::SID::Type::Stall:
            return "Stall (%s)";
        case r3::Constants::SID::Type::Ride_Event:
            return "Ride Event (Scenery -> Ride Events)";
        case r3::Constants::SID::Type::Firework:
            return "Firework (Scenery -> Mix Master -> Mix Master Bases)";
        case r3::Constants::SID::Type::Litter_Bin:
            return "Litter Bin (Scenery -> Path Extras)";
        case r3::Constants::SID::Type::Bench:
            return "Bench (Scenery -> Path Extras)";
        case r3::Constants::SID::Type::Sign:
            return "Sign (Scenery -> Path Extras)";
        case r3::Constants::SID::Type::Photo_Point:
            return "Photo Point (Scenery -> Path Extras)";
        case r3::Constants::SID::Type::Wall_Straight:
            return "Straight Wall (Scenery -> Walls, Roofs and Buildings)";
        case r3::Constants::SID::Type::Wall_Roof:
            return "Roof (Scenery -> Walls, Roofs and Buildings)";
        case r3::Constants::SID::Type::Wall_Corner:
            return "Corner (Scenery -> Walls, Roofs and Buildings)";
        case r3::Constants::SID::Type::Water_Cannon:
            return "Water Cannon (Scenery -> Path Extras)";
        case r3::Constants::SID::Type::Pool_Piece:
            return "Pool Piece (Nowhere)";
        case r3::Constants::SID::Type::Pool_Extra:
            return "Pool Extra (Nowhere)";
        case r3::Constants::SID::Type::Changing_Room:
            return "Changing Room (%s)";
        case r3::Constants::SID::Type::Laser_Dome:
            return "Laser Dome (Scenery -> Mix Master -> Mix Master bases)";
        case r3::Constants::SID::Type::Water_Jet:
            return "Water Jet (Scenery -> Mix Master -> Mix Master bases)";
        case r3::Constants::SID::Type::Terrain_Piece:
            return "Terrain Piece (Scenery -> Terrain Surfaces)";
        case r3::Constants::SID::Type::Particle_Effect:
            return "Particle Effect (Scenery -> Scenery Items)";
        case r3::Constants::SID::Type::Animal_Fence:
            return "Animal Fence (Nowhere)";
        case r3::Constants::SID::Type::Animal_Misc:
            return "Animal Misc. (Nowhere)";
        default:
            return wxString::Format("Unknown (%ld)", type);
    }
}

wxString GetTypeExtra(cOVLDump& dovl, const string& sid, uint32_t type, bool numeric) {
    map<string, map<string, OvlRelocation*> >::const_iterator me;
    me = dovl.GetStructures(OVLT_UNIQUE).find("anr");
    if (me != dovl.GetStructures(OVLT_UNIQUE).end()) {
        for (map<string, OvlRelocation*>::const_iterator it = me->second.begin(); it != me->second.end(); ++it) {
            AnimatedRide_V* ch = reinterpret_cast<AnimatedRide_V*>(it->second->target);
            uint8_t atttype;
            if (ch->att.type == 0xFFFFFFFF) {
                AnimatedRide_SW* sw = reinterpret_cast<AnimatedRide_SW*>(ch);
                if (sid != dovl.GetSymbolReference(&sw->sid_ref))
                    continue;
                Ride_S* ri = reinterpret_cast<Ride_S*>(dovl.GetTargets()[reinterpret_cast<unsigned long>(sw->ride_sub_s)]);
                Attraction_S* att = reinterpret_cast<Attraction_S*>(dovl.GetTargets()[reinterpret_cast<unsigned long>(ri->s.att)]);
                atttype = att->v.typech[0];
            } else {
                if (ch->unk22)
                    // ANR in Scenery
                    continue;
                if (sid != dovl.GetSymbolReference(&ch->sid_ref))
                    continue;
                atttype = ch->att.typech[0];
            }
            if (numeric) {
                return wxString::Format("%hhd", atttype);
            }
            if (type == r3::Constants::SID::Type::Ride) {
                switch (atttype) {
                    case r3::Constants::Attraction::Type::Ride_Transport_Standard:
                        return "Rides -> Transport Rides";
                    case r3::Constants::Attraction::Type::Ride_Gentle:
                        return "Rides -> Gentle Rides";
                    case r3::Constants::Attraction::Type::Ride_Water:
                        return "Rides -> Water Rides";
                    case r3::Constants::Attraction::Type::Ride_Coaster_Steel:
                        return "Rides -> RollerCoasters";
                    case r3::Constants::Attraction::Type::Ride_Thrill:
                        return "Rides -> Thrill Rides";
                    case r3::Constants::Attraction::Type::Ride_Other:
                        return "Rides -> Other Rides and Attractions";
                    case r3::Constants::Attraction::Type::Ride_Junior:
                        return "Rides -> Junior Rides";
                    case r3::Constants::Attraction::Type::Stall_Food:
                    case r3::Constants::Attraction::Type::Stall_Drink:
                    case r3::Constants::Attraction::Type::Stall_Misc:
                    case r3::Constants::Attraction::Type::Special_Toilet:
                    case r3::Constants::Attraction::Type::Special_ATM:
                    case r3::Constants::Attraction::Type::Special_1st_Aid:
                    case r3::Constants::Attraction::Type::Stall_Facility:
                        return "Nowhere";
                    case r3::Constants::Attraction::Type::Ride_Coaster_Wooden:
                    case r3::Constants::Attraction::Type::Ride_Coaster_Suspended:
                    case r3::Constants::Attraction::Type::Ride_Coaster_Water:
                        return "Rides -> RollerCoasters";
                    case r3::Constants::Attraction::Type::Changing_Room:
                        return "Nowhere";
                    case r3::Constants::Attraction::Type::Ride_Pool:
                        return "Rides -> Pool Slides and Rides";
                    case r3::Constants::Attraction::Type::Viewing_Gallery:
                        return "Nowhere";
                    case r3::Constants::Attraction::Type::Ride_Transport_Safari:
                        return "Rides -> Transport Rides";
                    default:
                        return wxString::Format("Unknown: %hhd", atttype);
                }
            } else {
                return "Nowhere";
            }
        }
    }
    me = dovl.GetStructures(OVLT_UNIQUE).find("chg");
    if (me != dovl.GetStructures(OVLT_UNIQUE).end()) {
        for (map<string, OvlRelocation*>::const_iterator it = me->second.begin(); it != me->second.end(); ++it) {
            ChangingRoom* ch = reinterpret_cast<ChangingRoom*>(it->second->target);
            if (sid != dovl.GetSymbolReference(&ch->sid_ref))
                continue;
            Attraction_S* att = reinterpret_cast<Attraction_S*>(dovl.GetTargets()[reinterpret_cast<unsigned long>(ch->att)]);
            uint8_t atttype = att->v.typech[0];
            if (numeric) {
                return wxString::Format("%hhd", atttype);
            }
            return "Rides -> Swimming Pools -> Changing Rooms"; // Doesn't give a damn about type...
        }
    }
    me = dovl.GetStructures(OVLT_UNIQUE).find("sat");
    if (me != dovl.GetStructures(OVLT_UNIQUE).end()) {
        for (map<string, OvlRelocation*>::const_iterator it = me->second.begin(); it != me->second.end(); ++it) {
            SpecialAttraction_V* ch = reinterpret_cast<SpecialAttraction_V*>(it->second->target);
            uint8_t atttype;
            if (ch->att.type == 0xFFFFFFFF) {
                SpecialAttraction_SW* sw = reinterpret_cast<SpecialAttraction_SW*>(ch);
                if (sid != dovl.GetSymbolReference(&sw->sid_ref))
                    continue;
                Attraction_S* att = reinterpret_cast<Attraction_S*>(dovl.GetTargets()[reinterpret_cast<unsigned long>(sw->att)]);
                atttype = att->v.typech[0];
            } else {
                if (sid != dovl.GetSymbolReference(&ch->sid_ref))
                    continue;
                atttype = ch->att.typech[0];
            }
            if (numeric) {
                return wxString::Format("%hhd", atttype);
            }
            if (type != r3::Constants::SID::Type::Ride) {
                switch (atttype) {
                    case r3::Constants::Attraction::Type::Ride_Transport_Standard:
                    case r3::Constants::Attraction::Type::Ride_Gentle:
                    case r3::Constants::Attraction::Type::Ride_Water:
                    case r3::Constants::Attraction::Type::Ride_Coaster_Steel:
                    case r3::Constants::Attraction::Type::Ride_Thrill:
                    case r3::Constants::Attraction::Type::Ride_Other:
                    case r3::Constants::Attraction::Type::Ride_Junior:
                        return "Nowhere";
                    case r3::Constants::Attraction::Type::Stall_Food:
                        return "Shops & Facilities -> Food";
                    case r3::Constants::Attraction::Type::Stall_Drink:
                        return "Shops & Facilities -> Drink";
                    case r3::Constants::Attraction::Type::Stall_Misc:
                        return "Shops & Facilities -> Stalls";
                    case r3::Constants::Attraction::Type::Special_Toilet:
                    case r3::Constants::Attraction::Type::Special_ATM:
                    case r3::Constants::Attraction::Type::Special_1st_Aid:
                    case r3::Constants::Attraction::Type::Stall_Facility:
                        return "Shops & Facilities -> Facilities";
                    case r3::Constants::Attraction::Type::Ride_Coaster_Wooden:
                    case r3::Constants::Attraction::Type::Ride_Coaster_Suspended:
                    case r3::Constants::Attraction::Type::Ride_Coaster_Water:
                    case r3::Constants::Attraction::Type::Changing_Room:
                    case r3::Constants::Attraction::Type::Ride_Pool:
                    case r3::Constants::Attraction::Type::Viewing_Gallery:
                    case r3::Constants::Attraction::Type::Ride_Transport_Safari:
                        return "Nowhere";
                    default:
                        return wxString::Format("Unknown: %hhd", atttype);
                }
            } else {
                return "Nowhere";
            }
        }
    }
    me = dovl.GetStructures(OVLT_UNIQUE).find("sta");
    if (me != dovl.GetStructures(OVLT_UNIQUE).end()) {
        for (map<string, OvlRelocation*>::const_iterator it = me->second.begin(); it != me->second.end(); ++it) {
            Stall_V* ch = reinterpret_cast<Stall_V*>(it->second->target);
            uint8_t atttype;
            if (ch->att.type == 0xFFFFFFFF) {
                Stall_SW* sw = reinterpret_cast<Stall_SW*>(ch);
                if (sid != dovl.GetSymbolReference(&sw->sid_ref))
                    continue;
                Attraction_S* att = reinterpret_cast<Attraction_S*>(dovl.GetTargets()[reinterpret_cast<unsigned long>(sw->att)]);
                atttype = att->v.typech[0];
            } else {
                if (sid != dovl.GetSymbolReference(&ch->sid_ref))
                    continue;
                atttype = ch->att.typech[0];
            }
            if (numeric) {
                return wxString::Format("%hhd", atttype);
            }
            if (type != r3::Constants::SID::Type::Ride) {
                switch (atttype) {
                    case r3::Constants::Attraction::Type::Ride_Transport_Standard:
                    case r3::Constants::Attraction::Type::Ride_Gentle:
                    case r3::Constants::Attraction::Type::Ride_Water:
                    case r3::Constants::Attraction::Type::Ride_Coaster_Steel:
                    case r3::Constants::Attraction::Type::Ride_Thrill:
                    case r3::Constants::Attraction::Type::Ride_Other:
                    case r3::Constants::Attraction::Type::Ride_Junior:
                        return "Nowhere";
                    case r3::Constants::Attraction::Type::Stall_Food:
                        return "Shops & Facilities -> Food";
                    case r3::Constants::Attraction::Type::Stall_Drink:
                        return "Shops & Facilities -> Drink";
                    case r3::Constants::Attraction::Type::Stall_Misc:
                        return "Shops & Facilities -> Stalls";
                    case r3::Constants::Attraction::Type::Special_Toilet:
                    case r3::Constants::Attraction::Type::Special_ATM:
                    case r3::Constants::Attraction::Type::Special_1st_Aid:
                    case r3::Constants::Attraction::Type::Stall_Facility:
                        return "Shops & Facilities -> Facilities";
                    case r3::Constants::Attraction::Type::Ride_Coaster_Wooden:
                    case r3::Constants::Attraction::Type::Ride_Coaster_Suspended:
                    case r3::Constants::Attraction::Type::Ride_Coaster_Water:
                    case r3::Constants::Attraction::Type::Changing_Room:
                    case r3::Constants::Attraction::Type::Ride_Pool:
                    case r3::Constants::Attraction::Type::Viewing_Gallery:
                    case r3::Constants::Attraction::Type::Ride_Transport_Safari:
                        return "Nowhere";
                    default:
                        return wxString::Format("Unknown: %hhd", atttype);
                }
            } else {
                return "Nowhere";
            }
        }
    }
}

wxString GetPositionString(uint32_t type, bool numeric) {
    if (numeric)
        return wxString::Format("%ld", type);
    switch (type) {
        r3::Constants::SID::Position::Tile_Full:
            return "Full Tile";
        r3::Constants::SID::Position::Path_Edge_Inner:
            return "Path Edge, Inner";
        r3::Constants::SID::Position::Path_Edge_Outer:
            return "Path Edge, Outer";
        r3::Constants::SID::Position::Wall:
            return "Wall";
        r3::Constants::SID::Position::Tile_Quarter:
            return "Quarter Tile";
        r3::Constants::SID::Position::Tile_Half:
            return "Half Tile";
        r3::Constants::SID::Position::Path_Center:
            return "Path Center";
        r3::Constants::SID::Position::Corner:
            return "Corner";
        r3::Constants::SID::Position::Path_Edge_Outer:
            return "Path Edge, Outer";
                Path_Edge_Inner =           1,
                Path_Edge_Outer =           2,
                Wall =                      3,
                Tile_Quarter =              4,
                Tile_Half =                 5,
                Path_Center =               6,
                Corner =                    7,
                Path_Edge_Join =            8
        default:
            return wxString::Format("Unknown (%ld)", type);
    }
}

int DoDump(const wxCmdLineParser& parser) {
    int ret = 0;
    try {
        wxString inputfilestr = parser.GetParam(0);
        bool tabbed = parser.Found(wxT("t"));
        bool numeric = parser.Found(wxT("n"));
        bool installed = true;
        FILE* o = stdout;
        wxMBConv* oconv = &wxConvLocal;
        wxString outputfilestr;
        if (parser.Found(wxT("o"), &outputfilestr)) {
            o = fopen(outputfilestr.mb_str(wxConvLocal), "w");
            if (!o)
                throw EOvlD("Could not open output file");
            oconv = &wxConvUTF8;
        }
        wxString installdir("");
        if (!parser.Found(wxT("installdir"), &installdir)) {
#ifdef __WXMSW__
            wxRegKey key("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{907B4640-266B-4A21-92FB-CD1A86CD0F63}");
            if (key.Exists()) {
                key.QueryValue("InstallLocation", installdir);
            }
            if (installdir.IsEmpty()) {
#endif
                wxLogWarning(_("Cannot determine RCT3 installatinon directory. Use --installdir to set manually."));
#ifdef __WXMSW__
            }
#endif
        }

        wxFileName inputfile(inputfilestr);
        if (!inputfile.IsFileReadable()) {
            throw EOvlD("Input file not readable");
        }

        wxString outputformat;
        if (tabbed) {
            outputformat = "%s\t%s\t%s\t%s\n";
        } else {
            outputformat =
                "Name: %s\n"
                "Group: %s\n"
                "Type: %s\n"
                "Positioning: %s\n"
                "\n"
                ;
        }

        vector<cOVLDump> dovls;
        cOVLDump dovl;
        dovl.Load(inputfile.GetFullPath().mb_str(wxConvLocal));
        dovls.push_back(dovl);

        LoadRecursive(dovls, dovl);

        map<string, map<string, OvlRelocation*> >::const_iterator me;
        me = dovl.GetStructures(OVLT_UNIQUE).find("sid");
        if (me != dovl.GetStructures(OVLT_UNIQUE).end()) {
            for (map<string, OvlRelocation*>::const_iterator it = me->second.begin(); it != me->second.end(); ++it) {
                //wxLogMessage(it->first.c_str());
                SceneryItem_V* ch = reinterpret_cast<SceneryItem_V*>(it->second->target);
                wxString name = FindText(dovls, dovl.GetSymbolReference(&ch->name_ref));
                wxString group;
                if (has(dovl.GetSymbolReferences(), &ch->group_name_ref)) {
                    group = FindText(dovls, dovl.GetSymbolReference(&ch->group_name_ref));
                }
                wxString type = GetTypeString(ch->type, numeric);
                if ((ch->type == r3::Constants::SID::Type::Ride) ||
                    (ch->type == r3::Constants::SID::Type::Stall) ||
                    (ch->type == r3::Constants::SID::Type::Changing_Room)) {
                    wxString extra = GetTypeExtra(dovl, it->first, ch->type, numeric);
                    if (numeric) {
                        type += "/";
                        type += extra;
                    } else {
                        type = wxString::Format(type, extra);
                    }
                }

                fprintf(o, wxString::Format(outputformat,
                    name,
                    group,
                    type,
                    GetPositionString(ch->position_type, numeric);
                    ).mb_str(*oconv));
            }
        } else {
            throw EOvlD("No structures I can spy out in given file.");
        }

    } catch (EOvlD& e) {
        wxLogMessage(_("Error: "), wxString(e.what(), wxConvLocal).c_str());
        ret = -1;
    } catch (std::exception& e) {
        wxLogMessage(_("General error: %s"), wxString(e.what(), wxConvLocal).c_str());
        ret = -1;
    }
    return ret;
}

int main(int argc, char **argv)
{
    int ret = 0;
#if wxUSE_UNICODE
    wxChar **wxArgv = new wxChar *[argc + 1];

    {
        int n;

        for (n = 0; n < argc; n++ )
        {
            wxMB2WXbuf warg = wxConvertMB2WX(argv[n]);
            wxArgv[n] = wxStrdup(warg);
        }

        wxArgv[n] = NULL;
    }
#else // !wxUSE_UNICODE
    #define wxArgv argv
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

#ifdef PUBLICDEBUG
    if (!LoadLibrary("exchndl.dll"))
        fprintf(stderr, "Failed to load debug library!\n");
#endif

    // *&^$% GraphicsMagick
//    wxFileName app = wxString(wxArgv[0]);
//    if (!app.IsAbsolute())
//        app.MakeAbsolute();
//    try {
//        wxGXImage::CheckGraphicsMagick(app.GetPathWithSep());
//    } catch (exception& e) {
//        fprintf(stderr, "Error initializing the graphics library:\n");
//        fprintf(stderr, "%s\n", e.what());
//        fprintf(stderr, "All .mgk files expected in '%s'.\n", static_cast<const char*>(app.GetPathWithSep().mb_str(wxConvLocal)));
//        return -1;
//    }


    wxInitializer initializer;
    if ( !initializer ) {
        fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");
        return -1;
    }

    wxLocale loc(wxLANGUAGE_ENGLISH);


    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "h", "help", "show this help message",
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_OPTION, "o", "output", "write output to this file instead of stdout." },
        { wxCMD_LINE_OPTION, "i", "installdir", "override RCT3 installation directory read from the registry." },
        { wxCMD_LINE_SWITCH, "t", "tab", "use tab separated values instead of human-readable as output format." },
        { wxCMD_LINE_SWITCH, "n", "numeric", "use numeric values where applicable." },

        { wxCMD_LINE_PARAM,  NULL, NULL, "ovl file", wxCMD_LINE_VAL_STRING },

        wxCMD_LINE_DESC_END
    };

    wxCmdLineParser parser(cmdLineDesc, argc, wxArgv);
    parser.SetSwitchChars(wxT("-"));

    switch ( parser.Parse(false) )
    {
        case -1:
            printVersion();
            parser.Usage();
            break;
        case 0:
            ret = DoDump(parser);
            break;

        default:
            printVersion();
            parser.Usage();
            wxLogError(_("Command line syntax error detected, aborting."));
            ret = -1;
            break;
    }

#if wxUSE_UNICODE
    {
        for ( int n = 0; n < argc; n++ )
            free(wxArgv[n]);

        delete [] wxArgv;
    }
#endif // wxUSE_UNICODE

    wxUnusedVar(argc);
    wxUnusedVar(argv);
    return ret;
}

