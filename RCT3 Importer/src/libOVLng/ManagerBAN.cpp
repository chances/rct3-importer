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

#include "OVLException.h"

const char* ovlBANManager::NAME = "BoneAnim";
const char* ovlBANManager::TAG = "ban";

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
    m_lsrman->AddLoader(OVLT_COMMON);
    m_lsrman->AddSymbol(OVLT_COMMON);
    m_animationnames.push_back(string(name));
    m_stable->AddSymbolString(name, Tag());

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

    m_stable->AddString(name);

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
        m_relman->AddRelocation((unsigned long *)&c_ban->Bones);

        // Symbol and Loader
        SymbolStruct* c_symbol = m_lsrman->MakeSymbol(OVLT_COMMON, m_stable->FindSymbolString(m_animationnames[i].c_str(), Tag()), reinterpret_cast<unsigned long*>(c_ban), true);
        m_lsrman->OpenLoader(OVLT_COMMON, TAG, reinterpret_cast<unsigned long*>(c_ban), false, c_symbol);
        m_lsrman->CloseLoader(OVLT_COMMON);

        for (unsigned long s = 0; s < c_ban->BoneCount; ++s) {
            // Bone name
            c_ban->Bones[s].Name = m_stable->FindString(m_animationlist[i]->Bones[s].Name);
            m_relman->AddRelocation((unsigned long *)&c_ban->Bones[s].Name);

            // Translations
            if (c_ban->Bones[s].TranslateCount) {
                c_ban->Bones[s].Translate = reinterpret_cast<txyz*>(c_data);
                c_data += c_ban->Bones[s].TranslateCount * sizeof(txyz);
                memcpy(c_ban->Bones[s].Translate, m_animationlist[i]->Bones[s].Translate, c_ban->Bones[s].TranslateCount * sizeof(txyz));
                m_relman->AddRelocation((unsigned long *)&c_ban->Bones[s].Translate);
            }

            // Rotations
            if (c_ban->Bones[s].RotateCount) {
                c_ban->Bones[s].Rotate = reinterpret_cast<txyz*>(c_data);
                c_data += c_ban->Bones[s].RotateCount * sizeof(txyz);
                memcpy(c_ban->Bones[s].Rotate, m_animationlist[i]->Bones[s].Rotate, c_ban->Bones[s].RotateCount * sizeof(txyz));
                m_relman->AddRelocation((unsigned long *)&c_ban->Bones[s].Rotate);
            }

        }
    }

    return m_data;
}

