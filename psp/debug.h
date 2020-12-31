/*
 *   frotz-psp
 *
 *   Copyright (c) 2008-2009 Norman B. Lancaster
 *
 *   This file is part of a work derived from Frotz, which is Copyright (c)
 *   1995-1997 Stefan Jokisch, under compatible licensing terms.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <string.h>

#define DEBUG_LOGFILE "ms0:/debug.log"
#define DEBUG_TEXT_BUF_LENGTH		1024

#define DEBUG

// Debuging enabled
#ifdef DEBUG

	// PSPLink debugging enabled
	#ifdef PSPLINK_DEBUG
		#define debugPrintf(fmt, args...) {printf(fmt, args);}
	#else//PSPLINK_DEBUG
		void debugPrintf(const char *fmt, ...);
	#endif//PSPLINK_DEBUG
#else//DEBUG
	#define debugPrintf(fmt, args...) {}
#endif//DEBUG

#endif // DEBUG_H_INCLUDED
