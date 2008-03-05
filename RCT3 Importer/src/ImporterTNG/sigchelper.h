///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Importer TNG
// Custom object importer for Ataris Roller Coaster Tycoon 3
// Copyright (C) 2008 Belgabor (Tobias Minich)
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
//   Belgabor (Tobias Minich) - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SIGCHELPER_H_INCLUDED
#define SIGCHELPER_H_INCLUDED

template <class T>
class PointerReturner {
public:
    typedef T* result_type;

public:
    template<typename IteratorT>
    result_type operator()(IteratorT First, IteratorT Last) {
        result_type bRet = NULL;

        while (First != Last) {
            bRet = *First;
            if (bRet)
                break;
            ++First;
        }
        return bRet;
    }
};

template <class T>
class RefReturner {
public:
    typedef T& result_type;

public:
    template<typename IteratorT>
    result_type operator()(IteratorT First, IteratorT Last) {
        result_type bRet = T();

        while (First != Last) {
            bRet = *First;
            if (bRet)
                break;
            ++First;
        }
        return bRet;
    }
};

template <class T>
class Returner {
public:
    typedef T result_type;

public:
    template<typename IteratorT>
    result_type operator()(IteratorT First, IteratorT Last) {
        result_type bRet;

        while (First != Last) {
            bRet = *First;
            if (bRet)
                break;
            ++First;
        }
        return bRet;
    }
};

#endif // SIGCHELPER_H_INCLUDED
