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

#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

#include <psptypes.h>

#define FONT_NUM	2

typedef struct
{
	int charWidth;
	int charHeight;
	int dataPitch;
	int sWidth;
	int sHeight;
	int sOfsX;
	int sOfsY;
	u8 *data;
} FONTSPEC;

extern FONTSPEC fonts[];

#endif // FONT_H_INCLUDED
