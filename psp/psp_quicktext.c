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

char quickText[PSP_QUICK_HEIGHT][PSP_MAX_INPUT + 1];
int quickTextSel = 0;
bool quickTextActive = FALSE;

void pspQuickTextInit()
{
	int i = 0;
	for(i = 0; i < PSP_QUICK_HEIGHT; ++i)
	{
		quickText[i][0] = '\0';
	}
}
void pspQuickTextAdd(char *c)
{
	// Look for the first empty slot, then add the string there.
	int i;
	for(i = 0; i < PSP_QUICK_HEIGHT; ++i)
	{
		if(quickText[i][0] == '\0')
		{
			strncpy(quickText[i], c, PSP_MAX_INPUT);
			return;
		}
	}
	// No slots left.
	return;
}
void pspQuickTextRender()
{
	char blankBuf[PSP_QUICK_WIDTH + 1];
	int i, j;

	blankBuf[PSP_QUICK_WIDTH] = '\0';

	// Render out all the lines
	for(i = 0; i < PSP_QUICK_HEIGHT; ++i)
	{
		bool endOfString = FALSE;
		for(j = 0; j < PSP_QUICK_WIDTH; ++j)
		{
			if(quickText[i][j] == '\0')
				endOfString = TRUE;
			if(endOfString == TRUE)
				blankBuf[j] = ' ';
			else
				blankBuf[j] = quickText[i][j];
		}

		// Render out this item
		if(quickTextActive == TRUE &&
			quickTextSel == i)
		{
			pspFloatString(PSP_QUICK_OFSX, PSP_QUICK_OFSY + i,
				pspColorFG, pspColorBG, blankBuf);
		}
		else
		{
			pspFloatString(PSP_QUICK_OFSX, PSP_QUICK_OFSY + i,
				pspColorBG, pspColorFG, blankBuf);
		}
	}
}
void pspQuickTextProc(bool deactivate)
{
	// Deactivation proc
	if(deactivate == TRUE)
	{
		quickTextActive = FALSE;
		pspQuickTextRender();
		return;
	}
	quickTextActive = TRUE;

	// Line up
	if(inWasKeyPressed(PSPKEY_UP))
		quickTextSel--;
	// Page up
	if(inWasKeyPressed(PSPKEY_LEFT))
		quickTextSel -= 5;
	// Line down
	if(inWasKeyPressed(PSPKEY_DOWN))
		quickTextSel++;
	// Page down
	if(inWasKeyPressed(PSPKEY_RIGHT))
		quickTextSel += 5;

	// Bound selection
	if(quickTextSel < 0) quickTextSel = 0;
	if(quickTextSel >= PSP_QUICK_HEIGHT) quickTextSel = PSP_QUICK_HEIGHT - 1;
	// Make sure we are on a valid selection.
	while(quickText[quickTextSel] == '\0' && quickTextSel > 0)
	{
		quickTextSel--;
	}

	// Select a quick text
	if(inWasKeyPressed(PSPKEY_CROSS))
	{
		pspInsertInput(quickText[quickTextSel]);
	}

	// Delete a quick text
	if(inWasKeyPressed(PSPKEY_TRIANGLE))
	{
		int i;
		for(i = quickTextSel; i < PSP_QUICK_HEIGHT; ++i)
		{
			strcpy(quickText[i], quickText[i + 1]);
		}
	}

	pspQuickTextRender();
}
