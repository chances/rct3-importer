///////////////////////////////////////////////////////////////////////////////
//
// libXmlCpp
// A light C++ wrapper for libxml2, libxslt and libexslt
// Copyright (C) 2008 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, <http://www.gnu.org/licenses/>.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CXMLEXCEPTION_H_INCLUDED
#define CXMLEXCEPTION_H_INCLUDED

#include <exception>
#include <string>

namespace xmlcpp {

/// Basic exception class for libXmlCpp
class eXml: public std::exception {
protected:
    std::string m_message;
public:
    eXml(const std::string& str):m_message(str) {}

    virtual const char* what() const throw() {
        return m_message.c_str();
    }
    virtual const std::string& swhat() const throw() {
        return m_message;
    }

    ~eXml() throw() {};
};

/// Exception class for invalid objects
/**
 * This class is used when a member function is called on an object that doesn't have a valid
 * libxml2 pointer
 */
class eXmlInvalid: public eXml {
public:
    eXmlInvalid(const std::string& str):eXml(str) {}
};

} // Namespace


#endif // CXMLEXCEPTION_H_INCLUDED
