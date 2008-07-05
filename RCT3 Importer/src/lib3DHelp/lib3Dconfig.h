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

#define READ_RCT3_TEXTURE()          wxcRead<long>(wxT("RCT3/TextureScaling"), RCT3_TEXTURE_ERROR_OUT)
#define WRITE_RCT3_TEXTURE(v)       wxcWrite<long>(wxT("RCT3/TextureScaling"), v)

#define READ_RCT3_EXPERTMODE()       wxcRead<bool>(wxT("RCT3/ExpertMode"), false)
#define WRITE_RCT3_EXPERTMODE(v)    wxcWrite<bool>(wxT("RCT3/ExpertMode"), v)

#define READ_RCT3_MOREEXPERTMODE()    wxcRead<bool>(wxT("RCT3/MoreExpertMode"), false)
#define WRITE_RCT3_MOREEXPERTMODE(v) wxcWrite<bool>(wxT("RCT3/MoreExpertMode"), v)

#define READ_RCT3_SPLITBONES()       wxcRead<bool>(wxT("RCT3/SplitBones"), true)
#define WRITE_RCT3_SPLITBONES(v)    wxcWrite<bool>(wxT("RCT3/SplitBones"), v)

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


#endif
