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

#ifndef BASECON_H_INCLUDED
#define BASECON_H_INCLUDED

// Configuration Definitions

// With this defined, we won't try to set the display mode in bcInit, but will
// instead use the current display mode. This is how PSPEText works.
//#define BASECON_TAKEOVER_SCREEN

#include <psptypes.h>

// Print String flags
#define BC_PRINT_NONE		0x00000000
#define BC_PRINT_NOSCROLL	0x00000001

int bcInit(int font);
void bcKill();
void bcFlush();
int bcGetWidth();
int bcGetHeight();
void bcSetPos(int x, int y);
void bcPrintChar(int c, int flags);
void bcPrintString(const u8 *str, int flags);
void bcPrintRawString(const u8 *str, int flags);
void bcClear();
void bcRenderChar(int x, int y, int c);
void bcSetFG(u32 c);
void bcSetBG(u32 c);
void bcClearToEndOfLine();

#endif // BASECON_H_INCLUDED
