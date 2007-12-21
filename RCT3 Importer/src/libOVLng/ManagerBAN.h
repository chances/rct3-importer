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


#ifndef MANAGERBAN_H_INCLUDED
#define MANAGERBAN_H_INCLUDED

#include <map>
#include <set>
#include <string>
#include <vector>

#include "boneanim.h"
#include "ManagerOVL.h"

using namespace std;

struct cTXYZComp {
    bool operator() (const txyz& lhs, const txyz& rhs) const {
        return lhs.Time<rhs.Time;
    }
};

class cBoneAnimBone {
public:
    string name;
    set<txyz, cTXYZComp> translations;
    set<txyz, cTXYZComp> rotations;

    cBoneAnimBone(){};
    float Fill(BoneAnimBone* bone) {
        float f = 0.0;
        //bone->Name = NULL; will be assigned before Fill is called
        bone->TranslateCount = translations.size();
        unsigned long c = 0;
        for (set<txyz>::iterator it = translations.begin(); it != translations.end(); ++it) {
            bone->Translate[c] = *it;
            if (it->Time > f)
                f = it->Time;
            c++;
        }
        bone->RotateCount = rotations.size();
        c = 0;
        for (set<txyz>::iterator it = rotations.begin(); it != rotations.end(); ++it) {
            bone->Rotate[c] = *it;
            if (it->Time > f)
                f = it->Time;
            c++;
        }
        return f;
    }
};

class cBoneAnim {
public:
    string name;
    float totaltime;
    bool calc_time;
    vector<cBoneAnimBone> bones;

    cBoneAnim() {
        totaltime = 0.0;
        calc_time = true;
    }
    void Fill(BoneAnim* ban) {
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
};

class ovlBANManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    map<string, cBoneAnim> m_items;

public:
    ovlBANManager(): ovlOVLManager() {};

    void AddAnimation(const cBoneAnim& item);

    virtual void Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};

/*
class ovlBANManager: public ovlOVLManager {
public:
    static const char* NAME;
    static const char* TAG;
private:
    vector<BoneAnim*> m_animationlist;
    vector<string> m_animationnames;

    //BoneAnim* m_canim;
    BoneAnimBone* m_cbone;
    long m_bonecount;
    long m_transcount;
    long m_rotcount;

    unsigned long m_ntrans;
    unsigned long m_nrot;

    void CloseBone();
public:
    ovlBANManager(): ovlOVLManager() {
        //m_canim = NULL;
        m_cbone = NULL;
        m_bonecount = 0;
        m_transcount = 0;
        m_rotcount = 0;
        m_ntrans = 0;
        m_nrot = 0;
    };
    virtual ~ovlBANManager();

    void AddAnimation(const char* name, unsigned long bones, float totaltime);
    void AddBone(const char* name, unsigned long translations, unsigned long rotations);
    void AddTranslation(const txyz& t);
    void AddRotation(const txyz& t);

    virtual unsigned char* Make(cOvlInfo* info);

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};
*/

#endif
