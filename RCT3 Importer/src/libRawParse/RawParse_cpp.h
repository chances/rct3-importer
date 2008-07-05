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
#include <wx/filesys.h>
#include <wx/mstream.h>
#include <wx/strconv.h>
#include <wx/tokenzr.h>

#include "base64.h"
#include "counted_array_ptr.h"
#include "RCT3Exception.h"
#include "RCT3Macros.h"
#include "SCNFile.h"
#include "gximage.h"
#include "matrix.h"
#include "pretty.h"
#include "rct3log.h"
#include "xmlhelper.h"
#include "xmldefs.h"


#include "OVLDump.h"

#include "cXmlDoc.h"

#define RAWXML_SECTION "section"
#define RAWXML_CHECK   "check"
#define RAWXML_COPY    "copy"
#define RAWXML_PATCH   "patch"
#define RAWXML_WRITE   "write"
#define RAWXML_DATA    "data"

#define RAWXML_ANR "anr"
#define RAWXML_BAN "ban"
#define RAWXML_BSH "bsh"
#define RAWXML_BTBL "btbl"
#define RAWXML_CED "ced"
#define RAWXML_CHG "chg"
#define RAWXML_CID "cid"
#define RAWXML_FTX "ftx"
#define RAWXML_GSI "gsi"
#define RAWXML_PTD "ptd"
#define RAWXML_QTD "qtd"
#define RAWXML_SAT "sat"
#define RAWXML_SHS "shs"
#define RAWXML_SID "sid"
#define RAWXML_SPL "spl"
#define RAWXML_STA "sta"
#define RAWXML_SVD "svd"
#define RAWXML_TEX "tex"
#define RAWXML_TRR "trr"
#define RAWXML_TXT "txt"
#define RAWXML_WAI "wai"

#define RAWXML_UNKNOWNS "unknowns"

#define RAWXML_BAN_BONE             "banbone"
#define RAWXML_BAN_BONE_TRANSLATION "translation"
#define RAWXML_BAN_BONE_ROTATION    "rotation"

#define RAWXML_BSH_BBOX1            "boundingboxmin"
#define RAWXML_BSH_BBOX2            "boundingboxmax"
#define RAWXML_BSH_MESH             "bshmesh"
#define RAWXML_BSH_MESH_VERTEX2     "vertex2"
#define RAWXML_BSH_MESH_VERTEX2_P   "position"
#define RAWXML_BSH_MESH_VERTEX2_N   "normal"
#define RAWXML_BSH_MESH_VERTEX2_B   "boneassignment"
#define RAWXML_BSH_MESH_TRANSFORM   "transformation"
#define RAWXML_BSH_MESH_TRIANGLE    "triangle"
#define RAWXML_BSH_BONE             "bshbone"
#define RAWXML_BSH_BONE_POS1        "position1"
#define RAWXML_BSH_BONE_POS2        "position2"

#define RAWXML_CED_MORE             "cedmore"

#define RAWXML_CID_SHAPE            "cidshape"
#define RAWXML_CID_MORE             "cidmore"
#define RAWXML_CID_EXTRA            "cidextra"
#define RAWXML_CID_SETTINGS         "cidsettings"
#define RAWXML_CID_TRASH            "cidtrash"
#define RAWXML_CID_SOAKED           "cidsoaked"
#define RAWXML_CID_SIZEUNK          "cidsizeunknowns"
#define RAWXML_CID_UNK              "cidunknowns"

#define RAWXML_FTX_ANIMATION        "ftxanimation"
#define RAWXML_FTX_FRAME            "ftxframe"
#define RAWXML_FTX_FRAME_IMAGE      "image"
#define RAWXML_FTX_FRAME_ALPHA      "alpha"
#define RAWXML_FTX_FRAME_PDATA      "palettedata"
#define RAWXML_FTX_FRAME_TDATA      "texturedata"
#define RAWXML_FTX_FRAME_ADATA      "alphadata"

#define RAWXML_PTD_TEXTURE_A        "texture_a"
#define RAWXML_PTD_TEXTURE_B        "texture_b"
#define RAWXML_PTD_FLAT             "flat"
#define RAWXML_PTD_STRAIGHT_A       "straight_a"
#define RAWXML_PTD_STRAIGHT_B       "straight_b"
#define RAWXML_PTD_TURN_U           "turn_u"
#define RAWXML_PTD_TURN_LA          "turn_la"
#define RAWXML_PTD_TURN_LB          "turn_lb"
#define RAWXML_PTD_TURN_TA          "turn_ta"
#define RAWXML_PTD_TURN_TB          "turn_tb"
#define RAWXML_PTD_TURN_TC          "turn_tc"
#define RAWXML_PTD_TURN_X           "turn_x"
#define RAWXML_PTD_CORNER_A         "corner_a"
#define RAWXML_PTD_CORNER_B         "corner_b"
#define RAWXML_PTD_CORNER_C         "corner_c"
#define RAWXML_PTD_CORNER_D         "corner_d"
#define RAWXML_PTD_SLOPE            "slope"
#define RAWXML_PTD_SLOPESTRAIGHT    "slopestraight"
#define RAWXML_PTD_SLOPESTRAIGHTLEFT        "slopestraightleft"
#define RAWXML_PTD_SLOPESTRAIGHTRIGHT       "slopestraightright"
#define RAWXML_PTD_SLOPEMID         "slopemid"
#define RAWXML_PTD_FLAT_FC          "flat_fc"
#define RAWXML_PTD_SLOPE_FC         "slope_fc"
#define RAWXML_PTD_SLOPE_BC         "slope_bc"
#define RAWXML_PTD_SLOPE_TC         "slope_tc"
#define RAWXML_PTD_SLOPESTRAIGHT_FC "slopestraight_fc"
#define RAWXML_PTD_SLOPESTRAIGHT_BC "slopestraight_bc"
#define RAWXML_PTD_SLOPESTRAIGHT_TC "slopestraight_tc"
#define RAWXML_PTD_SLOPESTRAIGHTLEFT_FC     "slopestraightleft_fc"
#define RAWXML_PTD_SLOPESTRAIGHTLEFT_BC     "slopestraightleft_bc"
#define RAWXML_PTD_SLOPESTRAIGHTLEFT_TC     "slopestraightleft_tc"
#define RAWXML_PTD_SLOPESTRAIGHTRIGHT_FC    "slopestraightright_fc"
#define RAWXML_PTD_SLOPESTRAIGHTRIGHT_BC    "slopestraightright_bc"
#define RAWXML_PTD_SLOPESTRAIGHTRIGHT_TC    "slopestraightright_tc"
#define RAWXML_PTD_SLOPEMID_FC      "slopemid_fc"
#define RAWXML_PTD_SLOPEMID_BC      "slopemid_bc"
#define RAWXML_PTD_SLOPEMID_TC      "slopemid_tc"
#define RAWXML_PTD_PAVING           "paving"

#define RAWXML_QTD_STRAIGHT         "straight"
#define RAWXML_QTD_TURN_L           "turn_l"
#define RAWXML_QTD_TURN_R           "turn_r"
#define RAWXML_QTD_SLOPEUP          "slopeup"
#define RAWXML_QTD_SLOPEDOWN        "slopedown"
#define RAWXML_QTD_SLOPESTRAIGHT    "slopestraight"

#define RAWXML_SHS_BBOX1            "boundingboxmin"
#define RAWXML_SHS_BBOX2            "boundingboxmax"
#define RAWXML_SHS_MESH             "shsmesh"
#define RAWXML_SHS_MESH_VERTEX      "vertex"
#define RAWXML_SHS_MESH_VERTEX_P    "position"
#define RAWXML_SHS_MESH_VERTEX_N    "normal"
#define RAWXML_SHS_MESH_TRANSFORM   "transformation"
#define RAWXML_SHS_MESH_TRIANGLE    "triangle"
#define RAWXML_SHS_EFFECT           "shseffect"
#define RAWXML_SHS_EFFECT_POS       "position"

#define RAWXML_SID_GROUP            "group"
#define RAWXML_SID_TYPE             "type"
#define RAWXML_SID_POSITION         "position"
#define RAWXML_SID_COLOURS          "colours"
//#define RAWXML_SID_SVD              "sidvisual"
#define RAWXML_SID_IMPORTERUNKNOWNS "importerUnknowns"
#define RAWXML_SID_EXTRA            "extra"
#define RAWXML_SID_SQUAREUNKNOWNS   "squareUnknowns"
#define RAWXML_SID_STALLUNKNOWNS    "stallUnknowns"
#define RAWXML_SID_PARAMETER        "parameter"

#define RAWXML_SPL_NODE             "splnode"
#define RAWXML_SPL_LENGTH           "spllength"
#define RAWXML_SPL_DATA             "spldata"

#define RAWXML_STA_ITEM             "staitem"
#define RAWXML_STA_STALLUNKNOWNS    "stastallunknowns"

#define RAWXML_SVD_UNK              "svdunknowns"
#define RAWXML_SVD_LOD              "svdlod"
#define RAWXML_SVD_LOD_ANIMATION    "svdlodanimation"
#define RAWXML_SVD_LOD_UNK          "svdlodunknowns"

#define RAWXML_TEX_TEXTURE          "texture"

#define RAWXML_TRR_SECTION          "trackSection"
#define RAWXML_TRR_SECTION_UNIQUE   "trackSectionRemoveDuplicates"
#define RAWXML_TRR_TRAIN            "train"
#define RAWXML_TRR_PATH             "trackPath"
#define RAWXML_TRR_SPECIALS         "specials"
#define RAWXML_TRR_OPTIONS          "options"
#define RAWXML_TRR_STATION          "station"
#define RAWXML_TRR_LAUNCHED         "launched"
#define RAWXML_TRR_CHAIN            "chain"
#define RAWXML_TRR_CONSTANT         "specials"
#define RAWXML_TRR_COST             "cost"
#define RAWXML_TRR_SPLINES          "splines"
#define RAWXML_TRR_TOWER            "tower"
#define RAWXML_TRR_SOAKED           "soaked"
#define RAWXML_TRR_WILD             "wild"

#define RAWXML_WAI_PARAMETERS       "waiparameters"
#define RAWXML_WAI_UNKNOWNS         "waiunknowns"

#define RAWXML_ATTRACTION           "attraction"
#define RAWXML_ATTRACTION_PATH      "attractionPath"
#define RAWXML_ATTRACTION_MISC      "attractionMisc"
#define RAWXML_ATTRACTION_STATIC_UNK "attractionStaticUnknowns"
#define RAWXML_ATTRACTION_EXTENSION "attractionExtension"

#define RAWXML_RIDE                 "ride"
#define RAWXML_RIDE_OPTION          "rideOption"
#define RAWXML_RIDE_OPTION_PARAMETER "parameter"
#define RAWXML_RIDE_STATIC_UNK      "rideStaticUnknowns"
#define RAWXML_RIDE_STATION_LIMIT   "rideStationLimit"


#define RAWXML_IMPORT    "import"
#define RAWXML_REFERENCE "reference"
#define RAWXML_SYMBOL    "symbol"

#define RAWBAKE_ABSOLUTE "absolute"
#define RAWBAKE_XML      "xml"

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
        node.go_next(); \
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
    if (child.hasProp("useprefix")) { \
        if (child.getPropVal("useprefix", "1") == "0") \
            useprefix = false; \
    }


#define DO_CONDITION_COMMENT(child) \
        if (m_mode != MODE_BAKE) { \
            if (child.hasProp("if")) { \
                bool haveit = true; \
                wxString t = UTF8STRINGWRAP(child.getPropVal("if")); \
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
                    child.go_next(); \
                    continue; \
                } \
            } \
            if (child.hasProp("ifnot")) { \
                bool haveit = true; \
                wxString t = UTF8STRINGWRAP(child.getPropVal("ifnot")); \
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
                    child.go_next(); \
                    continue; \
                } \
            } \
        } \
        if (child.hasProp("comment")) { \
            wxString t = UTF8STRINGWRAP(child.getPropVal("comment")); \
            MakeVariable(t); \
            wxLogMessage(t); \
        }

#define DO_CONDITION_COMMENT_FOR(child) \
        if (m_mode != MODE_BAKE) { \
            if (child.hasProp("if")) { \
                bool haveit = true; \
                wxString t = UTF8STRINGWRAP(child.getPropVal("if")); \
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
                    continue; \
                } \
            } \
            if (child.hasProp("ifnot")) { \
                bool haveit = true; \
                wxString t = UTF8STRINGWRAP(child.getPropVal("ifnot")); \
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
                    continue; \
                } \
            } \
        } \
        if (child.hasProp("comment")) { \
            wxString t = UTF8STRINGWRAP(child.getPropVal("comment")); \
            MakeVariable(t); \
            wxLogMessage(t); \
        }
using namespace r3;
using namespace std;
using namespace xmlcpp;

bool CanBeWrittenTo(const wxFileName& target);
bool EnsureDir(wxFileName& target);
unsigned char ParseDigit(char x);
void BakeScenery(cXmlNode& root, const cSCNFile& scn);

class RCT3InvalidValueException: public RCT3Exception {
public:
    RCT3InvalidValueException(const wxString& message):RCT3Exception(message){};
};

wxString FinishNodeError(const wxString& message, const cXmlNode& node);


