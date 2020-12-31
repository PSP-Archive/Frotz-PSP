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
#include "input.h"

#define PSP_KEYBOX_CHARWIDTH	26
char keyboxChars[PSP_KEYBOX_HEIGHT][PSP_KEYBOX_CHARWIDTH + 1] = {
	"ABCDEFGHI 1234 !\"#$%&'()*+",
	"JKLMNOPQR 5678 ,-./:;<=>?@",
	"STUVWXYZ, .90  [\\]^_`{|}~ "
};
void pspRenderKeyBox()
{
	pspFloatString(PSP_KEYBOX_OFSX, PSP_KEYBOX_OFSY + 0,
		pspColorBG, pspColorFG, keyboxChars[0]);
	pspFloatString(PSP_KEYBOX_OFSX, PSP_KEYBOX_OFSY + 1,
		pspColorBG, pspColorFG, keyboxChars[1]);
	pspFloatString(PSP_KEYBOX_OFSX, PSP_KEYBOX_OFSY + 2,
		pspColorBG, pspColorFG, keyboxChars[2]);
}
void pspKeyBoxProc(bool deactivate)
{
	static bool active = FALSE;
	static int selx = 0;
	static int sely = 0;
	char str[2];
	str[1] = '\0';

	// Deactivation proc
	if(deactivate == TRUE)
	{
		active = FALSE;
		// Black out the previous selection
		pspFloatChar(PSP_KEYBOX_OFSX + selx, PSP_KEYBOX_OFSY + sely,
			pspColorBG, pspColorFG, keyboxChars[sely][selx]);
		return;
	}
	active = TRUE;

	// Black out the previous selection
	pspFloatChar(PSP_KEYBOX_OFSX + selx, PSP_KEYBOX_OFSY + sely,
		pspColorBG, pspColorFG, keyboxChars[sely][selx]);

	// Handle input events
	// Line up
	if(inWasKeyPressed(PSPKEY_UP))
	{
		if(sely > 0) --sely;
	}
	// Line down
	if(inWasKeyPressed(PSPKEY_DOWN))
	{
		if(sely < PSP_KEYBOX_HEIGHT - 1) ++sely;
	}
	// Column left
	if(inWasKeyPressed(PSPKEY_LEFT))
	{
		if(selx > 0) --selx;
	}
	// Column right
	if(inWasKeyPressed(PSPKEY_RIGHT))
	{
		if(selx < PSP_KEYBOX_CHARWIDTH - 1) ++selx;
	}
	// Character select
	if(inWasKeyPressed(PSPKEY_CROSS))
	{
		str[0] = keyboxChars[sely][selx];
		pspInsertInput(str);
	}
	// Bookmark current line
	if(inWasKeyPressed(PSPKEY_TRIANGLE))
	{
		pspQuickTextAdd(uiState.inStream);
		pspQuickTextRender();
	}

	// Render selection
	if(active == TRUE)
	{
		pspFloatChar(PSP_KEYBOX_OFSX + selx, PSP_KEYBOX_OFSY + sely,
			pspColorFG, pspColorBG, keyboxChars[sely][selx]);
	}
	return;
}
