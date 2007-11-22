///////////////////////////////////////////////////////////////////////////////
//
// Config file implemetation
// Copyright (C) 2006 Tobias Minch, Jonathan Wilson
//
// Part of rct3tool
// Copyright 2005 Jonathan Wilson
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

#include "win_predefine.h"

#include <string>
#include <stdio.h>

#include <wx/tooltip.h>

#include "matrix.h"

#include "rct3config.h"

RCT3Config::RCT3Config() {
    m_configfile = wxT("RCT3 Importer.cfg");
    m_prefix = "";
    m_def_matrix = matrixGetUnity();
    m_def_orient = ORIENTATION_LEFT_YUP;
    m_tooltips = true;
}

RCT3Config::RCT3Config(wxString appdir) {
    m_configfile = appdir + wxT("RCT3 Importer.cfg");
    m_prefix = "";
    m_def_matrix = matrixGetUnity();
    m_def_orient = ORIENTATION_LEFT_YUP;
    m_tooltips = true;
}

bool RCT3Config::Save() {
    FILE *f;
    f = fopen(m_configfile.c_str(), "wb");
    if (!f)
        return false;
    unsigned long version = RCT3CONFIG_VERSION;
    fwrite(&version, sizeof(version), 1, f);

    unsigned long len = m_prefix.length()+1;
    fwrite(&len, sizeof(len), 1, f);
    fwrite(m_prefix.c_str(), 1, len, f);

    fwrite(&m_def_matrix._11, sizeof(m_def_matrix._11), 1, f);
    fwrite(&m_def_matrix._12, sizeof(m_def_matrix._12), 1, f);
    fwrite(&m_def_matrix._13, sizeof(m_def_matrix._13), 1, f);
    fwrite(&m_def_matrix._14, sizeof(m_def_matrix._14), 1, f);
    fwrite(&m_def_matrix._21, sizeof(m_def_matrix._21), 1, f);
    fwrite(&m_def_matrix._22, sizeof(m_def_matrix._22), 1, f);
    fwrite(&m_def_matrix._23, sizeof(m_def_matrix._23), 1, f);
    fwrite(&m_def_matrix._24, sizeof(m_def_matrix._24), 1, f);
    fwrite(&m_def_matrix._31, sizeof(m_def_matrix._31), 1, f);
    fwrite(&m_def_matrix._32, sizeof(m_def_matrix._32), 1, f);
    fwrite(&m_def_matrix._33, sizeof(m_def_matrix._33), 1, f);
    fwrite(&m_def_matrix._34, sizeof(m_def_matrix._34), 1, f);
    fwrite(&m_def_matrix._41, sizeof(m_def_matrix._41), 1, f);
    fwrite(&m_def_matrix._42, sizeof(m_def_matrix._42), 1, f);
    fwrite(&m_def_matrix._43, sizeof(m_def_matrix._43), 1, f);
    fwrite(&m_def_matrix._44, sizeof(m_def_matrix._44), 1, f);

    len = m_workdir.length()+1;
    fwrite(&len, sizeof(len), 1, f);
    fwrite(m_workdir.c_str(), 1, len, f);
    unsigned long ori = m_def_orient;
    fwrite(&ori, sizeof(ori), 1, f);
    fwrite(&m_tooltips, sizeof(m_tooltips), 1, f);

    fclose(f);
    return true;
}

void RCT3Config::Load() {
    FILE *f;
    f = fopen(m_configfile.c_str(), "rb");
    if (!f)
        return;
    unsigned long version = 0;
    fread(&version, sizeof(version), 1, f);

    unsigned long len;
    fread(&len, sizeof(len), 1, f);
    char *tmp = new char[len];
    fread(tmp, 1, len, f);
    m_prefix = tmp;
    delete[] tmp;

    fread(&m_def_matrix._11, sizeof(m_def_matrix._11), 1, f);
    fread(&m_def_matrix._12, sizeof(m_def_matrix._12), 1, f);
    fread(&m_def_matrix._13, sizeof(m_def_matrix._13), 1, f);
    fread(&m_def_matrix._14, sizeof(m_def_matrix._14), 1, f);
    fread(&m_def_matrix._21, sizeof(m_def_matrix._21), 1, f);
    fread(&m_def_matrix._22, sizeof(m_def_matrix._22), 1, f);
    fread(&m_def_matrix._23, sizeof(m_def_matrix._23), 1, f);
    fread(&m_def_matrix._24, sizeof(m_def_matrix._24), 1, f);
    fread(&m_def_matrix._31, sizeof(m_def_matrix._31), 1, f);
    fread(&m_def_matrix._32, sizeof(m_def_matrix._32), 1, f);
    fread(&m_def_matrix._33, sizeof(m_def_matrix._33), 1, f);
    fread(&m_def_matrix._34, sizeof(m_def_matrix._34), 1, f);
    fread(&m_def_matrix._41, sizeof(m_def_matrix._41), 1, f);
    fread(&m_def_matrix._42, sizeof(m_def_matrix._42), 1, f);
    fread(&m_def_matrix._43, sizeof(m_def_matrix._43), 1, f);
    fread(&m_def_matrix._44, sizeof(m_def_matrix._44), 1, f);

    if (version > 1) {
        fread(&len, sizeof(len), 1, f);
        tmp = new char[len];
        fread(tmp, 1, len, f);
        m_workdir = tmp;
        delete[] tmp;
    }
    if (version > 2) {
        unsigned long ori;
        fread(&ori, sizeof(ori), 1, f);
        m_def_orient = static_cast<c3DLoaderOrientation>(ori);
    }
    if (version > 3) {
        fread(&m_tooltips, sizeof(m_tooltips), 1, f);
        wxToolTip::Enable(m_tooltips);
    }

    fclose(f);
}
