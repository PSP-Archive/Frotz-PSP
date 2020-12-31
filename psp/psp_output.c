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

// Color variables
u32 pspColorFG		= 0xffbfbfbf;
u32 pspColorBG		= 0xff000000;

/* Renders a single character cell with the given background and foreground
 * colors at the specified character location. The character location is
 * absolute and begins at 0,0.
 */
void pspFloatChar(int x, int y, u32 bg, u32 fg, u8 chr)
{
    u8 str[2];
    str[1] = '\0';
    str[0] = (char)chr;
    bcSetPos(x, y);
    bcSetFG(fg);
    bcSetBG(bg);
    bcPrintRawString(str, BC_PRINT_NOSCROLL);
}
/* Renders a null-terminated string, does NOT respect special characters.
 */
void pspFloatString(int x, int y, u32 bg, u32 fg, const char *str)
{
    bcSetPos(x, y);
    bcSetFG(fg);
    bcSetBG(bg);
    bcPrintRawString((u8*)str, BC_PRINT_NOSCROLL);
}
void pspRenderFrames()
{
}
void pspOutputInit()
{
	// Set up Frotz globals
	h_screen_height = PSP_SCREEN_HEIGHT;
	h_screen_width = PSP_SCREEN_WIDTH;
	h_font_width = 1;
	h_font_height = 1;

	// Init all of our subsystems
	bcInit(1);
	pspQuickTextInit();
	// TODO Replace this with some sort of default word set.
	pspQuickTextAdd("save");
	pspQuickTextAdd("restore");
	pspQuickTextAdd("get");
	pspQuickTextAdd("look");
	pspQuickTextAdd("drop");
	pspHistoryInit();

	// Render the screen
	pspRenderAll();
}
void pspRenderAll()
{
	pspRenderFrames();
	pspRenderKeyBox();
	pspQuickTextRender();
	pspRenderScreen();
}
