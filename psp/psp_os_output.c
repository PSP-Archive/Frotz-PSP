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

#include "psp_frotz.h"
#include "basecon.h"

// Globals
int curx = 0;
int cury = 0;
unsigned char outStyle = 0;
pspScreenCell pspScreenBuffer[PSP_SCREEN_HEIGHT][PSP_SCREEN_WIDTH];

// Renders the given cell from the screen buffer onto the screen.
void pspRenderCell(int col, int row)
{
	u32 textColor, bgColor;

	// Set output style
	if(pspScreenBuffer[row][col].style & REVERSE_STYLE)
	{
		textColor = (pspColorBG);
		bgColor = (pspColorFG);
	}
	else
	{
		textColor = (pspColorFG);
		bgColor = (pspColorBG);
	}

	// Output the character
	pspFloatChar(col + PSP_SCREEN_OFSX, row + PSP_SCREEN_OFSY,
		bgColor, textColor, pspScreenBuffer[row][col].charCode);
}
// Re-render the entire screen buffer
void pspRenderScreen()
{
	int y, x;
	for(y = 0; y < PSP_SCREEN_HEIGHT; ++y)
	{
		for(x = 0; x < PSP_SCREEN_WIDTH; ++x)
		{
			pspRenderCell(x, y);
		}
	}
}
int os_char_width(zchar z) {return 1;}
int os_string_width(const zchar *s)
{
	//PSPDebugMsg("os_string_width()");
	int width = 0;
	zchar c = *s;

	while(c != 0)
	{
		s++;
		if(c != ZC_NEW_STYLE && c != ZC_NEW_FONT) width ++;
		c = *s;
	}

	return width;
}
void os_set_cursor(int row, int col) {curx = col - 1; cury = row - 1;}
void os_set_text_style(int x) {outStyle = x & REVERSE_STYLE;}
void os_display_char(zchar c)
{
	int x;

	// Set buffer
	pspScreenBuffer[cury][curx].charCode = c;
	pspScreenBuffer[cury][curx].style = outStyle;

	// And have the cell rendered
	pspRenderCell(curx, cury);

	// Increment character position
	curx ++;
	if(curx >= PSP_SCREEN_WIDTH)
	{
		curx = 0;
		cury ++;
		if(cury >= PSP_SCREEN_HEIGHT) cury = 0;
		// Blank the new row
		for(x = 0; x < PSP_SCREEN_WIDTH; x ++)
		{
			pspScreenBuffer[cury][x].charCode = ' ';
			pspScreenBuffer[cury][x].style = 0;
		}
	}
}
void os_display_string(const zchar *s)
{
	//PSPDebugMsg("os_display_string()");
	zchar c = *s;

	while(c != 0)
	{
		s++;
		if(c == ZC_NEW_FONT) {c = *s; continue;}
		if(c == ZC_NEW_STYLE)
		{
			os_set_text_style(*s);
			s++;
		}
		else os_display_char(c);
		c = *s;
	}
}
void os_erase_area(int top, int left, int bottom, int right)
{
	//PSPDebugMsg("os_erase_area()");
	int x, y;
	bcSetBG(pspColorBG);
	for(y = top - 1; y < bottom; y ++)
	{
		for(x = left - 1; x < right; x ++)
		{
			pspScreenBuffer[y][x].charCode = ' ';
			pspScreenBuffer[y][x].style = 0;
			pspRenderCell(x, y);
		}
	}
}
void os_scroll_area(int top, int left, int bottom, int right, int units)
{
	int row, col;
	top--; left--; bottom--; right--;

	if (units > 0)
	{
		for (row = top; row <= bottom - units; row++)
		{
			for (col = left; col <= right; col++)
			{
				pspScreenBuffer[row][col] = pspScreenBuffer[row + units][col];
				pspRenderCell(col, row);
			}
		}
		os_erase_area(bottom - units + 2, left + 1, bottom + 1, right + 1);
	}
	else if (units < 0)
	{
		for (row = bottom; row >= top - units; row--)
		{
			for (col = left; col <= right; col++)
			{
				pspScreenBuffer[row][col] = pspScreenBuffer[row + units][col];
				pspRenderCell(col, row);
			}
		}
		os_erase_area(top + 1, left + 1, top - units, right + 1);
	}
}
int os_font_data(int font, int *height, int *width)
{
	if(font == TEXT_FONT)
	{
		*height = 1; *width = 1; return 1;
	}
	return 0;
}
void os_set_colour(int x, int y) {}
void os_set_font(int x) {}
void os_reset_screen(void) {}
void os_beep(int volume) {}
