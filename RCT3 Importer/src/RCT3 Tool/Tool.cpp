/*	RCT3 Tool
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 Tool
	The RCT3 File Importer Program is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.

	Small modifications made by Klinn - May 2005 - search for 'by KLN'
	1) Added crude support for recolorable objects:
			- controls added to IDD_CREATESCENERY dialog box
			- 'Recolor' parameter added to SaveOVL function
		 Note that the object texture bitmap's colors will be replaced when recolored in-game:
			- color 0 = not known, avoid use
			- colors 1-85 = re-color #1, shades from light to dark
			- colors 86-170 = re-color #2, light to dark
			- colors 171-255 = re-color #3, light to dark
		 A suitable custom indexed color table should be created and used when converting
		 an RGB-format image to indexed 256-color mode, so that the results can be more
		 easily visualized. Frontier appears to have used shades of blue for 1-85, magenta
		 for 86-170, and yellow for 171-255. Suggest leaving color 0 = white.
	2) Changed Texture Style from an edit box to a combobox.
	3) Added confirmation MessageBox after theme is successfully installed.
	4) Revised the GUISkinItemPos struct, swapped the 'Left' and 'Top' members, and
	swapped the 'Right' and 'Bottom' members, so that game will pick up correct coords.

	Changes by Belgabor:
    - Lots

*/

#include "win_predefine.h"
#include "wx_pch.h"

#pragma warning (disable: 4100)
#include "StyleOVL.h"
#include "OVL.h"
#include "Common.h"

#include "matrix.h"
#include "ASE3DLoader.h"
#include "RCT3Structs.h"
#include "SCNFile.h"

#include "wxdevil.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/xrc/xmlres.h>
//#include <wx/filename.h>

#include <float.h>
#include <commdlg.h>
#include <shlobj.h>
#include <math.h>
#include <list>
#include <string>

#include "global.h"
#include "resource.h"
//#include "scenerydialog.h"
#include "wxdlgCreateScenery.h"
#include "ilhelper.h"
#include "rct3log.h"

#include "wxdlgStallMan.h"
#include "wxdlgAttractMan.h"

#include "wxapp.h"

//////////////////////////////////
//
//  Globals (verified)
//
//////////////////////////////////

IMPLEMENT_APP_NO_MAIN(ToolApp)

//////////////////////////////////
//
//  Globals (???)
//
//////////////////////////////////
#define TITLE_VERSION "RCT3 Object Importer (v16fix4)"



unsigned char *data, *data2;
bool save = false;
unsigned long ShapeSize;
long CurrentTextString = -1;
long CurrentIcon = -1;
long CurrentIconTexture = -1;
long CurrentScenery = -1;
/*
struct FlexiTexture {
    char *Name;
    char *Texture;
    char *Alpha;
    unsigned long Recolorable;
};
struct MeshStruct {
    BOOL disabled;
    char *TXS;
    char *FTX;
    unsigned long place;
    unsigned long flags;
    unsigned long unknown;
    char *Name;
    BOOL effectpoint;
    D3DVECTOR effectpoint_vert;
};
*/
std::vector <Icon *> Icons;
std::vector <IconTexture *> IconTextures;
std::vector <Scenery *> SceneryItems;
//std::vector <EffectPoint *> EffectPoints;
//std::vector <char *> References;
std::vector<cText> cTextStrings;
std::vector <StallStr *> Stalls;
std::vector <AttractionStr *> Attractions;

//std::vector < EffectPoint * >::iterator EffectPointsIterator;
std::vector<cText>::iterator TextStringsIterator;
std::vector < Icon * >::iterator IconsIterator;
std::vector < IconTexture * >::iterator IconTexturesIterator;
std::vector < Scenery * >::iterator SceneryItemsIterator;
char *stdstyle = "Style\\Themed\\";
char themefile[MAX_PATH]="";

//unsigned long local_log2(unsigned long y);
bool InstallTheme(HWND hwnd);
bool ReadIconTexture(HWND hwnd, IconTexture * t);
char *LastCreatedOVL = NULL;


BOOL CALLBACK TextEditDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG:
        if (CurrentTextString != -1) {
            SetDlgItemText(hwnd, IDC_NAME, cTextStrings[CurrentTextString].name.c_str());
            SetDlgItemText(hwnd, IDC_TEXT, cTextStrings[CurrentTextString].text.c_str());
        } else
            EnableWindow(GetDlgItem(hwnd, IDOK), false);
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_COPY_TEXT: {
            unsigned long len, len2;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_NAME));
            len2 = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT));
            if (len == 0) {
                if (len2 == 0) {
                    MessageBox(hwnd, "Nothing to copy =)", "Error", MB_OK | MB_ICONSTOP);
                } else {
                    char *tmp = new char[len2 + 1];
                    GetDlgItemText(hwnd, IDC_TEXT, tmp, len2 + 1);
                    SetDlgItemText(hwnd, IDC_NAME, tmp);
                    delete tmp;
                }
            } else {
                if (len2 == 0) {
                    char *tmp = new char[len + 1];
                    GetDlgItemText(hwnd, IDC_NAME, tmp, len + 1);
                    SetDlgItemText(hwnd, IDC_TEXT, tmp);
                    delete tmp;
                } else {
                    MessageBox(hwnd, "Um, I'm confused.\nEither Name or Text should be empty.",
                               "Error", MB_OK | MB_ICONSTOP);
                }
            }
        }
        break;
        case IDC_NAME:
        case IDC_TEXT: {
            unsigned long len, len2;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_NAME));
            len2 = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT));
            EnableWindow(GetDlgItem(hwnd, IDOK), !((len == 0) && (len2 == 0)));
        }
        break;
        case IDOK:
            unsigned long len, len2;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_NAME));
            len2 = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT));
            if ((len == 0) && (len2 == 0)) {
                MessageBox(hwnd, "You entered nothing. Use \'Cancel\' to end this dialog!", "Error",
                           MB_OK | MB_ICONSTOP);
            } else {
                save = true;
                if (CurrentTextString == -1) {
                    cText t;
                    cTextStrings.push_back(t);
                    CurrentTextString = cTextStrings.size() - 1;
                }
                int id = IDC_NAME;
                if (len == 0) {
                    id = IDC_TEXT;
                    len = len2;
                }
                char *temp;
                temp = new char[((len>len2)?len:len2) + 1];
                GetDlgItemText(hwnd, id, temp, len + 1);
                cTextStrings[CurrentTextString].name = temp;
                GetDlgItemText(hwnd, IDC_TEXT, temp, len2 + 1);
                cTextStrings[CurrentTextString].text = temp;
                delete[] temp;
                EndDialog(hwnd, 1);
            }
            break;
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

bool enabled;
BOOL CALLBACK GSIEditDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG:
        SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
        for (IconTexturesIterator = IconTextures.begin();
                IconTexturesIterator != IconTextures.end(); IconTexturesIterator++) {
            SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                               (LPARAM) (*IconTexturesIterator)->name);
        }
        if (CurrentIcon != -1) {
            SetDlgItemText(hwnd, IDC_NAME, Icons[CurrentIcon]->name);
            SetDlgItemInt(hwnd, IDC_TOP, Icons[CurrentIcon]->top, FALSE);
            SetDlgItemInt(hwnd, IDC_LEFT, Icons[CurrentIcon]->left, FALSE);
            SetDlgItemInt(hwnd, IDC_BOTTOM, Icons[CurrentIcon]->bottom, FALSE);
            SetDlgItemInt(hwnd, IDC_RIGHT, Icons[CurrentIcon]->right, FALSE);
            unsigned long pos = SendDlgItemMessage(hwnd, IDC_LIST, LB_FINDSTRING,
                                                   (WPARAM) - 1,
                                                   (WPARAM) Icons[CurrentIcon]->texture);
            SendDlgItemMessage(hwnd, IDC_LIST, LB_SETCURSEL, pos, 0);
        }
        EnableWindow(GetDlgItem(hwnd, IDOK),
                     SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0) >= 0);
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_TOP:
        case IDC_LEFT:
        case IDC_BOTTOM:
        case IDC_RIGHT:
        case IDC_LIST:
            EnableWindow(GetDlgItem(hwnd, IDOK),
                         (SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0) >= 0) &&
                         GetWindowTextLength(GetDlgItem(hwnd, IDC_NAME)) &&
                         GetWindowTextLength(GetDlgItem(hwnd, IDC_TOP)) &&
                         GetWindowTextLength(GetDlgItem(hwnd, IDC_LEFT)) &&
                         GetWindowTextLength(GetDlgItem(hwnd, IDC_BOTTOM)) &&
                         GetWindowTextLength(GetDlgItem(hwnd, IDC_RIGHT))
                         );
            break;
        case IDOK:
            unsigned long len;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_NAME));
            BOOL fail1, fail2, fail3, fail4;
            unsigned long top, left, bottom, right, texture;
            top = GetDlgItemInt(hwnd, IDC_TOP, &fail1, FALSE);
            left = GetDlgItemInt(hwnd, IDC_LEFT, &fail2, FALSE);
            bottom = GetDlgItemInt(hwnd, IDC_BOTTOM, &fail3, FALSE);
            right = GetDlgItemInt(hwnd, IDC_RIGHT, &fail4, FALSE);
            texture = SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0);
            if ((len != 0) && (fail1 == TRUE) && (fail2 == TRUE)
                    && (fail3 == TRUE) && (fail4 == TRUE) && (texture != -1)) {
                save = true;
                if (CurrentIcon != -1) {
                    delete Icons[CurrentIcon]->name;
                    delete Icons[CurrentIcon]->texture;
                } else {
                    Icon *t = new Icon;
                    Icons.push_back(t);
                    CurrentIcon = Icons.size() - 1;
                }
                Icons[CurrentIcon]->name = new char[len + 1];
                GetDlgItemText(hwnd, IDC_NAME, Icons[CurrentIcon]->name, len + 1);
                Icons[CurrentIcon]->top = top;
                Icons[CurrentIcon]->left = left;
                Icons[CurrentIcon]->bottom = bottom;
                Icons[CurrentIcon]->right = right;
                Icons[CurrentIcon]->texture = new char[strlen(IconTextures[texture]->name) + 1];
                strcpy(Icons[CurrentIcon]->texture, IconTextures[texture]->name);
            }
            EndDialog(hwnd, 1);
            break;
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        case IDC_CALCULATE:
            BOOL fail;
            unsigned long iconnum = GetDlgItemInt(hwnd, IDC_ICONNUM, &fail, FALSE);
            if (fail == TRUE) {
                int x = iconnum / 8;
                int y = iconnum % 8;
                SetDlgItemInt(hwnd, IDC_TOP, y * 64, FALSE);
                SetDlgItemInt(hwnd, IDC_BOTTOM, y * 64 + 63, FALSE);
                SetDlgItemInt(hwnd, IDC_LEFT, x * 64, FALSE);
                SetDlgItemInt(hwnd, IDC_RIGHT, x * 64 + 63, FALSE);
            }
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK TextureEditDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    char sfile[MAX_PATH] = "";
    switch (Message) {
    case WM_INITDIALOG:
        if (CurrentIconTexture != -1) {
            SetDlgItemText(hwnd, IDC_NAME, IconTextures[CurrentIconTexture]->name);
            SetDlgItemText(hwnd, IDC_FILENAME, IconTextures[CurrentIconTexture]->filename);
        }
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_TEXTURE_INFO: {
            char tmp[MAX_PATH];
            GetDlgItemText(hwnd, IDC_FILENAME, tmp, MAX_PATH - 1);
            if (strlen(tmp))
                showBitmapInfo(NULL, tmp);
            else
                MessageBox(hwnd, "Please select a texture before requesting info on it ;-)",
                           "Error", MB_OK | MB_ICONSTOP);
        }
        break;
        case IDC_LOAD:
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter =
                "Textures (*.png;*.bmp;*.jpg;*.tga)\0*.png;*.jpg;*.jpeg;*.bmp;*.tga\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open Icon Texture";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = ".png";
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                SetDlgItemText(hwnd, IDC_FILENAME, sfile);
                unsigned long NameLen = GetWindowTextLength(GetDlgItem(hwnd, IDC_NAME));
                if (!NameLen) {
                    char temp[MAX_PATH];
                    _splitpath(sfile, NULL, NULL, temp, NULL);
                    SetDlgItemText(hwnd, IDC_NAME, temp);
                }
            }
            break;
        case IDOK: {
                unsigned long len, len2;
                len = GetWindowTextLength(GetDlgItem(hwnd, IDC_NAME));
                len2 = GetWindowTextLength(GetDlgItem(hwnd, IDC_FILENAME));
                if ((len != 0) && (len2 != 0)) {
                    char *temp = new char[len2 + 1];
                    GetDlgItemText(hwnd, IDC_FILENAME, temp, len2 + 1);
                    {
                        wxSize s = getBitmapSize(temp);
                        if (s == wxDefaultSize) {
                            ::wxMessageBox(_("There was an error opening the icon texture bitmap."), _("Error"), wxOK|wxICON_ERROR);
                            delete[] temp;
                            break;
                        }
                        if (s.GetWidth() != s.GetHeight()) {
                            ::wxMessageBox(_("The icon texture bitmap is not square."), _("Error"), wxOK|wxICON_ERROR);
                            delete[] temp;
                            break;
                        }
                        if ((1 << local_log2(s.GetWidth())) != s.GetWidth()) {
                            ::wxMessageBox(_("Icon texture's width/height is not a power of 2"), _("Error"), wxOK|wxICON_ERROR);
                            delete[] temp;
                            break;
                        }
                    }
                    save = true;
                    if (CurrentIconTexture != -1) {
                        delete IconTextures[CurrentIconTexture]->name;
                        delete IconTextures[CurrentIconTexture]->filename;
                    } else {
                        IconTexture *t = new IconTexture;
                        IconTextures.push_back(t);
                        CurrentIconTexture = IconTextures.size() - 1;
                    }
                    IconTextures[CurrentIconTexture]->name = new char[len + 1];
                    GetDlgItemText(hwnd, IDC_NAME, IconTextures[CurrentIconTexture]->name, len + 1);
                    IconTextures[CurrentIconTexture]->filename = temp;

                    ReadIconTexture(hwnd, IconTextures[CurrentIconTexture]);

                }
                EndDialog(hwnd, 1);
            }
            break;
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK SIDEditDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    char sfile[MAX_PATH] = "";
    switch (Message) {
    case WM_INITDIALOG:
        // Circumvent Bug in ResEdit
        SetWindowLong(GetDlgItem(hwnd, IDC_SID_SHOW_PATH), GWL_STYLE,
                      GetWindowLong(GetDlgItem(hwnd, IDC_SID_SHOW_PATH),
                                    GWL_STYLE) | SS_PATHELLIPSIS);

        SendDlgItemMessage(hwnd, IDC_SCENICON, CB_RESETCONTENT, 0, 0);
        for (IconsIterator = Icons.begin(); IconsIterator != Icons.end(); IconsIterator++) {
            SendDlgItemMessage(hwnd, IDC_SCENICON, CB_ADDSTRING, 0,
                               (LPARAM) (*IconsIterator)->name);
        }
        SendDlgItemMessage(hwnd, IDC_WALLICON, CB_RESETCONTENT, 0, 0);
        for (IconsIterator = Icons.begin(); IconsIterator != Icons.end(); IconsIterator++) {
            SendDlgItemMessage(hwnd, IDC_WALLICON, CB_ADDSTRING, 0,
                               (LPARAM) (*IconsIterator)->name);
        }
        SendDlgItemMessage(hwnd, IDC_WALLNAME, CB_RESETCONTENT, 0, 0);
        for (TextStringsIterator = cTextStrings.begin();
                TextStringsIterator != cTextStrings.end(); TextStringsIterator++) {
            SendDlgItemMessage(hwnd, IDC_WALLNAME, CB_ADDSTRING, 0,
                               (LPARAM) (TextStringsIterator->name.c_str()));
        }
        SendDlgItemMessage(hwnd, IDC_SCENNAME, CB_RESETCONTENT, 0, 0);
        for (TextStringsIterator = cTextStrings.begin();
                TextStringsIterator != cTextStrings.end(); TextStringsIterator++) {
            SendDlgItemMessage(hwnd, IDC_SCENNAME, CB_ADDSTRING, 0,
                               (LPARAM) (TextStringsIterator->name.c_str()));
        }
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Tree");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Plant");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Shrub");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Flowers");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Fence");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Wall Misc");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Path Lamp");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Small Scenery");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Medium Scenery");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Large Scenery");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Anamatronic Scenery");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Misc Scenery");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Support Middle");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Support Top");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Support Bottom");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Support Bottom Extra");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Support Girder");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Support Cap");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Ride Track");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Path");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Park Entrance");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Litter");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Guest Injection");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Ride");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Ride Entrance");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Ride Exit");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Keep Clear Fence");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Stall");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Ride Event");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Firework");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Litter Bin");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Bench");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Sign");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Photo Point");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Wall Straight");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Wall Roof");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Wall Corner");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Water Cannon (not used)");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Pool Piece");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Pool Extra");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Changing Room");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Laser Dome");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Water Jet");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Terrain Piece");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Particle Effect");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Animal Fence");
        SendDlgItemMessage(hwnd, IDC_TYPE, CB_ADDSTRING, 0, (WPARAM) "Animal Misc");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "None");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Adventure");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Crazy Golf");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Crazy Golf Holes");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Girder");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Ornate");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Asphalt");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Crazy");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Dirt");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Leafy");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Marble");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Tarmac");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Queue Set 1");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Roman Pillar");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Scaffolding");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Space");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Spooky");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Steel");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "TS1");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "TS2");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "TS3");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "TS4");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "TS5");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Wild West");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Aquarium");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Aquarium Corner");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Pool");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Pool Path");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Pool Small");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Sand");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "Underwater");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "TS1A");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "TS6");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "TS7");
        SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_ADDSTRING, 0, (WPARAM) "TS8");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 1");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 2");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 3");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 4");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 5");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 6");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 7");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 8");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 9");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 10");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 11");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 12");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 13");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 14");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 15");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 16");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 17");
        SendDlgItemMessage(hwnd, IDC_UNK4, CB_ADDSTRING, 0, (WPARAM) "unknown 18");
        SendDlgItemMessage(hwnd, IDC_UNK17, CB_ADDSTRING, 0, (WPARAM) "unknown 1");
        SendDlgItemMessage(hwnd, IDC_UNK17, CB_ADDSTRING, 0, (WPARAM) "unknown 2");
        SendDlgItemMessage(hwnd, IDC_UNK17, CB_ADDSTRING, 0, (WPARAM) "unknown 3");
        SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_ADDSTRING, 0, (WPARAM) "Full Tile");
        SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_ADDSTRING, 0, (WPARAM) "Path Edge (Inner)");
        SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_ADDSTRING, 0, (WPARAM) "Path Edge (Outer)");
        SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_ADDSTRING, 0, (WPARAM) "Wall");
        SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_ADDSTRING, 0, (WPARAM) "1/4 Tile");
        SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_ADDSTRING, 0, (WPARAM) "1/2 Tile");
        SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_ADDSTRING, 0, (WPARAM) "Path Center");
        SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_ADDSTRING, 0, (WPARAM) "Corner");
        SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_ADDSTRING, 0, (WPARAM) "Path Edge (Join)");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "None");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Guest Injection Point");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Litter Bin");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Bench");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Park Entrance");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Queue Line Entertainer");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Ride Event");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Sign");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Visa Point");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Waterable");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Path Fountain");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Loudspeaker");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Passport Post");
        SendDlgItemMessage(hwnd, IDC_PARAMS, CB_ADDSTRING, 0, (WPARAM) "Water Cannon");
        if (SceneryItems.size() > 0) {
            for (SceneryItemsIterator = SceneryItems.begin();
                    SceneryItemsIterator != SceneryItems.end(); SceneryItemsIterator++) {
                if (SendDlgItemMessage(hwnd, IDC_LOCATION, CB_FINDSTRING, 0,
                                       (LPARAM) (*SceneryItemsIterator)->location) == CB_ERR)
                    SendDlgItemMessage(hwnd, IDC_LOCATION, CB_ADDSTRING, 0,
                                       (LPARAM) (*SceneryItemsIterator)->location);
            }
        }

        if (CurrentScenery != -1) {
            SetDlgItemText(hwnd, IDC_OVL, SceneryItems[CurrentScenery]->ovl);
            SetDlgItemText(hwnd, IDC_LOCATION, SceneryItems[CurrentScenery]->location);
            SetDlgItemInt(hwnd, IDC_COST, SceneryItems[CurrentScenery]->cost, TRUE);
            SetDlgItemInt(hwnd, IDC_REFUND, SceneryItems[CurrentScenery]->refund, TRUE);
            SetDlgItemInt(hwnd, IDC_XSQUARES, SceneryItems[CurrentScenery]->sizex, FALSE);
            SetDlgItemInt(hwnd, IDC_YSQUARES, SceneryItems[CurrentScenery]->sizey, FALSE);
            SetDlgItemInt(hwnd, IDC_UNK31, SceneryItems[CurrentScenery]->unk31, FALSE);
            SetDlgItemInt(hwnd, IDC_UNK32, SceneryItems[CurrentScenery]->unk32, FALSE);
            SetDlgItemInt(hwnd, IDC_UNK33, SceneryItems[CurrentScenery]->unk33, FALSE);
            CheckDlgButton(hwnd, IDC_FLAG33, SceneryItems[CurrentScenery]->flag33);
            CheckDlgButton(hwnd, IDC_FLAG34, SceneryItems[CurrentScenery]->flag34);
            CheckDlgButton(hwnd, IDC_FLAG35, SceneryItems[CurrentScenery]->flag35);
            CheckDlgButton(hwnd, IDC_FLAG36, SceneryItems[CurrentScenery]->flag36);
            CheckDlgButton(hwnd, IDC_FLAG37, SceneryItems[CurrentScenery]->flag37);
            CheckDlgButton(hwnd, IDC_FLAG38, SceneryItems[CurrentScenery]->flag38);
            CheckDlgButton(hwnd, IDC_FLAG39, SceneryItems[CurrentScenery]->flag39);
            CheckDlgButton(hwnd, IDC_FLAG40, SceneryItems[CurrentScenery]->flag40);
            CheckDlgButton(hwnd, IDC_FLAG41, SceneryItems[CurrentScenery]->flag41);
            CheckDlgButton(hwnd, IDC_FLAG42, SceneryItems[CurrentScenery]->flag42);
            CheckDlgButton(hwnd, IDC_FLAG43, SceneryItems[CurrentScenery]->flag43);
            CheckDlgButton(hwnd, IDC_FLAG44, SceneryItems[CurrentScenery]->flag44);
            CheckDlgButton(hwnd, IDC_FLAG45, SceneryItems[CurrentScenery]->flag45);
            CheckDlgButton(hwnd, IDC_FLAG46, SceneryItems[CurrentScenery]->flag46);
            CheckDlgButton(hwnd, IDC_FLAG47, SceneryItems[CurrentScenery]->flag47);
            CheckDlgButton(hwnd, IDC_FLAG48, SceneryItems[CurrentScenery]->flag48);
            CheckDlgButton(hwnd, IDC_FLAG49, SceneryItems[CurrentScenery]->flag49);
            CheckDlgButton(hwnd, IDC_FLAG50, SceneryItems[CurrentScenery]->flag50);
            CheckDlgButton(hwnd, IDC_FLAG51, SceneryItems[CurrentScenery]->flag51);
            CheckDlgButton(hwnd, IDC_FLAG52, SceneryItems[CurrentScenery]->flag52);
            CheckDlgButton(hwnd, IDC_FLAG53, SceneryItems[CurrentScenery]->flag53);
            CheckDlgButton(hwnd, IDC_FLAG54, SceneryItems[CurrentScenery]->flag54);
            CheckDlgButton(hwnd, IDC_FLAG55, SceneryItems[CurrentScenery]->flag55);
            CheckDlgButton(hwnd, IDC_FLAG56, SceneryItems[CurrentScenery]->flag56);
            CheckDlgButton(hwnd, IDC_FLAG57, SceneryItems[CurrentScenery]->flag57);
            CheckDlgButton(hwnd, IDC_FLAG58, SceneryItems[CurrentScenery]->flag58);
            CheckDlgButton(hwnd, IDC_FLAG59, SceneryItems[CurrentScenery]->flag59);
            CheckDlgButton(hwnd, IDC_FLAG60, SceneryItems[CurrentScenery]->flag60);
            CheckDlgButton(hwnd, IDC_FLAG61, SceneryItems[CurrentScenery]->flag61);
            CheckDlgButton(hwnd, IDC_FLAG62, SceneryItems[CurrentScenery]->flag62);
            CheckDlgButton(hwnd, IDC_FLAG63, SceneryItems[CurrentScenery]->flag63);
            CheckDlgButton(hwnd, IDC_FLAG64, SceneryItems[CurrentScenery]->flag64);
            SetDlgItemInt(hwnd, IDC_DUNK2, SceneryItems[CurrentScenery]->dunk2, TRUE);
            SetDlgItemInt(hwnd, IDC_DUNK3, SceneryItems[CurrentScenery]->dunk3, FALSE);
            SetDlgItemInt(hwnd, IDC_DUNK4, SceneryItems[CurrentScenery]->dunk4, TRUE);
            if (SceneryItems[CurrentScenery]->dunk4 == 0xFFFFFFFF) {
                CheckDlgButton(hwnd, IDC_DDUNK4, true);
                SetDlgItemInt(hwnd, IDC_DUNK4, 0, true);
                EnableWindow(GetDlgItem(hwnd, IDC_DUNK4), false);
            } else {
                CheckDlgButton(hwnd, IDC_DDUNK4, false);
                EnableWindow(GetDlgItem(hwnd, IDC_DUNK4), true);
            }
            SetDlgItemInt(hwnd, IDC_DUNK5, SceneryItems[CurrentScenery]->dunk5, FALSE);
            CheckDlgButton(hwnd, IDC_FLAG1, SceneryItems[CurrentScenery]->flag1);
            CheckDlgButton(hwnd, IDC_FLAG2, SceneryItems[CurrentScenery]->flag2);
            CheckDlgButton(hwnd, IDC_FLAG3, SceneryItems[CurrentScenery]->flag3);
            CheckDlgButton(hwnd, IDC_FLAG4, SceneryItems[CurrentScenery]->flag4);
            CheckDlgButton(hwnd, IDC_FLAG5, SceneryItems[CurrentScenery]->flag5);
            CheckDlgButton(hwnd, IDC_FLAG6, SceneryItems[CurrentScenery]->flag6);
            CheckDlgButton(hwnd, IDC_FLAG7, SceneryItems[CurrentScenery]->flag7);
            CheckDlgButton(hwnd, IDC_FLAG8, SceneryItems[CurrentScenery]->flag8);
            CheckDlgButton(hwnd, IDC_FLAG9, SceneryItems[CurrentScenery]->flag9);
            CheckDlgButton(hwnd, IDC_FLAG10, SceneryItems[CurrentScenery]->flag10);
            CheckDlgButton(hwnd, IDC_FLAG11, SceneryItems[CurrentScenery]->flag11);
            CheckDlgButton(hwnd, IDC_FLAG12, SceneryItems[CurrentScenery]->flag12);
            CheckDlgButton(hwnd, IDC_FLAG13, SceneryItems[CurrentScenery]->flag13);
            CheckDlgButton(hwnd, IDC_FLAG14, SceneryItems[CurrentScenery]->flag14);
            CheckDlgButton(hwnd, IDC_FLAG15, SceneryItems[CurrentScenery]->flag15);
            CheckDlgButton(hwnd, IDC_FLAG16, SceneryItems[CurrentScenery]->flag16);
            CheckDlgButton(hwnd, IDC_FLAG17, SceneryItems[CurrentScenery]->flag17);
            CheckDlgButton(hwnd, IDC_FLAG18, SceneryItems[CurrentScenery]->flag18);
            CheckDlgButton(hwnd, IDC_FLAG19, SceneryItems[CurrentScenery]->flag19);
            CheckDlgButton(hwnd, IDC_FLAG20, SceneryItems[CurrentScenery]->flag20);
            CheckDlgButton(hwnd, IDC_FLAG21, SceneryItems[CurrentScenery]->flag21);
            CheckDlgButton(hwnd, IDC_FLAG22, SceneryItems[CurrentScenery]->flag22);
            CheckDlgButton(hwnd, IDC_FLAG23, SceneryItems[CurrentScenery]->flag23);
            CheckDlgButton(hwnd, IDC_FLAG24, SceneryItems[CurrentScenery]->flag24);
            CheckDlgButton(hwnd, IDC_FLAG25, SceneryItems[CurrentScenery]->flag25);
            CheckDlgButton(hwnd, IDC_FLAG26, SceneryItems[CurrentScenery]->flag26);
            CheckDlgButton(hwnd, IDC_FLAG27, SceneryItems[CurrentScenery]->flag27);
            CheckDlgButton(hwnd, IDC_FLAG28, SceneryItems[CurrentScenery]->flag28);
            CheckDlgButton(hwnd, IDC_FLAG29, SceneryItems[CurrentScenery]->flag29);
            CheckDlgButton(hwnd, IDC_FLAG30, SceneryItems[CurrentScenery]->flag30);
            CheckDlgButton(hwnd, IDC_FLAG31, SceneryItems[CurrentScenery]->flag31);
            CheckDlgButton(hwnd, IDC_FLAG32, SceneryItems[CurrentScenery]->flag32);
            SendDlgItemMessage(hwnd, IDC_TYPE, CB_SETCURSEL, SceneryItems[CurrentScenery]->type, 0);
            SendDlgItemMessage(hwnd, IDC_UNK4, CB_SETCURSEL, SceneryItems[CurrentScenery]->unk4, 0);
            SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_SETCURSEL,
                               SceneryItems[CurrentScenery]->supportstype, 0);
            SendDlgItemMessage(hwnd, IDC_UNK17, CB_SETCURSEL,
                               SceneryItems[CurrentScenery]->unk17, 0);
            SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_SETCURSEL,
                               SceneryItems[CurrentScenery]->sizeflag, 0);
            SendDlgItemMessage(hwnd, IDC_PARAMS, CB_SETCURSEL,
                               SceneryItems[CurrentScenery]->extra, 0);
            unsigned long pos = SendDlgItemMessage(hwnd, IDC_SCENICON, CB_FINDSTRING,
                                                   (WPARAM) - 1,
                                                   (WPARAM) SceneryItems[CurrentScenery]->icon);
            SendDlgItemMessage(hwnd, IDC_SCENICON, CB_SETCURSEL, pos, 0);
            pos =
                SendDlgItemMessage(hwnd, IDC_SCENNAME, CB_FINDSTRING, (WPARAM) - 1,
                                   (WPARAM) SceneryItems[CurrentScenery]->name);
            SendDlgItemMessage(hwnd, IDC_SCENNAME, CB_SETCURSEL, pos, 0);
            if (SceneryItems[CurrentScenery]->wallicon != 0) {
                enabled = true;
                pos = SendDlgItemMessage(hwnd, IDC_WALLNAME, CB_FINDSTRING, (WPARAM) - 1, (WPARAM)
                                         SceneryItems[CurrentScenery]->wallname);
                SendDlgItemMessage(hwnd, IDC_WALLNAME, CB_SETCURSEL, pos, 0);
                pos = SendDlgItemMessage(hwnd, IDC_WALLICON, CB_FINDSTRING, (WPARAM) - 1, (WPARAM)
                                         SceneryItems[CurrentScenery]->wallicon);
                SendDlgItemMessage(hwnd, IDC_WALLICON, CB_SETCURSEL, pos, 0);
            } else {
                enabled = false;
                EnableWindow(GetDlgItem(hwnd, IDC_WALLICON), false);
                EnableWindow(GetDlgItem(hwnd, IDC_WALLNAME), false);
            }
            if (enabled == true) {
                CheckDlgButton(hwnd, IDC_DISABLE, false);
            } else {
                CheckDlgButton(hwnd, IDC_DISABLE, true);
            }
            char temp[100];
            sprintf(temp, "%f", SceneryItems[CurrentScenery]->position.x);
            SetDlgItemText(hwnd, IDC_XPOS, temp);
            sprintf(temp, "%f", SceneryItems[CurrentScenery]->position.y);
            SetDlgItemText(hwnd, IDC_YPOS, temp);
            sprintf(temp, "%f", SceneryItems[CurrentScenery]->position.z);
            SetDlgItemText(hwnd, IDC_ZPOS, temp);
            sprintf(temp, "%f", SceneryItems[CurrentScenery]->size.x);
            SetDlgItemText(hwnd, IDC_XSIZE, temp);
            sprintf(temp, "%f", SceneryItems[CurrentScenery]->size.y);
            SetDlgItemText(hwnd, IDC_YSIZE, temp);
            sprintf(temp, "%f", SceneryItems[CurrentScenery]->size.z);
            SetDlgItemText(hwnd, IDC_ZSIZE, temp);
            sprintf(temp, "Edit Scenery Settings #%ld", CurrentScenery + 1);
            SetWindowText(hwnd, temp);
        } else {
            enabled = true;
            CheckDlgButton(hwnd, IDC_DISABLE, false);
            char temp[100];
            sprintf(temp, "Edit Scenery Settings #%d", SceneryItems.size() + 1);
            SetWindowText(hwnd, temp);
            SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_SETCURSEL, 0, 0);
            SetDlgItemInt(hwnd, IDC_COST, 0, TRUE);
            SetDlgItemInt(hwnd, IDC_REFUND, 0, TRUE);
        }
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_DISABLE:
            if (enabled == true) {
                enabled = false;
                EnableWindow(GetDlgItem(hwnd, IDC_WALLICON), false);
                EnableWindow(GetDlgItem(hwnd, IDC_WALLNAME), false);
            } else {
                enabled = true;
                EnableWindow(GetDlgItem(hwnd, IDC_WALLICON), true);
                EnableWindow(GetDlgItem(hwnd, IDC_WALLNAME), true);
            }
            break;
        case IDC_DDUNK4:
            if (IsDlgButtonChecked(hwnd, IDC_DDUNK4)) {
                EnableWindow(GetDlgItem(hwnd, IDC_DUNK4), false);
            } else {
                EnableWindow(GetDlgItem(hwnd, IDC_DUNK4), true);
            }
            break;
        case IDOK: {
                unsigned long ovllen, loclen, sizex, sizey, wallicon, icon, wallname, name, type,
                unk4, supportstype, unk17;
                long cost, refund;
                unsigned long extra, sizeflag;
                unsigned long posxlen, posylen, poszlen, sizexlen, sizeylen, sizezlen;
                unsigned long unk31, unk32, unk33, dunk3, dunk5;
                long dunk2, dunk4;
                BOOL unk31fail, unk32fail, unk33fail, dunk2fail, dunk3fail, dunk4fail, dunk5fail;
                char *temp;
                BOOL costfail, refundfail, sizexfail, sizeyfail;
                ovllen = GetWindowTextLength(GetDlgItem(hwnd, IDC_OVL));
                loclen = GetWindowTextLength(GetDlgItem(hwnd, IDC_LOCATION));
                posxlen = GetWindowTextLength(GetDlgItem(hwnd, IDC_XPOS));
                posylen = GetWindowTextLength(GetDlgItem(hwnd, IDC_YPOS));
                poszlen = GetWindowTextLength(GetDlgItem(hwnd, IDC_ZPOS));
                sizexlen = GetWindowTextLength(GetDlgItem(hwnd, IDC_XSIZE));
                sizeylen = GetWindowTextLength(GetDlgItem(hwnd, IDC_YSIZE));
                sizezlen = GetWindowTextLength(GetDlgItem(hwnd, IDC_ZSIZE));
                sizex = GetDlgItemInt(hwnd, IDC_XSQUARES, &sizexfail, FALSE);
                sizey = GetDlgItemInt(hwnd, IDC_YSQUARES, &sizeyfail, FALSE);
                refund = GetDlgItemInt(hwnd, IDC_REFUND, &refundfail, TRUE);
                cost = GetDlgItemInt(hwnd, IDC_COST, &costfail, TRUE);
                unk31 = GetDlgItemInt(hwnd, IDC_UNK31, &unk31fail, FALSE);
                unk32 = GetDlgItemInt(hwnd, IDC_UNK32, &unk32fail, FALSE);
                unk33 = GetDlgItemInt(hwnd, IDC_UNK33, &unk33fail, FALSE);
                dunk2 = GetDlgItemInt(hwnd, IDC_DUNK2, &dunk2fail, TRUE);
                dunk3 = GetDlgItemInt(hwnd, IDC_DUNK3, &dunk3fail, FALSE);
                dunk4 = GetDlgItemInt(hwnd, IDC_DUNK4, &dunk4fail, TRUE);
                dunk5 = GetDlgItemInt(hwnd, IDC_DUNK5, &dunk5fail, FALSE);
                if (enabled == true) {
                    wallicon = SendDlgItemMessage(hwnd, IDC_WALLICON, CB_GETCURSEL, 0, 0);
                    wallname = SendDlgItemMessage(hwnd, IDC_WALLNAME, CB_GETCURSEL, 0, 0);
                } else {
                    wallicon = 0;
                    wallname = 0;
                }
                icon = SendDlgItemMessage(hwnd, IDC_SCENICON, CB_GETCURSEL, 0, 0);
                name = SendDlgItemMessage(hwnd, IDC_SCENNAME, CB_GETCURSEL, 0, 0);
                type = SendDlgItemMessage(hwnd, IDC_TYPE, CB_GETCURSEL, 0, 0);
                unk4 = SendDlgItemMessage(hwnd, IDC_UNK4, CB_GETCURSEL, 0, 0);
                supportstype = SendDlgItemMessage(hwnd, IDC_SUPPORTS, CB_GETCURSEL, 0, 0);
                unk17 = SendDlgItemMessage(hwnd, IDC_UNK17, CB_GETCURSEL, 0, 0);
                sizeflag = SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_GETCURSEL, 0, 0);
                extra = SendDlgItemMessage(hwnd, IDC_PARAMS, CB_GETCURSEL, 0, 0);
                if ((ovllen != 0) && (loclen != 0) && (posxlen != 0)
                        && (posylen != 0) && (poszlen != 0) && (sizexlen != 0)
                        && (sizeylen != 0) && (sizezlen != 0)
                        && (unk31fail == TRUE)
                        && (unk32fail == TRUE) && (unk33fail == TRUE)
                        && (dunk2fail == TRUE) && (dunk3fail == TRUE)
                        && (dunk4fail == TRUE) && (dunk5fail == TRUE)
                        && (sizexfail == TRUE) && (sizeyfail == TRUE)
                        && (refundfail == TRUE) && (costfail == TRUE)
                        && (icon != -1)
                        && (name != -1) && (type != -1) && (unk4 != -1)
                        && (supportstype != -1) && (unk17 != -1)
                        && (sizeflag != -1)
                        && (extra != -1)) {
                    if ((enabled == false)
                            || ((enabled == true) && (wallicon != -1)
                                && (wallname != -1))) {
                        temp = new char[loclen + 1];
                        GetDlgItemText(hwnd, IDC_LOCATION, temp, loclen + 1);
                        {
                            wxString test = temp;
                            if (!test.IsAscii()) {
                                ::wxMessageBox(_("The location may only contain AscII characters.\nOther characters (eg. German umlauts, accented characters, ...) can cause problems and are therefore not allowed."), _("Error"), wxOK|wxICON_ERROR);
                                delete[] temp;
                                break;
                            }
                        }

                        save = true;
                        if (CurrentScenery != -1) {
                            delete SceneryItems[CurrentScenery]->name;
                            delete SceneryItems[CurrentScenery]->ovl;
                            delete SceneryItems[CurrentScenery]->location;
                            delete SceneryItems[CurrentScenery]->icon;
                            if (SceneryItems[CurrentScenery]->wallicon != 0) {
                                delete SceneryItems[CurrentScenery]->wallicon;
                                delete SceneryItems[CurrentScenery]->wallname;
                            }
                        } else {
                            Scenery *t = new Scenery;
                            SceneryItems.push_back(t);
                            CurrentScenery = SceneryItems.size() - 1;
                        }
                        Scenery *s = SceneryItems[CurrentScenery];

                        s->location = temp;

                        s->ovl = new char[ovllen + 1];
                        GetDlgItemText(hwnd, IDC_OVL, s->ovl, ovllen + 1);
                        temp = new char[posxlen + 1];
                        GetDlgItemText(hwnd, IDC_XPOS, temp, posxlen + 1);
                        s->position.x = (float) atof(temp);
                        delete temp;
                        temp = new char[posylen + 1];
                        GetDlgItemText(hwnd, IDC_YPOS, temp, posylen + 1);
                        s->position.y = (float) atof(temp);
                        delete temp;
                        temp = new char[poszlen + 1];
                        GetDlgItemText(hwnd, IDC_XPOS, temp, poszlen + 1);
                        s->position.z = (float) atof(temp);
                        delete temp;
                        temp = new char[sizexlen + 1];
                        GetDlgItemText(hwnd, IDC_XSIZE, temp, sizexlen + 1);
                        s->size.x = (float) atof(temp);
                        delete temp;
                        temp = new char[sizeylen + 1];
                        GetDlgItemText(hwnd, IDC_YSIZE, temp, sizeylen + 1);
                        s->size.y = (float) atof(temp);
                        delete temp;
                        temp = new char[sizezlen + 1];
                        GetDlgItemText(hwnd, IDC_ZSIZE, temp, sizezlen + 1);
                        s->size.z = (float) atof(temp);
                        delete temp;
                        s->cost = cost;
                        s->refund = refund;
                        s->sizex = sizex;
                        s->sizey = sizey;
                        s->unk31 = unk31;
                        s->unk32 = unk32;
                        s->unk33 = unk33;
                        s->dunk2 = dunk2;
                        s->dunk3 = dunk3;
                        if (dunk4 == 0xFFFFFFFF) {
                            dunk4 = 0;
                        }
                        if (IsDlgButtonChecked(hwnd, IDC_DDUNK4)) {
                            dunk4 = 0xFFFFFFFF;
                        }
                        s->dunk4 = dunk4;
                        s->dunk5 = dunk5;
                        s->flag1 = IsDlgButtonChecked(hwnd, IDC_FLAG1);
                        s->flag2 = IsDlgButtonChecked(hwnd, IDC_FLAG2);
                        s->flag3 = IsDlgButtonChecked(hwnd, IDC_FLAG3);
                        s->flag4 = IsDlgButtonChecked(hwnd, IDC_FLAG4);
                        s->flag5 = IsDlgButtonChecked(hwnd, IDC_FLAG5);
                        s->flag6 = IsDlgButtonChecked(hwnd, IDC_FLAG6);
                        s->flag7 = IsDlgButtonChecked(hwnd, IDC_FLAG7);
                        s->flag8 = IsDlgButtonChecked(hwnd, IDC_FLAG8);
                        s->flag9 = IsDlgButtonChecked(hwnd, IDC_FLAG9);
                        s->flag10 = IsDlgButtonChecked(hwnd, IDC_FLAG10);
                        s->flag11 = IsDlgButtonChecked(hwnd, IDC_FLAG11);
                        s->flag12 = IsDlgButtonChecked(hwnd, IDC_FLAG12);
                        s->flag13 = IsDlgButtonChecked(hwnd, IDC_FLAG13);
                        s->flag14 = IsDlgButtonChecked(hwnd, IDC_FLAG14);
                        s->flag15 = IsDlgButtonChecked(hwnd, IDC_FLAG15);
                        s->flag16 = IsDlgButtonChecked(hwnd, IDC_FLAG16);
                        s->flag17 = IsDlgButtonChecked(hwnd, IDC_FLAG17);
                        s->flag18 = IsDlgButtonChecked(hwnd, IDC_FLAG18);
                        s->flag19 = IsDlgButtonChecked(hwnd, IDC_FLAG19);
                        s->flag20 = IsDlgButtonChecked(hwnd, IDC_FLAG20);
                        s->flag21 = IsDlgButtonChecked(hwnd, IDC_FLAG21);
                        s->flag22 = IsDlgButtonChecked(hwnd, IDC_FLAG22);
                        s->flag23 = IsDlgButtonChecked(hwnd, IDC_FLAG23);
                        s->flag24 = IsDlgButtonChecked(hwnd, IDC_FLAG24);
                        s->flag25 = IsDlgButtonChecked(hwnd, IDC_FLAG25);
                        s->flag26 = IsDlgButtonChecked(hwnd, IDC_FLAG26);
                        s->flag27 = IsDlgButtonChecked(hwnd, IDC_FLAG27);
                        s->flag28 = IsDlgButtonChecked(hwnd, IDC_FLAG28);
                        s->flag29 = IsDlgButtonChecked(hwnd, IDC_FLAG29);
                        s->flag30 = IsDlgButtonChecked(hwnd, IDC_FLAG30);
                        s->flag31 = IsDlgButtonChecked(hwnd, IDC_FLAG31);
                        s->flag32 = IsDlgButtonChecked(hwnd, IDC_FLAG32);
                        s->flag33 = IsDlgButtonChecked(hwnd, IDC_FLAG33);
                        s->flag34 = IsDlgButtonChecked(hwnd, IDC_FLAG34);
                        s->flag35 = IsDlgButtonChecked(hwnd, IDC_FLAG35);
                        s->flag36 = IsDlgButtonChecked(hwnd, IDC_FLAG36);
                        s->flag37 = IsDlgButtonChecked(hwnd, IDC_FLAG37);
                        s->flag38 = IsDlgButtonChecked(hwnd, IDC_FLAG38);
                        s->flag39 = IsDlgButtonChecked(hwnd, IDC_FLAG39);
                        s->flag40 = IsDlgButtonChecked(hwnd, IDC_FLAG40);
                        s->flag41 = IsDlgButtonChecked(hwnd, IDC_FLAG41);
                        s->flag42 = IsDlgButtonChecked(hwnd, IDC_FLAG42);
                        s->flag43 = IsDlgButtonChecked(hwnd, IDC_FLAG43);
                        s->flag44 = IsDlgButtonChecked(hwnd, IDC_FLAG44);
                        s->flag45 = IsDlgButtonChecked(hwnd, IDC_FLAG45);
                        s->flag46 = IsDlgButtonChecked(hwnd, IDC_FLAG46);
                        s->flag47 = IsDlgButtonChecked(hwnd, IDC_FLAG47);
                        s->flag48 = IsDlgButtonChecked(hwnd, IDC_FLAG48);
                        s->flag49 = IsDlgButtonChecked(hwnd, IDC_FLAG49);
                        s->flag50 = IsDlgButtonChecked(hwnd, IDC_FLAG50);
                        s->flag51 = IsDlgButtonChecked(hwnd, IDC_FLAG51);
                        s->flag52 = IsDlgButtonChecked(hwnd, IDC_FLAG52);
                        s->flag53 = IsDlgButtonChecked(hwnd, IDC_FLAG53);
                        s->flag54 = IsDlgButtonChecked(hwnd, IDC_FLAG54);
                        s->flag55 = IsDlgButtonChecked(hwnd, IDC_FLAG55);
                        s->flag56 = IsDlgButtonChecked(hwnd, IDC_FLAG56);
                        s->flag57 = IsDlgButtonChecked(hwnd, IDC_FLAG57);
                        s->flag58 = IsDlgButtonChecked(hwnd, IDC_FLAG58);
                        s->flag59 = IsDlgButtonChecked(hwnd, IDC_FLAG59);
                        s->flag60 = IsDlgButtonChecked(hwnd, IDC_FLAG60);
                        s->flag61 = IsDlgButtonChecked(hwnd, IDC_FLAG61);
                        s->flag62 = IsDlgButtonChecked(hwnd, IDC_FLAG62);
                        s->flag63 = IsDlgButtonChecked(hwnd, IDC_FLAG63);
                        s->flag64 = IsDlgButtonChecked(hwnd, IDC_FLAG64);
                        s->type = type;
                        s->unk4 = unk4;
                        s->supportstype = supportstype;
                        s->unk17 = unk17;
                        s->sizeflag = sizeflag;
                        s->extra = extra;
                        s->icon = new char[strlen(Icons[icon]->name) + 1];
                        strcpy(s->icon, Icons[icon]->name);
                        s->name = new char[strlen(cTextStrings[name].name.c_str()) + 1];
                        strcpy(s->name, cTextStrings[name].name.c_str());
                        if (enabled == true) {
                            s->wallicon = new char[strlen(Icons[wallicon]->name) + 1];
                            strcpy(s->wallicon, Icons[wallicon]->name);
                            s->wallname = new char[strlen(cTextStrings[wallname].name.c_str()) + 1];
                            strcpy(s->wallname, cTextStrings[wallname].name.c_str());
                        } else {
                            s->wallicon = 0;
                            s->wallname = 0;
                        }
                        EndDialog(hwnd, 1);
                    }
                }
            }
            break;
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        case IDC_SAVESID: {
                unsigned long ovllen, loclen, sizex, sizey, wallicon, icon, wallname, name, type,
                unk4, supportstype, unk17;
                long cost, refund;
                unsigned long extra, sizeflag;
                unsigned long posxlen, posylen, poszlen, sizexlen, sizeylen, sizezlen;
                unsigned long unk31, unk32, unk33, dunk3, dunk5;
                long dunk2, dunk4;
                BOOL unk31fail, unk32fail, unk33fail, dunk2fail, dunk3fail, dunk4fail, dunk5fail;
                char *temp;
                BOOL costfail, refundfail, sizexfail, sizeyfail;

                posxlen = GetWindowTextLength(GetDlgItem(hwnd, IDC_XPOS));
                posylen = GetWindowTextLength(GetDlgItem(hwnd, IDC_YPOS));
                poszlen = GetWindowTextLength(GetDlgItem(hwnd, IDC_ZPOS));
                sizexlen = GetWindowTextLength(GetDlgItem(hwnd, IDC_XSIZE));
                sizeylen = GetWindowTextLength(GetDlgItem(hwnd, IDC_YSIZE));
                sizezlen = GetWindowTextLength(GetDlgItem(hwnd, IDC_ZSIZE));
                sizex = GetDlgItemInt(hwnd, IDC_XSQUARES, &sizexfail, FALSE);
                sizey = GetDlgItemInt(hwnd, IDC_YSQUARES, &sizeyfail, FALSE);
                unk31 = GetDlgItemInt(hwnd, IDC_UNK31, &unk31fail, FALSE);
                unk32 = GetDlgItemInt(hwnd, IDC_UNK32, &unk32fail, FALSE);
                unk33 = GetDlgItemInt(hwnd, IDC_UNK33, &unk33fail, FALSE);
                dunk2 = GetDlgItemInt(hwnd, IDC_DUNK2, &dunk2fail, TRUE);
                dunk3 = GetDlgItemInt(hwnd, IDC_DUNK3, &dunk3fail, FALSE);
                dunk4 = GetDlgItemInt(hwnd, IDC_DUNK4, &dunk4fail, TRUE);
                dunk5 = GetDlgItemInt(hwnd, IDC_DUNK5, &dunk5fail, FALSE);
                type = SendDlgItemMessage(hwnd, IDC_TYPE, CB_GETCURSEL, 0, 0);
                unk4 = SendDlgItemMessage(hwnd, IDC_UNK4, CB_GETCURSEL, 0, 0);
                unk17 = SendDlgItemMessage(hwnd, IDC_UNK17, CB_GETCURSEL, 0, 0);
                sizeflag = SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_GETCURSEL, 0, 0);
                extra = SendDlgItemMessage(hwnd, IDC_PARAMS, CB_GETCURSEL, 0, 0);
                if ((posxlen != 0) && (posylen != 0) && (poszlen != 0)
                        && (sizexlen != 0) && (sizeylen != 0) && (sizezlen != 0)
                        && (unk31fail == TRUE) && (unk32fail == TRUE)
                        && (unk33fail == TRUE) && (dunk2fail == TRUE)
                        && (dunk3fail == TRUE) && (dunk4fail == TRUE)
                        && (dunk5fail == TRUE) && (sizexfail == TRUE)
                        && (sizeyfail == TRUE) && (type != -1) && (unk4 != -1)
                        && (unk17 != -1) && (sizeflag != -1) && (extra != -1)) {
                    OPENFILENAME ofn;
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = "SID Files (*.sid)\0*.sid\0";
                    ofn.lpstrFile = sfile;
                    ofn.lpstrTitle = "Save SID File";
                    ofn.lpstrDefExt = ".sid";
                    ofn.nMaxFile = MAX_PATH;
                    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
                    if (GetSaveFileName(&ofn)) {
                        SetDlgItemText(hwnd, IDC_SID_SHOW_PATH, sfile);
                        SIDData *s = new SIDData;
                        temp = new char[posxlen + 1];
                        GetDlgItemText(hwnd, IDC_XPOS, temp, posxlen + 1);
                        s->position.x = (float) atof(temp);
                        delete temp;
                        temp = new char[posylen + 1];
                        GetDlgItemText(hwnd, IDC_YPOS, temp, posylen + 1);
                        s->position.y = (float) atof(temp);
                        delete temp;
                        temp = new char[poszlen + 1];
                        GetDlgItemText(hwnd, IDC_ZPOS, temp, poszlen + 1);
                        s->position.z = (float) atof(temp);
                        delete temp;
                        temp = new char[sizexlen + 1];
                        GetDlgItemText(hwnd, IDC_XSIZE, temp, sizexlen + 1);
                        s->size.x = (float) atof(temp);
                        delete temp;
                        temp = new char[sizeylen + 1];
                        GetDlgItemText(hwnd, IDC_YSIZE, temp, sizeylen + 1);
                        s->size.y = (float) atof(temp);
                        delete temp;
                        temp = new char[sizezlen + 1];
                        GetDlgItemText(hwnd, IDC_ZSIZE, temp, sizezlen + 1);
                        s->size.z = (float) atof(temp);
                        delete temp;
                        s->sizex = sizex;
                        s->sizey = sizey;
                        s->unk31 = unk31;
                        s->unk32 = unk32;
                        s->unk33 = unk33;
                        s->dunk2 = dunk2;
                        s->dunk3 = dunk3;
                        if (dunk4 == 0xFFFFFFFF) {
                            dunk4 = 0;
                        }
                        if (IsDlgButtonChecked(hwnd, IDC_DDUNK4)) {
                            dunk4 = 0xFFFFFFFF;
                        }
                        s->dunk4 = dunk4;
                        s->dunk5 = dunk5;
                        s->flag1 = IsDlgButtonChecked(hwnd, IDC_FLAG1);
                        s->flag2 = IsDlgButtonChecked(hwnd, IDC_FLAG2);
                        s->flag3 = IsDlgButtonChecked(hwnd, IDC_FLAG3);
                        s->flag4 = IsDlgButtonChecked(hwnd, IDC_FLAG4);
                        s->flag5 = IsDlgButtonChecked(hwnd, IDC_FLAG5);
                        s->flag6 = IsDlgButtonChecked(hwnd, IDC_FLAG6);
                        s->flag7 = IsDlgButtonChecked(hwnd, IDC_FLAG7);
                        s->flag8 = IsDlgButtonChecked(hwnd, IDC_FLAG8);
                        s->flag9 = IsDlgButtonChecked(hwnd, IDC_FLAG9);
                        s->flag10 = IsDlgButtonChecked(hwnd, IDC_FLAG10);
                        s->flag11 = IsDlgButtonChecked(hwnd, IDC_FLAG11);
                        s->flag12 = IsDlgButtonChecked(hwnd, IDC_FLAG12);
                        s->flag13 = IsDlgButtonChecked(hwnd, IDC_FLAG13);
                        s->flag14 = IsDlgButtonChecked(hwnd, IDC_FLAG14);
                        s->flag15 = IsDlgButtonChecked(hwnd, IDC_FLAG15);
                        s->flag16 = IsDlgButtonChecked(hwnd, IDC_FLAG16);
                        s->flag17 = IsDlgButtonChecked(hwnd, IDC_FLAG17);
                        s->flag18 = IsDlgButtonChecked(hwnd, IDC_FLAG18);
                        s->flag19 = IsDlgButtonChecked(hwnd, IDC_FLAG19);
                        s->flag20 = IsDlgButtonChecked(hwnd, IDC_FLAG20);
                        s->flag21 = IsDlgButtonChecked(hwnd, IDC_FLAG21);
                        s->flag22 = IsDlgButtonChecked(hwnd, IDC_FLAG22);
                        s->flag23 = IsDlgButtonChecked(hwnd, IDC_FLAG23);
                        s->flag24 = IsDlgButtonChecked(hwnd, IDC_FLAG24);
                        s->flag25 = IsDlgButtonChecked(hwnd, IDC_FLAG25);
                        s->flag26 = IsDlgButtonChecked(hwnd, IDC_FLAG26);
                        s->flag27 = IsDlgButtonChecked(hwnd, IDC_FLAG27);
                        s->flag28 = IsDlgButtonChecked(hwnd, IDC_FLAG28);
                        s->flag29 = IsDlgButtonChecked(hwnd, IDC_FLAG29);
                        s->flag30 = IsDlgButtonChecked(hwnd, IDC_FLAG30);
                        s->flag31 = IsDlgButtonChecked(hwnd, IDC_FLAG31);
                        s->flag32 = IsDlgButtonChecked(hwnd, IDC_FLAG32);
                        s->flag33 = IsDlgButtonChecked(hwnd, IDC_FLAG33);
                        s->flag34 = IsDlgButtonChecked(hwnd, IDC_FLAG34);
                        s->flag35 = IsDlgButtonChecked(hwnd, IDC_FLAG35);
                        s->flag36 = IsDlgButtonChecked(hwnd, IDC_FLAG36);
                        s->flag37 = IsDlgButtonChecked(hwnd, IDC_FLAG37);
                        s->flag38 = IsDlgButtonChecked(hwnd, IDC_FLAG38);
                        s->flag39 = IsDlgButtonChecked(hwnd, IDC_FLAG39);
                        s->flag40 = IsDlgButtonChecked(hwnd, IDC_FLAG40);
                        s->flag41 = IsDlgButtonChecked(hwnd, IDC_FLAG41);
                        s->flag42 = IsDlgButtonChecked(hwnd, IDC_FLAG42);
                        s->flag43 = IsDlgButtonChecked(hwnd, IDC_FLAG43);
                        s->flag44 = IsDlgButtonChecked(hwnd, IDC_FLAG44);
                        s->flag45 = IsDlgButtonChecked(hwnd, IDC_FLAG45);
                        s->flag46 = IsDlgButtonChecked(hwnd, IDC_FLAG46);
                        s->flag47 = IsDlgButtonChecked(hwnd, IDC_FLAG47);
                        s->flag48 = IsDlgButtonChecked(hwnd, IDC_FLAG48);
                        s->flag49 = IsDlgButtonChecked(hwnd, IDC_FLAG49);
                        s->flag50 = IsDlgButtonChecked(hwnd, IDC_FLAG50);
                        s->flag51 = IsDlgButtonChecked(hwnd, IDC_FLAG51);
                        s->flag52 = IsDlgButtonChecked(hwnd, IDC_FLAG52);
                        s->flag53 = IsDlgButtonChecked(hwnd, IDC_FLAG53);
                        s->flag54 = IsDlgButtonChecked(hwnd, IDC_FLAG54);
                        s->flag55 = IsDlgButtonChecked(hwnd, IDC_FLAG55);
                        s->flag56 = IsDlgButtonChecked(hwnd, IDC_FLAG56);
                        s->flag57 = IsDlgButtonChecked(hwnd, IDC_FLAG57);
                        s->flag58 = IsDlgButtonChecked(hwnd, IDC_FLAG58);
                        s->flag59 = IsDlgButtonChecked(hwnd, IDC_FLAG59);
                        s->flag60 = IsDlgButtonChecked(hwnd, IDC_FLAG60);
                        s->flag61 = IsDlgButtonChecked(hwnd, IDC_FLAG61);
                        s->flag62 = IsDlgButtonChecked(hwnd, IDC_FLAG62);
                        s->flag63 = IsDlgButtonChecked(hwnd, IDC_FLAG63);
                        s->flag64 = IsDlgButtonChecked(hwnd, IDC_FLAG64);
                        s->type = type;
                        s->unk4 = unk4;
                        s->unk17 = unk17;
                        s->sizeflag = sizeflag;
                        s->extra = extra;
                        FILE *f = fopen(sfile, "wb");
                        fwrite(s, sizeof(SIDData), 1, f);
                        fclose(f);
                    }
                }
            }
            break;
        case IDC_OPENSID:
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "SID Files (*.sid)\0*.sid\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open SID File";
            ofn.lpstrDefExt = ".sid";
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                SIDData *s = new SIDData;
                FILE *f = fopen(sfile, "rb");
                SetDlgItemText(hwnd, IDC_SID_SHOW_PATH, sfile);
                fread(s, sizeof(SIDData), 1, f);
                fclose(f);
                SetDlgItemInt(hwnd, IDC_XSQUARES, s->sizex, FALSE);
                SetDlgItemInt(hwnd, IDC_YSQUARES, s->sizey, FALSE);
                SetDlgItemInt(hwnd, IDC_UNK31, s->unk31, FALSE);
                SetDlgItemInt(hwnd, IDC_UNK32, s->unk32, FALSE);
                SetDlgItemInt(hwnd, IDC_UNK33, s->unk33, FALSE);
                CheckDlgButton(hwnd, IDC_FLAG33, s->flag33);
                CheckDlgButton(hwnd, IDC_FLAG34, s->flag34);
                CheckDlgButton(hwnd, IDC_FLAG35, s->flag35);
                CheckDlgButton(hwnd, IDC_FLAG36, s->flag36);
                CheckDlgButton(hwnd, IDC_FLAG37, s->flag37);
                CheckDlgButton(hwnd, IDC_FLAG38, s->flag38);
                CheckDlgButton(hwnd, IDC_FLAG39, s->flag39);
                CheckDlgButton(hwnd, IDC_FLAG40, s->flag40);
                CheckDlgButton(hwnd, IDC_FLAG41, s->flag41);
                CheckDlgButton(hwnd, IDC_FLAG42, s->flag42);
                CheckDlgButton(hwnd, IDC_FLAG43, s->flag43);
                CheckDlgButton(hwnd, IDC_FLAG44, s->flag44);
                CheckDlgButton(hwnd, IDC_FLAG45, s->flag45);
                CheckDlgButton(hwnd, IDC_FLAG46, s->flag46);
                CheckDlgButton(hwnd, IDC_FLAG47, s->flag47);
                CheckDlgButton(hwnd, IDC_FLAG48, s->flag48);
                CheckDlgButton(hwnd, IDC_FLAG49, s->flag49);
                CheckDlgButton(hwnd, IDC_FLAG50, s->flag50);
                CheckDlgButton(hwnd, IDC_FLAG51, s->flag51);
                CheckDlgButton(hwnd, IDC_FLAG52, s->flag52);
                CheckDlgButton(hwnd, IDC_FLAG53, s->flag53);
                CheckDlgButton(hwnd, IDC_FLAG54, s->flag54);
                CheckDlgButton(hwnd, IDC_FLAG55, s->flag55);
                CheckDlgButton(hwnd, IDC_FLAG56, s->flag56);
                CheckDlgButton(hwnd, IDC_FLAG57, s->flag57);
                CheckDlgButton(hwnd, IDC_FLAG58, s->flag58);
                CheckDlgButton(hwnd, IDC_FLAG59, s->flag59);
                CheckDlgButton(hwnd, IDC_FLAG60, s->flag60);
                CheckDlgButton(hwnd, IDC_FLAG61, s->flag61);
                CheckDlgButton(hwnd, IDC_FLAG62, s->flag62);
                CheckDlgButton(hwnd, IDC_FLAG63, s->flag63);
                CheckDlgButton(hwnd, IDC_FLAG64, s->flag64);
                SetDlgItemInt(hwnd, IDC_DUNK2, s->dunk2, TRUE);
                SetDlgItemInt(hwnd, IDC_DUNK3, s->dunk3, FALSE);
                SetDlgItemInt(hwnd, IDC_DUNK4, s->dunk4, TRUE);
                if (s->dunk4 == 0xFFFFFFFF) {
                    CheckDlgButton(hwnd, IDC_DDUNK4, true);
                    SetDlgItemInt(hwnd, IDC_DUNK4, 0, true);
                    EnableWindow(GetDlgItem(hwnd, IDC_DUNK4), false);
                } else {
                    CheckDlgButton(hwnd, IDC_DDUNK4, false);
                    EnableWindow(GetDlgItem(hwnd, IDC_DUNK4), true);
                }
                SetDlgItemInt(hwnd, IDC_DUNK5, s->dunk5, FALSE);
                CheckDlgButton(hwnd, IDC_FLAG1, s->flag1);
                CheckDlgButton(hwnd, IDC_FLAG2, s->flag2);
                CheckDlgButton(hwnd, IDC_FLAG3, s->flag3);
                CheckDlgButton(hwnd, IDC_FLAG4, s->flag4);
                CheckDlgButton(hwnd, IDC_FLAG5, s->flag5);
                CheckDlgButton(hwnd, IDC_FLAG6, s->flag6);
                CheckDlgButton(hwnd, IDC_FLAG7, s->flag7);
                CheckDlgButton(hwnd, IDC_FLAG8, s->flag8);
                CheckDlgButton(hwnd, IDC_FLAG9, s->flag9);
                CheckDlgButton(hwnd, IDC_FLAG10, s->flag10);
                CheckDlgButton(hwnd, IDC_FLAG11, s->flag11);
                CheckDlgButton(hwnd, IDC_FLAG12, s->flag12);
                CheckDlgButton(hwnd, IDC_FLAG13, s->flag13);
                CheckDlgButton(hwnd, IDC_FLAG14, s->flag14);
                CheckDlgButton(hwnd, IDC_FLAG15, s->flag15);
                CheckDlgButton(hwnd, IDC_FLAG16, s->flag16);
                CheckDlgButton(hwnd, IDC_FLAG17, s->flag17);
                CheckDlgButton(hwnd, IDC_FLAG18, s->flag18);
                CheckDlgButton(hwnd, IDC_FLAG19, s->flag19);
                CheckDlgButton(hwnd, IDC_FLAG20, s->flag20);
                CheckDlgButton(hwnd, IDC_FLAG21, s->flag21);
                CheckDlgButton(hwnd, IDC_FLAG22, s->flag22);
                CheckDlgButton(hwnd, IDC_FLAG23, s->flag23);
                CheckDlgButton(hwnd, IDC_FLAG24, s->flag24);
                CheckDlgButton(hwnd, IDC_FLAG25, s->flag25);
                CheckDlgButton(hwnd, IDC_FLAG26, s->flag26);
                CheckDlgButton(hwnd, IDC_FLAG27, s->flag27);
                CheckDlgButton(hwnd, IDC_FLAG28, s->flag28);
                CheckDlgButton(hwnd, IDC_FLAG29, s->flag29);
                CheckDlgButton(hwnd, IDC_FLAG30, s->flag30);
                CheckDlgButton(hwnd, IDC_FLAG31, s->flag31);
                CheckDlgButton(hwnd, IDC_FLAG32, s->flag32);
                SendDlgItemMessage(hwnd, IDC_TYPE, CB_SETCURSEL, s->type, 0);
                SendDlgItemMessage(hwnd, IDC_UNK4, CB_SETCURSEL, s->unk4, 0);
                SendDlgItemMessage(hwnd, IDC_UNK17, CB_SETCURSEL, s->unk17, 0);
                s->sizeflag &= 0x0000FFFF;
                SendDlgItemMessage(hwnd, IDC_SIZEFLAG, CB_SETCURSEL, s->sizeflag, 0);
                SendDlgItemMessage(hwnd, IDC_PARAMS, CB_SETCURSEL, s->extra, 0);
                char temp[100];
                sprintf(temp, "%f", s->position.x);
                SetDlgItemText(hwnd, IDC_XPOS, temp);
                sprintf(temp, "%f", s->position.y);
                SetDlgItemText(hwnd, IDC_YPOS, temp);
                sprintf(temp, "%f", s->position.z);
                SetDlgItemText(hwnd, IDC_ZPOS, temp);
                sprintf(temp, "%f", s->size.x);
                SetDlgItemText(hwnd, IDC_XSIZE, temp);
                sprintf(temp, "%f", s->size.y);
                SetDlgItemText(hwnd, IDC_YSIZE, temp);
                sprintf(temp, "%f", s->size.z);
                SetDlgItemText(hwnd, IDC_ZSIZE, temp);
            }
            break;
        case IDC_SELECT:
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Overlay Files (*.common.ovl)\0*.common.ovl\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open OVL File";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = ".common.ovl";
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                SetDlgItemText(hwnd, IDC_OVL, sfile);
            }
            break;
        case ID_EDIT_CREATESCENERY: {
            char *tmp = new char[MAX_PATH];
            GetDlgItemText(GetParent(hwnd), IDC_THEMEPREFIX, tmp, MAX_PATH);
            wxString prefix = tmp;
            GetDlgItemText(GetParent(hwnd), IDC_THEMENAME, tmp, MAX_PATH);
            wxString theme = tmp;
            delete[] tmp;
            wxWindow *tempwind = new wxWindow();
            WXHWND t = tempwind->GetHWND();
            tempwind->SetHWND(hwnd);
            dlgCreateScenery *dialog = new dlgCreateScenery(tempwind);
            dialog->SetDefaultRef(prefix, theme);
            dialog->ShowModal();
            if (dialog->GetOVLPath() != wxT("")) {
                if (::wxMessageBox(_("Do you want to use the created OVL for this scenery item?"), _("Question"), wxYES_NO | wxICON_QUESTION) == wxYES) {
                    SetDlgItemText(hwnd,IDC_OVL,dialog->GetOVLPath().fn_str());
                }
            }
            dialog->Destroy();
            tempwind->SetHWND(t);
            tempwind->Destroy();
        }
            //DialogBox(hInst, MAKEINTRESOURCE(IDD_SCENERY), hwnd, SceneryDlgProc);
            /*int res = DialogBox(hInst, MAKEINTRESOURCE(IDD_CREATESCENERY), hwnd, CreateSceneryDlgProc);
               if (res==1) //ok is pressed
               {
               SetDlgItemText(hwnd,IDC_OVL,LastCreatedOVL);
               delete LastCreatedOVL;
               LastCreatedOVL=NULL;
               } */

            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK SIDManDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG:
        SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
        for (SceneryItemsIterator = SceneryItems.begin();
                SceneryItemsIterator != SceneryItems.end(); SceneryItemsIterator++) {
            char name[MAX_PATH];
            _splitpath((*SceneryItemsIterator)->ovl, NULL, NULL, name, NULL);
            strchr(name, '.')[0] = 0;
            SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                               (LPARAM) name);
        }
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_ADD:
            CurrentScenery = -1;
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_SIDEDIT), hwnd, SIDEditDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                for (SceneryItemsIterator = SceneryItems.begin();
                        SceneryItemsIterator != SceneryItems.end(); SceneryItemsIterator++) {
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                       (LPARAM) (*SceneryItemsIterator)->name);
                }
            }
            break;
        case IDC_LIST:
            if (((int) HIWORD(wParam)) != LBN_DBLCLK)
                break;
            // fall through if double click
        case IDC_EDIT:
            CurrentScenery = SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0);
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_SIDEDIT), hwnd, SIDEditDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                for (SceneryItemsIterator = SceneryItems.begin();
                        SceneryItemsIterator != SceneryItems.end(); SceneryItemsIterator++) {
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                       (LPARAM) (*SceneryItemsIterator)->name);
                }
            }
            break;
        case IDC_DEL: {
                CurrentScenery = SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0);
                if ((CurrentScenery < 0) || (CurrentScenery >= SceneryItems.size()))
                    break;
                if (MessageBox
                        (hwnd, "Are you sure you want to delete?", "Are You Sure",
                         MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    save = true;
                    delete SceneryItems[CurrentScenery]->name;
                    delete SceneryItems[CurrentScenery]->ovl;
                    delete SceneryItems[CurrentScenery]->location;
                    delete SceneryItems[CurrentScenery]->icon;
                    if (SceneryItems[CurrentScenery]->wallicon != 0) {
                        delete SceneryItems[CurrentScenery]->wallicon;
                        delete SceneryItems[CurrentScenery]->wallname;
                    }
                    delete SceneryItems[CurrentScenery];
                    SceneryItems.erase(SceneryItems.begin() + CurrentScenery);
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                    for (SceneryItemsIterator = SceneryItems.begin();
                            SceneryItemsIterator != SceneryItems.end(); SceneryItemsIterator++) {
                        SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                           (LPARAM) (*SceneryItemsIterator)->name);
                    }
                }
            }
            break;
        case IDC_CLOSE:
            EndDialog(hwnd, 0);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK TextureManDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG:
        SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
        for (IconTexturesIterator = IconTextures.begin();
                IconTexturesIterator != IconTextures.end(); IconTexturesIterator++) {
            SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                               (LPARAM) (*IconTexturesIterator)->name);
        }
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_ADD:
            CurrentIconTexture = -1;
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_TEXTUREEDIT), hwnd, TextureEditDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                for (IconTexturesIterator = IconTextures.begin();
                        IconTexturesIterator != IconTextures.end(); IconTexturesIterator++) {
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                       (LPARAM) (*IconTexturesIterator)->name);
                }
            }
            break;
        case IDC_LIST:
            if (((int) HIWORD(wParam)) != LBN_DBLCLK)
                break;
            // fall through if double click
        case IDC_EDIT:
            CurrentIconTexture = SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0);
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_TEXTUREEDIT), hwnd, TextureEditDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                for (IconTexturesIterator = IconTextures.begin();
                        IconTexturesIterator != IconTextures.end(); IconTexturesIterator++) {
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                       (LPARAM) (*IconTexturesIterator)->name);
                }
            }
            break;
        case IDC_DEL: {
                CurrentIconTexture = SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0);
                if ((CurrentIconTexture < 0) || (CurrentIconTexture >= IconTextures.size()))
                    break;
                if (MessageBox
                        (hwnd, "Are you sure you want to delete?", "Are You Sure",
                         MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    save = true;
                    delete IconTextures[CurrentIconTexture]->filename;
                    delete IconTextures[CurrentIconTexture]->name;
                    delete IconTextures[CurrentIconTexture];
                    IconTextures.erase(IconTextures.begin() + CurrentIconTexture);
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                    for (IconTexturesIterator = IconTextures.begin();
                            IconTexturesIterator != IconTextures.end(); IconTexturesIterator++) {
                        SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                           (LPARAM) (*IconTexturesIterator)->name);
                    }
                }
            }
            break;
        case IDC_CLOSE:
            EndDialog(hwnd, 0);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK GSIAutoDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG:
        SendDlgItemMessage(hwnd, IDC_GSIAUTOLIST, LB_RESETCONTENT, 0, 0);
        for (IconTexturesIterator = IconTextures.begin();
                IconTexturesIterator != IconTextures.end(); IconTexturesIterator++) {
            SendDlgItemMessage(hwnd, IDC_GSIAUTOLIST, LB_ADDSTRING, 0, (LPARAM) (*IconTexturesIterator)->name);
        }
        SendDlgItemMessage(hwnd, IDC_GSIAUTOLIST, LB_SETSEL, 1, -1);
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            char temp[512];
            int i = 0;
            for (IconTexturesIterator = IconTextures.begin();
                    IconTexturesIterator != IconTextures.end(); IconTexturesIterator++) {
                if (SendDlgItemMessage(hwnd, IDC_GSIAUTOLIST, LB_GETSEL, i, 0)) {
                    int n = (*IconTexturesIterator)->fh.Width / 64;
                    for (int y = 0; y < n; y++)
                        for (int x = 0; x < n; x++) {
                            snprintf(temp, 511, "%s Nr%03d x%02d/y%02d",
                                     (*IconTexturesIterator)->name, (x + (y * n)) + 1, x + 1,
                                     y + 1);

                            Icon *t = new Icon;
                            Icons.push_back(t);
                            CurrentIcon = Icons.size() - 1;
                            Icons[CurrentIcon]->name = new char[strlen(temp) + 1];
                            strcpy(Icons[CurrentIcon]->name, temp);
                            Icons[CurrentIcon]->top = (y * 64);
                            Icons[CurrentIcon]->left = (x * 64);
                            Icons[CurrentIcon]->bottom = ((y + 1) * 64) - 1;
                            Icons[CurrentIcon]->right = ((x + 1) * 64) - 1;
                            Icons[CurrentIcon]->texture =
                                new char[strlen((*IconTexturesIterator)->name) + 1];
                            strcpy(Icons[CurrentIcon]->texture, (*IconTexturesIterator)->name);
                        }
                }
                i++;
            }
            EndDialog(hwnd, 1);
        }
        break;
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        }
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK GSIManDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG:
        SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
        for (IconsIterator = Icons.begin(); IconsIterator != Icons.end(); IconsIterator++) {
            SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM) (*IconsIterator)->name);
        }
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_ADD:
            CurrentIcon = -1;
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_GSIEDIT), hwnd, GSIEditDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                for (IconsIterator = Icons.begin(); IconsIterator != Icons.end(); IconsIterator++) {
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                       (LPARAM) (*IconsIterator)->name);
                }
            }
            break;
        case IDC_AUTO_ICONS: {
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_GSIAUTO), hwnd, GSIAutoDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                for (IconsIterator = Icons.begin(); IconsIterator != Icons.end(); IconsIterator++) {
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                       (LPARAM) (*IconsIterator)->name);
                }
            }
        }
        break;
        case IDC_LIST:
            if (((int) HIWORD(wParam)) != LBN_DBLCLK)
                break;
            // fall through if double click
        case IDC_EDIT:
            CurrentIcon = SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0);
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_GSIEDIT), hwnd, GSIEditDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                for (IconsIterator = Icons.begin(); IconsIterator != Icons.end(); IconsIterator++) {
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                       (LPARAM) (*IconsIterator)->name);
                }
            }
            break;
        case IDC_DEL: {
                CurrentIcon = SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0);
                if ((CurrentIcon < 0) || (CurrentIcon >= Icons.size()))
                    break;
                if (MessageBox
                        (hwnd, "Are you sure you want to delete?", "Are You Sure",
                         MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    save = true;
                    delete Icons[CurrentIcon]->name;
                    delete Icons[CurrentIcon]->texture;
                    delete Icons[CurrentIcon];
                    Icons.erase(Icons.begin() + CurrentIcon);
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                    for (IconsIterator = Icons.begin(); IconsIterator != Icons.end(); IconsIterator++) {
                        SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                           (LPARAM) (*IconsIterator)->name);
                    }
                }
            }
            break;
        case IDC_CLOSE:
            EndDialog(hwnd, 0);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK TextManDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG:
        SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
        for (TextStringsIterator = cTextStrings.begin();
                TextStringsIterator != cTextStrings.end(); TextStringsIterator++) {
            SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                               (LPARAM) (TextStringsIterator->name.c_str()));
        }
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_ADD:
            CurrentTextString = -1;
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_TEXTEDIT), hwnd, TextEditDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                for (TextStringsIterator = cTextStrings.begin();
                        TextStringsIterator != cTextStrings.end(); TextStringsIterator++) {
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                       (LPARAM) (TextStringsIterator->name.c_str()));
                }
            }
            break;
        case IDC_LIST:
            if (((int) HIWORD(wParam)) != LBN_DBLCLK)
                break;
            // fall through if double click
        case IDC_EDIT:
            CurrentTextString = SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0);
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_TEXTEDIT), hwnd, TextEditDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                for (TextStringsIterator = cTextStrings.begin();
                        TextStringsIterator != cTextStrings.end(); TextStringsIterator++) {
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                       (LPARAM) (TextStringsIterator->name.c_str()));
                }
            }
            break;
        case IDC_DEL: {
                CurrentTextString = SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0);
                if ((CurrentTextString < 0) || (CurrentTextString >= cTextStrings.size()))
                    break;
                if (MessageBox
                        (hwnd, "Are you sure you want to delete?", "Are You Sure",
                         MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    save = true;
                    cTextStrings.erase(cTextStrings.begin() + CurrentTextString);
                    SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
                    for (TextStringsIterator = cTextStrings.begin();
                            TextStringsIterator != cTextStrings.end(); TextStringsIterator++) {
                        SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0,
                                           (LPARAM) (TextStringsIterator->name.c_str()));
                    }
                }
            }
            break;
        case IDC_CLOSE:
            EndDialog(hwnd, 0);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK MainDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
/*
    OPENFILENAME ofn;
    char sfile[MAX_PATH] = "";
    unsigned long i;
*/
    switch (Message) {
    case WM_INITDIALOG:
        save = false;
        SetMenu(hwnd, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)));
        SetWindowText(hwnd, TITLE_VERSION);
        SendDlgItemMessage(hwnd, IDC_THEME, LB_ADDSTRING, 0, (LPARAM) "Generic");
        SendDlgItemMessage(hwnd, IDC_THEME, LB_ADDSTRING, 0, (LPARAM) "Western");
        SendDlgItemMessage(hwnd, IDC_THEME, LB_ADDSTRING, 0, (LPARAM) "Spooky");
        SendDlgItemMessage(hwnd, IDC_THEME, LB_ADDSTRING, 0, (LPARAM) "Sci-fi");
        SendDlgItemMessage(hwnd, IDC_THEME, LB_ADDSTRING, 0, (LPARAM) "Adventure");
        SendDlgItemMessage(hwnd, IDC_THEME, LB_ADDSTRING, 0, (LPARAM) "$Atlantis");
        SendDlgItemMessage(hwnd, IDC_THEME, LB_ADDSTRING, 0, (LPARAM) "$Paradise Island");
        SendDlgItemMessage(hwnd, IDC_THEME, LB_ADDSTRING, 0, (LPARAM) "#Safari");
        SendDlgItemMessage(hwnd, IDC_THEME, LB_ADDSTRING, 0, (LPARAM) "#Prehistoric");
        SetDlgItemText(hwnd, IDC_NOTES,
                       "Notes about RCT3 Soaked! and RCT3 Wild! expansion packs\r\nItems marked with a # will only work with the Wild! addon.\r\nItems marked with a $ will only work with the Soaked! addon.\r\nAll features assume the most recent patch is installed (either the\r\nnew Soaked! patch or the RCT3 Patch 3) as this adds features\r\n(such as billboards in Soaked! and normal RCT3 and tunnels and\r\nspeakers in normal RCT3) not otherwise available.");
        strcpy(themefile, "");
        SetDlgItemText(hwnd, IDC_THEMEPREFIX, g_config->m_prefix.c_str());
        break;
    case WM_CLOSE:
        if (save == true) {
            if (MessageBox(hwnd, "Unsaved changes, continue?", "Unsaved changes", MB_YESNO) == IDNO) {
                break;
            }
        }
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_FILE_EXIT:
            if (save == true) {
                if (MessageBox(hwnd, "Unsaved changes, continue?", "Unsaved changes", MB_YESNO)
                        == IDNO) {
                    break;
                }
            }
            EndDialog(hwnd, 0);
            break;
        case ID_FILE_NEWTHEME: {
            unsigned int i;
            if (save == true) {
                if (MessageBox(hwnd, "Unsaved changes, continue?", "Unsaved changes", MB_YESNO)
                        == IDNO) {
                    break;
                }
            }
            save = false;
            for (i = 0; i < SceneryItems.size(); i++) {
                delete SceneryItems[i]->name;
                delete SceneryItems[i]->ovl;
                delete SceneryItems[i]->location;
                delete SceneryItems[i]->icon;
                if (SceneryItems[i]->wallicon != 0) {
                    delete SceneryItems[i]->wallicon;
                    delete SceneryItems[i]->wallname;
                }
                delete SceneryItems[i];
            }
            for (i = 0; i < IconTextures.size(); i++) {
                delete IconTextures[i]->filename;
                delete IconTextures[i]->name;
                delete IconTextures[i];
            }
            for (i = 0; i < Icons.size(); i++) {
                delete Icons[i]->name;
                delete Icons[i]->texture;
                delete Icons[i];
            }
            for (i = 0; i < Stalls.size(); i++) {
                delete Stalls[i]->Name;
                delete Stalls[i]->NameString;
                delete Stalls[i]->DescriptionString;
                delete Stalls[i]->SID;
                if (Stalls[i]->Items)
                {
                    delete Stalls[i]->Items;
                }
                delete Stalls[i];
            }
            for (i = 0; i < Attractions.size(); i++) {
                delete Attractions[i]->Name;
                delete Attractions[i]->NameString;
                delete Attractions[i]->DescriptionString;
                delete Attractions[i]->SID;
                delete Attractions[i];
            }
            SetDlgItemText(hwnd, IDC_THEMENAME, "");
            SendDlgItemMessage(hwnd, IDC_THEME, LB_SETCURSEL, (WPARAM) - 1, 0);
            cTextStrings.clear();
            Icons.clear();
            IconTextures.clear();
            SceneryItems.clear();
            ::wxGetApp().g_texturecache.clear();
            Stalls.clear();
            Attractions.clear();
            strcpy(themefile, "");
            SetDlgItemText(hwnd, IDC_THEMEPREFIX, g_config->m_prefix.c_str());
        }
            break;
        case IDM_SAVE_DEFAULT_PREFIX: {
                unsigned long namelen = GetWindowTextLength(GetDlgItem(hwnd, IDC_THEMEPREFIX));
                char *tmp = new char[namelen+1];
                GetDlgItemText(hwnd, IDC_THEMEPREFIX, tmp, namelen+1);
                g_config->m_prefix = tmp;
                delete[] tmp;
                if (!g_config->Save())
                    MessageBox(hwnd, "Saving default prefix failed.", "Error", MB_OK);
            }
            break;
        case ID_FILE_SAVETHEME: {
            OPENFILENAME ofn;
            char sfile[MAX_PATH] = "";
            unsigned int i;
            unsigned long style = SendDlgItemMessage(hwnd, IDC_THEME, LB_GETCURSEL, 0,
                                  0);
            unsigned long len = GetWindowTextLength(GetDlgItem(hwnd, IDC_THEMENAME));
            if ((SceneryItems.size() == 0) || (style == -1)
                    || (len == 0)) {
                if (SceneryItems.size() == 0)
                    MessageBox(hwnd,
                               "Unable to save because of missing data.\nNo scenery items.",
                               "Error", MB_OK);
                else if (style == -1)
                    MessageBox(hwnd,
                               "Unable to save because of missing data.\nNo theme type selected.",
                               "Error", MB_OK);
                else
                    MessageBox(hwnd,
                               "Unable to save because of missing data.\nTheme name missing.",
                               "Error", MB_OK);
                break;
            }
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Theme Files (*.thm)\0*.thm\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Save Theme file";
            ofn.lpstrDefExt = ".thm";
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
            if (GetSaveFileName(&ofn)) {
                ::wxGetApp().g_workdir.AssignDir(wxFileName(sfile).GetPath());
                strcpy(themefile, sfile);
                save = false;
                FILE *f = fopen(sfile, "wb");
                fwrite(&style, 4, 1, f);
                len++;
                char *temp = new char[len];
                GetDlgItemText(hwnd, IDC_THEMENAME, temp, len);
                fwrite(&len, 4, 1, f);
                fwrite(temp, len, 1, f);
                delete temp;
                len = cTextStrings.size();
                fwrite(&len, 4, 1, f);
                for (i = 0; i < cTextStrings.size(); i++) {
                    len = strlen(cTextStrings[i].name.c_str());
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(cTextStrings[i].name.c_str(), len, 1, f);
                    len = strlen(cTextStrings[i].text.c_str());
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(cTextStrings[i].text.c_str(), len, 1, f);
                }
                len = IconTextures.size();
                fwrite(&len, 4, 1, f);
                for (i = 0; i < IconTextures.size(); i++) {
                    len = strlen(IconTextures[i]->name);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(IconTextures[i]->name, len, 1, f);
                    len = strlen(IconTextures[i]->filename);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(IconTextures[i]->filename, len, 1, f);
                }
                len = Icons.size();
                fwrite(&len, 4, 1, f);
                for (i = 0; i < Icons.size(); i++) {
                    len = strlen(Icons[i]->name);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(Icons[i]->name, len, 1, f);
                    len = strlen(Icons[i]->texture);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(Icons[i]->texture, len, 1, f);
                    fwrite(&Icons[i]->top, 4, 1, f);
                    fwrite(&Icons[i]->left, 4, 1, f);
                    fwrite(&Icons[i]->bottom, 4, 1, f);
                    fwrite(&Icons[i]->right, 4, 1, f);
                }
                len = SceneryItems.size();
                fwrite(&len, 4, 1, f);
                for (unsigned long i = 0; i < SceneryItems.size(); i++) {
                    Scenery *s = SceneryItems[i];
                    len = strlen(s->ovl);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->ovl, len, 1, f);
                    if (s->wallicon != 0) {
                        len = strlen(s->wallicon);
                        len++;
                        fwrite(&len, 4, 1, f);
                        fwrite(s->wallicon, len, 1, f);
                    } else {
                        len = 0;
                        fwrite(&len, 4, 1, f);
                    }
                    len = strlen(s->icon);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->icon, len, 1, f);
                    len = strlen(s->name);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->name, len, 1, f);
                    if (s->wallname != 0) {
                        len = strlen(s->wallname);
                        len++;
                        fwrite(&len, 4, 1, f);
                        fwrite(s->wallname, len, 1, f);
                    } else {
                        len = 0;
                        fwrite(&len, 4, 1, f);
                    }
                    len = strlen(s->location);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->location, len, 1, f);
                    fwrite(&s->cost, 4, 1, f);
                    fwrite(&s->refund, 4, 1, f);
                    fwrite(&s->type, 4, 1, f);
                    fwrite(&s->unk4, 4, 1, f);
                    fwrite(&s->supportstype, 4, 1, f);
                    fwrite(&s->unk17, 4, 1, f);
                    fwrite(&s->sizex, 4, 1, f);
                    fwrite(&s->sizey, 4, 1, f);
                    fwrite(&s->sizeflag, 4, 1, f);
                    fwrite(&s->extra, 4, 1, f);
                    fwrite(&s->size.x, 4, 1, f);
                    fwrite(&s->size.y, 4, 1, f);
                    fwrite(&s->size.z, 4, 1, f);
                    fwrite(&s->position.x, 4, 1, f);
                    fwrite(&s->position.y, 4, 1, f);
                    fwrite(&s->position.z, 4, 1, f);
                    fwrite(&s->flag1, 4, 1, f);
                    fwrite(&s->flag2, 4, 1, f);
                    fwrite(&s->flag3, 4, 1, f);
                    fwrite(&s->flag4, 4, 1, f);
                    fwrite(&s->flag5, 4, 1, f);
                    fwrite(&s->flag6, 4, 1, f);
                    fwrite(&s->flag7, 4, 1, f);
                    fwrite(&s->flag8, 4, 1, f);
                    fwrite(&s->flag9, 4, 1, f);
                    fwrite(&s->flag10, 4, 1, f);
                    fwrite(&s->flag11, 4, 1, f);
                    fwrite(&s->flag12, 4, 1, f);
                    fwrite(&s->flag13, 4, 1, f);
                    fwrite(&s->flag14, 4, 1, f);
                    fwrite(&s->flag15, 4, 1, f);
                    fwrite(&s->flag16, 4, 1, f);
                    fwrite(&s->flag17, 4, 1, f);
                    fwrite(&s->flag18, 4, 1, f);
                    fwrite(&s->flag19, 4, 1, f);
                    fwrite(&s->flag20, 4, 1, f);
                    fwrite(&s->flag21, 4, 1, f);
                    fwrite(&s->flag22, 4, 1, f);
                    fwrite(&s->flag23, 4, 1, f);
                    fwrite(&s->flag24, 4, 1, f);
                    fwrite(&s->flag25, 4, 1, f);
                    fwrite(&s->flag26, 4, 1, f);
                    fwrite(&s->flag27, 4, 1, f);
                    fwrite(&s->flag28, 4, 1, f);
                    fwrite(&s->flag29, 4, 1, f);
                    fwrite(&s->flag30, 4, 1, f);
                    fwrite(&s->flag31, 4, 1, f);
                    fwrite(&s->flag32, 4, 1, f);
                    fwrite(&s->flag33, 4, 1, f);
                    fwrite(&s->flag34, 4, 1, f);
                    fwrite(&s->flag35, 4, 1, f);
                    fwrite(&s->flag36, 4, 1, f);
                    fwrite(&s->flag37, 4, 1, f);
                    fwrite(&s->flag38, 4, 1, f);
                    fwrite(&s->flag39, 4, 1, f);
                    fwrite(&s->flag40, 4, 1, f);
                    fwrite(&s->flag41, 4, 1, f);
                    fwrite(&s->flag42, 4, 1, f);
                    fwrite(&s->flag43, 4, 1, f);
                    fwrite(&s->flag44, 4, 1, f);
                    fwrite(&s->flag45, 4, 1, f);
                    fwrite(&s->flag46, 4, 1, f);
                    fwrite(&s->flag47, 4, 1, f);
                    fwrite(&s->flag48, 4, 1, f);
                    fwrite(&s->flag49, 4, 1, f);
                    fwrite(&s->flag50, 4, 1, f);
                    fwrite(&s->flag51, 4, 1, f);
                    fwrite(&s->flag52, 4, 1, f);
                    fwrite(&s->flag53, 4, 1, f);
                    fwrite(&s->flag54, 4, 1, f);
                    fwrite(&s->flag55, 4, 1, f);
                    fwrite(&s->flag56, 4, 1, f);
                    fwrite(&s->flag57, 4, 1, f);
                    fwrite(&s->flag58, 4, 1, f);
                    fwrite(&s->flag59, 4, 1, f);
                    fwrite(&s->flag60, 4, 1, f);
                    fwrite(&s->flag61, 4, 1, f);
                    fwrite(&s->flag62, 4, 1, f);
                    fwrite(&s->flag63, 4, 1, f);
                    fwrite(&s->flag64, 4, 1, f);
                    fwrite(&s->unk31, 4, 1, f);
                    fwrite(&s->unk32, 4, 1, f);
                    fwrite(&s->unk33, 4, 1, f);
                    fwrite(&s->dunk2, 4, 1, f);
                    fwrite(&s->dunk3, 4, 1, f);
                    fwrite(&s->dunk4, 4, 1, f);
                    fwrite(&s->dunk5, 4, 1, f);
                }
                len = GetWindowTextLength(GetDlgItem(hwnd, IDC_THEMEPREFIX)) + 1;
                fwrite(&len, 4, 1, f);
                if (len > 0) {
                    temp = new char[len];
                    GetDlgItemText(hwnd, IDC_THEMEPREFIX, temp, len);
                    fwrite(temp, len, 1, f);
                    delete temp;
                }
                len = Stalls.size();
                fwrite(&len, 4, 1, f);
                for (unsigned int i = 0;i < Stalls.size();i++)
                {
                    StallStr *s = Stalls[i];
                    len = strlen(s->Name);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->Name, len, 1, f);
                    fwrite(&s->StallType,4,1,f);
                    len = strlen(s->NameString);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->NameString, len, 1, f);
                    len = strlen(s->DescriptionString);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->DescriptionString, len, 1, f);
                    fwrite(&s->Unk2,4,1,f);
                    fwrite(&s->Unk3,4,1,f);
                    len = strlen(s->SID);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->SID, len, 1, f);
                    fwrite(&s->ItemCount,4,1,f);
                    for (unsigned int j = 0;j < s->ItemCount;j++)
                    {
                        len = strlen(s->Items[j].CID);
                        len++;
                        fwrite(&len, 4, 1, f);
                        fwrite(s->Items[j].CID, len, 1, f);
                        fwrite(&s->Items[j].cost,4,1,f);
                    }
                    fwrite(&s->Unk11,4,1,f);
                    fwrite(&s->Unk12,4,1,f);
                    fwrite(&s->Unk13,4,1,f);
                    fwrite(&s->Unk14,4,1,f);
                    fwrite(&s->Unk15,4,1,f);
                    fwrite(&s->Unk16,4,1,f);
                }
                len = Attractions.size();
                fwrite(&len, 4, 1, f);
                for (unsigned int i = 0;i < Attractions.size();i++)
                {
                    AttractionStr *s = Attractions[i];
                    len = strlen(s->Name);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->Name, len, 1, f);
                    fwrite(&s->AttractionType,4,1,f);
                    len = strlen(s->NameString);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->NameString, len, 1, f);
                    len = strlen(s->DescriptionString);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->DescriptionString, len, 1, f);
                    fwrite(&s->Unk2,4,1,f);
                    fwrite(&s->Unk3,4,1,f);
                    len = strlen(s->SID);
                    len++;
                    fwrite(&len, 4, 1, f);
                    fwrite(s->SID, len, 1, f);
                }
                fclose(f);
            }
        }
        break;
        case ID_FILE_OPENTHEME: {
            OPENFILENAME ofn;
            char sfile[MAX_PATH] = "";
            unsigned int i;
            if (save == true) {
                if (MessageBox(hwnd, "Unsaved changes, continue?", "Unsaved changes", MB_YESNO)
                        == IDNO) {
                    break;
                }
            }
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Theme Files (*.thm)\0*.thm\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open Theme File";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = ".thm";
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                ::wxGetApp().g_workdir.AssignDir(wxFileName(sfile).GetPath());
                strcpy(themefile, sfile);
                save = false;
                for (i = 0; i < SceneryItems.size(); i++) {
                    delete SceneryItems[i]->name;
                    if (SceneryItems[i]->ovl != 0) {
                        delete SceneryItems[i]->ovl;
                    }
                    delete SceneryItems[i]->location;
                    delete SceneryItems[i]->icon;
                    if (SceneryItems[i]->wallicon != 0) {
                        delete SceneryItems[i]->wallicon;
                        delete SceneryItems[i]->wallname;
                    }
                    delete SceneryItems[i];
                }
                for (i = 0; i < IconTextures.size(); i++) {
                    delete IconTextures[i]->filename;
                    delete IconTextures[i]->name;
                    delete IconTextures[i];
                }
                for (i = 0; i < Icons.size(); i++) {
                    delete Icons[i]->name;
                    delete Icons[i]->texture;
                    delete Icons[i];
                }
                for (i = 0; i < Stalls.size(); i++) {
                    delete Stalls[i]->Name;
                    delete Stalls[i]->NameString;
                    delete Stalls[i]->DescriptionString;
                    delete Stalls[i]->SID;
                    if (Stalls[i]->Items)
                    {
                        delete Stalls[i]->Items;
                    }
                    delete Stalls[i];
                }
                for (i = 0; i < Attractions.size(); i++) {
                    delete Attractions[i]->Name;
                    delete Attractions[i]->NameString;
                    delete Attractions[i]->DescriptionString;
                    delete Attractions[i]->SID;
                    delete Attractions[i];
                }
                cTextStrings.clear();
                Icons.clear();
                IconTextures.clear();
                SceneryItems.clear();
                Stalls.clear();
                Attractions.clear();
                FILE *f = fopen(sfile, "rb");
                unsigned long style;
                unsigned long len;
                char *temp;
                fread(&style, 4, 1, f);
                fread(&len, 4, 1, f);
                temp = new char[len];
                fread(temp, len, 1, f);
                SendDlgItemMessage(hwnd, IDC_THEME, LB_SETCURSEL, style, 0);
                SetDlgItemText(hwnd, IDC_THEMENAME, temp);
                delete temp;
                unsigned long len2;
                fread(&len, 4, 1, f);
                for (i = 0; i < len; i++) {
                    cText t;
                    fread(&len2, 4, 1, f);
                    char *buf = new char[len2];
                    fread(buf, len2, 1, f);
                    t.name = buf;
                    delete[] buf;
                    fread(&len2, 4, 1, f);
                    buf = new char[len2];
                    fread(buf, len2, 1, f);
                    t.text = buf;
                    delete[] buf;
                    cTextStrings.push_back(t);
                }
                fread(&len, 4, 1, f);
                for (i = 0; i < len; i++) {
                    IconTexture *t = new IconTexture;
                    fread(&len2, 4, 1, f);
                    t->name = new char[len2];
                    fread(t->name, len2, 1, f);
                    fread(&len2, 4, 1, f);
                    t->filename = new char[len2];
                    fread(t->filename, len2, 1, f);

                    ReadIconTexture(hwnd, t);

                    IconTextures.push_back(t);
                }
                fread(&len, 4, 1, f);
                for (i = 0; i < len; i++) {
                    Icon *t = new Icon;
                    fread(&len2, 4, 1, f);
                    t->name = new char[len2];
                    fread(t->name, len2, 1, f);
                    fread(&len2, 4, 1, f);
                    t->texture = new char[len2];
                    fread(t->texture, len2, 1, f);
                    Icons.push_back(t);
                    fread(&t->top, 4, 1, f);
                    fread(&t->left, 4, 1, f);
                    fread(&t->bottom, 4, 1, f);
                    fread(&t->right, 4, 1, f);
                }
                fread(&len, 4, 1, f);
                for (i = 0; i < len; i++) {
                    Scenery *t = new Scenery;
                    fread(&len2, 4, 1, f);
                    t->ovl = new char[len2];
                    fread(t->ovl, len2, 1, f);
                    fread(&len2, 4, 1, f);
                    if (len2 != 0) {
                        t->wallicon = new char[len2];
                        fread(t->wallicon, len2, 1, f);
                    } else {
                        t->wallicon = 0;
                    }
                    fread(&len2, 4, 1, f);
                    t->icon = new char[len2];
                    fread(t->icon, len2, 1, f);
                    fread(&len2, 4, 1, f);
                    t->name = new char[len2];
                    fread(t->name, len2, 1, f);
                    fread(&len2, 4, 1, f);
                    if (len2 != 0) {
                        t->wallname = new char[len2];
                        fread(t->wallname, len2, 1, f);
                    } else {
                        t->wallname = 0;
                    }
                    fread(&len2, 4, 1, f);
                    t->location = new char[len2];
                    fread(t->location, len2, 1, f);
                    fread(&t->cost, 4, 1, f);
                    fread(&t->refund, 4, 1, f);
                    fread(&t->type, 4, 1, f);
                    fread(&t->unk4, 4, 1, f);
                    fread(&t->supportstype, 4, 1, f);
                    fread(&t->unk17, 4, 1, f);
                    fread(&t->sizex, 4, 1, f);
                    fread(&t->sizey, 4, 1, f);
                    fread(&t->sizeflag, 4, 1, f);
                    fread(&t->extra, 4, 1, f);
                    fread(&t->size.x, 4, 1, f);
                    fread(&t->size.y, 4, 1, f);
                    fread(&t->size.z, 4, 1, f);
                    fread(&t->position.x, 4, 1, f);
                    fread(&t->position.y, 4, 1, f);
                    fread(&t->position.z, 4, 1, f);
                    fread(&t->flag1, 4, 1, f);
                    fread(&t->flag2, 4, 1, f);
                    fread(&t->flag3, 4, 1, f);
                    fread(&t->flag4, 4, 1, f);
                    fread(&t->flag5, 4, 1, f);
                    fread(&t->flag6, 4, 1, f);
                    fread(&t->flag7, 4, 1, f);
                    fread(&t->flag8, 4, 1, f);
                    fread(&t->flag9, 4, 1, f);
                    fread(&t->flag10, 4, 1, f);
                    fread(&t->flag11, 4, 1, f);
                    fread(&t->flag12, 4, 1, f);
                    fread(&t->flag13, 4, 1, f);
                    fread(&t->flag14, 4, 1, f);
                    fread(&t->flag15, 4, 1, f);
                    fread(&t->flag16, 4, 1, f);
                    fread(&t->flag17, 4, 1, f);
                    fread(&t->flag18, 4, 1, f);
                    fread(&t->flag19, 4, 1, f);
                    fread(&t->flag20, 4, 1, f);
                    fread(&t->flag21, 4, 1, f);
                    fread(&t->flag22, 4, 1, f);
                    fread(&t->flag23, 4, 1, f);
                    fread(&t->flag24, 4, 1, f);
                    fread(&t->flag25, 4, 1, f);
                    fread(&t->flag26, 4, 1, f);
                    fread(&t->flag27, 4, 1, f);
                    fread(&t->flag28, 4, 1, f);
                    fread(&t->flag29, 4, 1, f);
                    fread(&t->flag30, 4, 1, f);
                    fread(&t->flag31, 4, 1, f);
                    fread(&t->flag32, 4, 1, f);
                    fread(&t->flag33, 4, 1, f);
                    fread(&t->flag34, 4, 1, f);
                    fread(&t->flag35, 4, 1, f);
                    fread(&t->flag36, 4, 1, f);
                    fread(&t->flag37, 4, 1, f);
                    fread(&t->flag38, 4, 1, f);
                    fread(&t->flag39, 4, 1, f);
                    fread(&t->flag40, 4, 1, f);
                    fread(&t->flag41, 4, 1, f);
                    fread(&t->flag42, 4, 1, f);
                    fread(&t->flag43, 4, 1, f);
                    fread(&t->flag44, 4, 1, f);
                    fread(&t->flag45, 4, 1, f);
                    fread(&t->flag46, 4, 1, f);
                    fread(&t->flag47, 4, 1, f);
                    fread(&t->flag48, 4, 1, f);
                    fread(&t->flag49, 4, 1, f);
                    fread(&t->flag50, 4, 1, f);
                    fread(&t->flag51, 4, 1, f);
                    fread(&t->flag52, 4, 1, f);
                    fread(&t->flag53, 4, 1, f);
                    fread(&t->flag54, 4, 1, f);
                    fread(&t->flag55, 4, 1, f);
                    fread(&t->flag56, 4, 1, f);
                    fread(&t->flag57, 4, 1, f);
                    fread(&t->flag58, 4, 1, f);
                    fread(&t->flag59, 4, 1, f);
                    fread(&t->flag60, 4, 1, f);
                    fread(&t->flag61, 4, 1, f);
                    fread(&t->flag62, 4, 1, f);
                    fread(&t->flag63, 4, 1, f);
                    fread(&t->flag64, 4, 1, f);
                    fread(&t->unk31, 4, 1, f);
                    fread(&t->unk32, 4, 1, f);
                    fread(&t->unk33, 4, 1, f);
                    fread(&t->dunk2, 4, 1, f);
                    fread(&t->dunk3, 4, 1, f);
                    fread(&t->dunk4, 4, 1, f);
                    fread(&t->dunk5, 4, 1, f);
                    SceneryItems.push_back(t);
                }
                if (!feof(f)) {
                    len = 0;
                    if (!fread(&len, 4, 1, f))
                        len = 0;
                    if (len > 0) {
                        temp = new char[len];
                        fread(temp, len, 1, f);
                        SetDlgItemText(hwnd, IDC_THEMEPREFIX, temp);
                        delete temp;
                    }
                }
                if (!feof(f)) {
                    unsigned int stallcount = 0;
                    fread(&stallcount, 4, 1, f);
                    for (unsigned int i = 0;i < stallcount;i++)
                    {
                        StallStr *s = new StallStr;
                        fread(&len, 4, 1, f);
                        s->Name = new char[len];
                        fread(s->Name, len, 1, f);
                        fread(&s->StallType,4,1,f);
                        fread(&len, 4, 1, f);
                        s->NameString = new char[len];
                        fread(s->NameString, len, 1, f);
                        fread(&len, 4, 1, f);
                        s->DescriptionString = new char[len];
                        fread(s->DescriptionString, len, 1, f);
                        fread(&s->Unk2,4,1,f);
                        fread(&s->Unk3,4,1,f);
                        fread(&len, 4, 1, f);
                        s->SID = new char[len];
                        fread(s->SID, len, 1, f);
                        fread(&s->ItemCount,4,1,f);
                        if (s->ItemCount)
                        {
                            s->Items = new StallItem[s->ItemCount];
                            for (unsigned int j = 0;j < s->ItemCount;j++)
                            {
                                fread(&len, 4, 1, f);
                                s->Items[j].CID = new char[len];
                                fread(s->Items[j].CID, len, 1, f);
                                fread(&s->Items[j].cost,4,1,f);
                            }
                        }
                        else
                        {
                            s->Items = 0;
                        }
                        fread(&s->Unk11,4,1,f);
                        fread(&s->Unk12,4,1,f);
                        fread(&s->Unk13,4,1,f);
                        fread(&s->Unk14,4,1,f);
                        fread(&s->Unk15,4,1,f);
                        fread(&s->Unk16,4,1,f);
                        Stalls.push_back(s);
                    }
                }
                if (!feof(f)) {
                    unsigned int attractioncount = 0;
                    fread(&attractioncount, 4, 1, f);
                    for (unsigned int i = 0;i < attractioncount;i++)
                    {
                        AttractionStr *s = new AttractionStr;
                        fread(&len, 4, 1, f);
                        s->Name = new char[len];
                        fread(s->Name, len, 1, f);
                        fread(&s->AttractionType,4,1,f);
                        fread(&len, 4, 1, f);
                        s->NameString = new char[len];
                        fread(s->NameString, len, 1, f);
                        fread(&len, 4, 1, f);
                        s->DescriptionString = new char[len];
                        fread(s->DescriptionString, len, 1, f);
                        fread(&s->Unk2,4,1,f);
                        fread(&s->Unk3,4,1,f);
                        fread(&len, 4, 1, f);
                        s->SID = new char[len];
                        fread(s->SID, len, 1, f);
                        Attractions.push_back(s);
                    }
                }
                ::wxGetApp().g_texturecache.clear();
                fclose(f);
            }
        }
            break;
        case ID_FILE_OPENTHEME2: {
            OPENFILENAME ofn;
            char sfile[MAX_PATH] = "";
            unsigned int i;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Theme Files (*.thm)\0*.thm\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open Theme File";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = ".thm";
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                ::wxGetApp().g_workdir.AssignDir(wxFileName(sfile).GetPath());
                save = true;
                FILE *f = fopen(sfile, "rb");
                unsigned long style;
                unsigned long len;
                char *temp;
                fread(&style, 4, 1, f);
                fread(&len, 4, 1, f);
                temp = new char[len];
                fread(temp, len, 1, f);
                SendDlgItemMessage(hwnd, IDC_THEME, LB_SETCURSEL, style, 0);
                SetDlgItemText(hwnd, IDC_THEMENAME, temp);
                delete temp;
                unsigned long len2;
                fread(&len, 4, 1, f);
                for (i = 0; i < len; i++) {
                    cText t;
                    fread(&len2, 4, 1, f);
                    char *buf = new char[len2];
                    fread(buf, len2, 1, f);
                    t.name = buf;
                    delete[] buf;
                    fread(&len2, 4, 1, f);
                    buf = new char[len2];
                    fread(buf, len2, 1, f);
                    t.text = buf;
                    delete[] buf;
                    cTextStrings.push_back(t);
                }
                fread(&len, 4, 1, f);
                for (i = 0; i < len; i++) {
                    IconTexture *t = new IconTexture;
                    fread(&len2, 4, 1, f);
                    t->name = new char[len2];
                    fread(t->name, len2, 1, f);
                    fread(&len2, 4, 1, f);
                    t->filename = new char[len2];
                    fread(t->filename, len2, 1, f);

                    ReadIconTexture(hwnd, t);

                    IconTextures.push_back(t);
                }
                fread(&len, 4, 1, f);
                for (i = 0; i < len; i++) {
                    Icon *t = new Icon;
                    fread(&len2, 4, 1, f);
                    t->name = new char[len2];
                    fread(t->name, len2, 1, f);
                    fread(&len2, 4, 1, f);
                    t->texture = new char[len2];
                    fread(t->texture, len2, 1, f);
                    Icons.push_back(t);
                    fread(&t->top, 4, 1, f);
                    fread(&t->left, 4, 1, f);
                    fread(&t->bottom, 4, 1, f);
                    fread(&t->right, 4, 1, f);
                }
                fread(&len, 4, 1, f);
                for (i = 0; i < len; i++) {
                    Scenery *t = new Scenery;
                    fread(&len2, 4, 1, f);
                    t->ovl = new char[len2];
                    fread(t->ovl, len2, 1, f);
                    fread(&len2, 4, 1, f);
                    if (len2 != 0) {
                        t->wallicon = new char[len2];
                        fread(t->wallicon, len2, 1, f);
                    } else {
                        t->wallicon = 0;
                    }
                    fread(&len2, 4, 1, f);
                    t->icon = new char[len2];
                    fread(t->icon, len2, 1, f);
                    fread(&len2, 4, 1, f);
                    t->name = new char[len2];
                    fread(t->name, len2, 1, f);
                    fread(&len2, 4, 1, f);
                    if (len2 != 0) {
                        t->wallname = new char[len2];
                        fread(t->wallname, len2, 1, f);
                    } else {
                        t->wallname = 0;
                    }
                    fread(&len2, 4, 1, f);
                    t->location = new char[len2];
                    fread(t->location, len2, 1, f);
                    fread(&t->cost, 4, 1, f);
                    fread(&t->refund, 4, 1, f);
                    fread(&t->type, 4, 1, f);
                    fread(&t->unk4, 4, 1, f);
                    fread(&t->supportstype, 4, 1, f);
                    fread(&t->unk17, 4, 1, f);
                    fread(&t->sizex, 4, 1, f);
                    fread(&t->sizey, 4, 1, f);
                    fread(&t->sizeflag, 4, 1, f);
                    fread(&t->extra, 4, 1, f);
                    fread(&t->size.x, 4, 1, f);
                    fread(&t->size.y, 4, 1, f);
                    fread(&t->size.z, 4, 1, f);
                    fread(&t->position.x, 4, 1, f);
                    fread(&t->position.y, 4, 1, f);
                    fread(&t->position.z, 4, 1, f);
                    fread(&t->flag1, 4, 1, f);
                    fread(&t->flag2, 4, 1, f);
                    fread(&t->flag3, 4, 1, f);
                    fread(&t->flag4, 4, 1, f);
                    fread(&t->flag5, 4, 1, f);
                    fread(&t->flag6, 4, 1, f);
                    fread(&t->flag7, 4, 1, f);
                    fread(&t->flag8, 4, 1, f);
                    fread(&t->flag9, 4, 1, f);
                    fread(&t->flag10, 4, 1, f);
                    fread(&t->flag11, 4, 1, f);
                    fread(&t->flag12, 4, 1, f);
                    fread(&t->flag13, 4, 1, f);
                    fread(&t->flag14, 4, 1, f);
                    fread(&t->flag15, 4, 1, f);
                    fread(&t->flag16, 4, 1, f);
                    fread(&t->flag17, 4, 1, f);
                    fread(&t->flag18, 4, 1, f);
                    fread(&t->flag19, 4, 1, f);
                    fread(&t->flag20, 4, 1, f);
                    fread(&t->flag21, 4, 1, f);
                    fread(&t->flag22, 4, 1, f);
                    fread(&t->flag23, 4, 1, f);
                    fread(&t->flag24, 4, 1, f);
                    fread(&t->flag25, 4, 1, f);
                    fread(&t->flag26, 4, 1, f);
                    fread(&t->flag27, 4, 1, f);
                    fread(&t->flag28, 4, 1, f);
                    fread(&t->flag29, 4, 1, f);
                    fread(&t->flag30, 4, 1, f);
                    fread(&t->flag31, 4, 1, f);
                    fread(&t->flag32, 4, 1, f);
                    fread(&t->flag33, 4, 1, f);
                    fread(&t->flag34, 4, 1, f);
                    fread(&t->flag35, 4, 1, f);
                    fread(&t->flag36, 4, 1, f);
                    fread(&t->flag37, 4, 1, f);
                    fread(&t->flag38, 4, 1, f);
                    fread(&t->flag39, 4, 1, f);
                    fread(&t->flag40, 4, 1, f);
                    fread(&t->flag41, 4, 1, f);
                    fread(&t->flag42, 4, 1, f);
                    fread(&t->flag43, 4, 1, f);
                    fread(&t->flag44, 4, 1, f);
                    fread(&t->flag45, 4, 1, f);
                    fread(&t->flag46, 4, 1, f);
                    fread(&t->flag47, 4, 1, f);
                    fread(&t->flag48, 4, 1, f);
                    fread(&t->flag49, 4, 1, f);
                    fread(&t->flag50, 4, 1, f);
                    fread(&t->flag51, 4, 1, f);
                    fread(&t->flag52, 4, 1, f);
                    fread(&t->flag53, 4, 1, f);
                    fread(&t->flag54, 4, 1, f);
                    fread(&t->flag55, 4, 1, f);
                    fread(&t->flag56, 4, 1, f);
                    fread(&t->flag57, 4, 1, f);
                    fread(&t->flag58, 4, 1, f);
                    fread(&t->flag59, 4, 1, f);
                    fread(&t->flag60, 4, 1, f);
                    fread(&t->flag61, 4, 1, f);
                    fread(&t->flag62, 4, 1, f);
                    fread(&t->flag63, 4, 1, f);
                    fread(&t->flag64, 4, 1, f);
                    fread(&t->unk31, 4, 1, f);
                    fread(&t->unk32, 4, 1, f);
                    fread(&t->unk33, 4, 1, f);
                    fread(&t->dunk2, 4, 1, f);
                    fread(&t->dunk3, 4, 1, f);
                    fread(&t->dunk4, 4, 1, f);
                    fread(&t->dunk5, 4, 1, f);
                    SceneryItems.push_back(t);
                }
                if (!feof(f)) {
                    len = 0;
                    if (!fread(&len, 4, 1, f))
                        len = 0;
                    if (len > 0) {
                        temp = new char[len];
                        fread(temp, len, 1, f);
                        SetDlgItemText(hwnd, IDC_THEMEPREFIX, temp);
                        delete temp;
                    }
                }
                if (!feof(f)) {
                    unsigned int stallcount = 0;
                    fread(&stallcount, 4, 1, f);
                    for (unsigned int i = 0;i < stallcount;i++)
                    {
                        StallStr *s = new StallStr;
                        fread(&len, 4, 1, f);
                        s->Name = new char[len];
                        fread(s->Name, len, 1, f);
                        fread(&s->StallType,4,1,f);
                        fread(&len, 4, 1, f);
                        s->NameString = new char[len];
                        fread(s->NameString, len, 1, f);
                        fread(&len, 4, 1, f);
                        s->DescriptionString = new char[len];
                        fread(s->DescriptionString, len, 1, f);
                        fread(&s->Unk2,4,1,f);
                        fread(&s->Unk3,4,1,f);
                        fread(&len, 4, 1, f);
                        s->SID = new char[len];
                        fread(s->SID, len, 1, f);
                        fread(&s->ItemCount,4,1,f);
                        if (s->ItemCount)
                        {
                            s->Items = new StallItem[s->ItemCount];
                            for (unsigned int j = 0;j < s->ItemCount;j++)
                            {
                                fread(&len, 4, 1, f);
                                s->Items[j].CID = new char[len];
                                fread(s->Items[j].CID, len, 1, f);
                                fread(&s->Items[j].cost,4,1,f);
                            }
                        }
                        else
                        {
                            s->Items = 0;
                        }
                        fread(&s->Unk11,4,1,f);
                        fread(&s->Unk12,4,1,f);
                        fread(&s->Unk13,4,1,f);
                        fread(&s->Unk14,4,1,f);
                        fread(&s->Unk15,4,1,f);
                        fread(&s->Unk16,4,1,f);
                        Stalls.push_back(s);
                    }
                }
                if (!feof(f)) {
                    unsigned int attractioncount = 0;
                    fread(&attractioncount, 4, 1, f);
                    for (unsigned int i = 0;i < attractioncount;i++)
                    {
                        AttractionStr *s = new AttractionStr;
                        fread(&len, 4, 1, f);
                        s->Name = new char[len];
                        fread(s->Name, len, 1, f);
                        fread(&s->AttractionType,4,1,f);
                        fread(&len, 4, 1, f);
                        s->NameString = new char[len];
                        fread(s->NameString, len, 1, f);
                        fread(&len, 4, 1, f);
                        s->DescriptionString = new char[len];
                        fread(s->DescriptionString, len, 1, f);
                        fread(&s->Unk2,4,1,f);
                        fread(&s->Unk3,4,1,f);
                        fread(&len, 4, 1, f);
                        s->SID = new char[len];
                        fread(s->SID, len, 1, f);
                        Attractions.push_back(s);
                    }
                }
                fclose(f);
            }
        }
            break;
        case ID_FILE_INSTALL:
            unsigned long styleval, len;
            styleval = SendDlgItemMessage(hwnd, IDC_THEME, LB_GETCURSEL, 0, 0);
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_THEMENAME));
            if ((SceneryItems.size() == 0) || (styleval == -1)
                    || (len == 0)) {
                if (SceneryItems.size() == 0)
                    MessageBox(hwnd, "Unable to save because of missing data.\nNo scenery items.",
                               "Error", MB_OK);
                else if (styleval == -1)
                    MessageBox(hwnd,
                               "Unable to save because of missing data.\nNo theme type selected.",
                               "Error", MB_OK);
                else
                    MessageBox(hwnd, "Unable to save because of missing data.\nTheme name missing.",
                               "Error", MB_OK);
            } else {
                if (!InstallTheme(hwnd))
                    MessageBox(hwnd, "Theme not successfully installed.", "Error", MB_OK|MB_ICONERROR);	//added by KLN

            }
            break;
        case ID_EDIT_MANAGESCENERY:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_SIDMAN), hwnd, SIDManDlgProc);
            break;
        case ID_EDIT_MANAGETEXTURES:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_TEXTUREMAN), hwnd, TextureManDlgProc);
            break;
        case ID_EDIT_MANAGEREFERENCES:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_GSIMAN), hwnd, GSIManDlgProc);
            break;
        case ID_EDIT_MANAGESTRINGS:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_TEXTMAN), hwnd, TextManDlgProc);
            break;
        case ID_EDIT_CREATESCENERY: {
            char *tmp = new char[MAX_PATH];
            GetDlgItemText(hwnd, IDC_THEMEPREFIX, tmp, MAX_PATH);
            wxString prefix = tmp;
            GetDlgItemText(hwnd, IDC_THEMENAME, tmp, MAX_PATH);
            wxString theme = tmp;
            delete[] tmp;
            wxWindow *tempwind = new wxWindow();
            WXHWND t = tempwind->GetHWND();
            tempwind->SetHWND(hwnd);
            dlgCreateScenery *dialog = new dlgCreateScenery(tempwind);
            dialog->SetDefaultRef(prefix, theme);
            dialog->ShowModal();
            dialog->Destroy();
            tempwind->SetHWND(t);
            tempwind->Destroy();
        }
            //DialogBox(hInst, MAKEINTRESOURCE(IDD_SCENERY), hwnd, SceneryDlgProc);
            break;
        case ID_EDIT_MANAGESTALLS:
        {
            dlgStallMan *dialog = new dlgStallMan(NULL);
            dialog->ShowModal();
            dialog->Destroy();
            break;
        }
        case ID_EDIT_MANAGEATTRACTIONS:
        {
            dlgAttractMan *dialog = new dlgAttractMan(NULL);
            dialog->ShowModal();
            dialog->Destroy();
            break;
        }
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

bool ToolApp::OnInit()
{
	//(*AppInitialize

    int ovl_version;
    if (ovl_version = LibOVLCheckVersion(LIBOVL_VERSION)) {
        ::wxMessageBox(wxString::Format(_("LibOVL version mismatch.\nThis importer version was compiled against libovl v%d.\nFound was libovl v%d."), LIBOVL_VERSION, ovl_version), _("Fatal Error"), wxOK);
        return false;
    }

	bool wxsOK = true;
	::wxInitAllImageHandlers();
    wxXmlResource::Get()->InitAllHandlers();
    InitXmlResource();

    char *dir = new char[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, dir);
    g_appdir = dir;
    g_appdir += "\\";
    delete[] dir;

    g_config = new RCT3Config(g_appdir);
    g_config->Load();

    if (g_config->m_workdir != wxT(""))
        g_workdir.AssignDir(g_config->m_workdir);
    else
        g_workdir.AssignDir(g_appdir);

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); // We need this for directory selection

    // Load/Create Palettes
    // Blue/Magenta/Yellow Palette
    HRSRC hp = FindResourceA(hInst, MAKEINTRESOURCE(IDR_PALETTE_BMY), "PALETTE");
    HGLOBAL hp2 = LoadResource(hInst, hp);
    void *hp3 = LockResource(hp2);
    char tmprgb[3] = "\0\0";	// Last 0 is the 0-terminator =)
    {
        wxMutexLocker lock(wxILMutex);
        ILuint tmp = ilGenImage();
        ilBindImage(tmp);
        ilTexImage(1, 1, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, &tmprgb);
        ilConvertImage(IL_COLOUR_INDEX, IL_UNSIGNED_BYTE);
        ilConvertPal(IL_PAL_RGB24);
        memcpy(ilGetPalette(), hp3, 256 * 3);
        ilConvertPal(IL_PAL_BGR32);
        memcpy(&g_recolpalette_bmy, ilGetPalette(), 256 * 4);
        ilDeleteImage(tmp);
    }
    // Red/Green/Blue Palette
    ZeroMemory(&g_recolpalette_rgb, sizeof(g_recolpalette_rgb));
    int i;
    g_recolpalette_bmy[0].rgbReserved = 0;
    for (i = 1; i <= 85; i++) {
        g_recolpalette_rgb[i].rgbRed = ((86 - i) * 255) / 85;
        g_recolpalette_rgb[i + 85].rgbGreen = ((86 - i) * 255) / 85;
        g_recolpalette_rgb[i + 170].rgbBlue = ((86 - i) * 255) / 85;
        g_recolpalette_bmy[i].rgbReserved = 0;
        g_recolpalette_bmy[i+85].rgbReserved = 0;
        g_recolpalette_bmy[i+170].rgbReserved = 0;
    }
    /*
       char tmptxt[256];
       snprintf(tmptxt, 256, "Palette Colours: %d %d %d\n", (int)recolpalette_rgb[20].rgbRed, (int)recolpalette_rgb[50].rgbRed, (int)recolpalette_rgb[70].rgbRed);
       MessageBox(NULL, tmptxt, "Test", MB_OK);
     */

	return wxsOK;
}

int ToolApp::OnRun() {

    DialogBox(hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC) MainDlgProc);
    g_config->m_workdir = g_workdir.GetPath();
    g_config->Save();
    delete g_config;

    return 0;

}

RCT3Config* ToolApp::GetConfig() {
    return g_config;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

#ifdef DEBUG
    LoadLibrary("exchndl.dll");
#endif

    ilInit();
    iluInit();
    hInst = hInstance;

    return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

/*
    char *dir = new char[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, dir);
    g_appdir = dir;
    g_appdir += "\\";
    delete[] dir;

    g_config = new RCT3Config(g_appdir);
    g_config->Load();

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); // We need this for directory selection

    // Load/Create Palettes
    // Blue/Magenta/Yellow Palette
    HRSRC hp = FindResourceA(hInst, MAKEINTRESOURCE(IDR_PALETTE_BMY), "PALETTE");
    HGLOBAL hp2 = LoadResource(hInst, hp);
    void *hp3 = LockResource(hp2);
    char tmprgb[3] = "\0\0";	// Last 0 is the 0-terminator =)
    ILuint tmp = ilGenImage();
    ilBindImage(tmp);
    ilTexImage(1, 1, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, &tmprgb);
    ilConvertImage(IL_COLOUR_INDEX, IL_UNSIGNED_BYTE);
    ilConvertPal(IL_PAL_RGB24);
    memcpy(ilGetPalette(), hp3, 256 * 3);
    ilConvertPal(IL_PAL_BGR32);
    memcpy(&recolpalette_bmy, ilGetPalette(), 256 * 4);
    ilDeleteImage(tmp);
    // Red/Green/Blue Palette
    ZeroMemory(&recolpalette_rgb, sizeof(recolpalette_bmy));
    int i;
    for (i = 1; i <= 85; i++) {
        recolpalette_rgb[i].rgbRed = ((86 - i) * 255) / 85;
        recolpalette_rgb[i + 85].rgbGreen = ((86 - i) * 255) / 85;
        recolpalette_rgb[i + 170].rgbBlue = ((86 - i) * 255) / 85;
    }
    DialogBox(hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC) MainDlgProc);
    delete g_config;
    return 0;
*/
}

bool InstallTheme(HWND hwnd) {
    unsigned long styleval, len;
//    char InstallLocation[MAX_PATH] = "";
//    char Location[MAX_PATH] = "";
    std::string sInstallLocation = "";
    std::string sLocation = "";
    std::string stemp = "";
    HKEY key;

    char *temp = new char[MAX_PATH];

//    wxString test1 = "blah";
//    wxFileName test2 = test1;

    styleval = SendDlgItemMessage(hwnd, IDC_THEME, LB_GETCURSEL, 0, 0);
    len =
        GetWindowTextLength(GetDlgItem(hwnd, IDC_THEMENAME)) +
        GetWindowTextLength(GetDlgItem(hwnd, IDC_THEMEPREFIX)) + 1;
    char *theme = new char[len];
    GetDlgItemText(hwnd, IDC_THEMEPREFIX, theme, len);
    {
        wxString test = theme;
        if (!test.IsAscii()) {
            ::wxMessageBox(_("The prefix may only contain AscII characters.\nOther characters (eg. German umlauts, accented characters, ...) can cause problems and are therefore not allowed."), _("Error"), wxOK|wxICON_ERROR);
            delete[] temp;
            delete[] theme;
            return false;
        }
    }
    GetDlgItemText(hwnd, IDC_THEMENAME, temp, len);
    {
        wxString test = temp;
        if (!test.IsAscii()) {
            ::wxMessageBox(_("The theme name may only contain AscII characters.\nOther characters (eg. German umlauts, accented characters, ...) can cause problems and are therefore not allowed."), _("Error"), wxOK|wxICON_ERROR);
            delete[] temp;
            delete[] theme;
            return false;
        }
    }
    strcat(theme, temp);

    LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{907B4640-266B-4A21-92FB-CD1A86CD0F63}",
                            0, KEY_QUERY_VALUE, &key);
    unsigned long length = MAX_PATH;
    unsigned long i;
    res = RegQueryValueEx(key, "InstallLocation", 0, NULL, (LPBYTE) temp, &length);
    sInstallLocation = temp;
    RegCloseKey(key);
    if (res != ERROR_SUCCESS) {
        MessageBox(hwnd, "RCT3 install directory not found. Please select a direcory where the theme should be installed.\n\"\\Style\\Themed\" will be appended automatically.", "Error", MB_OK | MB_ICONERROR);
        BROWSEINFO bi;
        ZeroMemory(&bi, sizeof(bi));
        bi.hwndOwner = hwnd;
        bi.pszDisplayName = temp;
        bi.lpszTitle = "Select install directory for theme. Do not select links.";
        bi.ulFlags = 0;
        LPITEMIDLIST pids = SHBrowseForFolder(&bi);
        if (pids) {
            if (SHGetPathFromIDList(pids, temp)) {
                CoTaskMemFree(pids);
                sInstallLocation = temp;
            } else {
                MessageBox(hwnd, "InstallTheme: Invalid Folder selected.", "Error", MB_OK | MB_ICONERROR);
                delete[] theme;
                delete[] temp;
                return false;
            }
        } else {
            delete[] theme;
            delete[] temp;
            return false;
        }
    }
    unsigned long stringtablesize = 0;

    std::string sFileName = "";
    sprintf(temp, "%s\\%s%s\\", sInstallLocation.c_str(), stdstyle, theme);
    sFileName = temp;
    sFileName += "style.common.ovl";

#ifndef LIBOVL_STATIC
    CreateStyleOvl(const_cast<char *> (sFileName.c_str()), styleval);
#else
    styleovl::CreateStyleOvl(sFileName.c_str(), styleval);
#endif
    for (i = 0; i < cTextStrings.size(); i++) {
// FIXME (tobi#1#): Make textstring give-over sane
        Text *txt = new Text;
        txt->name = new char[cTextStrings[i].name.length()+1];
        strcpy(txt->name, cTextStrings[i].name.c_str());
        txt->text = new char[cTextStrings[i].text.length()+1];
        strcpy(txt->text, cTextStrings[i].text.c_str());
#ifndef LIBOVL_STATIC
        AddStyleOVLInfo_TextString(txt);
#else
        styleovl::AddStyleOVLInfo_TextString(txt);
#endif
        delete[] txt->name;
        delete[] txt->text;
        delete txt;
    }
    for (i = 0; i < IconTextures.size(); i++) {
        IconTexture *icot = IconTextures[i];
#ifndef LIBOVL_STATIC
        AddStyleOVLInfo_IconTexture(icot);
#else
        styleovl::AddStyleOVLInfo_IconTexture(icot);
#endif
    }
    for (i = 0; i < Icons.size(); i++) {
        Icon *ico = Icons[i];
#ifndef LIBOVL_STATIC
        AddStyleOVLInfo_Icon(ico);
#else
        styleovl::AddStyleOVLInfo_Icon(ico);
#endif
    }
    for (unsigned long i = 0; i < SceneryItems.size(); i++) {
//        char fname[_MAX_FNAME];
        Scenery *sc = SceneryItems[i];
        char *ovlfile = sc->ovl;
        _splitpath(sc->ovl, NULL, NULL, temp, NULL);
        strchr(temp, '.')[0] = 0;
        std::string sfname = temp;
        stringtablesize = 0;
        stringtablesize += strlen(temp) + 5;
        stringtablesize += strlen(temp) + 5;
        stringtablesize +=
            strlen(stdstyle) + strlen(theme) + 1 + strlen(SceneryItems[i]->location) + 1 +
            strlen(sfname.c_str()) + 1;


        sc->ovl2 = new char[stringtablesize];
        sc->ovlWithoutExt = new char[stringtablesize - 10];
        sc->InternalOVLName = new char[strlen(sfname.c_str()) + 1];
        strcpy(sc->InternalOVLName, sfname.c_str());

        char *strings = sc->ovl2;
        char *fullstrings = strings;

        strcpy(strings, stdstyle);
        strings += strlen(stdstyle);
        strcpy(strings, theme);
        strings += strlen(theme);
        strcpy(strings, "\\");
        strings++;
        strcpy(strings, sc->location);
        strings += strlen(sc->location);
        sprintf(temp, "%s\\%s", sInstallLocation.c_str(), fullstrings);
        sLocation = sInstallLocation + "\\" + fullstrings;
        sprintf(sc->ovlWithoutExt, "%s\\%s", fullstrings, sfname.c_str());
        SHCreateDirectoryEx(hwnd, sLocation.c_str(), NULL);
        strcpy(strings, "\\");
        strings++;
        strcpy(strings, sfname.c_str());
        strings += strlen(sfname.c_str()) + 1;
        sLocation += "\\";
        sLocation += sfname;

        stemp = sLocation + ".common.ovl";
        CopyFile(ovlfile, stemp.c_str(), FALSE);
        sc->ovl3 = new char[strlen(stemp.c_str()) + 1];
        strcpy(sc->ovl3, stemp.c_str());

        stemp = sLocation + ".unique.ovl";
        strcpy(strstr(ovlfile, ".common.ovl"), ".unique.ovl");
        CopyFile(ovlfile, stemp.c_str(), FALSE);

        strcpy(strstr(ovlfile, ".unique.ovl"), ".common.ovl");

#ifndef LIBOVL_STATIC
        AddStyleOVLInfo_SceneryItem(sc);
#else
        styleovl::AddStyleOVLInfo_SceneryItem(sc);
#endif
    }

    for (unsigned long i = 0; i < Stalls.size(); i++) {
        AddStyleOVLInfo_Stalls(Stalls[i]);
    };
    for (unsigned long i = 0; i < Attractions.size(); i++) {
        AddStyleOVLInfo_Attractions(Attractions[i]);
    };
#ifndef LIBOVL_STATIC
    saveStyleOVL();
#else
    styleovl::saveStyleOVL();
#endif
    _splitpath(themefile, temp, NULL, NULL, NULL);
    stemp = temp;
    _splitpath(themefile, NULL, temp, NULL, NULL);
    stemp += temp;
//    char *texref = new char[GetWindowTextLength(GetDlgItem(GetParent(hwnd), IDC_THEMEPREFIX))+
//                            GetWindowTextLength(GetDlgItem(GetParent(hwnd), IDC_THEMENAME))+9+1];
    GetDlgItemText(hwnd, IDC_THEMEPREFIX, temp, MAX_PATH);
    std::string texname = temp;
    GetDlgItemText(hwnd, IDC_THEMENAME, temp, MAX_PATH);
    texname += temp;
    texname += "-texture";
    std::string texbase = stemp + texname;
    std::string textobase = "";

    stemp = texbase + ".common.ovl";
    FILE *f = fopen(stemp.c_str(), "r");
    if (f) {
        fclose(f);
        textobase = sInstallLocation + "\\" + stdstyle + theme + "\\shared\\";
        SHCreateDirectoryEx(hwnd, textobase.c_str(), NULL);
        textobase += texname;
        sLocation = textobase + ".common.ovl";
        CopyFile(stemp.c_str(), sLocation.c_str(), FALSE);
        sLocation = textobase + ".unique.ovl";
        stemp = texbase + ".unique.ovl";
        CopyFile(stemp.c_str(), sLocation.c_str(), FALSE);
        MessageBox(hwnd, "Theme successfully installed.\nTexture OVL found & installed.", "Completed", MB_OK);
    } else
        MessageBox(hwnd, "Theme successfully installed.", "Completed", MB_OK);

//    delete[] texref;
    delete[] temp;
    delete[] theme;

    return true;
}

bool ReadIconTexture(HWND hwnd, IconTexture * t) {
    ILuint tex;
    ILenum Error;

    ilGenImages(1, &tex);
    ilBindImage(tex);

    if (!ilLoadImage(t->filename)) {
        MessageBox(hwnd, "Can't open icon texture", "Error", MB_OK);
        while ((Error = ilGetError())) {
            MessageBox(hwnd, iluErrorString(Error), "Error", MB_OK);
        }
        return false;
    }
    int width = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);
    if (width != height) {
        MessageBox(hwnd, "Icon texture is not square", "Error", MB_OK);
        ilDeleteImages(1, &tex);
        return false;
    }
    if ((1 << local_log2(width)) != width) {
        MessageBox(hwnd, "Icon texture's width/height is not a power of 2", "Error",
                   MB_OK);
        ilDeleteImages(1, &tex);
        return false;
    }
    if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
        MessageBox(hwnd, "Couldn't convert texture to RGBA", "Error", MB_OK);
        ilDeleteImages(1, &tex);
        return false;
    }
    if (ilGetInteger(IL_IMAGE_ORIGIN) == IL_ORIGIN_UPPER_LEFT)
        if (!iluFlipImage()) {
            MessageBox(hwnd, "Couldn't flip icon texture", "Error", MB_OK);
            ilDeleteImages(1, &tex);
            return false;
        }
    int datasize = ilGetDXTCData(NULL, 0, IL_DXT3);
    data = new unsigned char[datasize];
    ilGetDXTCData(data, datasize, IL_DXT3);
    t->fh.Format = 0x13;
    t->fh.Height = height;
    t->fh.Width = width;
    t->fh.Mipcount = 0;
    t->fmh.Pitch = width * 4;
    t->fmh.Blocks = datasize / t->fmh.Pitch;
    t->fmh.MHeight = height;
    t->fmh.MWidth = width;
    t->data = data;

    ilDeleteImages(1, &tex);
    return true;
}
