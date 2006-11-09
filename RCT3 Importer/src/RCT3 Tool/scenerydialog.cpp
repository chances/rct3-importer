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

#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <math.h>
#include <list>
#include <vector>
#include <commctrl.h>

#include <wx/filename.h>

#include <IL/il.h>
#include <IL/ilu.h>

#include "OVL.H"

#include "3DLoader.h"
#include "SCNFile.h"
#include "RCT3STructs.h"
#include "matrix.h"
#include "auipicfiledlg.h"
#include "rct3log.h"

#include "wxdlgCreateScenery.h"
#include "wxdlgEffect.h"
#include "wxdlgMesh.h"
#include "wxdlgTexture.h"

#include "global.h"
#include "scenerydialog.h"
#include "resource.h"
#include "ilhelper.h"

#define TextStrings
#define Icons
#define IconTextures
#define SceneryItems
#define EffectPoints
#define References

///////////////////////////
//
//  Global Variables
//
///////////////////////////
long CurrentEffectPoint = -1;
long CurrentFlexiTexture = -1;
long CurrentMeshStruct = -1;
long CurrentReference = -1;
COLORREF g_colors[16];
bool scenerysave = false;
bool updating = false;

cSCNFile *sceneryfile = new cSCNFile();
std::vector<D3DMATRIX> c_effecttransforms;
wxArrayString c_effecttransformnames;
std::vector<D3DMATRIX> c_transforms;
wxArrayString c_transformnames;
wxArrayString c_choices;

BOOL CALLBACK ChoiceDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG: {
        if (lParam)
            SetWindowText(hwnd, (char *) lParam);
        else
            SetWindowText(hwnd, "Please Choose");
        SendDlgItemMessage(hwnd, IDC_EFFECT_PARTICLE_LIST, LB_RESETCONTENT, 0, 0);
        for (cStringIterator st = c_choices.begin(); st != c_choices.end(); st++)
            SendDlgItemMessage(hwnd, IDC_EFFECT_PARTICLE_LIST, LB_ADDSTRING, 0,
                               (LPARAM) st->c_str());
    }
    break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            long sel = SendDlgItemMessage(hwnd, IDC_EFFECT_PARTICLE_LIST,
                                          LB_GETCURSEL, 0, 0);
            EndDialog(hwnd, sel+1);
        }
        break;
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        default:
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void setMatrix(HWND hwnd, D3DMATRIX m, int width = 4) {
    char temp[256];
    sprintf(temp, "%.*f", width, m._11);
    SetDlgItemText(hwnd, IDC_M11, temp);
    sprintf(temp, "%.*f", width, m._12);
    SetDlgItemText(hwnd, IDC_M12, temp);
    sprintf(temp, "%.*f", width, m._13);
    SetDlgItemText(hwnd, IDC_M13, temp);
    sprintf(temp, "%.*f", width, m._14);
    SetDlgItemText(hwnd, IDC_M14, temp);
    sprintf(temp, "%.*f", width, m._21);
    SetDlgItemText(hwnd, IDC_M21, temp);
    sprintf(temp, "%.*f", width, m._22);
    SetDlgItemText(hwnd, IDC_M22, temp);
    sprintf(temp, "%.*f", width, m._23);
    SetDlgItemText(hwnd, IDC_M23, temp);
    sprintf(temp, "%.*f", width, m._24);
    SetDlgItemText(hwnd, IDC_M24, temp);
    sprintf(temp, "%.*f", width, m._31);
    SetDlgItemText(hwnd, IDC_M31, temp);
    sprintf(temp, "%.*f", width, m._32);
    SetDlgItemText(hwnd, IDC_M32, temp);
    sprintf(temp, "%.*f", width, m._33);
    SetDlgItemText(hwnd, IDC_M33, temp);
    sprintf(temp, "%.*f", width, m._34);
    SetDlgItemText(hwnd, IDC_M34, temp);
    sprintf(temp, "%.*f", width, m._41);
    SetDlgItemText(hwnd, IDC_M41, temp);
    sprintf(temp, "%.*f", width, m._42);
    SetDlgItemText(hwnd, IDC_M42, temp);
    sprintf(temp, "%.*f", width, m._43);
    SetDlgItemText(hwnd, IDC_M43, temp);
    sprintf(temp, "%.*f", width, m._44);
    SetDlgItemText(hwnd, IDC_M44, temp);
}

void EnableMatrix(HWND hwnd, bool enable) {
    EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_EDIT_S), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_NAME), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M11), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M12), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M13), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M14), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M21), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M22), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M23), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M24), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M31), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M32), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M33), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M34), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M41), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M42), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M43), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_M44), enable);
}

void SelectMatrix(HWND hwnd, int sel) {
    updating = true;
    EnableMatrix(hwnd, sel>0);
    EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_UP_DOWN), sel>0);
    EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_DEL), sel>0);
    if (sel>0) {
        setMatrix(hwnd, c_transforms[sel-1], 10);
        SetDlgItemText(hwnd, IDC_MATRIX_NAME, c_transformnames[sel-1].c_str());
    } else {
        setMatrix(hwnd, matrixTranspose(matrixInverse(matrixMultiply(c_transforms))));
        SetDlgItemText(hwnd, IDC_MATRIX_NAME, "<Preview of the combined matrix>");
    }
    updating = false;
}

void AddMatrix(HWND hwnd, const char *name, const D3DMATRIX m, bool setcur = true) {
    long sel = SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_GETCURSEL, 0, 0);
    if (sel < 0)
        return;
    wxString sname = name;
    if (sel < c_transforms.size()) {
        c_transforms.insert(c_transforms.begin() + sel, m);
        c_transformnames.insert(c_transformnames.begin() + sel, sname);
        SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_INSERTSTRING, sel+1, (LPARAM) sname.c_str());
    } else {
        c_transforms.push_back(m);
        c_transformnames.push_back(sname);
        SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_ADDSTRING, 0, (LPARAM) sname.c_str());
    }
    if (setcur) {
        SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_SETCURSEL, sel+1, 0);
        SelectMatrix(hwnd, sel+1);
    } else
        SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_SETCURSEL, sel, 0);
    InvalidateRect(GetDlgItem(hwnd, IDC_MATRIX_LIST), NULL, true);
}

void UpdateMatrix(HWND hwnd) {
    if (updating)
        return;

    long sel = SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_GETCURSEL, 0, 0);
    if (sel<=0)
        return;
    sel--;

    unsigned long m11len, m12len, m13len, m14len, m21len, m22len, m23len, m24len, m31len,
    m32len, m33len, m34len, m41len, m42len, m43len, m44len, namelen;
    char *temp;
    namelen = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_NAME));
    m11len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M11));
    m12len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M12));
    m13len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M13));
    m14len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M14));
    m21len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M21));
    m22len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M22));
    m23len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M23));
    m24len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M24));
    m31len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M31));
    m32len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M32));
    m33len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M33));
    m34len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M34));
    m41len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M41));
    m42len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M42));
    m43len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M43));
    m44len = GetWindowTextLength(GetDlgItem(hwnd, IDC_M44));

    temp = new char[namelen + 1];
    GetDlgItemText(hwnd, IDC_MATRIX_NAME, temp, namelen + 1);
    c_transformnames[sel] = temp;
    delete[] temp;
    temp = new char[m11len + 1];
    GetDlgItemText(hwnd, IDC_M11, temp, m11len + 1);
    c_transforms[sel]._11 = (float) atof(temp);
    delete[] temp;
    temp = new char[m12len + 1];
    GetDlgItemText(hwnd, IDC_M12, temp, m12len + 1);
    c_transforms[sel]._12 = (float) atof(temp);
    delete[] temp;
    temp = new char[m13len + 1];
    GetDlgItemText(hwnd, IDC_M13, temp, m13len + 1);
    c_transforms[sel]._13 = (float) atof(temp);
    delete[] temp;
    temp = new char[m14len + 1];
    GetDlgItemText(hwnd, IDC_M14, temp, m14len + 1);
    c_transforms[sel]._14 = (float) atof(temp);
    delete[] temp;
    temp = new char[m21len + 1];
    GetDlgItemText(hwnd, IDC_M21, temp, m21len + 1);
    c_transforms[sel]._21 = (float) atof(temp);
    delete[] temp;
    temp = new char[m22len + 1];
    GetDlgItemText(hwnd, IDC_M22, temp, m22len + 1);
    c_transforms[sel]._22 = (float) atof(temp);
    delete[] temp;
    temp = new char[m23len + 1];
    GetDlgItemText(hwnd, IDC_M23, temp, m23len + 1);
    c_transforms[sel]._23 = (float) atof(temp);
    delete[] temp;
    temp = new char[m24len + 1];
    GetDlgItemText(hwnd, IDC_M24, temp, m24len + 1);
    c_transforms[sel]._24 = (float) atof(temp);
    delete[] temp;
    temp = new char[m31len + 1];
    GetDlgItemText(hwnd, IDC_M31, temp, m31len + 1);
    c_transforms[sel]._31 = (float) atof(temp);
    delete[] temp;
    temp = new char[m32len + 1];
    GetDlgItemText(hwnd, IDC_M32, temp, m32len + 1);
    c_transforms[sel]._32 = (float) atof(temp);
    delete[] temp;
    temp = new char[m33len + 1];
    GetDlgItemText(hwnd, IDC_M33, temp, m33len + 1);
    c_transforms[sel]._33 = (float) atof(temp);
    delete[] temp;
    temp = new char[m34len + 1];
    GetDlgItemText(hwnd, IDC_M34, temp, m34len + 1);
    c_transforms[sel]._34 = (float) atof(temp);
    delete[] temp;
    temp = new char[m41len + 1];
    GetDlgItemText(hwnd, IDC_M41, temp, m41len + 1);
    c_transforms[sel]._41 = (float) atof(temp);
    delete[] temp;
    temp = new char[m42len + 1];
    GetDlgItemText(hwnd, IDC_M42, temp, m42len + 1);
    c_transforms[sel]._42 = (float) atof(temp);
    delete[] temp;
    temp = new char[m43len + 1];
    GetDlgItemText(hwnd, IDC_M43, temp, m43len + 1);
    c_transforms[sel]._43 = (float) atof(temp);
    delete[] temp;
    temp = new char[m44len + 1];
    GetDlgItemText(hwnd, IDC_M44, temp, m44len + 1);
    c_transforms[sel]._44 = (float) atof(temp);
    delete[] temp;

}

BOOL CALLBACK MatrixDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG: {
        SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_RESETCONTENT, 0, 0);
        SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_ADDSTRING, 0,
                           (LPARAM) "(Insert at start)");
        for (cStringIterator si = c_transformnames.begin(); si != c_transformnames.end(); si++)
            SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_ADDSTRING, 0,
                               (LPARAM) si->c_str());
        SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_SETCURSEL, 0, 0);
        SelectMatrix(hwnd, 0);

        SendDlgItemMessage(hwnd, IDC_MATRIX_TRANS_CHOICE, CB_RESETCONTENT, 0, 0);
        SendDlgItemMessage(hwnd, IDC_MATRIX_TRANS_CHOICE, CB_ADDSTRING, 0,
                           (LPARAM) "Custom...");
        bool haspoints = false;
        for (cMeshStructIterator ms = sceneryfile->meshstructs.begin();
                ms != sceneryfile->meshstructs.end(); ms++) {
            if (ms->effectpoint) {
                SendDlgItemMessage(hwnd, IDC_MATRIX_TRANS_CHOICE, CB_ADDSTRING, 0,
                                   (LPARAM) ms->Name.c_str());
                haspoints = true;
            }
        }
        SendDlgItemMessage(hwnd, IDC_MATRIX_TRANS_CHOICE, CB_SETCURSEL, 0, 0);
        EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_CHOICE), haspoints);

        SetDlgItemText(hwnd, IDC_MATRIX_SCALE_X, "1.0");
        SetDlgItemText(hwnd, IDC_MATRIX_SCALE_Y, "1.0");
        SetDlgItemText(hwnd, IDC_MATRIX_SCALE_Z, "1.0");
    }
    break;
    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;

        if (lpdis->itemID == -1)
            break;

        switch (lpdis->CtlID) {
        case IDC_MATRIX_LIST:
            switch (lpdis->itemAction) {
            case ODA_FOCUS:
            case ODA_SELECT:
            case ODA_DRAWENTIRE:

                // Display the text associated with the item.

                //SendMessage(lpdis->hwndItem, LB_GETTEXT, lpdis->itemID, (LPARAM) tchBuffer);
                TEXTMETRIC tm;
                if (lpdis->itemState & ODS_SELECTED) {
                    FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH) GetStockObject(BLACK_BRUSH));
                    SetBkColor(lpdis->hDC, RGB(0, 0, 0));
                } else {
                    FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH) GetStockObject(WHITE_BRUSH));
                    SetBkColor(lpdis->hDC, RGB(255, 255, 255));
                }

                if (lpdis->itemID==0)
                    SetTextColor(lpdis->hDC, RGB(128, 128, 128));
                else if (lpdis->itemState & ODS_SELECTED)
                    SetTextColor(lpdis->hDC, RGB(255, 255, 255));
                else
                    SetTextColor(lpdis->hDC, RGB(0, 0, 0));


                GetTextMetrics(lpdis->hDC, &tm);

                int y = (lpdis->rcItem.bottom + lpdis->rcItem.top - tm.tmHeight) / 2;

                if (lpdis->itemID == 0)
                    TextOut(lpdis->hDC, 6, y, "(Insert at start)", 17);
                else
                    TextOut(lpdis->hDC, 6, y, c_transformnames[lpdis->itemID-1].c_str(),
                            c_transformnames[lpdis->itemID-1].length());

                if (lpdis->itemState & ODS_FOCUS) {
                    DrawFocusRect(lpdis->hDC, &lpdis->rcItem);
                }
                break;
            }
        }
    }
    break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_MATRIX_LIST: {
            long sel = SendDlgItemMessage(hwnd, IDC_MATRIX_LIST,
                                          LB_GETCURSEL, 0, 0);
            SelectMatrix(hwnd, sel);
        }
        break;
        case IDC_MATRIX_TRANS_CHOICE: {
            long sel = SendDlgItemMessage(hwnd, IDC_MATRIX_TRANS_CHOICE,
                                          CB_GETCURSEL, 0, 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_X), sel == 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_Y), sel == 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_Z), sel == 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_XS), sel == 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_YS), sel == 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_ZS), sel == 0);
            if (sel<0)
                break;

            if (sel) {
                char temp[100];
                long i = 0;
                for (cMeshStructIterator ms = sceneryfile->meshstructs.begin(); ms != sceneryfile->meshstructs.end(); ms++) {
                    if (ms->effectpoint) {
                        i++;
                        if (i == sel) {
                            sprintf(temp, "%.10f", ms->effectpoint_vert.x);
                            SetDlgItemText(hwnd, IDC_MATRIX_TRANS_X, temp);
                            sprintf(temp, "%.10f", ms->effectpoint_vert.y);
                            SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Y, temp);
                            sprintf(temp, "%.10f", ms->effectpoint_vert.z);
                            SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Z, temp);
                        }
                    }
                }
            } else {
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_X, NULL);
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Y, NULL);
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Z, NULL);
            }
        }
        break;
        case IDC_MATRIX_TRANS: {
            D3DVECTOR v;
            unsigned long len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_TRANS_X));
            char *tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_TRANS_X, tmp, len + 1);
            v.x = (float) atof(tmp);
            delete[] tmp;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_TRANS_Y));
            tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_TRANS_Y, tmp, len + 1);
            v.y = (float) atof(tmp);
            delete[] tmp;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_TRANS_Z));
            tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_TRANS_Z, tmp, len + 1);
            v.z = (float) atof(tmp);
            delete[] tmp;
            tmp = new char[256];
            snprintf(tmp, 255, "Translation by <%.4f,%.4f,%.4f>", v.x, v.y, v.z);
            AddMatrix(hwnd, tmp, matrixGetTranslation(v));
            delete[] tmp;
            long sel = SendDlgItemMessage(hwnd, IDC_MATRIX_TRANS_CHOICE,
                                          CB_GETCURSEL, 0, 0);
            if (sel<=0) {
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_X, NULL);
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Y, NULL);
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Z, NULL);
            }
        }
        break;
        case IDC_MATRIX_TRANS_ORIGIN: {
            D3DVECTOR v;
            unsigned long len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_TRANS_X));
            char *tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_TRANS_X, tmp, len + 1);
            v.x = (float) atof(tmp);
            delete[] tmp;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_TRANS_Y));
            tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_TRANS_Y, tmp, len + 1);
            v.y = (float) atof(tmp);
            delete[] tmp;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_TRANS_Z));
            tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_TRANS_Z, tmp, len + 1);
            v.z = (float) atof(tmp);
            delete[] tmp;
            tmp = new char[256];
            snprintf(tmp, 255, "[ Set Origin to <%.4f,%.4f,%.4f>", v.x, v.y, v.z);
            v = vectorInvert(v);
            AddMatrix(hwnd, tmp, matrixGetTranslation(v));
            v = vectorInvert(v);
            snprintf(tmp, 255, "] Restore Origin from <%.4f,%.4f,%.4f>", v.x, v.y, v.z);
            AddMatrix(hwnd, tmp, matrixGetTranslation(v), false);
            delete[] tmp;
            long sel = SendDlgItemMessage(hwnd, IDC_MATRIX_TRANS_CHOICE,
                                          CB_GETCURSEL, 0, 0);
            if (sel<=0) {
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_X, NULL);
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Y, NULL);
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Z, NULL);
            }
        }
        break;
        case IDC_MATRIX_ROT_X: {
            unsigned long len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_ROT));
            char *tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_ROT, tmp, len + 1);
            float ang = (float) atof(tmp);
            delete[] tmp;
            tmp = new char[256];
            snprintf(tmp, 255, "Rotate by %.1fº around the X-axis", ang);
            AddMatrix(hwnd, tmp, matrixGetRotationX(Deg2Rad(ang)));
            delete[] tmp;
        }
        break;
        case IDC_MATRIX_ROT_Y: {
            unsigned long len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_ROT));
            char *tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_ROT, tmp, len + 1);
            float ang = (float) atof(tmp);
            delete[] tmp;
            tmp = new char[256];
            snprintf(tmp, 255, "Rotate by %.1fº around the Y-axis", ang);
            AddMatrix(hwnd, tmp, matrixGetRotationY(Deg2Rad(ang)));
            delete[] tmp;
        }
        break;
        case IDC_MATRIX_ROT_Z: {
            unsigned long len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_ROT));
            char *tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_ROT, tmp, len + 1);
            float ang = (float) atof(tmp);
            delete[] tmp;
            tmp = new char[256];
            snprintf(tmp, 255, "Rotate by %.1fº around the Z-axis", ang);
            AddMatrix(hwnd, tmp, matrixGetRotationZ(Deg2Rad(ang)));
            delete[] tmp;
        }
        break;
        case IDC_MATRIX_SCALE: {
            D3DVECTOR v;
            unsigned long len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_SCALE_X));
            char *tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_SCALE_X, tmp, len + 1);
            v.x = (float) atof(tmp);
            delete[] tmp;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_SCALE_Y));
            tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_SCALE_Y, tmp, len + 1);
            v.y = (float) atof(tmp);
            delete[] tmp;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_SCALE_Z));
            tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_SCALE_Z, tmp, len + 1);
            v.z = (float) atof(tmp);
            delete[] tmp;
            tmp = new char[256];
            snprintf(tmp, 255, "Scale by <%.4f,%.4f,%.4f>", v.x, v.y, v.z);
            AddMatrix(hwnd, tmp, matrixGetScale(v));
            delete[] tmp;
            SetDlgItemText(hwnd, IDC_MATRIX_SCALE_X, "1.0");
            SetDlgItemText(hwnd, IDC_MATRIX_SCALE_Y, "1.0");
            SetDlgItemText(hwnd, IDC_MATRIX_SCALE_Z, "1.0");
        }
        break;
        case IDC_MATRIX_MIRROR_X: {
            AddMatrix(hwnd, "Mirror along the X-axis", matrixGetScale(-1.0, 1.0, 1.0));
        }
        break;
        case IDC_MATRIX_MIRROR_Y: {
            AddMatrix(hwnd, "Mirror along the Y-axis", matrixGetScale(1.0, -1.0, 1.0));
        }
        break;
        case IDC_MATRIX_MIRROR_Z: {
            AddMatrix(hwnd, "Mirror along the Z-axis", matrixGetScale(1.0, 1.0, -1.0));
        }
        break;
        case IDC_MATRIX_UNITY: {
            AddMatrix(hwnd, "Unity (Null Transform)", matrixGetUnity());
        }
        break;
        case IDC_MATRIX_FIX_ORIENT: {
            AddMatrix(hwnd, "Legacy Fix Orientation", matrixGetLegacyFixOrientation());
        }
        break;
        case IDC_MATRIX_DEFAULT: {
            AddMatrix(hwnd, "Default Matrix", g_config->m_def_matrix);
        }
        break;
        case IDC_MATRIX_LOAD: {
            OPENFILENAME ofn;
            char sfile[MAX_PATH]="";
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Scenery Files (*.scn)\0*.scn\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open Scenery File (Load single matrix)";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = ".scn";
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                cSCNFile *texscn = new cSCNFile(sfile);
                if (texscn) {
                    if ((texscn->effectpoints.size()==0) && (texscn->transforms.size()==0)) {
                        MessageBox(hwnd, "The selected scenery file contains no matrices.", "Warning", MB_OK | MB_ICONWARNING);
                        delete texscn;
                        break;
                    }
                    std::vector<D3DMATRIX> matchoices;
                    unsigned int i;
                    if (texscn->transforms.size()!=0) {
                        if ((texscn->transforms.size()>1) && (!matrixIsEqual(matrixMultiply(texscn->transforms), matrixGetUnity()))) {
                            c_choices.push_back("Model Transforms, combined");
                            matchoices.push_back(matrixMultiply(texscn->transforms));
                        }
                        for (i = 0; i < texscn->transforms.size(); i++) {
                            c_choices.push_back("Model Transforms -> "+texscn->transformnames[i]);
                            matchoices.push_back(texscn->transforms[i]);
                        }
                    }
                    for (cEffectPointIterator ep = texscn->effectpoints.begin(); ep != texscn->effectpoints.end(); ep++) {
                        if (ep->Transform.size()!=0) {
                            if ((ep->Transform.size()>1) && (!matrixIsEqual(matrixMultiply(ep->Transform), matrixGetUnity()))) {
                                c_choices.push_back(ep->Name+" Transforms, combined");
                                matchoices.push_back(matrixMultiply(ep->Transform));
                            }
                            for (i = 0; i < ep->Transform.size(); i++) {
                                c_choices.push_back(ep->Name+" Transforms -> "+ep->TransformNames[i]);
                                matchoices.push_back(ep->Transform[i]);
                            }
                        }
                    }
                    if (c_choices.size() > 0) {
                        std::string title = "Choose matrix to import";
                        int choice = DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EFFECT_PARTICLE), hwnd, ChoiceDlgProc, (LPARAM) title.c_str());
                        if (choice > 0) {
                            choice--;
                            AddMatrix(hwnd, c_choices[choice].c_str(), matchoices[choice]);
                        }
                        c_choices.clear();
                    }
                    delete texscn;
                }
            }
        }
        break;
        case IDC_MATRIX_LOAD_ALL: {
            if (c_transforms.size()) {
                if (MessageBox(hwnd, "This will replace your current matrix stack.\nDo you want to continue?", "Question", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)==IDNO)
                    break;
            }
            OPENFILENAME ofn;
            char sfile[MAX_PATH]="";
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Scenery Files (*.scn)\0*.scn\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open Scenery File (Load matrix stack)";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = ".scn";
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                cSCNFile *texscn = new cSCNFile(sfile);
                if (texscn) {
                    if ((texscn->effectpoints.size()==0) && (texscn->transforms.size()==0)) {
                        MessageBox(hwnd, "The selected scenery file contains no matrices.", "Warning", MB_OK | MB_ICONWARNING);
                        delete texscn;
                        break;
                    }
                    std::vector< std::vector<D3DMATRIX> > matchoices;
                    std::vector< wxArrayString > matnames;
                    unsigned int i;
                    if (texscn->transforms.size()!=0) {
                        c_choices.push_back("Model Transforms");
                        matchoices.push_back(texscn->transforms);
                        matnames.push_back(texscn->transformnames);
                    }
                    for (cEffectPointIterator ep = texscn->effectpoints.begin(); ep != texscn->effectpoints.end(); ep++) {
                        if (ep->Transform.size()!=0) {
                            c_choices.push_back(ep->Name);
                            matchoices.push_back(ep->Transform);
                            matnames.push_back(ep->TransformNames);
                        }
                    }
                    if (c_choices.size() > 0) {
                        std::string title = "Choose matrix stack to import";
                        int choice = DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EFFECT_PARTICLE), hwnd, ChoiceDlgProc, (LPARAM) title.c_str());
                        if (choice > 0) {
                            choice--;
                            SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_RESETCONTENT, 0, 0);
                            SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_ADDSTRING, 0,
                                               (LPARAM) "(Insert at start)");
                            c_transformnames.clear();
                            c_transforms.clear();

                            c_transformnames = matnames[choice];
                            c_transforms = matchoices[choice];

                            for (cStringIterator si = c_transformnames.begin(); si != c_transformnames.end(); si++)
                                SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_ADDSTRING, 0,
                                                   (LPARAM) si->c_str());

                            SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_SETCURSEL, 0, 0);
                            SelectMatrix(hwnd, 0);
                        }
                        c_choices.clear();
                    }
                    delete texscn;
                }
            }
        }
        break;
        case IDC_MATRIX_CLEAR_ALL: {
            SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_RESETCONTENT, 0, 0);
            SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_ADDSTRING, 0,
                               (LPARAM) "(Insert at start)");
            c_transformnames.clear();
            c_transforms.clear();
            SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_SETCURSEL, 0, 0);
            SelectMatrix(hwnd, 0);
        }
        break;
        case IDC_MATRIX_DEL: {
            long sel = SendDlgItemMessage(hwnd, IDC_MATRIX_LIST,
                                          LB_GETCURSEL, 0, 0);
            SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_DELETESTRING, sel, 0);
            sel--;
            c_transforms.erase(c_transforms.begin() + sel);
            c_transformnames.erase(c_transformnames.begin() + sel);
            SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_SETCURSEL, sel, 0);
            SelectMatrix(hwnd, sel);
            InvalidateRect(GetDlgItem(hwnd, IDC_MATRIX_LIST), NULL, true);
        }
        break;
        case IDC_MATRIX_NAME:
        case IDC_M11:
        case IDC_M12:
        case IDC_M13:
        case IDC_M14:
        case IDC_M21:
        case IDC_M22:
        case IDC_M23:
        case IDC_M24:
        case IDC_M31:
        case IDC_M32:
        case IDC_M33:
        case IDC_M34:
        case IDC_M41:
        case IDC_M42:
        case IDC_M43:
        case IDC_M44: {
            UpdateMatrix(hwnd);
            InvalidateRect(GetDlgItem(hwnd, IDC_MATRIX_LIST), NULL, true);
        }
        break;
        case IDOK:
            EndDialog(hwnd, 1);
            break;
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        default:
            break;
        }
        break;
    case WM_NOTIFY: {
        LPNMUPDOWN lpnmup = (LPNMUPDOWN) lParam;
        if (lpnmup->hdr.code == UDN_DELTAPOS) {
            long sel = SendDlgItemMessage(hwnd, IDC_MATRIX_LIST,
                                          LB_GETCURSEL, 0, 0);
            bool shiftpressed = (GetKeyState(VK_SHIFT)&0x80);
            if (lpnmup->iDelta > 0) {
                //MessageBox(hwnd, "Down", "Down", MB_OK);
                if ((sel<=0) || (sel>=c_transforms.size()))
                    break;
                if (shiftpressed) {
                    D3DMATRIX temp = c_transforms[sel];
                    matrixMultiplyIP(&c_transforms[sel-1], &temp);
                    c_transforms.erase(c_transforms.begin() + sel);
                    wxString temps = c_transformnames[sel];
                    c_transformnames[sel-1] += " & " + temps;
                    c_transformnames.erase(c_transformnames.begin() + sel);
                    SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_DELETESTRING, sel+1, 0);
                    SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_SETCURSEL, sel, 0);
                    SelectMatrix(hwnd, sel);
                } else {
                    D3DMATRIX temp = c_transforms[sel-1];
                    c_transforms.erase(c_transforms.begin() + sel-1);
                    c_transforms.insert(c_transforms.begin() + sel, temp);
                    wxString temps = c_transformnames[sel-1];
                    c_transformnames.erase(c_transformnames.begin() + sel-1);
                    c_transformnames.insert(c_transformnames.begin() + sel, temps);
                    SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_SETCURSEL, sel+1, 0);
                    SelectMatrix(hwnd, sel+1);
                }
                InvalidateRect(GetDlgItem(hwnd, IDC_MATRIX_LIST), NULL, true);
            } else {
                //MessageBox(hwnd, "Up", "Up", MB_OK);
                if (sel<=1)
                    break;
                if (shiftpressed) {
                    D3DMATRIX temp = c_transforms[sel-1];
                    c_transforms.erase(c_transforms.begin() + sel-1);
                    matrixMultiplyIP(&c_transforms[sel-2], &temp);
                    wxString temps = c_transformnames[sel-1];
                    c_transformnames.erase(c_transformnames.begin() + sel-1);
                    c_transformnames[sel-2] += " & " + temps;
                    SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_DELETESTRING, sel, 0);
                    SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_SETCURSEL, sel-1, 0);
                    SelectMatrix(hwnd, sel-1);
                    InvalidateRect(GetDlgItem(hwnd, IDC_MATRIX_LIST), NULL, true);
                } else {
                    D3DMATRIX temp = c_transforms[sel-1];
                    c_transforms.erase(c_transforms.begin() + sel-1);
                    c_transforms.insert(c_transforms.begin() + sel-2, temp);
                    wxString temps = c_transformnames[sel-1];
                    c_transformnames.erase(c_transformnames.begin() + sel-1);
                    c_transformnames.insert(c_transformnames.begin() + sel-2, temps);
                    SendDlgItemMessage(hwnd, IDC_MATRIX_LIST, LB_SETCURSEL, sel-1, 0);
                    SelectMatrix(hwnd, sel-1);
                    InvalidateRect(GetDlgItem(hwnd, IDC_MATRIX_LIST), NULL, true);
                }
            }
            return TRUE;
        }
    }
    break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK EffectPointParticleDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG: {
        SetWindowText(hwnd, "Create Particle Effect Point");
        SendDlgItemMessage(hwnd, IDC_EFFECT_PARTICLE_LIST, LB_RESETCONTENT, 0, 0);
        SendDlgItemMessage(hwnd, IDC_EFFECT_PARTICLE_LIST, LB_ADDSTRING, 0,
                           (LPARAM) "(Enter Manually)");
        HKEY key;
        LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{907B4640-266B-4A21-92FB-CD1A86CD0F63}",
                                0, KEY_QUERY_VALUE, &key);
        char InstallLocation[MAX_PATH] = "";
        char Location[MAX_PATH] = "";
        unsigned long length = MAX_PATH;
//        unsigned long i;
        res =
            RegQueryValueEx(key, "InstallLocation", 0, NULL, (LPBYTE) InstallLocation, &length);
        res = RegCloseKey(key);
        sprintf(Location, "%s\\ParticleEffects\\", InstallLocation);
//        int n;
        DIR *d = opendir(Location);
        struct dirent *e;
        while (e = readdir(d)) {
            char t[MAX_PATH];
            char ex[MAX_PATH];
            _splitpath(e->d_name, NULL, NULL, t, ex);
            if (!stricmp(ex, ".frw"))
                SendDlgItemMessage(hwnd, IDC_EFFECT_PARTICLE_LIST, LB_ADDSTRING, 0, (LPARAM) t);
        }
        closedir(d);
        SendDlgItemMessage(hwnd, IDC_EFFECT_PARTICLE_LIST, LB_SETCURSEL, 0, 0);
    }
    break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            long sel = SendDlgItemMessage(hwnd, IDC_EFFECT_PARTICLE_LIST,
                                          LB_GETCURSEL, 0, 0);
            char t[MAX_PATH + 18];
            strcpy(t, "particlestart01_");
            if (sel <= 0)
                strcat(t, "enternameofparticleeffecthere");
            else {
                char t2[MAX_PATH];
                SendDlgItemMessage(hwnd, IDC_EFFECT_PARTICLE_LIST, LB_GETTEXT, sel,
                                   (LPARAM) t2);
                for (unsigned int i = 0; i < strlen(t2); i++)
                    t2[i] = tolower(t2[i]);
                strcat(t, t2);
            }
            SetWindowText(GetDlgItem(GetParent(hwnd), IDC_NAME), t);
            EndDialog(hwnd, 1);
        }
        break;
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        default:
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void MakeLight(HWND hdlg, HWND htarget) {
    char temp[256];
    int f = 0;
    long sel = SendDlgItemMessage(hdlg, IDC_LIGHT_TYPE, CB_GETCURSEL, 0, 0);
    switch (sel) {
    case 1:
        strcpy(temp, "simplelightstart01");
        f = 16;
        break;
    case 2:
        strcpy(temp, "nblightstart01");
        f = 12;
        break;
    default:
        strcpy(temp, "lightstart01");
        f = 10;
        break;
    }
    sel = SendDlgItemMessage(hdlg, IDC_LIGHT_OPTION, CB_GETCURSEL, 0, 0);
    switch (sel) {
    case 1:
        strcat(temp, "_lamp");
        break;
    case 2:
        strcat(temp, "_torch");
        break;
    case 3:
        strcat(temp, "_flexi0");
        break;
    case 4:
        strcat(temp, "_flexi1");
        break;
    case 5:
        strcat(temp, "_flexi2");
        break;
    default:
        sel = 0;
        break;
    }
//    int l = 0;
    char ptemp[5];
    if (sel < 3) {
        GetDlgItemText(hdlg, IDC_LIGHT_COLOR_R, ptemp, 4);
        strcat(temp, "_r");
        if (!strcmp(ptemp, ""))
            strncat(temp, "0", 255);
        else
            strncat(temp, ptemp, 255);
        strncat(temp, "g", 255);
        GetDlgItemText(hdlg, IDC_LIGHT_COLOR_G, ptemp, 4);
        if (!strcmp(ptemp, ""))
            strncat(temp, "0", 255);
        else
            strncat(temp, ptemp, 255);
        strncat(temp, "b", 255);
        GetDlgItemText(hdlg, IDC_LIGHT_COLOR_B, ptemp, 4);
        if (!strcmp(ptemp, ""))
            strncat(temp, "0", 255);
        else
            strncat(temp, ptemp, 255);
    }
    strncat(temp, "_radius", 255);
    GetDlgItemText(hdlg, IDC_LIGHT_COLOR_RADIUS, ptemp, 4);
    if (!strcmp(ptemp, ""))
        strncat(temp, "0", 255);
    else
        strncat(temp, ptemp, 255);
    if (SendDlgItemMessage(hdlg, IDC_LIGHT_COLOR_CM, BM_GETCHECK, 0, 0))
        strncat(temp, "cm", 255);
    SetWindowText(htarget, temp);
    SendMessage(htarget, EM_SETSEL, f, f + 2);
}

BOOL CALLBACK EffectPointLightDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG: {
        SendDlgItemMessage(hwnd, IDC_LIGHT_TYPE, CB_RESETCONTENT, 0, 0);
        SendDlgItemMessage(hwnd, IDC_LIGHT_TYPE, CB_ADDSTRING, 0, (LPARAM) "Normal");
        SendDlgItemMessage(hwnd, IDC_LIGHT_TYPE, CB_ADDSTRING, 0, (LPARAM) "Simple");
        SendDlgItemMessage(hwnd, IDC_LIGHT_TYPE, CB_ADDSTRING, 0, (LPARAM) "No Bulb");
        SendDlgItemMessage(hwnd, IDC_LIGHT_TYPE, CB_SETCURSEL, 0, 0);
        CheckDlgButton(hwnd, IDC_LIGHT_SHINE, true);
        CheckDlgButton(hwnd, IDC_LIGHT_BULB, true);
        SendDlgItemMessage(hwnd, IDC_LIGHT_OPTION, CB_RESETCONTENT, 0, 0);
        SendDlgItemMessage(hwnd, IDC_LIGHT_OPTION, CB_ADDSTRING, 0, (LPARAM) "None");
        SendDlgItemMessage(hwnd, IDC_LIGHT_OPTION, CB_ADDSTRING, 0, (LPARAM) "Lamp");
        SendDlgItemMessage(hwnd, IDC_LIGHT_OPTION, CB_ADDSTRING, 0, (LPARAM) "Torch");
        SendDlgItemMessage(hwnd, IDC_LIGHT_OPTION, CB_ADDSTRING, 0, (LPARAM) "Custom Color 1");
        SendDlgItemMessage(hwnd, IDC_LIGHT_OPTION, CB_ADDSTRING, 0, (LPARAM) "Custom Color 2");
        SendDlgItemMessage(hwnd, IDC_LIGHT_OPTION, CB_ADDSTRING, 0, (LPARAM) "Custom Color 3");
        SendDlgItemMessage(hwnd, IDC_LIGHT_OPTION, CB_SETCURSEL, 0, 0);
        SetDlgItemText(hwnd, IDC_LIGHT_COLOR_R, "255");
        SetDlgItemText(hwnd, IDC_LIGHT_COLOR_G, "255");
        SetDlgItemText(hwnd, IDC_LIGHT_COLOR_B, "255");
        SetDlgItemText(hwnd, IDC_LIGHT_COLOR_RADIUS, "10");
        CheckDlgButton(hwnd, IDC_LIGHT_COLOR_CM, false);
        MakeLight(hwnd, GetDlgItem(hwnd, IDC_LIGHT_PREVIEW));
    }
    break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_LIGHT_TYPE: {
            long sel = SendDlgItemMessage(hwnd, IDC_LIGHT_TYPE, CB_GETCURSEL,
                                          0, 0);
            CheckDlgButton(hwnd, IDC_LIGHT_SHINE, (sel == 0)
                           || (sel == 2));
            CheckDlgButton(hwnd, IDC_LIGHT_BULB, (sel == 0)
                           || (sel == 1));

            MakeLight(hwnd, GetDlgItem(hwnd, IDC_LIGHT_PREVIEW));
        }
        break;
        case IDC_LIGHT_SHINE:
            MakeLight(hwnd, GetDlgItem(hwnd, IDC_LIGHT_PREVIEW));
            break;
        case IDC_LIGHT_BULB:
            MakeLight(hwnd, GetDlgItem(hwnd, IDC_LIGHT_PREVIEW));
            break;
        case IDC_LIGHT_OPTION:
            MakeLight(hwnd, GetDlgItem(hwnd, IDC_LIGHT_PREVIEW));
            break;
        case IDC_LIGHT_COLOR: {
            CHOOSECOLOR cc;
            char ptemp[5];
            GetDlgItemText(hwnd, IDC_LIGHT_COLOR_R, ptemp, 4);
            int r = atoi(ptemp);
            GetDlgItemText(hwnd, IDC_LIGHT_COLOR_G, ptemp, 4);
            int g = atoi(ptemp);
            GetDlgItemText(hwnd, IDC_LIGHT_COLOR_B, ptemp, 4);
            int b = atoi(ptemp);
            ZeroMemory(&cc, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.rgbResult = RGB(r, g, b);
            cc.lpCustColors = (COLORREF *) & g_colors;
            cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc)) {
                snprintf(ptemp, 4, "%d", GetRValue(cc.rgbResult));
                SetDlgItemText(hwnd, IDC_LIGHT_COLOR_R, ptemp);
                snprintf(ptemp, 4, "%d", GetGValue(cc.rgbResult));
                SetDlgItemText(hwnd, IDC_LIGHT_COLOR_G, ptemp);
                snprintf(ptemp, 4, "%d", GetBValue(cc.rgbResult));
                SetDlgItemText(hwnd, IDC_LIGHT_COLOR_B, ptemp);
                MakeLight(hwnd, GetDlgItem(hwnd, IDC_LIGHT_PREVIEW));
            }
        }
        break;
        case IDC_LIGHT_COLOR_R:
            MakeLight(hwnd, GetDlgItem(hwnd, IDC_LIGHT_PREVIEW));
            break;
        case IDC_LIGHT_COLOR_G:
            MakeLight(hwnd, GetDlgItem(hwnd, IDC_LIGHT_PREVIEW));
            break;
        case IDC_LIGHT_COLOR_B:
            MakeLight(hwnd, GetDlgItem(hwnd, IDC_LIGHT_PREVIEW));
            break;
        case IDC_LIGHT_COLOR_RADIUS:
            MakeLight(hwnd, GetDlgItem(hwnd, IDC_LIGHT_PREVIEW));
            break;
        case IDC_LIGHT_COLOR_CM:
            MakeLight(hwnd, GetDlgItem(hwnd, IDC_LIGHT_PREVIEW));
            break;
        case IDOK:
            MakeLight(hwnd, GetDlgItem(GetParent(hwnd), IDC_NAME));
            EndDialog(hwnd, 1);
            break;
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        default:
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK EffectPointDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG: {
        if (CurrentEffectPoint != -1) {
            cEffectPoint e = sceneryfile->effectpoints[CurrentEffectPoint];
            D3DMATRIX mat = matrixMultiply(e.Transform);
            setMatrix(hwnd, mat);
            SetDlgItemText(hwnd, IDC_NAME, e.Name.c_str());
            c_effecttransforms = e.Transform;
            c_effecttransformnames = e.TransformNames;
        } else {
            c_effecttransforms.clear();
            c_effecttransformnames.clear();
            setMatrix(hwnd, matrixGetUnity());
        }
        BOOL haspoints = false;

        SendDlgItemMessage(hwnd, IDC_EFFECT_POINTS_CB, CB_RESETCONTENT, 0, 0);
        SendDlgItemMessage(hwnd, IDC_EFFECT_POINTS_CB, CB_ADDSTRING, 0,
                           (LPARAM) "Custom...");
        for (cMeshStructIterator ms = sceneryfile->meshstructs.begin();
                ms != sceneryfile->meshstructs.end(); ms++) {
            if (ms->effectpoint) {
                SendDlgItemMessage(hwnd, IDC_EFFECT_POINTS_CB, CB_ADDSTRING, 0,
                                   (LPARAM) ms->Name.c_str());
                haspoints = true;
            }
        }
        SendDlgItemMessage(hwnd, IDC_EFFECT_POINTS_CB, CB_SETCURSEL, 0, 0);
        EnableWindow(GetDlgItem(hwnd, IDC_EFFECT_POINTS_CB), haspoints);

    }
    break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        case IDC_LOAD: {
            OPENFILENAME ofn;
            char sfile[MAX_PATH]="";
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Scenery Files (*.scn)\0*.scn\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open Scenery File (Load effect point settings)";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = ".scn";
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                cSCNFile *texscn = new cSCNFile(sfile);
                if (texscn) {
                    if (texscn->effectpoints.size()==0) {
                        MessageBox(hwnd, "The selected scenery file contains no effect points.", "Warning", MB_OK | MB_ICONWARNING);
                        delete texscn;
                        break;
                    }
                    for (cEffectPointIterator ms = texscn->effectpoints.begin(); ms != texscn->effectpoints.end(); ms++) {
                        c_choices.push_back(ms->Name);
                    }
                    if (c_choices.size() > 0) {
                        std::string title = "Choose effect point settings to import";
                        int choice = DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EFFECT_PARTICLE), hwnd, ChoiceDlgProc, (LPARAM) title.c_str());
                        if (choice > 0) {
                            choice--;
                            D3DMATRIX mat = matrixMultiply(texscn->effectpoints[choice].Transform);
                            setMatrix(hwnd, mat);
                            SetDlgItemText(hwnd, IDC_NAME, texscn->effectpoints[choice].Name.c_str());
                            c_effecttransforms = texscn->effectpoints[choice].Transform;
                            c_effecttransformnames = texscn->effectpoints[choice].TransformNames;
                        }
                        c_choices.clear();
                    }
                    delete texscn;
                }
            }
        }
        break;
        case IDC_EFFECT_POINTS_CB: {
            long sel = SendDlgItemMessage(hwnd, IDC_EFFECT_POINTS_CB,
                                          CB_GETCURSEL, 0, 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_X), sel == 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_Y), sel == 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_Z), sel == 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_XS), sel == 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_YS), sel == 0);
            EnableWindow(GetDlgItem(hwnd, IDC_MATRIX_TRANS_ZS), sel == 0);
            if (sel<0)
                break;

            if (sel) {
                char temp[100];
                long i = 0;
                for (cMeshStructIterator ms = sceneryfile->meshstructs.begin(); ms != sceneryfile->meshstructs.end(); ms++) {
                    if (ms->effectpoint) {
                        i++;
                        if (i == sel) {
                            sprintf(temp, "%.10f", ms->effectpoint_vert.x);
                            SetDlgItemText(hwnd, IDC_MATRIX_TRANS_X, temp);
                            sprintf(temp, "%.10f", ms->effectpoint_vert.y);
                            SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Y, temp);
                            sprintf(temp, "%.10f", ms->effectpoint_vert.z);
                            SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Z, temp);
                        }
                    }
                }
            } else {
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_X, NULL);
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Y, NULL);
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Z, NULL);
            }
        }
        break;
        case IDC_EFFECT_POINTS_AUTO: {
            if (c_effecttransforms.size()>0) {
                if (MessageBox(hwnd, "This will reset your transformation matrix stack.\nDo you want to continue?", "Warning", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2)==IDNO)
                    break;
            }
            D3DVECTOR v;
            unsigned long len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_TRANS_X));
            char *tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_TRANS_X, tmp, len + 1);
            v.x = (float) atof(tmp);
            delete[] tmp;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_TRANS_Y));
            tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_TRANS_Y, tmp, len + 1);
            v.y = (float) atof(tmp);
            delete[] tmp;
            len = GetWindowTextLength(GetDlgItem(hwnd, IDC_MATRIX_TRANS_Z));
            tmp = new char[len+1];
            GetDlgItemText(hwnd, IDC_MATRIX_TRANS_Z, tmp, len + 1);
            v.z = (float) atof(tmp);
            delete[] tmp;
            tmp = new char[256];
            snprintf(tmp, 255, "Translation by <%.4f,%.4f,%.4f>", v.x, v.y, v.z);
            c_effecttransforms.clear();
            c_effecttransforms.push_back(matrixGetTranslation(v));
            c_effecttransformnames.clear();
            c_effecttransformnames.push_back(tmp);
            delete[] tmp;
            setMatrix(hwnd, matrixMultiply(c_effecttransforms));
            long sel = SendDlgItemMessage(hwnd, IDC_EFFECT_POINTS_CB,
                                          CB_GETCURSEL, 0, 0);
            if (sel<=0) {
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_X, NULL);
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Y, NULL);
                SetDlgItemText(hwnd, IDC_MATRIX_TRANS_Z, NULL);
            }
        }
        break;
        case IDC_MATRIX_CLEAR: {
            if (c_effecttransforms.size()>0) {
                if (MessageBox(hwnd, "This will reset your transformation matrix stack.\nDo you want to continue?", "Warning", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2)==IDNO)
                    break;
            }
            c_effecttransforms.clear();
            c_effecttransformnames.clear();
            setMatrix(hwnd, matrixMultiply(c_effecttransforms));
        }
        break;
        case IDC_MATRIX_EDIT:
            c_transforms = c_effecttransforms;
            c_transformnames = c_effecttransformnames;
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_MATRIX), hwnd, MatrixDlgProc) == 1) {
                c_effecttransforms = c_transforms;
                c_effecttransformnames = c_transformnames;
                setMatrix(hwnd, matrixMultiply(c_effecttransforms));
            }
            break;
        case IDC_EFFECT_POINTS_CREATE: {
            HMENU pu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_EFFECTPOINT));
            POINT pos;
            GetCursorPos(&pos);
            TrackPopupMenuEx(GetSubMenu(pu, 0), TPM_LEFTALIGN, pos.x, pos.y, hwnd, NULL);
        }
        break;
        case IDM_EFFECT_LIGHT_START:
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_EFFECT_LIGHT), hwnd, EffectPointLightDlgProc)
                    == 1)
                SetFocus(GetDlgItem(hwnd, IDC_NAME));
            break;
        case IDM_EFFECT_PARTICLE_START:
            if (DialogBox
                    (hInst, MAKEINTRESOURCE(IDD_EFFECT_PARTICLE), hwnd,
                     EffectPointParticleDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_NAME, EM_SETSEL, 13, 15);
                SetFocus(GetDlgItem(hwnd, IDC_NAME));
            }
            break;
        case IDM_EFFECT_PARTICLE_END:
            SetDlgItemText(hwnd, IDC_NAME, "particleend01");
            SendDlgItemMessage(hwnd, IDC_NAME, EM_SETSEL, 11, 13);
            SetFocus(GetDlgItem(hwnd, IDC_NAME));
            break;
        case IDM_EFFECT_LAUNCHER_START:
            SetDlgItemText(hwnd, IDC_NAME, "fireworkstart01");
            SendDlgItemMessage(hwnd, IDC_NAME, EM_SETSEL, 13, 15);
            SetFocus(GetDlgItem(hwnd, IDC_NAME));
            break;
        case IDM_EFFECT_LAUNCHER_END:
            SetDlgItemText(hwnd, IDC_NAME, "fireworkend01");
            SendDlgItemMessage(hwnd, IDC_NAME, EM_SETSEL, 11, 13);
            SetFocus(GetDlgItem(hwnd, IDC_NAME));
            break;
        case IDM_EFFECT_PEEP_1:
            SetDlgItemText(hwnd, IDC_NAME, "peep01");
            SendDlgItemMessage(hwnd, IDC_NAME, EM_SETSEL, 4, 6);
            SetFocus(GetDlgItem(hwnd, IDC_NAME));
            break;
        case IDM_EFFECT_PEEP_VENDOR:
            SetDlgItemText(hwnd, IDC_NAME, "vendor");
            break;
        case IDM_EFFECT_FOUNTAIN_NOZBL:
            SetDlgItemText(hwnd, IDC_NAME, "nozbl");
            break;
        case IDM_EFFECT_FOUNTAIN_NOZBR:
            SetDlgItemText(hwnd, IDC_NAME, "nozbr");
            break;
        case IDM_EFFECT_FOUNTAIN_NOZTL:
            SetDlgItemText(hwnd, IDC_NAME, "noztl");
            break;
        case IDM_EFFECT_FOUNTAIN_NOZTR:
            SetDlgItemText(hwnd, IDC_NAME, "noztr");
            break;
        case IDOK: {
            if (CurrentEffectPoint == -1) {
                cEffectPoint en;
                sceneryfile->effectpoints.push_back(en);
                CurrentEffectPoint = sceneryfile->effectpoints.size() - 1;
            }
            cEffectPoint *e = &sceneryfile->effectpoints[CurrentEffectPoint];

            unsigned long namelen = GetWindowTextLength(GetDlgItem(hwnd, IDC_NAME));
            char *tmp = new char[namelen+1];
            GetDlgItemText(hwnd, IDC_NAME, tmp, namelen + 1);
            e->Name = tmp;
            delete[] tmp;
            e->Transform = c_effecttransforms;
            e->TransformNames = c_effecttransformnames;
            EndDialog(hwnd, 1);
        }
        break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void SetMeshSettings(HWND hwnd, cMeshStruct *ms) {
    CheckDlgButton(hwnd, IDC_DISABLE, ms->disabled);
    if (ms->disabled) {
        EnableWindow(GetDlgItem(hwnd, IDC_PLACE), false);
        EnableWindow(GetDlgItem(hwnd, IDC_FLAGS), false);
        EnableWindow(GetDlgItem(hwnd, IDC_UNKNOWN), false);
        EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
        EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
    }
    SendDlgItemMessage(hwnd, IDC_PLACE, CB_SETCURSEL, ms->place, 0);
    if (ms->flags == 0) {
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_SETCURSEL, 0, 0);
    }
    if (ms->flags == 12) {
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_SETCURSEL, 1, 0);
    }
    if (ms->flags == 12288) {
        EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
        EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_SETCURSEL, 2, 0);
    }
    if (ms->flags == 20480) {
        EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
        EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_SETCURSEL, 3, 0);
    }
    if (ms->flags == 32788) {
        EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
        EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_SETCURSEL, 6, 0);
    }
    if (ms->FTX != "") {
        if (stricmp(ms->FTX.c_str(), "siwater") == 0) {
            EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
            EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
            SendDlgItemMessage(hwnd, IDC_FLAGS, CB_SETCURSEL, 4, 0);
        }
        if (stricmp(ms->FTX.c_str(), "watermask")
                == 0) {
            EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
            EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
            SendDlgItemMessage(hwnd, IDC_FLAGS, CB_SETCURSEL, 5, 0);
        }
    }
    if (ms->unknown == 1) {
        SendDlgItemMessage(hwnd, IDC_UNKNOWN, CB_SETCURSEL, 0, 0);
    }
    if (ms->unknown == 3) {
        SendDlgItemMessage(hwnd, IDC_UNKNOWN, CB_SETCURSEL, 1, 0);
    }
    if (ms->TXS != "") {
        SendDlgItemMessage(hwnd, IDC_TXS, CB_SELECTSTRING, (WPARAM) - 1,
                           (LPARAM) ms->TXS.c_str());
        SetDlgItemText(hwnd, IDC_TXS, ms->TXS.c_str());
    } else {
        SendDlgItemMessage(hwnd, IDC_TXS, CB_SETCURSEL, (WPARAM) - 1, 0);
    }
    if (ms->FTX != "") {
        SendDlgItemMessage(hwnd, IDC_FTX, CB_SELECTSTRING, (WPARAM) - 1,
                           (LPARAM) ms->FTX.c_str());
        SetDlgItemText(hwnd, IDC_FTX, ms->FTX.c_str());
    } else {
        SendDlgItemMessage(hwnd, IDC_FTX, CB_SETCURSEL, (WPARAM) - 1, 0);
    }

}

BOOL CALLBACK MeshDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG: {
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaque");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueChrome");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueChromeModulate");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueGlossReflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueGlossSpecular100");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueGlossSpecular50");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueReflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueSpecular100");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0,
                           (LPARAM) "SIOpaqueSpecular100Reflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueSpecular50");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueSpecular50Reflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueText");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueUnlit");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIGlass");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlpha");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaChrome");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMask");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskChrome");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskLow");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskLowSpecular100");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0,
                           (LPARAM) "SIAlphaMaskLowSpecular100Reflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskLowSpecular50");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0,
                           (LPARAM) "SIAlphaMaskLowSpecular50Reflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskReflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskSpecular100");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0,
                           (LPARAM) "SIAlphaMaskSpecular100Reflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskSpecular50");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0,
                           (LPARAM) "SIAlphaMaskSpecular50Reflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskUnlit");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskUnlitLow");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaReflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaSpecular100");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaSpecular100Reflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaSpecular50");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaSpecular50Reflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaText");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaUnlit");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIFillZ");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskCaustics");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskLowCaustics");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIEmissiveMask");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIEmissiveMaskReflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIEmissiveMaskSpecular100");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0,
                           (LPARAM) "SIEmissiveMaskSpecular100Reflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIEmissiveMaskSpecular50");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0,
                           (LPARAM) "SIEmissiveMaskSpecular50Reflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueCaustics");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0,
                           (LPARAM) "SIOpaqueCausticsGlossSpecular100");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueCausticsReflection");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueCausticsSpecular100");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueCausticsSpecular50");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIWater");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIOpaqueDS");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaDS");
        SendDlgItemMessage(hwnd, IDC_TXS, CB_ADDSTRING, 0, (LPARAM) "SIAlphaMaskDS");
        SendDlgItemMessage(hwnd, IDC_PLACE, CB_ADDSTRING, 0,
                           (LPARAM) "Do Not Show Texture When Placing");
        SendDlgItemMessage(hwnd, IDC_PLACE, CB_ADDSTRING, 0, (LPARAM) "Show Texture When Placing");
        SendDlgItemMessage(hwnd, IDC_PLACE, CB_ADDSTRING, 0, (LPARAM) "Unknown");
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_ADDSTRING, 0, (LPARAM) "None");
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_ADDSTRING, 0, (LPARAM) "Scrolling Sign");
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_ADDSTRING, 0, (LPARAM) "Terrain Texture");
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_ADDSTRING, 0, (LPARAM) "Cliffs Texture");
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_ADDSTRING, 0, (LPARAM) "Water Texture");
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_ADDSTRING, 0, (LPARAM) "Water Mask Texture");
        SendDlgItemMessage(hwnd, IDC_FLAGS, CB_ADDSTRING, 0, (LPARAM) "Advertising Sign Texture");
        SendDlgItemMessage(hwnd, IDC_UNKNOWN, CB_ADDSTRING, 0, (LPARAM) "Unknown 1");
        SendDlgItemMessage(hwnd, IDC_UNKNOWN, CB_ADDSTRING, 0, (LPARAM) "Unknown 2");

        for (std::vector<cFlexiTexture>::iterator tx = sceneryfile->flexitextures.begin();
                tx != sceneryfile->flexitextures.end(); tx++) {
            SendDlgItemMessage(hwnd, IDC_FTX, CB_ADDSTRING, 0,
                               (LPARAM) tx->Name.c_str());
        }
        for (std::list<wxString>::iterator tx = texturecache.begin(); tx != texturecache.end(); tx++) {
            SendDlgItemMessage(hwnd, IDC_FTX, CB_ADDSTRING, 0,
                               (LPARAM) (tx->c_str()));
        }

        SetMeshSettings(hwnd, &sceneryfile->meshstructs[CurrentMeshStruct]);

    }
    break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        case IDC_LOAD: {
            OPENFILENAME ofn;
            char sfile[MAX_PATH]="";
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Scenery Files (*.scn)\0*.scn\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open Scenery File (Load mesh settings)";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = ".scn";
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                cSCNFile *texscn = new cSCNFile(sfile);
                if (texscn) {
                    if (texscn->meshstructs.size()==0) {
                        MessageBox(hwnd, "The selected scenery file contains no meshes.", "Warning", MB_OK | MB_ICONWARNING);
                        delete texscn;
                        break;
                    }
                    for (cMeshStructIterator ms = texscn->meshstructs.begin(); ms != texscn->meshstructs.end(); ms++) {
                        c_choices.push_back(ms->Name);
                    }
                    if (c_choices.size() > 0) {
                        std::string title = "Choose mesh settings to import";
                        int choice = DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EFFECT_PARTICLE), hwnd, ChoiceDlgProc, (LPARAM) title.c_str());
                        if (choice > 0) {
                            choice--;
                            SetMeshSettings(hwnd, &texscn->meshstructs[choice]);
                        }
                        c_choices.clear();
                    }
                    delete texscn;
                }
            }
        }
        break;
        case IDC_DISABLE:
            if (IsDlgButtonChecked(hwnd, IDC_DISABLE)) {
                EnableWindow(GetDlgItem(hwnd, IDC_PLACE), false);
                EnableWindow(GetDlgItem(hwnd, IDC_FLAGS), false);
                EnableWindow(GetDlgItem(hwnd, IDC_UNKNOWN), false);
                EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
                EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
            } else {
                EnableWindow(GetDlgItem(hwnd, IDC_PLACE), true);
                EnableWindow(GetDlgItem(hwnd, IDC_FLAGS), true);
                EnableWindow(GetDlgItem(hwnd, IDC_UNKNOWN), true);
                EnableWindow(GetDlgItem(hwnd, IDC_FTX), true);
                EnableWindow(GetDlgItem(hwnd, IDC_TXS), true);
            }
            break;
        case IDC_FLAGS:
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                unsigned long flags = SendDlgItemMessage(hwnd, IDC_FLAGS, CB_GETCURSEL, 0,
                                      0);
                switch (flags) {
                case 0:
                    SendDlgItemMessage(hwnd, IDC_TXS, CB_SETCURSEL, (WPARAM) - 1, 0);
                    SendDlgItemMessage(hwnd, IDC_FTX, CB_SETCURSEL, (WPARAM) - 1, 0);
                    SetDlgItemText(hwnd, IDC_FTX, NULL);
                    SetDlgItemText(hwnd, IDC_TXS, NULL);
                    EnableWindow(GetDlgItem(hwnd, IDC_FTX), true);
                    EnableWindow(GetDlgItem(hwnd, IDC_TXS), true);
                    break;
                case 1:
                    SendDlgItemMessage(hwnd, IDC_FTX, CB_SETCURSEL, (WPARAM) - 1, 0);
                    SendDlgItemMessage(hwnd, IDC_TXS, CB_SELECTSTRING, (WPARAM) - 1,
                                       (LPARAM) "SIOpaqueUnlit");
                    SetDlgItemText(hwnd, IDC_FTX, NULL);
                    SetDlgItemText(hwnd, IDC_TXS, "SIOpaqueUnlit");
                    EnableWindow(GetDlgItem(hwnd, IDC_FTX), true);
                    EnableWindow(GetDlgItem(hwnd, IDC_TXS), true);
                    break;
                case 2:
                    EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
                    EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
                    SetDlgItemText(hwnd, IDC_FTX, "useterraintexture");
                    SendDlgItemMessage(hwnd, IDC_TXS, CB_SELECTSTRING, (WPARAM) - 1,
                                       (LPARAM) "SIOpaque");
                    SetDlgItemText(hwnd, IDC_TXS, "SIOpaque");
                    break;
                case 3:
                    EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
                    EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
                    SetDlgItemText(hwnd, IDC_FTX, "useclifftexture");
                    SendDlgItemMessage(hwnd, IDC_TXS, CB_SELECTSTRING, (WPARAM) - 1,
                                       (LPARAM) "SIOpaque");
                    SetDlgItemText(hwnd, IDC_TXS, "SIOpaque");
                    break;
                case 4:
                    EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
                    EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
                    SetDlgItemText(hwnd, IDC_FTX, "siwater");
                    SendDlgItemMessage(hwnd, IDC_TXS, CB_SELECTSTRING, (WPARAM) - 1,
                                       (LPARAM) "SIWater");
                    SetDlgItemText(hwnd, IDC_TXS, "SIWater");
                    break;
                case 5:
                    EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
                    EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
                    SetDlgItemText(hwnd, IDC_FTX, "watermask");
                    SendDlgItemMessage(hwnd, IDC_TXS, CB_SELECTSTRING, (WPARAM) - 1,
                                       (LPARAM) "SIFillZ");
                    SetDlgItemText(hwnd, IDC_TXS, "SIFillZ");
                    break;
                case 6:
                    EnableWindow(GetDlgItem(hwnd, IDC_FTX), false);
                    EnableWindow(GetDlgItem(hwnd, IDC_TXS), false);
                    SetDlgItemText(hwnd, IDC_FTX, "UseAdTexture");
                    SendDlgItemMessage(hwnd, IDC_TXS, CB_SELECTSTRING, (WPARAM) - 1,
                                       (LPARAM) "SIOpaque");
                    SetDlgItemText(hwnd, IDC_TXS, "SIOpaque");
                    break;
                }
            }
            break;
        case IDOK:
            unsigned long place = SendDlgItemMessage(hwnd, IDC_PLACE, CB_GETCURSEL, 0, 0);
            unsigned long flags = SendDlgItemMessage(hwnd, IDC_FLAGS, CB_GETCURSEL, 0, 0);
            unsigned long unknown = SendDlgItemMessage(hwnd, IDC_UNKNOWN, CB_GETCURSEL, 0, 0);
            unsigned long ftxlen = GetWindowTextLength(GetDlgItem(hwnd, IDC_FTX));
            unsigned long txslen = GetWindowTextLength(GetDlgItem(hwnd, IDC_TXS));
            unsigned long doall = SendDlgItemMessage(hwnd, IDC_DOALL, BM_GETCHECK, 0, 0);
            if ((flags != -1) || (place != -1) && (unknown != -1)
                    && (ftxlen != 0) && (txslen != 0)) {
                ftxlen++;
                txslen++;
                char *ftx = new char[ftxlen];
                char *txs = new char[txslen];
                GetDlgItemText(hwnd, IDC_FTX, ftx, ftxlen);
                GetDlgItemText(hwnd, IDC_TXS, txs, txslen);
                cMeshStruct *ms;
                if (doall != BST_CHECKED) {
                    ms = &sceneryfile->meshstructs[CurrentMeshStruct];
                    ms->disabled = IsDlgButtonChecked(hwnd, IDC_DISABLE);
                    if ((flags == 0) || (flags == 4) || (flags == 5)) {
                        ms->flags = 0;
                    }
                    if (flags == 1) {
                        ms->flags = 12;
                    }
                    if (flags == 2) {
                        ms->flags = 12288;
                    }
                    if (flags == 3) {
                        ms->flags = 20480;
                    }
                    if (flags == 6) {
                        ms->flags = 32788;
                    }
                    ms->place = place;
                    ms->FTX = ftx;
                    ms->TXS = txs;
                    if (unknown == 0) {
                        ms->unknown = 1;
                    }
                    if (unknown == 1) {
                        ms->unknown = 3;
                    }
                } else {
                    for (UINT ttt = 0; ttt < (UINT) sceneryfile->meshstructs.size(); ++ttt) {
                        ms = &sceneryfile->meshstructs[ttt];
                        ms->disabled = IsDlgButtonChecked(hwnd, IDC_DISABLE);
                        if ((flags == 0) || (flags == 4) || (flags == 5)) {
                            ms->flags = 0;
                        }
                        if (flags == 1) {
                            ms->flags = 12;
                        }
                        if (flags == 2) {
                            ms->flags = 12288;
                        }
                        if (flags == 3) {
                            ms->flags = 20480;
                        }
                        if (flags == 6) {
                            ms->flags = 32788;
                        }
                        ms->place = place;
                        ms->FTX = ftx;
                        ms->TXS = txs;
                        if (unknown == 0) {
                            ms->unknown = 1;
                        }
                        if (unknown == 1) {
                            ms->unknown = 3;
                        }

                    }
                }
                delete[] ftx;
                delete[] txs;

                EndDialog(hwnd, 1);
            }
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

/*
char *asename;
void CleanUpMeshStructs() {
    for (unsigned long i = 0; i < MeshStructs.size(); i++) {
        if (MeshStructs[i]->FTX != 0) {
            delete MeshStructs[i]->FTX;
        }
        if (MeshStructs[i]->TXS != 0) {
            delete MeshStructs[i]->TXS;
        }
        if (MeshStructs[i]->Name != 0) {
            delete MeshStructs[i]->Name;
        }
        delete MeshStructs[i];
    }
    MeshStructs.clear();
    CurrentMeshStruct = -1;
}

void CleanUpScenery() {
    unsigned long i;
    for (i = 0; i < FlexiTextures.size(); i++) {
        delete FlexiTextures[i]->Alpha;
        delete FlexiTextures[i]->Name;
        delete FlexiTextures[i]->Texture;
        delete FlexiTextures[i];
    }
    FlexiTextures.clear();
    CurrentFlexiTexture = -1;
    for (i = 0; i < References.size(); i++) {
        delete References[i];
    }
    References.clear();
    CurrentReference = -1;
    for (i = 0; i < EffectPoints.size(); i++) {
        if (EffectPoints[i]->Name != 0)
            delete EffectPoints[i]->Name;
        delete EffectPoints[i];
    }
    EffectPoints.clear();
    CurrentEffectPoint = -1;
    CleanUpMeshStructs();
    if (asename != 0) {
        delete asename;
    }
    asename = new char[1];
    asename[0] = 0;
    CurrentFixOrient = DEFAULT_SCN_FIXORIENT;
}
*/

BOOL CALLBACK SceneryDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_INITDIALOG: {
        SetMenu(hwnd, LoadMenu(hInst, MAKEINTRESOURCE(IDR_SCENERY)));
        SendDlgItemMessage(hwnd, IDC_MESHES, LB_RESETCONTENT, 0, 0);
        SendDlgItemMessage(hwnd, IDC_REFERENCES, LB_RESETCONTENT, 0, 0);
        SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_RESETCONTENT, 0, 0);
        SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_RESETCONTENT, 0, 0);
        if (sceneryfile)
            delete sceneryfile;
        sceneryfile = new cSCNFile();
        if (!matrixIsEqual(g_config->m_def_matrix, matrixGetUnity())) {
            sceneryfile->transforms.push_back(g_config->m_def_matrix);
            sceneryfile->transformnames.push_back("Default Matrix");
        }

        setMatrix(hwnd, matrixMultiply(sceneryfile->transforms));

        SetDlgItemText(hwnd, IDC_ASE, NULL);
        SendDlgItemMessage(hwnd, IDC_EXPANSIONFLAG, CB_ADDSTRING, 0, (LPARAM) "None");
        SendDlgItemMessage(hwnd, IDC_EXPANSIONFLAG, CB_ADDSTRING, 0, (LPARAM) "Soaked!");
        SendDlgItemMessage(hwnd, IDC_EXPANSIONFLAG, CB_ADDSTRING, 0, (LPARAM) "Wild!");
        SendDlgItemMessage(hwnd, IDC_EXPANSIONFLAG, CB_SETCURSEL, 0, 0);
        SetDlgItemText(hwnd, IDC_SWAY, "0.0");
        SetDlgItemText(hwnd, IDC_BRIGHTNESS, "1.0");
        SetDlgItemText(hwnd, IDC_UNKNOWNFACTOR, "1.0");
        SetDlgItemText(hwnd, IDC_SCALE, "0.0");
        char *texref = new char[10+GetWindowTextLength(GetDlgItem(GetParent(hwnd), IDC_THEMEPREFIX))+
                                GetWindowTextLength(GetDlgItem(GetParent(hwnd), IDC_THEMENAME))+9+1];
        strcpy(texref, "../shared/");
        char *tmp = new char[MAX_PATH];
        GetDlgItemText(GetParent(hwnd), IDC_THEMEPREFIX, tmp, MAX_PATH);
        strcat(texref, tmp);
        GetDlgItemText(GetParent(hwnd), IDC_THEMENAME, tmp, MAX_PATH);
        strcat(texref, tmp);
        strcat(texref, "-texture");
        SetDlgItemText(hwnd, IDC_REFERENCE, texref);
        delete[] tmp;
        delete[] texref;
        scenerysave = false;
    }
    break;
    case WM_CLOSE:
        if (scenerysave == true) {
            if (MessageBox(hwnd, "Unsaved changes, continue?", "Unsaved changes", MB_YESNO) == IDNO) {
                break;
            }
        }
        EndDialog(hwnd, 0);
        break;
    case WM_MEASUREITEM: {
        LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;
        lpmis->itemHeight = 16;
    }
    return TRUE;
    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;

        if (lpdis->itemID == -1)
            break;

        switch (lpdis->CtlID) {
        case IDC_MESHES:
            if (lpdis->itemID >= sceneryfile->meshstructs.size())
                break;
            switch (lpdis->itemAction) {
            case ODA_FOCUS:
            case ODA_SELECT:
            case ODA_DRAWENTIRE:

                // Display the text associated with the item.

                //SendMessage(lpdis->hwndItem, LB_GETTEXT, lpdis->itemID, (LPARAM) tchBuffer);
                TEXTMETRIC tm;
                if (lpdis->itemState & ODS_SELECTED) {
                    FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH) GetStockObject(BLACK_BRUSH));
                    SetBkColor(lpdis->hDC, RGB(0, 0, 0));
                } else {
                    FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH) GetStockObject(WHITE_BRUSH));
                    SetBkColor(lpdis->hDC, RGB(255, 255, 255));
                }

                if (sceneryfile->meshstructs[lpdis->itemID].effectpoint)
                    SetTextColor(lpdis->hDC, RGB(160, 210, 160));
                else if (sceneryfile->meshstructs[lpdis->itemID].disabled)
                    SetTextColor(lpdis->hDC, RGB(128, 128, 128));
                else if ((sceneryfile->meshstructs[lpdis->itemID].FTX == "")
                         || (sceneryfile->meshstructs[lpdis->itemID].TXS == ""))
                    SetTextColor(lpdis->hDC, RGB(255, 60, 60));
                else if (lpdis->itemState & ODS_SELECTED)
                    SetTextColor(lpdis->hDC, RGB(255, 255, 255));
                else
                    SetTextColor(lpdis->hDC, RGB(0, 0, 0));


                GetTextMetrics(lpdis->hDC, &tm);

                int y = (lpdis->rcItem.bottom + lpdis->rcItem.top - tm.tmHeight) / 2;

                TextOut(lpdis->hDC, 6, y, sceneryfile->meshstructs[lpdis->itemID].Name.c_str(),
                        sceneryfile->meshstructs[lpdis->itemID].Name.length());

                if (lpdis->itemState & ODS_FOCUS) {
                    DrawFocusRect(lpdis->hDC, &lpdis->rcItem);
                }
                break;
            }
        }
    }
    break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_MATRIX_EDIT:
            c_transforms = sceneryfile->transforms;
            c_transformnames = sceneryfile->transformnames;
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_MATRIX), hwnd, MatrixDlgProc) == 1) {
                sceneryfile->transforms = c_transforms;
                sceneryfile->transformnames = c_transformnames;
                scenerysave = true;
                setMatrix(hwnd, matrixMultiply(sceneryfile->transforms));
            }
            break;
        case IDC_MATRIX_CLEAR:
            sceneryfile->transforms.clear();
            sceneryfile->transformnames.clear();
            setMatrix(hwnd, matrixGetUnity());
            break;
        case IDC_MATRIX_FIX:
            sceneryfile->transforms.clear();
            sceneryfile->transformnames.clear();
            sceneryfile->transforms.push_back(matrixGetLegacyFixOrientation());
            sceneryfile->transformnames.push_back("Legacy Fix Orientation");
            setMatrix(hwnd, matrixGetLegacyFixOrientation());
            break;
        case IDM__CLEAR:
            texturecache.clear();
            break;
        case IDM__OPEN_TEXTURE_SCN_FILE: {
            char sfile[MAX_PATH]="";
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Scenery Files (*.scn)\0*.scn\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open Scenery File (Add to texture cache)";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = ".scn";
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                cSCNFile *texscn = new cSCNFile(sfile);
                if (texscn) {
                    if (texscn->flexitextures.size()==0) {
                        MessageBox(hwnd, "The selected scenery file contains no textures.", "Warning", MB_OK | MB_ICONWARNING);
                        delete texscn;
                        break;
                    }
                    int pre = texturecache.size();
                    for (cFlexiTextureIterator ft = texscn->flexitextures.begin(); ft != texscn->flexitextures.end(); ft++)
                        texturecache.push_back(ft->Name);
                    texturecache.sort();
                    texturecache.unique();
                    int post = texturecache.size();
                    if (post-pre > 0)
                        MessageBox(hwnd, "Textures added.", "Information", MB_OK | MB_ICONINFORMATION);
                    else
                        MessageBox(hwnd, "The selected scenery file contained no new textures.", "Warning", MB_OK | MB_ICONWARNING);
                    delete texscn;
                }
            }
        }
        break;
        case IDM_SAVE_DEFAULT_MATRIX:
            g_config->m_def_matrix = matrixMultiply(sceneryfile->transforms);
            if (!g_config->Save())
                MessageBox(hwnd, "Saving default matrix failed.", "Error", MB_OK);
            break;
        case IDC_EXPANSIONFLAG:
        case IDC_GREENERY:
        case IDC_FLOWERS:
        case IDC_VARYROTATION:
        case IDC_UNKNOWNBITS: {
            unsigned int sivflags = 0;
            if (SendDlgItemMessage(hwnd, IDC_GREENERY, BM_GETCHECK, 0, 0))
                sivflags |= OVL_SIV_GREENERY;
            if (SendDlgItemMessage(hwnd, IDC_FLOWERS, BM_GETCHECK, 0, 0))
                sivflags |= OVL_SIV_FLOWER;
            if (SendDlgItemMessage(hwnd, IDC_VARYROTATION, BM_GETCHECK, 0, 0))
                sivflags |= OVL_SIV_ROTATION;
            if (SendDlgItemMessage(hwnd, IDC_UNKNOWNBITS, BM_GETCHECK, 0, 0))
                sivflags |= OVL_SIV_UNKNOWN;
            unsigned long exp =
                SendDlgItemMessage(hwnd, IDC_EXPANSIONFLAG, CB_GETCURSEL, 0, 0);
            if (exp == 1)
                sivflags |= OVL_SIV_SOAKED;
            else if (exp == 2)
                sivflags |= OVL_SIV_WILD;
            sceneryfile->sivsettings.sivflags = sivflags;
            scenerysave = true;
        }
        break;
        case IDC_SWAY: {
            int len = GetWindowTextLength(GetDlgItem(hwnd, IDC_SWAY));
            char *temp = new char[len + 1];
            GetDlgItemText(hwnd, IDC_SWAY, temp, len + 1);
            sceneryfile->sivsettings.sway = (float) atof(temp);
            delete temp;
            scenerysave = true;
        }
        break;
        case IDC_BRIGHTNESS: {
            int len = GetWindowTextLength(GetDlgItem(hwnd, IDC_BRIGHTNESS));
            char *temp = new char[len + 1];
            GetDlgItemText(hwnd, IDC_BRIGHTNESS, temp, len + 1);
            sceneryfile->sivsettings.brightness = (float) atof(temp);
            delete temp;
            scenerysave = true;
        }
        break;
        case IDC_UNKNOWNFACTOR: {
            int len = GetWindowTextLength(GetDlgItem(hwnd, IDC_UNKNOWNFACTOR));
            char *temp = new char[len + 1];
            GetDlgItemText(hwnd, IDC_UNKNOWNFACTOR, temp, len + 1);
            sceneryfile->sivsettings.unknown = (float) atof(temp);
            delete temp;
            scenerysave = true;
        }
        break;
        case IDC_SCALE: {
            int len = GetWindowTextLength(GetDlgItem(hwnd, IDC_SCALE));
            char *temp = new char[len + 1];
            GetDlgItemText(hwnd, IDC_SCALE, temp, len + 1);
            sceneryfile->sivsettings.scale = (float) atof(temp);
            delete temp;
            scenerysave = true;
        }
        break;
        case IDC_NORMALSCENERY: {
            SendDlgItemMessage(hwnd, IDC_EXPANSIONFLAG, CB_SETCURSEL, 0, 0);
            SetDlgItemText(hwnd, IDC_SWAY, "0.0");
            SetDlgItemText(hwnd, IDC_BRIGHTNESS, "1.0");
            SetDlgItemText(hwnd, IDC_UNKNOWNFACTOR, "1.0");
            SetDlgItemText(hwnd, IDC_SCALE, "0.0");
            CheckDlgButton(hwnd, IDC_GREENERY, false);
            CheckDlgButton(hwnd, IDC_FLOWERS, false);
            CheckDlgButton(hwnd, IDC_VARYROTATION, false);
            CheckDlgButton(hwnd, IDC_UNKNOWNBITS, false);
            sceneryfile->sivsettings.sivflags = 0;
            sceneryfile->sivsettings.sway = 0.0;
            sceneryfile->sivsettings.brightness = 1.0;
            sceneryfile->sivsettings.unknown = 1.0;
            sceneryfile->sivsettings.scale = 0.0;
            scenerysave = true;
        }
        break;
        case IDC_SWAYINGTREE: {
            SendDlgItemMessage(hwnd, IDC_EXPANSIONFLAG, CB_SETCURSEL, 0, 0);
            SetDlgItemText(hwnd, IDC_SWAY, "0.2");
            SetDlgItemText(hwnd, IDC_BRIGHTNESS, "0.8");
            SetDlgItemText(hwnd, IDC_UNKNOWNFACTOR, "1.0");
            SetDlgItemText(hwnd, IDC_SCALE, "0.4");
            CheckDlgButton(hwnd, IDC_GREENERY, true);
            CheckDlgButton(hwnd, IDC_FLOWERS, false);
            CheckDlgButton(hwnd, IDC_VARYROTATION, true);
            CheckDlgButton(hwnd, IDC_UNKNOWNBITS, false);
            sceneryfile->sivsettings.sivflags = OVL_SIV_GREENERY | OVL_SIV_ROTATION;
            sceneryfile->sivsettings.sway = 0.2;
            sceneryfile->sivsettings.brightness = 0.8;
            sceneryfile->sivsettings.unknown = 1.0;
            sceneryfile->sivsettings.scale = 0.4;
            scenerysave = true;
        }
        break;
        case IDC_NOTES: {
            MessageBox(GetActiveWindow(),
                       "- Expansion Flag: Set on objects of the respective expansion. No known in-game effect.\n"
                       "- Greenery: Set on trees, shrubs and prehistoric fern. No known in-game effect.\n"
                       "- Flowers: Set on flowers. Effect untested.\n"
                       "- Vary Rotation: Random object rotation on placement. Set on trees & shrubs.\n"
                       "- Unknown Bits: Set on several objects. Effect untested.\n"
                       "- Swaying Factor: Amount of swaying. 0.2 on almost all trees & shrubs. 0.0 for safari trees.\n"
                       "- Brightness Variation: If < 1.0, object brightness varies on placement.\n"
                       "- Unknown Factor: 1.0 in all checked original objects. Also seems to affect brightness.\n"
                       "- Scale Variation: If > 0.0, object size varies on placement.\n"
                       "These settings only affect the model, not the effect points. Shadows are also unaffected.\n\n"
                       "All in-game trees & shrubs (with the noted Safari exception) have the same values for Swaying,\n"
                       "Brightness and Scale. They also have Greenery and Rotation set.\n"
                       "Some in-game trees are normal scenery in regard to these settings (e.g. Sci-Fi).",
                       "Visual Options Notes", MB_OK | MB_ICONINFORMATION);
        }
        break;
        case IDC_ASE:
            if (HIWORD(wParam) == EN_KILLFOCUS) {
                unsigned int aselen = GetWindowTextLength(GetDlgItem(hwnd, IDC_ASE));
                if (aselen != 0) {
                    char *ase = new char[aselen + 1];
                    GetDlgItemText(hwnd, IDC_ASE, ase, aselen + 1);
                    if (stricmp(ase, sceneryfile->objfile.c_str()) != 0) {
                        sceneryfile->objfile = ase;
                        if (sceneryfile->LoadObject()) {
                            SendDlgItemMessage(hwnd, IDC_MESHES, LB_RESETCONTENT, 0, 0);
                            for (std::vector<cMeshStruct>::iterator ms = sceneryfile->meshstructs.begin(); ms != sceneryfile->meshstructs.end(); ms++) {
                                SendDlgItemMessage(hwnd, IDC_MESHES, LB_ADDSTRING, 0,
                                                   (LPARAM) ms->Name.c_str());
                            }
                            scenerysave = true;
                        } else {
                            MessageBox(hwnd, "Possible Reasons:\n- File not found\n- Unknown object format\n- Object contains no 3D data", "Error Loading Object File", MB_OK|MB_ICONERROR);
                        }

                    }
                    delete[] ase;
                }
            }
            break;
        case IDCANCEL:
            if (scenerysave == true) {
                if (MessageBox(hwnd, "Unsaved changes, continue?", "Unsaved changes", MB_YESNO)
                        == IDNO) {
                    break;
                }
            }
            EndDialog(hwnd, 0);
            break;
        case ID_FILE_SAVESCENERY: {
            /*
                        if (asename[0] == 0) {
                            MessageBox(hwnd, "Can't Save Scenery File due to missing data", "Error", MB_OK);
                            break;
                        }
            {
            bool fail = false;
            for (MeshStructsIterator = MeshStructs.begin();
                    MeshStructsIterator != MeshStructs.end(); MeshStructsIterator++) {
                if ((*MeshStructsIterator)->disabled == false) {
                    if (((*MeshStructsIterator)->FTX == 0)
                            || ((*MeshStructsIterator)->TXS == 0)) {
                        char err[256];
                        snprintf(err, 255,
                                 "Can't Save Scenery File due to missing data in mesh \"%s\"",
                                 (*MeshStructsIterator)->Name);
                        MessageBox(hwnd, err, "Error", MB_OK);
                        fail = true;
                    }
                }
            }
            if (fail == true) {
                break;
            }
            }
            */
            if (sceneryfile->filename == "") {
                char fname[_MAX_FNAME];
                _splitpath(sceneryfile->objfile.c_str(), NULL, NULL, fname, NULL);
                sceneryfile->filename = fname;
            }
            char sfile[MAX_PATH];
            OPENFILENAME ofn;
            strcpy(sfile, sceneryfile->filename.c_str());
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Scenery Files (*.scn)\0*.scn\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Save Scenery file";
            ofn.lpstrDefExt = ".scn";
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
            if (GetSaveFileName(&ofn)) {
                sceneryfile->filename = sfile;
                if (!sceneryfile->Save())
                    MessageBox(hwnd, "Saving Scenery file failed.", "Error", MB_OK | MB_ICONERROR);
                else
                    scenerysave = false;
            }
        }
        break;
        case ID_FILE_OPENSCENERY: {
            if (scenerysave == true) {
                if (MessageBox(hwnd, "Unsaved changes, continue?", "Unsaved changes", MB_YESNO)
                        == IDNO) {
                    break;
                }
            }
            char sfile[MAX_PATH]="";
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Scenery Files (*.scn)\0*.scn\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open Scenery File";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = ".scn";
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                delete sceneryfile;
                sceneryfile = new cSCNFile(sfile);

                if (sceneryfile->error & CSCNFILE_ERROR_OBJNOTFOUND) {
                    if (MessageBox(hwnd, "Can't Open Object File.\nDo you want to choose a new one?\n(This is your only chance to do sowithout loosing the mesh settings)", "Error", MB_YESNO | MB_ICONERROR)==IDYES) {
                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hwnd;
                        ofn.lpstrFilter = "Supported 3D Files\0*.ase;*.ms3d\0ASE Files (*.ase)\0*.ase\0Milkshape 3D (*.ms3d)\0*.ms3d\0All Files (*.*)\0*.*\0";
                        ofn.lpstrFile = sfile;
                        ofn.lpstrTitle = "Select ASE file";
                        ofn.nMaxFile = MAX_PATH;
                        ofn.lpstrDefExt = ".ase";
                        ofn.Flags =
                            OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                            OFN_NOCHANGEDIR;
                        if (GetOpenFileName(&ofn)) {
                            sceneryfile->objfile = sfile;
                            sceneryfile->Load();
                        }
                    }
                }

                std::string warnings = "";
                if (!sceneryfile->Ok())
                    warnings += "\n- The file was written by a newer version of the Importer. Some features may be missing.";
                if (sceneryfile->Error()) {
                    warnings += "\n- Some warnings/errors occured while matiching up the scenery file with the current contents of the 3D object file:";
                    if (sceneryfile->error == CSCNFILE_ERROR_OBJNOTFOUND) {
                        warnings += "\n-- 3D Object file not found.";
                    } else if (sceneryfile->error == CSCNFILE_ERROR_MESH_CONFUSION) {
                        warnings += "\n-- Fixing inconsistencies probably failed. Check closely if everything is ok.";
                    } else if (sceneryfile->error & CSCNFILE_ERROR_MESH_FIX) {
                        warnings += "\n-- Fixing inconsistencies seems to have succeeded, but nevertehless check closely if everything is ok.";
                        if (sceneryfile->error & CSCNFILE_ERROR_MESH_MISMATCH) {
                            warnings += "\n-- Your 3D Object file has more meshes than before.";
                        }
                        if (sceneryfile->error & CSCNFILE_ERROR_MESH_CONFUSION) {
                            warnings += "\n-- The names of the meshes changed, but the number remained the same. They were assigned in the old order.";
                        }
                        if (sceneryfile->error & CSCNFILE_ERROR_MESH_POOF) {
                            warnings += "\n-- Your 3D Object file has less meshes than before.";
                        }
                    } else
                        warnings += "\n-- Something I didn't think of (this is a bug, please report).";
                }

                if (warnings != "") {
                    warnings = "Problems occured loading your scenery file:\n" + warnings;
                    MessageBox(hwnd, warnings.c_str(), "Warning", MB_OK | MB_ICONWARNING);
                }

                SendDlgItemMessage(hwnd, IDC_MESHES, LB_RESETCONTENT, 0, 0);
                SendDlgItemMessage(hwnd, IDC_REFERENCES, LB_RESETCONTENT, 0, 0);
                SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_RESETCONTENT, 0, 0);
                SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_RESETCONTENT, 0, 0);

                SetDlgItemText(hwnd, IDC_ASE, sceneryfile->objfile.c_str());

                D3DMATRIX mat = matrixMultiply(sceneryfile->transforms);
                setMatrix(hwnd, mat);

                for (cMeshStructIterator ms = sceneryfile->meshstructs.begin(); ms != sceneryfile->meshstructs.end(); ms++)
                    SendDlgItemMessage(hwnd, IDC_MESHES, LB_ADDSTRING, 0,
                                       (LPARAM) ms->Name.c_str());
                for (cFlexiTextureIterator ft = sceneryfile->flexitextures.begin(); ft != sceneryfile->flexitextures.end(); ft++)
                    SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_ADDSTRING, 0,
                                       (LPARAM) ft->Name.c_str());
                for (cStringIterator st = sceneryfile->references.begin(); st != sceneryfile->references.end(); st++)
                    SendDlgItemMessage(hwnd, IDC_REFERENCES, LB_ADDSTRING, 0,
                                       (LPARAM) st->c_str());
                for (cEffectPointIterator ep = sceneryfile->effectpoints.begin(); ep != sceneryfile->effectpoints.end(); ep++)
                    SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_ADDSTRING, 0,
                                       (LPARAM) ep->Name.c_str());

                unsigned int sivflags = sceneryfile->sivsettings.sivflags;
                CheckDlgButton(hwnd, IDC_GREENERY, (sivflags & OVL_SIV_GREENERY));
                CheckDlgButton(hwnd, IDC_FLOWERS, (sivflags & OVL_SIV_FLOWER));
                CheckDlgButton(hwnd, IDC_VARYROTATION, (sivflags & OVL_SIV_ROTATION));
                CheckDlgButton(hwnd, IDC_UNKNOWNBITS, (sivflags & OVL_SIV_UNKNOWN));
                if (sivflags & OVL_SIV_SOAKED)
                    SendDlgItemMessage(hwnd, IDC_EXPANSIONFLAG, CB_SETCURSEL, 1, 0);
                else if (sivflags & OVL_SIV_WILD)
                    SendDlgItemMessage(hwnd, IDC_EXPANSIONFLAG, CB_SETCURSEL, 2, 0);
                else
                    SendDlgItemMessage(hwnd, IDC_EXPANSIONFLAG, CB_SETCURSEL, 0, 0);

                char temp[256];
                snprintf(temp, 255, "%f", sceneryfile->sivsettings.sway);
                SetDlgItemText(hwnd, IDC_SWAY, temp);

                snprintf(temp, 255, "%f", sceneryfile->sivsettings.brightness);
                SetDlgItemText(hwnd, IDC_BRIGHTNESS, temp);

                snprintf(temp, 255, "%f", sceneryfile->sivsettings.unknown);
                SetDlgItemText(hwnd, IDC_UNKNOWNFACTOR, temp);

                snprintf(temp, 255, "%f", sceneryfile->sivsettings.scale);
                SetDlgItemText(hwnd, IDC_SCALE, temp);

                scenerysave = false;
            }
        }
        break;
        case ID_FILE_NEWSCENERY: {
            if (scenerysave == true) {
                if (MessageBox(hwnd, "Unsaved changes, continue?", "Unsaved changes", MB_YESNO)
                        == IDNO) {
                    break;
                }
            }
            SendDlgItemMessage(hwnd, IDC_MESHES, LB_RESETCONTENT, 0, 0);
            SendDlgItemMessage(hwnd, IDC_REFERENCES, LB_RESETCONTENT, 0, 0);
            SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_RESETCONTENT, 0, 0);
            SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_RESETCONTENT, 0, 0);
            SendDlgItemMessage(hwnd, IDC_EXPANSIONFLAG, CB_SETCURSEL, 0, 0);
            SetDlgItemText(hwnd, IDC_SWAY, "0.0");
            SetDlgItemText(hwnd, IDC_BRIGHTNESS, "1.0");
            SetDlgItemText(hwnd, IDC_UNKNOWNFACTOR, "1.0");
            SetDlgItemText(hwnd, IDC_SCALE, "0.0");
            CheckDlgButton(hwnd, IDC_GREENERY, false);
            CheckDlgButton(hwnd, IDC_FLOWERS, false);
            CheckDlgButton(hwnd, IDC_VARYROTATION, false);
            CheckDlgButton(hwnd, IDC_UNKNOWNBITS, false);
            delete sceneryfile;
            sceneryfile = new cSCNFile();
            if (!matrixIsEqual(g_config->m_def_matrix, matrixGetUnity())) {
                sceneryfile->transforms.push_back(g_config->m_def_matrix);
                sceneryfile->transformnames.push_back("Default Matrix");
            }

            setMatrix(hwnd, matrixMultiply(sceneryfile->transforms));

            char sfile[MAX_PATH];
            SetDlgItemText(hwnd, IDC_ASE, NULL);
            char *texref = new char[10+GetWindowTextLength(GetDlgItem(GetParent(hwnd), IDC_THEMEPREFIX))+
                                    GetWindowTextLength(GetDlgItem(GetParent(hwnd), IDC_THEMENAME))+9+1];
            strcpy(texref, "../shared/");
            GetDlgItemText(GetParent(hwnd), IDC_THEMEPREFIX, sfile, MAX_PATH);
            strcat(texref, sfile);
            GetDlgItemText(GetParent(hwnd), IDC_THEMENAME, sfile, MAX_PATH);
            strcat(texref, sfile);
            strcat(texref, "-texture");
            SetDlgItemText(hwnd, IDC_REFERENCE, texref);
            delete[] texref;
            scenerysave = false;
        }
        break;
        case ID_FILE_SAVEOVL: {
            unsigned int i;
            bool textureovl = false;
            c3DLoader *object = NULL;
            if (sceneryfile->objfile == wxT("")) {
                if (MessageBox(hwnd,
                               "You did not select an 3D object file. Do you want to create a texture ovl?",
                               "Query", MB_YESNO | MB_ICONQUESTION) == IDNO)
                    break;
                textureovl = true;
            } else {
                object = c3DLoader::LoadFile(sceneryfile->objfile.c_str());
                if (!object) {
                    MessageBox(hwnd, "Couldn't load object file.", "Error", MB_OK);
                    break;
                }
            }
            unsigned long objectCount = 0;
            BOOL failed = false;
            for (cMeshStructIterator ms = sceneryfile->meshstructs.begin();
                    ms != sceneryfile->meshstructs.end(); ms++) {
                if (ms->disabled == false) {
                    objectCount++;
                    if ((ms->FTX == wxT(""))
                            || (ms->TXS == wxT(""))) {
                        wxString err = _("Can't Save Scenery Ovl File due to missing data in mesh \"") + ms->Name + _("\".");
                        MessageBox(hwnd, err.c_str(), "Error", MB_OK);
                        failed = true;
                        break;
                    }
                }
            }
            if (failed)
                break;
            if ((!textureovl) && (objectCount == 0)) {
                MessageBox(hwnd, "No meshes to export", "Error", MB_OK);
                break;
            }
            char sfile[MAX_PATH];
            if (!textureovl) {
                char fname[_MAX_FNAME];
                _splitpath(sceneryfile->objfile.c_str(), NULL, NULL, fname, NULL);
                GetDlgItemText(GetParent(hwnd), IDC_THEMEPREFIX, sfile, MAX_PATH);
                strncat(sfile, fname, MAX_PATH);
            } else {
                char *texref = new char[GetWindowTextLength(GetDlgItem(GetParent(hwnd), IDC_THEMEPREFIX))+
                                        GetWindowTextLength(GetDlgItem(GetParent(hwnd), IDC_THEMENAME))+9+1];
                GetDlgItemText(GetParent(hwnd), IDC_THEMEPREFIX, sfile, MAX_PATH);
                strcpy(texref, sfile);
                GetDlgItemText(GetParent(hwnd), IDC_THEMENAME, sfile, MAX_PATH);
                strcat(texref, sfile);
                strcat(texref, "-texture");
                strcpy(sfile, texref);
                delete[] texref;
            }
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Overlay Files (*.common.ovl)\0*.common.ovl\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Save Scenery OVL file";
            ofn.lpstrDefExt = ".common.ovl";
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
            if (GetSaveFileName(&ofn)) {
#ifndef LIBOVL_STATIC
                CreateSceneryOvl(sfile);
#else
                OVL sceneryovl(sfile);
#endif

                char fname[_MAX_FNAME];

                char *ovlname;
                _splitpath(sfile, NULL, NULL, fname, NULL);
                strchr(fname, '.')[0] = 0;
                ovlname = new char[strlen(fname) + 1];
                strcpy(ovlname, fname);

                ILuint tex[2];
                ILenum Error;

                ilGenImages(2, (ILuint *) & tex);

                // Predeclare pointers for deletion
                StaticShape1 *sh = NULL;
                char **ftx = NULL;
                char **txs = NULL;
                char *name = NULL;
                unsigned char *svd = NULL;
                std::vector<FlexiTextureInfoStruct *> ftistore;
                std::vector<char *> ftinamestore;

                if (object) {
                    _splitpath(sceneryfile->objfile.c_str(), NULL, NULL, fname, NULL);
                    name = strdup(fname);
                    sh = new StaticShape1;
                    sh->EffectCount = sceneryfile->effectpoints.size();
                    sh->MeshCount = objectCount;
                    sh->MeshCount2 = objectCount;
                    sh->TotalIndexCount = 0;
                    sh->TotalVertexCount = 0;
                    boundsInit(&sh->BoundingBox1, &sh->BoundingBox2);
                    sh->sh = new StaticShape2 *[objectCount];
                    D3DMATRIX transformMatrix = matrixMultiply(sceneryfile->transforms);
                    bool do_transform = !matrixIsEqual(transformMatrix, matrixGetUnity());

                    if (sceneryfile->effectpoints.size() != 0) {
                        unsigned int e;
                        sh->EffectName = new char *[sceneryfile->effectpoints.size()];
                        sh->EffectPosition = new D3DMATRIX[sceneryfile->effectpoints.size()];
                        for (e = 0; e < sceneryfile->effectpoints.size(); e++) {
                            sh->EffectName[e] = strdup(sceneryfile->effectpoints[e].Name.c_str());
                            D3DMATRIX me = matrixMultiply(sceneryfile->effectpoints[e].Transform);

                            if (do_transform)
                                sh->EffectPosition[e] = matrixMultiply(&me, &transformMatrix);
                            else
                                sh->EffectPosition[e] = me;
                        }
                    } else {
                        sh->EffectName = 0;
                        sh->EffectPosition = 0;
                    }

                    unsigned long CurrentMesh = 0;
                    unsigned long CurrentObj = 0;
                    for (cMeshStructIterator ms = sceneryfile->meshstructs.begin();
                            ms != sceneryfile->meshstructs.end(); ms++) {
                        if (ms->disabled == false) {
                            StaticShape2 *sh2 = new StaticShape2;
                            sh->sh[CurrentMesh] = sh2;
                            sh2->fts = 0;
                            sh2->PlaceTexturing = ms->place;
                            sh2->TextureData = 0;
                            sh2->textureflags = ms->flags;
                            sh2->unk1 = 0xFFFFFFFF;
                            sh2->unk4 = ms->unknown;

                            object->FetchObject(CurrentObj, &sh2->VertexCount, &sh2->Vertexes, &sh2->IndexCount,
                                                &sh2->Triangles, &sh->BoundingBox1, &sh->BoundingBox2,
                                                const_cast<D3DMATRIX *> ((do_transform)?(&transformMatrix):NULL));

                            CurrentMesh++;
                        }
                        CurrentObj++;
                    }

                    ftx = new char *[objectCount];
                    txs = new char *[objectCount];
                    CurrentObj = 0;
                    for (cMeshStructIterator ms = sceneryfile->meshstructs.begin();
                            ms != sceneryfile->meshstructs.end(); ms++) {
                        if (ms->disabled == false) {
                            ftx[CurrentObj] = strdup(ms->FTX.c_str());
                            txs[CurrentObj] = strdup(ms->TXS.c_str());
                            CurrentObj++;
                        }
                    }
#ifndef LIBOVL_STATIC
                    AddStaticShape(name, ftx, txs, sh);
#else
                    sceneryovl.AddStaticShape(name, ftx, txs, sh);
#endif

                    SceneryItemVisual *sv;
                    SceneryItemVisualLOD **svlod;
                    SceneryItemVisualLOD *svlod1;
                    SceneryItemVisualLOD *svlod2;
                    SceneryItemVisualLOD *svlod3;
                    unsigned long scenerysize =
                        sizeof(SceneryItemVisual) + (sizeof(SceneryItemVisualLOD) * 3) + 12;
                    svd = new unsigned char[scenerysize];
                    memset(svd, 0, scenerysize);
                    sv = (SceneryItemVisual *) svd;
                    svlod = (SceneryItemVisualLOD **) (svd + sizeof(SceneryItemVisual));
                    svlod1 = (SceneryItemVisualLOD *) (svd + sizeof(SceneryItemVisual)
                                                       + 12);
                    svlod2 = (SceneryItemVisualLOD *) (svd + sizeof(SceneryItemVisual)
                                                       + 12 + sizeof(SceneryItemVisualLOD));
                    svlod3 = (SceneryItemVisualLOD *) (svd + sizeof(SceneryItemVisual)
                                                       + 12 + (sizeof(SceneryItemVisualLOD) * 2));
                    sv->sivflags = sceneryfile->sivsettings.sivflags;
                    sv->sway = sceneryfile->sivsettings.sway;
                    sv->brightness = sceneryfile->sivsettings.brightness;
                    sv->unk4 = sceneryfile->sivsettings.unknown;
                    sv->scale = sceneryfile->sivsettings.scale;

                    sv->LODCount = 3;
                    sv->LODMeshes = svlod;
                    sv->unk6 = 0;
                    sv->unk7 = 0;
                    sv->unk8 = 0;
                    sv->unk9 = 0;
                    sv->unk10 = 0;
                    sv->unk11 = 0;
                    svlod[0] = svlod1;
                    svlod[1] = svlod2;
                    svlod[2] = svlod3;
                    svlod1->MeshType = 0;
                    svlod1->LODName = ovlname;
                    svlod1->StaticShape = 0;
                    svlod1->unk2 = 0;
                    svlod1->BoneShape = 0;
                    svlod1->unk4 = 0;
                    svlod1->unk5 = 0;
                    svlod1->unk6 = 0;
                    svlod1->unk7 = 1.0;
                    svlod1->unk8 = 1.0;
                    svlod1->unk9 = 0;
                    svlod1->unk10 = 1.0;
                    svlod1->unk11 = 0;
                    svlod1->unk12 = 1.0;
                    svlod1->distance = 40;
                    svlod1->AnimationCount = 0;
                    svlod1->unk14 = 0;
                    svlod1->AnimationArray = 0;
                    svlod2->MeshType = 0;
                    svlod2->LODName = ovlname;
                    svlod2->StaticShape = 0;
                    svlod2->unk2 = 0;
                    svlod2->BoneShape = 0;
                    svlod2->unk4 = 0;
                    svlod2->unk5 = 0;
                    svlod2->unk6 = 0;
                    svlod2->unk7 = 1.0;
                    svlod2->unk8 = 1.0;
                    svlod2->unk9 = 0;
                    svlod2->unk10 = 1.0;
                    svlod2->unk11 = 0;
                    svlod2->unk12 = 1.0;
                    svlod2->distance = 100;
                    svlod2->AnimationCount = 0;
                    svlod2->unk14 = 0;
                    svlod2->AnimationArray = 0;
                    svlod3->MeshType = 0;
                    svlod3->LODName = ovlname;
                    svlod3->StaticShape = 0;
                    svlod3->unk2 = 0;
                    svlod3->BoneShape = 0;
                    svlod3->unk4 = 0;
                    svlod3->unk5 = 0;
                    svlod3->unk6 = 0;
                    svlod3->unk7 = 1.0;
                    svlod3->unk8 = 1.0;
                    svlod3->unk9 = 0;
                    svlod3->unk10 = 1.0;
                    svlod3->unk11 = 0;
                    svlod3->unk12 = 1.0;
                    svlod3->distance = 4000;
                    svlod3->AnimationCount = 0;
                    svlod3->unk14 = 0;
                    svlod3->AnimationArray = 0;
                    char *Names[3];
                    Names[0] = name;
                    Names[1] = name;
                    Names[2] = name;
#ifndef LIBOVL_STATIC
                    AddSceneryItemVisual(ovlname, Names, NULL, NULL, sv);
#else
                    sceneryovl.AddSceneryItemVisual(ovlname, Names, NULL, NULL, sv);
#endif
                }

                std::string tmperror;
                for (i = 0; i < sceneryfile->flexitextures.size(); i++) {

                    if (textureovl) {
                        texturecache.push_back(sceneryfile->flexitextures[i].Name);
                    }

                    ilBindImage(tex[0]);

                    if (!ilLoadImage(sceneryfile->flexitextures[i].Texture.c_str())) {
                        tmperror = "Can't open texture '"+sceneryfile->flexitextures[i].Texture+"'.";
                        MessageBox(hwnd, tmperror.c_str(), "Error", MB_OK);
                        while ((Error = ilGetError())) {
                            MessageBox(hwnd, iluErrorString(Error), "Error", MB_OK);
                        }
                        goto scenerycleanup;
                    }
                    int width = ilGetInteger(IL_IMAGE_WIDTH);
                    int height = ilGetInteger(IL_IMAGE_HEIGHT);
                    if (width != height) {
                        tmperror = "Texture '"+sceneryfile->flexitextures[i].Texture+"' is not square.";
                        MessageBox(hwnd, tmperror.c_str(), "Error", MB_OK);
                        goto scenerycleanup;
                    }
                    if ((1 << local_log2(width)) != width) {
                        tmperror = "The width/height of texture '"+sceneryfile->flexitextures[i].Texture+"' is not a power of 2.";
                        MessageBox(hwnd, tmperror.c_str(), "Error", MB_OK);
                        goto scenerycleanup;
                    }
                    if (ilGetInteger(IL_IMAGE_FORMAT) != IL_COLOUR_INDEX) {
                        if (sceneryfile->flexitextures[i].Recolorable) {
                            if (ilGetInteger(IL_IMAGE_FORMAT) != IL_RGB)
                                ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
                            ILubyte *tmpdata = new ILubyte[width * height];
                            ILubyte *picdata = ilGetData();
                            unsigned int paldist /*[256] */ , palmax;
                            int r, d1, d2, d3;
                            int p;
                            for (r = 0; r < height * width * 3; r += 3) {
                                palmax = UINT_MAX;
                                for (p = 1; p < 256; p++) {	// Start with 1, do not assign white
                                    d1 = (int) picdata[r] - (int) recolpalette_rgb[p].rgbRed;
                                    d2 = (int) picdata[r + 1] -
                                         (int) recolpalette_rgb[p].rgbGreen;
                                    d3 = (int) picdata[r + 2] -
                                         (int) recolpalette_rgb[p].rgbBlue;
//                                      paldist[p] = d1 * d1 + d2 * d2 + d3 * d3;
                                    paldist = d1 * d1 + d2 * d2 + d3 * d3;
                                    if (paldist < palmax) {
                                        palmax = paldist;
                                        tmpdata[r / 3] = p;
                                    }
                                }
//                                  palmax = UINT_MAX;
//                                  for (p = 1; p < 256; p++) {
//                                      if (paldist[p] < palmax) {
//                                          palmax = paldist[p];
//                                          tmpdata[r / 3] = p;
//                                      }
//                                  }
                            }

                            ILuint temppic = ilGenImage();
                            ilBindImage(temppic);
                            ilCopyImage(tex[0]);
                            if (iluColorsUsed() > 256)
                                ilSetInteger(IL_QUANTIZATION_MODE, IL_NEU_QUANT);
                            else
                                ilSetInteger(IL_QUANTIZATION_MODE, IL_WU_QUANT);
                            ilConvertImage(IL_COLOUR_INDEX, IL_UNSIGNED_BYTE);
                            ilSetData(tmpdata);
                            ilConvertPal(IL_PAL_BGR32);
                            memcpy(ilGetPalette(), &recolpalette_bmy, 256 * 4);
                            ilBindImage(tex[0]);
                            ilCopyImage(temppic);
                            ilDeleteImage(temppic);
                            delete[] tmpdata;
                        } else {
                            if (iluColorsUsed() > 256)
                                ilSetInteger(IL_QUANTIZATION_MODE, IL_NEU_QUANT);
                            else
                                ilSetInteger(IL_QUANTIZATION_MODE, IL_WU_QUANT);
                            if (!ilConvertImage(IL_COLOUR_INDEX, IL_UNSIGNED_BYTE)) {
                                tmperror = "Couldn't convert texture '"+sceneryfile->flexitextures[i].Texture+"' to indexed colour.";
                                MessageBox(hwnd, tmperror.c_str(), "Error", MB_OK);
                                goto scenerycleanup;
                            }
                        }
                    }
                    if (ilGetInteger(IL_PALETTE_TYPE) != IL_PAL_BGR32)
                        if (!ilConvertPal(IL_PAL_BGR32)) {
                            tmperror = "Couldn't convert palette of texture '"+sceneryfile->flexitextures[i].Texture+"'.";
                            MessageBox(hwnd, tmperror.c_str(), "Error", MB_OK);
                            goto scenerycleanup;
                        }
                    if (ilGetInteger(IL_IMAGE_ORIGIN) == IL_ORIGIN_UPPER_LEFT)
                        if (!iluFlipImage()) {
                            tmperror = "Couldn't flip texture '"+sceneryfile->flexitextures[i].Texture+"'.";
                            MessageBox(hwnd, tmperror.c_str(), "Error", MB_OK);
                            goto scenerycleanup;
                        }
                    bool AlphaPresent = false;
                    if (sceneryfile->flexitextures[i].Alpha != "") {
                        AlphaPresent = true;
                    }
                    if (AlphaPresent == true) {
                        ilBindImage(tex[1]);

                        if (!ilLoadImage(sceneryfile->flexitextures[i].Alpha.c_str())) {
                            tmperror = "Can't open alpha texture '"+sceneryfile->flexitextures[i].Alpha+"'.";
                            MessageBox(hwnd, tmperror.c_str(), "Error", MB_OK);
                            while ((Error = ilGetError())) {
                                MessageBox(hwnd, iluErrorString(Error), "Error", MB_OK);
                            }
                            goto scenerycleanup;
                        }
                        int alwidth = ilGetInteger(IL_IMAGE_WIDTH);
                        int alheight = ilGetInteger(IL_IMAGE_HEIGHT);
                        if ((width != alwidth) || (height != alheight)) {
                            tmperror = "Size of alpha texture '"+sceneryfile->flexitextures[i].Alpha+"' does not match with '";
                            tmperror += sceneryfile->flexitextures[i].Texture+"'.";
                            MessageBox(hwnd, tmperror.c_str(), "Error", MB_OK);
                            goto scenerycleanup;
                        }
                        if (ilGetInteger(IL_IMAGE_FORMAT) != IL_LUMINANCE)
                            if (!ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE)) {
                                tmperror = "Couldn't convert alpha texture '"+sceneryfile->flexitextures[i].Alpha+"' to grayscale.";
                                MessageBox(hwnd, tmperror.c_str(), "Error", MB_OK);
                                goto scenerycleanup;
                            }
                        if (ilGetInteger(IL_IMAGE_ORIGIN) == IL_ORIGIN_UPPER_LEFT)
                            if (!iluFlipImage()) {
                                tmperror = "Couldn't flip alpha texture '"+sceneryfile->flexitextures[i].Alpha+"'.";
                                MessageBox(hwnd, tmperror.c_str(), "Error", MB_OK);
                                goto scenerycleanup;
                            }
                    }

                    RGBQUAD *colors;
                    colors = new RGBQUAD[256];
                    memset(colors, 0, 256 * sizeof(RGBQUAD));
                    unsigned char *texture = 0;
                    unsigned char *alphadata = 0;
                    texture = new unsigned char[width * height];
                    ilBindImage(tex[0]);
                    memcpy(texture, ilGetData(), width * height);
                    memcpy(colors, ilGetPalette(), ilGetInteger(IL_PALETTE_NUM_COLS) * 4);
                    for (unsigned int j = 0; j < 256; j++)
                        colors[j].rgbReserved = 0;
                    if (AlphaPresent == true) {
                        alphadata = new unsigned char[width * height];
                        ilBindImage(tex[1]);
                        memcpy(alphadata, ilGetData(), width * height);
                    }
                    FlexiTextureStruct *fts = new FlexiTextureStruct;
                    FlexiTextureInfoStruct *fti = new FlexiTextureInfoStruct;
                    ftistore.push_back(fti); // Store for deletion
                    char *FlexiTextureName = strdup(sceneryfile->flexitextures[i].Name.c_str());
                    ftinamestore.push_back(FlexiTextureName);
                    fti->scale = local_log2(width);
                    fti->height = height;
                    fti->width = width;
                    fti->unk1 = 0;
                    fti->Recolorable = sceneryfile->flexitextures[i].Recolorable;
                    fti->unk3 = 1;
                    fti->offset1 = 0;
                    fti->unk4 = 1;
                    fti->fts2 = fts;

                    fts->scale = fti->scale;
                    fts->width = fti->width;
                    fts->height = fti->height;
                    fts->Recolorable = fti->Recolorable;
                    fts->palette = (unsigned char *) colors;
                    fts->texture = texture;
                    fts->alpha = alphadata;
#ifndef LIBOVL_STATIC
                    AddFlexiTexture(FlexiTextureName, fti);
#else
                    sceneryovl.AddFlexiTexture(FlexiTextureName, fti);
#endif
//                                                      fclose(tex);
//                                                      fclose(alpha);
                }
                for (i = 0; i < sceneryfile->references.size(); i++) {
#ifndef LIBOVL_STATIC
                    AddReference(const_cast<char *> (sceneryfile->references[i].c_str()));
#else
                    sceneryovl.AddReference(const_cast<char *> (sceneryfile->references[i].c_str()));
#endif
                }
#ifndef LIBOVL_STATIC
                SaveSceneryOvl();
#else
                sceneryovl.SaveSceneryOvl();
#endif
scenerycleanup:
                ilDeleteImages(2, (ILuint *) & tex);
                if (textureovl) {
                    texturecache.sort();
                    texturecache.unique();
                }
                delete[] ovlname;
                free(name);
                if (sh) {
                    for (i = 0; i < sh->MeshCount; i++) {
                        delete[] sh->sh[i]->Vertexes;
                        delete[] sh->sh[i]->Triangles;
                        delete sh->sh[i];
                    }
                    delete[] sh->sh;
                    for (i = 0; i < sh->EffectCount; i++)
                        free(sh->EffectName[i]);
                    delete[] sh->EffectName;
                    delete[] sh->EffectPosition;
                    delete sh;
                }
                if (ftx) {
                    for (i = 0; i < objectCount; i++) {
                        free(ftx[i]);
                        free(txs[i]);
                    }
                    delete[] ftx;
                    delete[] txs;
                }
                delete[] svd;
                while (ftistore.size()) {
                    FlexiTextureInfoStruct *ftid = ftistore.back();
                    ftistore.pop_back();
                    delete[] ftid->fts2->palette;
                    delete[] ftid->fts2->texture;
                    delete[] ftid->fts2->alpha;
                    delete ftid->fts2;
                    delete ftid;
                }
                while (ftinamestore.size()) {
                    free(ftinamestore.back());
                    ftinamestore.pop_back();
                }
            }
        }
        break;
        case ID_FILE_EXIT:
            if (scenerysave == true) {
                if (MessageBox(hwnd, "Unsaved changes, continue?", "Unsaved changes", MB_YESNO)
                        == IDNO) {
                    break;
                }
            }
            EndDialog(hwnd, 0);
            break;
        case IDC_MESHES:
            if (((int) HIWORD(wParam)) != LBN_DBLCLK)
                break;
            if (GetKeyState(VK_SHIFT)&0x80) {
                CurrentMeshStruct = SendDlgItemMessage(hwnd, IDC_MESHES, LB_GETCURSEL, 0, 0);
                unsigned int i;
                for (i = 0; i < sceneryfile->meshstructs.size(); i++) {
                    if (i == CurrentMeshStruct)
                        sceneryfile->meshstructs[i].disabled = false;
                    else
                        sceneryfile->meshstructs[i].disabled = true;
                }
                scenerysave = true;
                InvalidateRect(GetDlgItem(hwnd, IDC_MESHES), NULL, TRUE);
                break;
            }
            // fall through if double click
        case IDC_MESHFLAGS: {
            CurrentMeshStruct = SendDlgItemMessage(hwnd, IDC_MESHES, LB_GETCURSEL, 0, 0);
            if (CurrentMeshStruct != -1) {
                wxArrayString textures;
                for (cFlexiTextureIterator ft = sceneryfile->flexitextures.begin();
                        ft != sceneryfile->flexitextures.end(); ft++) {
                    textures.Add(ft->Name);
                }
                for(std::list<wxString>::iterator st = texturecache.begin(); st != texturecache.end(); st++) {
                    textures.Add(*st);
                }
                dlgMesh *dialog = new dlgMesh(&textures, NULL);
                dialog->SetMeshStruct(sceneryfile->meshstructs[CurrentMeshStruct]);
                if (dialog->ShowModal() == wxID_OK) {
                    if (dialog->GetApplyAll()) {
                        cMeshStruct mstr = dialog->GetMeshStruct();
                        for(cMeshStructIterator ms = sceneryfile->meshstructs.begin(); ms != sceneryfile->meshstructs.end(); ms++) {
                            ms->disabled = mstr.disabled;
                            ms->FTX = mstr.FTX;
                            ms->TXS = mstr.TXS;
                            ms->flags = mstr.flags;
                            ms->place = mstr.place;
                            ms->unknown = mstr.unknown;
                        }
                    } else {
                        sceneryfile->meshstructs[CurrentMeshStruct] = dialog->GetMeshStruct();
                    }
                    scenerysave = true;
                    InvalidateRect(GetDlgItem(hwnd, IDC_MESHES), NULL, TRUE);
                }
                dialog->Destroy();
            }
        }
            break;
        case IDC_OPENASE: {
            char sfile[MAX_PATH] = "";
            OPENFILENAME ofn;
            if (sceneryfile->objfile != "")
                strcpy(sfile, sceneryfile->objfile.c_str());
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Supported 3D Files\0*.ase;*.ms3d\0ASE Files (*.ase)\0*.ase\0Milkshape 3D (*.ms3d)\0*.ms3d\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = sfile;
            ofn.lpstrTitle = "Open 3D File";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = ".ase";
            ofn.Flags =
                OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT |
                OFN_NOCHANGEDIR;
            if (GetOpenFileName(&ofn)) {
                SetDlgItemText(hwnd, IDC_ASE, sfile);
            }

            unsigned int aselen = GetWindowTextLength(GetDlgItem(hwnd, IDC_ASE));
            if (aselen != 0) {
                char *ase = new char[aselen + 1];
                GetDlgItemText(hwnd, IDC_ASE, ase, aselen + 1);
                if (stricmp(ase, sceneryfile->objfile.c_str()) != 0) {
                    sceneryfile->objfile = ase;
                    if (sceneryfile->LoadObject()) {
                        SendDlgItemMessage(hwnd, IDC_MESHES, LB_RESETCONTENT, 0, 0);
                        for (std::vector<cMeshStruct>::iterator ms = sceneryfile->meshstructs.begin(); ms != sceneryfile->meshstructs.end(); ms++) {
                            SendDlgItemMessage(hwnd, IDC_MESHES, LB_ADDSTRING, 0,
                                               (LPARAM) ms->Name.c_str());
                        }
                        scenerysave = true;
                    } else {
                        MessageBox(hwnd, "Possible Reasons:\n- File not found\n- Unknown object format\n- Object contains no 3D data", "Error Loading Object File", MB_OK|MB_ICONERROR);
                    }

                }
                delete[] ase;
            }

        }
        break;
        case IDC_ADDTEX: {
            dlgTexture *dialog = new dlgTexture(NULL);
            if (dialog->ShowModal() == wxID_OK) {
                sceneryfile->flexitextures.push_back(dialog->GetFlexiTexture());
                SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_RESETCONTENT, 0, 0);
                for (cFlexiTextureIterator ft = sceneryfile->flexitextures.begin();
                        ft != sceneryfile->flexitextures.end(); ft++) {
                    SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_ADDSTRING, 0,
                                       (LPARAM) ft->Name.c_str());
                }
                scenerysave = true;
            }
            delete dialog;
        }
            break;
        case IDC_TEXTURES:
            if (((int) HIWORD(wParam)) != LBN_DBLCLK)
                break;
            // fall through if double click
        case IDC_EDITTEX:
            CurrentFlexiTexture = SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_GETCURSEL, 0, 0);
            if (CurrentFlexiTexture != -1) {
                ::wxMessageBox(_("No Texture selected. Please add one first."), _("Error"), wxOK | wxICON_ERROR);
                break;
            }
            dlgTexture *dialog = new dlgTexture(NULL);
            dialog->SetFlexiTexture(sceneryfile->flexitextures[CurrentFlexiTexture]);
            if (dialog->ShowModal() == wxID_OK) {
                sceneryfile->flexitextures[CurrentFlexiTexture] = dialog->GetFlexiTexture();
                SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_RESETCONTENT, 0, 0);
                for (cFlexiTextureIterator ft = sceneryfile->flexitextures.begin();
                        ft != sceneryfile->flexitextures.end(); ft++) {
                    SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_ADDSTRING, 0,
                                       (LPARAM) ft->Name.c_str());
                }
                scenerysave = true;
            }
            delete dialog;
            break;
        case IDC_DELTEX:
            CurrentFlexiTexture = SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_GETCURSEL, 0, 0);
            if (CurrentFlexiTexture != -1) {
                sceneryfile->flexitextures.erase(sceneryfile->flexitextures.begin() + CurrentFlexiTexture);
                SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_RESETCONTENT, 0, 0);
                for (cFlexiTextureIterator ft = sceneryfile->flexitextures.begin();
                        ft != sceneryfile->flexitextures.end(); ft++) {
                    SendDlgItemMessage(hwnd, IDC_TEXTURES, LB_ADDSTRING, 0,
                                       (LPARAM) ft->Name.c_str());
                }
                scenerysave = true;
            }
            break;
        case IDC_ADDPT: {

//            dlgEffect *dialog = new dlgEffect(NULL);
            dlgCreateScenery *dialog = new dlgCreateScenery(NULL);
            dialog->ShowModal();
            delete dialog;

            CurrentEffectPoint = -1;
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_EFFECTPT), hwnd, EffectPointDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_RESETCONTENT, 0, 0);
                for (cEffectPointIterator ef = sceneryfile->effectpoints.begin();
                        ef != sceneryfile->effectpoints.end(); ef++) {
                    SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_ADDSTRING, 0,
                                       (LPARAM) ef->Name.c_str());
                }
                scenerysave = true;
            }
        }
            break;
        case IDC_EFFECTPT:
            if (((int) HIWORD(wParam)) != LBN_DBLCLK)
                break;
            // fall through if double click
        case IDC_EDITPT:
            CurrentEffectPoint = SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_GETCURSEL, 0, 0);
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_EFFECTPT), hwnd, EffectPointDlgProc) == 1) {
                SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_RESETCONTENT, 0, 0);
                for (cEffectPointIterator ef = sceneryfile->effectpoints.begin();
                        ef != sceneryfile->effectpoints.end(); ef++) {
                    SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_ADDSTRING, 0,
                                       (LPARAM) ef->Name.c_str());
                }
                scenerysave = true;
            }
            break;
        case IDC_DELPT:
            CurrentEffectPoint = SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_GETCURSEL, 0, 0);
            if (CurrentEffectPoint != -1) {
                sceneryfile->effectpoints.erase(sceneryfile->effectpoints.begin() + CurrentEffectPoint);
                SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_RESETCONTENT, 0, 0);
                for (cEffectPointIterator ef = sceneryfile->effectpoints.begin();
                        ef != sceneryfile->effectpoints.end(); ef++) {
                    SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_ADDSTRING, 0,
                                       (LPARAM) ef->Name.c_str());
                }
                scenerysave = true;
            }
            break;
        case IDC_AUTOPT: {
            BOOL haspoints = false;
            for (cMeshStructIterator ms = sceneryfile->meshstructs.begin();
                    ms != sceneryfile->meshstructs.end(); ms++) {
                if (ms->effectpoint) {
                    haspoints = true;
                    cEffectPoint t;

                    t.Name = ms->Name;
                    t.Transform.push_back(matrixGetTranslation(ms->effectpoint_vert));
                    wxString nam = wxT("Auto-generated form mesh '")+ms->Name+wxT("'");
                    t.TransformNames.push_back(nam);
                    sceneryfile->effectpoints.push_back(t);
                }
            }
            if (haspoints) {
                SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_RESETCONTENT, 0, 0);
                for (cEffectPointIterator ef = sceneryfile->effectpoints.begin();
                        ef != sceneryfile->effectpoints.end(); ef++) {
                    SendDlgItemMessage(hwnd, IDC_EFFECTPT, LB_ADDSTRING, 0,
                                       (LPARAM) ef->Name.c_str());
                }
                scenerysave = true;
            } else
                MessageBox(hwnd,
                           "No meshes with only one vertex found. These are necessary for automatic effect point creation.",
                           "Error", MB_OK | MB_ICONWARNING);
        }
        break;
        case IDC_DELREF:
            CurrentReference = SendDlgItemMessage(hwnd, IDC_REFERENCES, LB_GETCURSEL, 0, 0);
            if (CurrentReference != -1) {
                sceneryfile->references.erase(sceneryfile->references.begin() + CurrentReference);
                SendDlgItemMessage(hwnd, IDC_REFERENCES, LB_RESETCONTENT, 0, 0);
                for (cStringIterator ref = sceneryfile->references.begin();
                        ref != sceneryfile->references.end(); ref++) {
                    SendDlgItemMessage(hwnd, IDC_REFERENCES, LB_ADDSTRING, 0,
                                       (LPARAM) ref->c_str());
                }
                scenerysave = true;
            }
            break;
        case IDC_ADDREF: {
            unsigned int reflen = GetWindowTextLength(GetDlgItem(hwnd, IDC_REFERENCE));
            if (reflen != 0) {
                char *reference = new char[reflen + 1];
                GetDlgItemText(hwnd, IDC_REFERENCE, reference, reflen + 1);
                SetDlgItemText(hwnd, IDC_REFERENCE, NULL);
                wxString ref = reference;
                sceneryfile->references.push_back(ref);
                CurrentReference = sceneryfile->references.size() - 1;
                SendDlgItemMessage(hwnd, IDC_REFERENCES, LB_ADDSTRING, 0, (LPARAM) reference);
                delete[] reference;
                scenerysave = true;
            }
        }
        break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}
