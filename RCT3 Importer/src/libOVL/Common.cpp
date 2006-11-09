/*	RCT3 libOVL
	Copyright 2005 Jonathan Wilson

	This file is part of the RCT3 libOVL
	The RCT3 libOVL library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

#include "Common.h"

unsigned long log2(unsigned long y)
{
	unsigned long result = 0;
	while(y)
	{
		result++;
		y >>= 1;
	}
	return result-1;
}
