///////////////////////////////////////////////////////////////////////////////
//
// 3D Utility Library
// C++ structures for the importer
// Copyright (C) 2006 Tobias Minch
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

#include "RCT3Structs.h"

void cMeshStruct::CopySettingsFrom(const cMeshStruct& from) {
    disabled = from.disabled;
    FTX = from.FTX;
    TXS = from.TXS;
    flags = from.flags;
    place = from.place;
    unknown = from.unknown;
}

cModel::cModel(D3DMATRIX def): name(wxT("")), file(wxT("")), orientation(ORIENTATION_UNKNOWN), fatal_error(false) {
    transforms.push_back(def);
    transformnames.push_back(_("Default Matrix"));
}

void cModel::SetupFileProperties(cMeshStruct* ms, c3DLoader* obj, unsigned int n) {
    if ((obj->GetObjectName(n).CmpNoCase(wxT("root")) == 0)
            || (obj->GetObjectVertexCount(n) < 3)
            || (!obj->IsObjectValid(n))) {
        ms->valid = false;
        if (!ms->disabled)
            error.push_back(wxString::Format(_("Mesh '%s' has been automatically disabled as it is invalid (broken or missing uv-mapping)."), obj->GetObjectName(n).c_str()));
        ms->disabled = true;
    } else {
        ms->valid = true;
    }
    if (obj->GetObjectVertexCount(n) == 1) {
        ms->effectpoint = true;
        ms->effectpoint_vert = obj->GetObjectVertex(n, 0).position;
    } else {
        ms->effectpoint = false;
    }
    ms->faces = obj->GetObjectIndexCount(n) / 3;
}

cMeshStruct cModel::MakeMesh(c3DLoader* obj, unsigned int n) {
    cMeshStruct ms;
    if ((obj->GetObjectName(n).CmpNoCase(wxT("root")) == 0)
            || (obj->GetObjectVertexCount(n) < 3)
            || (!obj->IsObjectValid(n))) {
        ms.disabled = true;
    } else {
        ms.disabled = false;
    }
    SetupFileProperties(&ms, obj, n);
    ms.Name = obj->GetObjectName(n);
    ms.flags = 0;
    ms.place = 0;
    ms.unknown = 3;
    ms.FTX = "";
    ms.TXS = "";

    return ms;
}

bool cModel::Load() {
    c3DLoader *obj = c3DLoader::LoadFile(const_cast<char *>(file.GetFullPath().fn_str()));

    if (!obj) {
        fatal_error = true;
        error.Add(_("Couldn't load file. Wrong format or file not found."));
        return false;
    }

    if (name == wxT(""))
        name = obj->GetName();

    if (name == wxT("")) {
        wxFileName t = file;
        name = t.GetName();
    }
    fatal_error = false;
    error.clear();

    orientation = obj->GetOrientation();

    meshstructs.clear();
    // Load the object into mesh structs
    for (long j = 0; j < obj->GetObjectCount(); j++) {
        meshstructs.push_back(MakeMesh(obj, j));
    }

    delete obj;

    return true;
}

bool cModel::Sync() {
    fatal_error = false;
    error.clear();

    // No model file, no need to sync
    if (file == wxT(""))
        return true;

    // No mesh structs, reinit from the file
    if (meshstructs.size() == 0)
        return Load();

    // Load & check the object file
    c3DLoader *obj = c3DLoader::LoadFile(const_cast<char *>(file.GetFullPath().fn_str()));
    if (!obj) {
        fatal_error = true;
        error.push_back(_("Model file not found or of an unknown format."));
        return false;
    }

    orientation = obj->GetOrientation();

    // Predeclare to make goto work
    wxString notify;
    std::vector<cMeshStruct> fixup;
    std::vector<unsigned int> unfound;
    int msinfile = 0;
    int nrfound = 0;

    // See if it's an old scn file where the mesh names were not stored
    bool oldfile = (meshstructs[0].Name == wxT(""));

    if (oldfile) {
        if (obj->GetObjectCount() == meshstructs.size()) {
            // Number of meshes unchanged. We simply assume the order hasn't changed
            for(unsigned int i = 0; i < meshstructs.size(); i++)
                meshstructs[i].Name = obj->GetObjectName(i);
        } else if (obj->GetObjectCount() > meshstructs.size()) {
            // New meshes in the model file
            // Warn about it
            error.push_back(wxString::Format(_("%d new mesh(es) in the model file.\nAs the scn file is too old to map the meshes, the ones known were assigned in order and the new ones appended.\nYou should probably check if everything is allright."),
                        obj->GetObjectCount()-meshstructs.size()));
            // Assign the ones present
            for(unsigned int i = 0; i < meshstructs.size(); i++)
                meshstructs[i].Name = obj->GetObjectName(i);
            // Initialize the new ones
            for(unsigned int i = meshstructs.size(); i < obj->GetObjectCount(); i++)
                meshstructs.push_back(MakeMesh(obj, i));
        } else if (obj->GetObjectCount() < meshstructs.size()) {
            // Meshes were removed from the model file
            // Warn about it
            error.push_back(wxString::Format(_("%d mesh(es) were removed from the model file.\nAs the scn file is too old to map the meshes, the ones known were assigned in order and the rest was deleted.\nYou should probably check if everything is allright."),
                        meshstructs.size()-obj->GetObjectCount()));
            // Assign the ones present
            for(unsigned int i = 0; i < obj->GetObjectCount(); i++)
                meshstructs[i].Name = obj->GetObjectName(i);
            // Delete the unused entries
            meshstructs.erase(meshstructs.begin() + obj->GetObjectCount(), meshstructs.end());
        }
        goto fixupsinglevertexmeshes;
    }

    // We have a newer scn file with stored mesh names
    // before getting serious we do a simple check to see if everything is allright
    if (obj->GetObjectCount() == meshstructs.size()) {
        bool allright = true;
        for(unsigned int i = 0; i < meshstructs.size(); i++) {
            if (meshstructs[i].Name.CmpNoCase(obj->GetObjectName(i)) == 0) {
                // It matches, we update the name in case the case changed
                meshstructs[i].Name = obj->GetObjectName(i);
            } else {
                // It does not match
                allright = false;
                break;
            }
        }
        if (allright)
            goto fixupsinglevertexmeshes;
    }

    // We need to fix things
    msinfile = meshstructs.size();

    // First round of fixups
    nrfound = 0;
    for (unsigned int i = 0; i < obj->GetObjectCount(); i++) {
        bool found = false;
        for (cMeshStructIterator ms = meshstructs.begin(); ms != meshstructs.end(); ms++) {
            if (ms->Name.CmpNoCase(obj->GetObjectName(i)) == 0) {
                // Found!
                // We update the name in case the case changed
                ms->Name = obj->GetObjectName(i);
                fixup.push_back(*ms);
                found = true;
                nrfound++;
                meshstructs.erase(ms);
                break;
            }
        }
        if (!found) {
            unfound.push_back(i);
            fixup.push_back(MakeMesh(obj, i));
        }
    }

    // If the fixups were successful, meshstructs is empty
    if (meshstructs.size() == 0) {
        notify = _("The model file contents changed, everything could be matched up.");
        if (msinfile != obj->GetObjectCount()) {
            // More meshes in the model file
            notify += wxString::Format(_("\nThere were %d new meshes in the model file."), obj->GetObjectCount() - nrfound);
        }
        error.push_back(notify);
        meshstructs = fixup;
        goto fixupsinglevertexmeshes;
    }

    // Didn't work. Yay! -.-
    // Special case: Nothing worked, but we have the same number of meshes/objects
    if (obj->GetObjectCount() == meshstructs.size() == msinfile) {
        error.push_back(_("The model file contents changed completely.\nAs the number of meshes stayed constant, the settings were transferred in order.\nYou should prabably check if everything is allright."));
        for (unsigned int i = 0; i < obj->GetObjectCount(); i++) {
            meshstructs[i].Name = obj->GetObjectName(i);
        }
        goto fixupsinglevertexmeshes;
    }

    // Special case: Meshes/Objects disappeared
    if (nrfound == obj->GetObjectCount()) {
        error.push_back(_("The model file contents changed, it seems like you deleted at least one mesh.\nAll meshes in the object file could be matched, so probably everything is ok.\nYou should still check if everything is allright."));
        meshstructs.clear();
        meshstructs = fixup;
        goto fixupsinglevertexmeshes;
    }

    // Now we blindly apply unassigned mesh info
    notify = _("The model file contents changed, an attempt was made to match everything up. Log:");
    for (;;) {
        if ((!unfound.size()) || (!meshstructs.size())) {
            // No more model file or scn file meshes to assign, bail out
            break;
        }
        unsigned int a = unfound[0];
        unfound.erase(unfound.begin());
        notify += wxString::Format(_("\n- The settings of mesh '%s' were applied to mesh '%s' from the model file."), meshstructs[0].Name.c_str(), fixup[a].Name.c_str());
        fixup[a].CopySettingsFrom(meshstructs[0]);
        meshstructs.erase(meshstructs.begin());
    }
    // Whatever could be done has been done. warn about the rest
    if (unfound.size()) {
        // Additional meshes in the model file.
        notify += _("\nThere were additinal meshes in the model file, they were added to the list:");
        for (std::vector<unsigned int>::iterator it = unfound.begin(); it != unfound.end(); it++) {
            notify += wxString::Format(wxT("\n- %s"), fixup[*it].Name.c_str());
        }
    } else if (meshstructs.size()) {
        // Additional meshes in the model file.
        notify += _("\nThere were not enough meshes in the model file, surplus one were deleted:");
        for (cMeshStructIterator ms = meshstructs.begin(); ms != meshstructs.end(); ms++) {
            notify += wxString::Format(wxT("\n- %s"), ms->Name.c_str());
        }
    } else {
        notify += _("\nThe number of meshes in the model file matched.\n(Note: This is the result you probably get if you renamed a mesh in the model file.)");
    }
    error.push_back(notify + _("\nYou should check if everything is allright."));
    meshstructs.clear();
    meshstructs = fixup;

fixupsinglevertexmeshes:
    for (unsigned int i = 0; i < obj->GetObjectCount(); i++)
        SetupFileProperties(&meshstructs[i], obj, i);
    delete obj;
    return true;
}
