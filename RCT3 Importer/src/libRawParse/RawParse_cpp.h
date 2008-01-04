///////////////////////////////////////////////////////////////////////////////
//
// raw ovl xml interpreter
// Command line ovl creation utility
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



#include "RawParse.h"

#include <wx/file.h>
#include <wx/strconv.h>
#include <wx/tokenzr.h>

#include "base64.h"
#include "counted_array_ptr.h"
#include "OVLManagers.h"
#include "RCT3Exception.h"
#include "RCT3Macros.h"
#include "SCNFile.h"
#include "gximage.h"
#include "matrix.h"
#include "rct3log.h"

#include "OVLDump.h"

#define RAWXML_SECTION wxT("section")
#define RAWXML_CHECK   wxT("check")
#define RAWXML_COPY    wxT("copy")
#define RAWXML_PATCH   wxT("patch")
#define RAWXML_WRITE   wxT("write")

#define RAWXML_BAN wxT("ban")
#define RAWXML_BSH wxT("bsh")
#define RAWXML_CED wxT("ced")
#define RAWXML_CHG wxT("chg")
#define RAWXML_CID wxT("cid")
#define RAWXML_FTX wxT("ftx")
#define RAWXML_GSI wxT("gsi")
#define RAWXML_SID wxT("sid")
#define RAWXML_SPL wxT("spl")
#define RAWXML_STA wxT("sta")
#define RAWXML_SVD wxT("svd")
#define RAWXML_TEX wxT("tex")
#define RAWXML_TXT wxT("txt")

#define RAWXML_BAN_BONE             wxT("banbone")
#define RAWXML_BAN_BONE_TRANSLATION wxT("translation")
#define RAWXML_BAN_BONE_ROTATION    wxT("rotation")

#define RAWXML_BSH_BBOX1            wxT("boundingboxmin")
#define RAWXML_BSH_BBOX2            wxT("boundingboxmax")
#define RAWXML_BSH_MESH             wxT("bshmesh")
#define RAWXML_BSH_MESH_VERTEX2     wxT("vertex2")
#define RAWXML_BSH_MESH_VERTEX2_P   wxT("position")
#define RAWXML_BSH_MESH_VERTEX2_N   wxT("normal")
#define RAWXML_BSH_MESH_VERTEX2_B   wxT("boneassignment")
#define RAWXML_BSH_MESH_TRIANGLE    wxT("triangle")
#define RAWXML_BSH_BONE             wxT("bshbone")
#define RAWXML_BSH_BONE_POS1        wxT("position1")
#define RAWXML_BSH_BONE_POS2        wxT("position2")

#define RAWXML_CED_MORE             wxT("cedmore")

#define RAWXML_CID_SHAPE            wxT("cidshape")
#define RAWXML_CID_MORE             wxT("cidmore")
#define RAWXML_CID_EXTRA            wxT("cidextra")
#define RAWXML_CID_SETTINGS         wxT("cidsettings")
#define RAWXML_CID_TRASH            wxT("cidtrash")
#define RAWXML_CID_SOAKED           wxT("cidsoaked")
#define RAWXML_CID_SIZEUNK          wxT("cidsizeunknowns")
#define RAWXML_CID_UNK              wxT("cidunknowns")

#define RAWXML_FTX_ANIMATION        wxT("ftxanimation")
#define RAWXML_FTX_FRAME            wxT("ftxframe")
#define RAWXML_FTX_FRAME_IMAGE      wxT("image")
#define RAWXML_FTX_FRAME_ALPHA      wxT("alpha")
#define RAWXML_FTX_FRAME_PDATA      wxT("palettedata")
#define RAWXML_FTX_FRAME_TDATA      wxT("texturedata")
#define RAWXML_FTX_FRAME_ADATA      wxT("alphadata")

#define RAWXML_SID_GROUP            wxT("sidgroup")
#define RAWXML_SID_TYPE             wxT("sidtype")
#define RAWXML_SID_POSITION         wxT("sidposition")
#define RAWXML_SID_COLOURS          wxT("sidcolours")
#define RAWXML_SID_SVD              wxT("sidvisual")
#define RAWXML_SID_IMPORTERUNKNOWNS wxT("sidimporterunknowns")
#define RAWXML_SID_SQUAREUNKNOWNS   wxT("sidsquareunknowns")
#define RAWXML_SID_STALLUNKNOWNS    wxT("sidstallunknowns")
#define RAWXML_SID_PARAMETER        wxT("sidparameter")

#define RAWXML_SPL_NODE             wxT("splnode")
#define RAWXML_SPL_LENGTH           wxT("spllength")
#define RAWXML_SPL_DATA             wxT("spldata")

#define RAWXML_STA_ITEM             wxT("staitem")
#define RAWXML_STA_STALLUNKNOWNS    wxT("stastallunknowns")

#define RAWXML_TEX_FILE             wxT("texturefile")
#define RAWXML_TEX_DATA             wxT("texturedata")

#define RAWXML_ATTRACTION_BASE      wxT("attractionbase")
#define RAWXML_ATTRACTION_UNKNOWNS  wxT("attractionunknowns")


#define RAWXML_IMPORT    wxT("import")
#define RAWXML_REFERENCE wxT("reference")
#define RAWXML_SYMBOL    wxT("symbol")

#define RAWBAKE_ABSOLUTE wxT("absolute")
#define RAWBAKE_XML      wxT("xml")

enum {
    FAILMODE_WARN_FAIL = 0,
    FAILMODE_FAIL_FAIL = 1
};

enum {
    FILEBASE_INSTALLDIR = 0,
    FILEBASE_USERDIR    = 1
};

#define BAKE_SKIP(node) \
    if (m_mode == MODE_BAKE) { \
        node = node->GetNext(); \
        continue; \
    }

#define OPTION_PARSE(t, v, p) \
    try { \
        t x = p; \
        v = x; \
    } catch (RCT3InvalidValueException) { \
        throw; \
    } catch (RCT3Exception) {}

#define USE_PREFIX(child) \
    bool useprefix = true; \
    if (child->HasProp(wxT("useprefix"))) { \
        if (child->GetPropVal(wxT("useprefix"), wxT("1")) == wxT("0")) \
            useprefix = false; \
    }


#define DO_CONDITION_COMMENT(child) \
        if (m_mode != MODE_BAKE) { \
            if (child->HasProp(wxT("if"))) { \
                bool haveit = true; \
                wxString t = child->GetPropVal(wxT("if"), wxT("")); \
                MakeVariable(t); \
                cRawParserVars::iterator it = m_commandvariables.find(t); \
                if (it == m_commandvariables.end()) \
                    it = m_variables.find(t); \
                if (it == m_variables.end()) \
                    haveit = false; \
                if (haveit) { \
                    haveit = it->second != wxT("0"); \
                } \
                if (!haveit) { \
                    child = child->GetNext(); \
                    continue; \
                } \
            } \
            if (child->HasProp(wxT("ifnot"))) { \
                bool haveit = true; \
                wxString t = child->GetPropVal(wxT("ifnot"), wxT("")); \
                MakeVariable(t); \
                cRawParserVars::iterator it = m_commandvariables.find(t); \
                if (it == m_commandvariables.end()) \
                    it = m_variables.find(t); \
                if (it == m_variables.end()) \
                    haveit = false; \
                if (haveit) { \
                    haveit = it->second != wxT("0"); \
                } \
                if (haveit) { \
                    child = child->GetNext(); \
                    continue; \
                } \
            } \
        } \
        if (child->HasProp(wxT("comment"))) { \
            wxString t = child->GetPropVal(wxT("comment"), wxT("")); \
            MakeVariable(t); \
            wxLogMessage(t); \
        }



bool CanBeWrittenTo(const wxFileName& target);
bool EnsureDir(wxFileName& target);
unsigned char ParseDigit(char x);

class RCT3InvalidValueException: public RCT3Exception {
public:
    RCT3InvalidValueException(const wxString& message):RCT3Exception(message){};
};

