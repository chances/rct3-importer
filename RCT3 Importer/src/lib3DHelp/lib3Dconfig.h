///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// Defines for lib3DHelp relevant cxConfig paths
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
///////////////////////////////////////////////////////////////////////////////

#ifndef LIB3DCONFIG_H_INCLUDED
#define LIB3DCONFIG_H_INCLUDED

#define READ_RCT3_ORIENTATION()      wxcRead<long>(wxT("RCT3/Orientation"), ORIENTATION_LEFT_YUP)
#define WRITE_RCT3_ORIENTATION(v)   wxcWrite<long>(wxT("RCT3/Orientation"), v)

#define READ_RCT3_OVLVERSION()       wxcRead<long>(wxT("RCT3/OVLVersion"), 1)
#define WRITE_RCT3_OVLVERSION(v)    wxcWrite<long>(wxT("RCT3/OVLVersion"), v)

#define READ_RCT3_TEXTURE()          wxcRead<long>(wxT("RCT3/TextureScaling"), RCT3_TEXTURE_ERROR_OUT)
#define WRITE_RCT3_TEXTURE(v)       wxcWrite<long>(wxT("RCT3/TextureScaling"), v)

#define READ_RCT3_EXPERTMODE()       wxcRead<bool>(wxT("RCT3/ExpertMode"), false)
#define WRITE_RCT3_EXPERTMODE(v)    wxcWrite<bool>(wxT("RCT3/ExpertMode"), v)

#define READ_RCT3_MOREEXPERTMODE()    wxcRead<bool>(wxT("RCT3/MoreExpertMode"), false)
#define WRITE_RCT3_MOREEXPERTMODE(v) wxcWrite<bool>(wxT("RCT3/MoreExpertMode"), v)

#define READ_RCT3_WARNPREFIX()        wxcRead<bool>(wxT("RCT3/WarnPrefix"), true)
#define WRITE_RCT3_WARNPREFIX(v)     wxcWrite<bool>(wxT("RCT3/WarnPrefix"), v)

#define READ_RCT3_WARNASCII()         wxcRead<bool>(wxT("RCT3/WarnAscII"), true)
#define WRITE_RCT3_WARNASCII(v)      wxcWrite<bool>(wxT("RCT3/WarnAscII"), v)

#define READ_RCT3_COMPACTMESHES()     wxcRead<bool>(wxT("RCT3/CompactMeshes"), true)
#define WRITE_RCT3_COMPACTMESHES(v)  wxcWrite<bool>(wxT("RCT3/CompactMeshes"), v)

#define READ_RCT3_WARNOVERWRITE()       wxcRead<bool>(wxT("RCT3/WarnOverwrite"), true)
#define WRITE_RCT3_WARNOVERWRITE(v)    wxcWrite<bool>(wxT("RCT3/WarnOverwrite"), v)

#define READ_RCT3_VALIDATE()            wxcRead<bool>(wxT("RCT3/Validate"), true)
#define WRITE_RCT3_VALIDATE(v)         wxcWrite<bool>(wxT("RCT3/Validate"), v)

#define READ_RCT3_REPORTVALIDATION()    wxcRead<bool>(wxT("RCT3/ReportValidate"), false) // Only for ovlmake
#define WRITE_RCT3_REPORTVALIDATION(v) wxcWrite<bool>(wxT("RCT3/ReportValidate"), v)

#define READ_RCT3_DEEPVALIDATE()        wxcRead<bool>(wxT("RCT3/DeepValidate"), false)
#define WRITE_RCT3_DEEPVALIDATE(v)     wxcWrite<bool>(wxT("RCT3/DeepValidate"), v)

#define READ_RCT3_TRIANGLESORT_X()     wxcRead<wxString>(wxT("RCT3/TriangleSortX"), wxString(cTriangleSortAlgorithm::GetDefaultName(), wxConvLocal))
#define WRITE_RCT3_TRIANGLESORT_X(v)  wxcWrite<wxString>(wxT("RCT3/TriangleSortX"), v)

#define READ_RCT3_TRIANGLESORT_Y()     wxcRead<wxString>(wxT("RCT3/TriangleSortY"), wxString(cTriangleSortAlgorithm::GetDefaultName(), wxConvLocal))
#define WRITE_RCT3_TRIANGLESORT_Y(v)  wxcWrite<wxString>(wxT("RCT3/TriangleSortY"), v)

#define READ_RCT3_TRIANGLESORT_Z()     wxcRead<wxString>(wxT("RCT3/TriangleSortZ"), wxString(cTriangleSortAlgorithm::GetDefaultName(), wxConvLocal))
#define WRITE_RCT3_TRIANGLESORT_Z(v)  wxcWrite<wxString>(wxT("RCT3/TriangleSortZ"), v)

#define READ_RCT3_PRIORITY_WHEEL_FL()        wxcRead<long>(wxT("RCT3/Priority/Wheel/fl"),      40004)
#define WRITE_RCT3_PRIORITY_WHEEL_FL(v)     wxcWrite<long>(wxT("RCT3/Priority/Wheel/fl"), v)

#define READ_RCT3_PRIORITY_WHEEL_FR()        wxcRead<long>(wxT("RCT3/Priority/Wheel/fr"),      40003)
#define WRITE_RCT3_PRIORITY_WHEEL_FR(v)     wxcWrite<long>(wxT("RCT3/Priority/Wheel/fr"), v)

#define READ_RCT3_PRIORITY_WHEEL_RL()        wxcRead<long>(wxT("RCT3/Priority/Wheel/rl"),      40002)
#define WRITE_RCT3_PRIORITY_WHEEL_RL(v)     wxcWrite<long>(wxT("RCT3/Priority/Wheel/rl"), v)

#define READ_RCT3_PRIORITY_WHEEL_RR()        wxcRead<long>(wxT("RCT3/Priority/Wheel/rr"),      40001)
#define WRITE_RCT3_PRIORITY_WHEEL_RR(v)     wxcWrite<long>(wxT("RCT3/Priority/Wheel/rr"), v)

#define READ_RCT3_PRIORITY_FOUNTAIN_BL()      wxcRead<long>(wxT("RCT3/Priority/Fountain/bl"),  50004)
#define WRITE_RCT3_PRIORITY_FOUNTAIN_BL(v)   wxcWrite<long>(wxT("RCT3/Priority/Fountain/bl"), v)

#define READ_RCT3_PRIORITY_FOUNTAIN_BR()      wxcRead<long>(wxT("RCT3/Priority/Fountain/br"),  50003)
#define WRITE_RCT3_PRIORITY_FOUNTAIN_BR(v)   wxcWrite<long>(wxT("RCT3/Priority/Fountain/br"), v)

#define READ_RCT3_PRIORITY_FOUNTAIN_TL()      wxcRead<long>(wxT("RCT3/Priority/Fountain/tl"),  50002)
#define WRITE_RCT3_PRIORITY_FOUNTAIN_TL(v)   wxcWrite<long>(wxT("RCT3/Priority/Fountain/tl"), v)

#define READ_RCT3_PRIORITY_FOUNTAIN_TR()      wxcRead<long>(wxT("RCT3/Priority/Fountain/tr"),  50001)
#define WRITE_RCT3_PRIORITY_FOUNTAIN_TR(v)   wxcWrite<long>(wxT("RCT3/Priority/Fountain/tr"), v)

#define READ_RCT3_PRIORITY_LAUNCHER()        wxcRead<long>(wxT("RCT3/Priority/Launcher"),      60000)
#define WRITE_RCT3_PRIORITY_LAUNCHER(v)     wxcWrite<long>(wxT("RCT3/Priority/Launcher"), v)

#define READ_RCT3_PRIORITY_PARTICLE()        wxcRead<long>(wxT("RCT3/Priority/Particle"),      70000)
#define WRITE_RCT3_PRIORITY_PARTICLE(v)     wxcWrite<long>(wxT("RCT3/Priority/Particle"), v)

#define READ_RCT3_PRIORITY_LIGHT()           wxcRead<long>(wxT("RCT3/Priority/Light"),         80000)
#define WRITE_RCT3_PRIORITY_LIGHT(v)        wxcWrite<long>(wxT("RCT3/Priority/Light"), v)

#define READ_RCT3_PRIORITY_VENDOR()          wxcRead<long>(wxT("RCT3/Priority/Vendor"),        90000)
#define WRITE_RCT3_PRIORITY_VENDOR(v)       wxcWrite<long>(wxT("RCT3/Priority/Vendor"), v)

#define READ_RCT3_PRIORITY_PEEP()            wxcRead<long>(wxT("RCT3/Priority/Peep"),         100000)
#define WRITE_RCT3_PRIORITY_PEEP(v)         wxcWrite<long>(wxT("RCT3/Priority/Peep"), v)

#define READ_RCT3_PRIORITY_IK()              wxcRead<long>(wxT("RCT3/Priority/IK"),           200000)
#define WRITE_RCT3_PRIORITY_IK(v)           wxcWrite<long>(wxT("RCT3/Priority/IK"), v)

#define READ_RCT3_PRIORITY_OTHER()           wxcRead<long>(wxT("RCT3/Priority/Other"),        300000)
#define WRITE_RCT3_PRIORITY_OTHER(v)        wxcWrite<long>(wxT("RCT3/Priority/Other"), v)

#define READ_RCT3_PRIORITY_START()           wxcRead<long>(wxT("RCT3/Priority/Start"),      3000)
#define WRITE_RCT3_PRIORITY_START(v)        wxcWrite<long>(wxT("RCT3/Priority/Start"), v)

#define READ_RCT3_PRIORITY_COLOUR()          wxcRead<long>(wxT("RCT3/Priority/Colour"),     2000)
#define WRITE_RCT3_PRIORITY_COLOUR(v)       wxcWrite<long>(wxT("RCT3/Priority/Colour"), v)

#define READ_RCT3_PRIORITY_END()              wxcRead<long>(wxT("RCT3/Priority/End"),       1000)
#define WRITE_RCT3_PRIORITY_END(v)           wxcWrite<long>(wxT("RCT3/Priority/End"), v)

#endif
