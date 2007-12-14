///////////////////////////////////////////////////////////////////////////////
//
// OVL dumping library
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


#ifndef _OVLDEXCEPTION_H_
#define _OVLDEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class EOvlD: public std::exception {
public:
    EOvlD(const string& message);
    virtual const char* what() const throw() {
        return m_message.c_str();
    }
    virtual const string& swhat() const throw() {
        return m_message;
    }
    ~EOvlD() throw() {};
protected:
    string m_message;
};


#endif
