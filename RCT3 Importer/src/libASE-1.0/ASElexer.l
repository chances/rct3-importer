
%{

/*  $Id: ASElexer.l,v 1.5 2003/07/10 22:32:59 reed Exp $

    This file is part of the Virtual Object System of
    the Interreality project (http://interreality.org).

    Copyright (C) 2003 Peter Amstutz

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

    Peter Amstutz <tetron@interreality.org>
*/

#include <stdio.h>
#include "symbols.h"

int ASE_read(char* buf, int maxsize);

int ASE_linenum = 0;

#define YY_INPUT(buf,result,max_size) \
  { \
    result = ASE_read(buf, max_size); \
  }
%}

%option noyywrap
%option nodefault

%%
-?[0-9]+          { return INTEGER; }
-?[0-9]+\.[0-9]+  { return FLOAT; }
[A-Za-z0-9]+:     { return INDEX; }
\"[^"]*\"         { return STRING; }
\*[A-Z0-9_]+      { return NODE; }
[A-Za-z]+         { return SYMBOL; }
[[:space:]]+      { char* c; for(c = ASE_text; *c; c++) if(*c == '\n') ASE_linenum++; /* Space will be ignored */ }
\{                { return BLOCKSTART; }
\}                { return BLOCKEND; }
-?1.#QNB           { return FLOAT; }
%%
