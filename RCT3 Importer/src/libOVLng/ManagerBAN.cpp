///////////////////////////////////////////////////////////////////////////////
//
// New OVL creation library
// Manager class for BAN structures
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


#include "ManagerBAN.h"

#include "OVLDebug.h"
#include "OVLException.h"
#include "OVLng.h"

using namespace r3;
using namespace std;

const char* ovlBANManager::NAME = "BoneAnim";
const char* ovlBANManager::TAG = "ban";

float cBoneAnimBone::Fill(r3::BoneAnimBone* bone) {
    float f = 0.0;
    //bone->Name = NULL; will be assigned before Fill is called
    bone->TranslateCount = translations.size();
    unsigned long c = 0;
    for (set<r3::txyz, cTXYZComp>::iterator it = translations.begin(); it != translations.end(); ++it) {
        bone->Translate[c] = *it;
        if (it->Time > f)
            f = it->Time;
        c++;
    }
    bone->RotateCount = rotations.size();
    c = 0;
    for (set<r3::txyz, cTXYZComp>::iterator it = rotations.begin(); it != rotations.end(); ++it) {
        bone->Rotate[c] = *it;
        if (it->Time > f)
            f = it->Time;
        c++;
    }
    return f;
}

void cBoneAnim::Fill(r3::BoneAnim* ban) {
    if (calc_time)
        ban->TotalTime = 0.0;
    else
        ban->TotalTime = totaltime;
    ban->BoneCount = bones.size();
    for (unsigned int i = 0; i < bones.size(); ++i) {
        float f = bones[i].Fill(&ban->Bones[i]);
        if (calc_time && (f > ban->TotalTime))
            ban->TotalTime = f;
    }
}


void ovlBANManager::AddAnimation(const cBoneAnim& item) {
    Check("ovlBANManager::AddAnimation");
    if (item.name == "")
        throw EOvl("ovlBANManager::AddAnimation called without name");
    if (m_items.find(item.name) != m_items.end())
        throw EOvl("ovlBANManager::AddAnimation: Item with name '"+item.name+"' already exists");
    for (vector<cBoneAnimBone>::const_iterator it = item.bones.begin(); it != item.bones.end(); ++it) {
        if (it->name == "")
            throw EOvl("ovlBANManager::AddAnimation called with nameless bone");
    }

    m_items[item.name] = item;

    // BoneAnim
    m_size += sizeof(BoneAnim);
    // BoneAnimBones
    m_size += item.bones.size() * sizeof(BoneAnimBone);
    // Frames
    for (vector<cBoneAnimBone>::const_iterator it = item.bones.begin(); it != item.bones.end(); ++it) {
        GetStringTable()->AddString(it->name.c_str());
        // Translations
        m_size += it->translations.size() * sizeof(txyz);
        // Rotations
        m_size += it->rotations.size() * sizeof(txyz);
    }


    GetLSRManager()->AddSymbol(OVLT_COMMON);
    GetLSRManager()->AddLoader(OVLT_COMMON);
    GetStringTable()->AddSymbolString(item.name.c_str(), ovlBANManager::TAG);
}

void ovlBANManager::Make(cOvlInfo* info) {
    DUMP_LOG("Trace: ovlBANManager::Make()");
    Check("ovlBANManager::Make");

    m_blobs[""] = cOvlMemBlob(OVLT_COMMON, 4, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[""].data;

    for (map<string, cBoneAnim>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
        BoneAnim* c_ban = reinterpret_cast<BoneAnim*>(c_data);
        c_data += sizeof(BoneAnim);

        c_ban->Bones = reinterpret_cast<BoneAnimBone*>(c_data);
        c_data += it->second.bones.size() * sizeof(BoneAnimBone);
        GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_ban->Bones));

        unsigned long c = 0;
        for (vector<cBoneAnimBone>::iterator itb = it->second.bones.begin(); itb != it->second.bones.end(); ++itb) {
            c_ban->Bones[c].Name = GetStringTable()->FindString(itb->name.c_str());
            GetRelocationManager()->AddRelocation(reinterpret_cast<unsigned long *>(&c_ban->Bones[c].Name));

            if (itb->translations.size()) {
                c_ban->Bones[c].Translate = reinterpret_cast<txyz*>(c_data);
                c_data += itb->translations.size() * sizeof(txyz);
                GetRelocationManager()->AddRelocation((unsigned long *)&c_ban->Bones[c].Translate);
            }
            if (itb->rotations.size()) {
                c_ban->Bones[c].Rotate = reinterpret_cast<txyz*>(c_data);
                c_data += itb->rotations.size() * sizeof(txyz);
                GetRelocationManager()->AddRelocation((unsigned long *)&c_ban->Bones[c].Rotate);
            }
            c++;
        }

        it->second.Fill(c_ban);

        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_COMMON, GetStringTable()->FindSymbolString(it->second.name.c_str(), Tag()), reinterpret_cast<unsigned long*>(c_ban));
        GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(c_ban), false, c_symbol);
        GetLSRManager()->CloseLoader(OVLT_COMMON);
    }
}

/*
ovlBANManager::~ovlBANManager() {
    for (unsigned long i = 0; i < m_animationlist.size(); ++i) {
        for (unsigned long s = 0; s < m_animationlist[i]->BoneCount; ++s) {
            delete[] m_animationlist[i]->Bones[s].Name;
            delete[] m_animationlist[i]->Bones[s].Translate;
            delete[] m_animationlist[i]->Bones[s].Rotate;
        }
        delete[] m_animationlist[i]->Bones;
        delete m_animationlist[i];
    }
}

void ovlBANManager::AddAnimation(const char* name, unsigned long bones, float totaltime) {
    Check("ovlBANManager::AddAnimation");
    if (m_bonecount)
        throw EOvl("ovlBANManager::Make called but there are unassigned bones left");
    if (m_transcount)
        throw EOvl("ovlBANManager::Make called but there are unassigned translations left");
    if (m_rotcount)
        throw EOvl("ovlBANManager::Make called but there are unassigned rotations left");

    BoneAnim* c_ban = new BoneAnim;
    c_ban->BoneCount = bones;
    c_ban->Bones = new BoneAnimBone[c_ban->BoneCount];
    c_ban->TotalTime = totaltime;

    // BoneAnim
    m_size += sizeof(BoneAnim);
    // BoneAnimBones
    m_size += c_ban->BoneCount * sizeof(BoneAnimBone);

    m_animationlist.push_back(c_ban);
    GetLSRManager()->AddLoader(OVLT_COMMON);
    GetLSRManager()->AddSymbol(OVLT_COMMON);
    m_animationnames.push_back(string(name));
    GetStringTable()->AddSymbolString(name, Tag());

    //m_canim = c_ban;
    m_cbone = c_ban->Bones;
    m_bonecount = c_ban->BoneCount;
}

void ovlBANManager::AddBone(const char* name, unsigned long translations, unsigned long rotations) {
    Check("ovlBANManager::AddBone");
    if (!m_bonecount)
        throw EOvl("ovlBANManager::AddBone called but there are no unassigned bones left");
    if (!m_cbone)
        throw EOvl("ovlBANManager::AddBone called but there is no bone pointer");
    if (m_transcount)
        throw EOvl("ovlBANManager::AddBone called but there are unassigned translations left");
    if (m_rotcount)
        throw EOvl("ovlBANManager::AddBone called but there are unassigned rotations left");

    m_cbone->Name = new char[strlen(name)+1];
    strcpy(m_cbone->Name, name);
    m_cbone->TranslateCount = translations;
    if (m_cbone->TranslateCount)
        m_cbone->Translate = new txyz[m_cbone->TranslateCount];
    else
        m_cbone->Translate = NULL;
    m_cbone->RotateCount = rotations;
    if (m_cbone->RotateCount)
        m_cbone->Rotate = new txyz[m_cbone->RotateCount];
    else
        m_cbone->Rotate = NULL;

    // Translations
    m_size += m_cbone->TranslateCount * sizeof(txyz);
    // Rotations
    m_size += m_cbone->RotateCount * sizeof(txyz);

    GetStringTable()->AddString(name);

    m_transcount = m_cbone->TranslateCount;
    m_ntrans = 0;
    m_rotcount = m_cbone->RotateCount;
    m_nrot = 0;
}

void ovlBANManager::CloseBone() {
    if ((!m_transcount) && (!m_rotcount)) {
        m_cbone++;
        m_bonecount--;
    }
}

void ovlBANManager::AddTranslation(const txyz& t) {
    Check("ovlBANManager::AddTranslation");
    if (!m_cbone)
        throw EOvl("ovlBANManager::AddTranslation called but there is no bone pointer");
    if (!m_transcount)
        throw EOvl("ovlBSHManager::AddTranslation called but there are no unassigned translations left");

    m_cbone->Translate[m_ntrans++] = t;
    m_transcount--;
    CloseBone();
}

void ovlBANManager::AddRotation(const txyz& t) {
    Check("ovlBANManager::AddRotation");
    if (!m_cbone)
        throw EOvl("ovlBANManager::AddRotation called but there is no bone pointer");
    if (!m_rotcount)
        throw EOvl("ovlBANManager::AddRotation called but there are no unassigned rotations left");

    m_cbone->Rotate[m_nrot++] = t;
    m_rotcount--;
    CloseBone();
}

unsigned char* ovlBANManager::Make(cOvlInfo* info) {
    Check("ovlBANManager::Make");
    if (m_bonecount)
        throw EOvl("ovlBANManager::Make called but there are unassigned bones left");
    if (m_transcount)
        throw EOvl("ovlBANManager::Make called but there are unassigned translations left");
    if (m_rotcount)
        throw EOvl("ovlBANManager::Make called but there are unassigned rotations left");


    m_blobs[0] = cOvlMemBlob(OVLT_COMMON, 4, m_size);
    ovlOVLManager::Make(info);
    unsigned char* c_data = m_blobs[0].data;

    for (unsigned long i = 0; i < m_animationlist.size(); ++i) {
        // Data transfer, BoneAnim
        BoneAnim* c_ban = reinterpret_cast<BoneAnim*>(c_data);
        c_data += sizeof(BoneAnim);
        memcpy(c_ban, m_animationlist[i], sizeof(BoneAnim));

        // Assign space for bones and transfer counts
        c_ban->Bones = reinterpret_cast<BoneAnimBone*>(c_data);
        c_data += c_ban->BoneCount * sizeof(BoneAnimBone);
        memcpy(c_ban->Bones, m_animationlist[i]->Bones, c_ban->BoneCount * sizeof(BoneAnimBone));
        GetRelocationManager()->AddRelocation((unsigned long *)&c_ban->Bones);

        // Symbol and Loader
        SymbolStruct* c_symbol = GetLSRManager()->MakeSymbol(OVLT_COMMON, GetStringTable()->FindSymbolString(m_animationnames[i].c_str(), Tag()), reinterpret_cast<unsigned long*>(c_ban));
        GetLSRManager()->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(c_ban), false, c_symbol);
        GetLSRManager()->CloseLoader(OVLT_COMMON);

        for (unsigned long s = 0; s < c_ban->BoneCount; ++s) {
            // Bone name
            c_ban->Bones[s].Name = GetStringTable()->FindString(m_animationlist[i]->Bones[s].Name);
            GetRelocationManager()->AddRelocation((unsigned long *)&c_ban->Bones[s].Name);

            // Translations
            if (c_ban->Bones[s].TranslateCount) {
                c_ban->Bones[s].Translate = reinterpret_cast<txyz*>(c_data);
                c_data += c_ban->Bones[s].TranslateCount * sizeof(txyz);
                memcpy(c_ban->Bones[s].Translate, m_animationlist[i]->Bones[s].Translate, c_ban->Bones[s].TranslateCount * sizeof(txyz));
                GetRelocationManager()->AddRelocation((unsigned long *)&c_ban->Bones[s].Translate);
            }

            // Rotations
            if (c_ban->Bones[s].RotateCount) {
                c_ban->Bones[s].Rotate = reinterpret_cast<txyz*>(c_data);
                c_data += c_ban->Bones[s].RotateCount * sizeof(txyz);
                memcpy(c_ban->Bones[s].Rotate, m_animationlist[i]->Bones[s].Rotate, c_ban->Bones[s].RotateCount * sizeof(txyz));
                GetRelocationManager()->AddRelocation((unsigned long *)&c_ban->Bones[s].Rotate);
            }

        }
    }

    return m_data;
}
*/
