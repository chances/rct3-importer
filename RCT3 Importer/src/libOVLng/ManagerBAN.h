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
#include <string>
#include <vector>

#include "boneanim.h"
#include "ManagerOVL.h"

using namespace std;

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

    virtual unsigned char* Make();

    virtual const char* Name() const {
        return NAME;
    };
    virtual const char* Tag() const {
        return TAG;
    };
};


#endif
