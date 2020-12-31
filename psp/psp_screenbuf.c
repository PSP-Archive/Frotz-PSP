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
#include "input.h"
#include <stdlib.h>

int selX = 15;
int selY = 15;
int selLength = 0;
char wordBuffer[PSP_SCREEN_WIDTH + 1];

#define isWordChar(c)	(\
	(c >= '0' && c <= '9') ||\
	(c >= 'a' && c <= 'z') ||\
	(c >= 'A' && c <= 'Z') ||\
	c == '-' ||\
	c == '_' ||\
	c == '\''\
)

int findWordLength()
{
	int i;

	// Just find out how many word characters appear in a row begining with
	// the currently selected character.
	selLength = 0;
	for(i = selX; i < PSP_SCREEN_WIDTH; ++i)
	{
		if(!isWordChar(pspScreenBuffer[selY][i].charCode))
			break;
		++selLength;
	}
	// Current character was a non-word character.
	if(selLength <= 0) return 1;
	return 0;
}
int findNextWordOnLine(int reverse)
{
	//debugPrintf("======================================\n");
	// We are already in a word, so find the begining.
	if(isWordChar(pspScreenBuffer[selY][selX].charCode))
	{
		//debugPrintf("In Character at %d\n", selX);
		for(; selX >= 0; --selX)
		{
			if(!isWordChar(pspScreenBuffer[selY][selX].charCode))
				break;
		}
		// If we hit 0 and it was still a word character we will fall through
		// to here. selX will be -1 at this point.
		++selX;
		findWordLength();
		return 0;
	}

	// Otherwise we need to scan forward or backward to find the next start
	// character.
	// Forward scan
	if(reverse == 0)
	{
		//debugPrintf("Forward scan from %d\n", selX);
		for(++selX; selX < PSP_SCREEN_WIDTH; ++selX)
		{
			if(isWordChar(pspScreenBuffer[selY][selX].charCode))
			{
				findWordLength();
				return 0;
			}
		}
	}
	// Reverse scan
	else
	{
		//debugPrintf("Reverse scan from %d\n", selX);
		for(--selX; selX >= 0; --selX)
		{
			if(isWordChar(pspScreenBuffer[selY][selX].charCode))
			{
				// Ok, now we are on the last character of the previous word,
				// so call findNextWordOnLine again to let the top case take
				// care of us.
				return findNextWordOnLine(0);
			}
		}
		// If we did not return by now, then selX == -1, so set it to 0.
		selX = 0;
	}

	// If we never hit a word character, then return false.
	selLength = 0;
	return 1;
}
int pspFindPreviousWord()
{
	int oldSelX = selX;
	int oldSelY = selY;
	for(;;)
	{
		// Found a word
		if(findNextWordOnLine(1) == 0) break;
		// Failed to find any previous word
		if(selY <= 0)
		{
			selX = oldSelX;
			selY = oldSelY;
			return 1;
		}
		// Did not find a word on this line, go to the end of the previous.
		--selY;
		selX = PSP_SCREEN_WIDTH - 1;
	}
	return 0;
}
int pspFindNextWord()
{
	int oldSelX = selX;
	int oldSelY = selY;
	for(;;)
	{
		// Found a word
		if(findNextWordOnLine(0) == 0) break;
		// Did not find a word on this line, go to the end of the next.
		++selY;
		selX = 0;
		// Failed to find next word
		if(selY >= PSP_SCREEN_HEIGHT)
		{
			selX = oldSelX;
			selY = oldSelY;
			return 1;
		}
	}
	return 0;
}
void pspScreenBufferProc(bool deactivate)
{
	int i;

	// Blank the old selection
	for(i = selX; i < selX + selLength; ++i)
	{
		pspRenderCell(i, selY);
	}

	if(deactivate) return;

	// Recalculate selection (because this can change due to output going to
	// the screen since the last time we were called).
	if(pspFindNextWord()) pspFindPreviousWord();

	// Handle input
	// Word left
	if(inWasKeyPressed(PSPKEY_LEFT))
	{
		--selX;
		if(selX < 0)
		{
			selX = PSP_SCREEN_WIDTH - 1;
			--selY;
			if(selY < 0)
			{
				selY = 0;
				selX = 0;
			}
		}
		if(pspFindPreviousWord()) pspFindNextWord();
	}
	// Word right
	if(inWasKeyPressed(PSPKEY_RIGHT))
	{
		selX += selLength;
		if(selX >= PSP_SCREEN_WIDTH)
		{
			selX = 0;
			++selY;
			if(selY >= PSP_SCREEN_HEIGHT)
			{
				selX = PSP_SCREEN_WIDTH - 1;
				selY = PSP_SCREEN_HEIGHT - 1;
			}
		}
		if(pspFindNextWord()) pspFindPreviousWord();
	}
	// Line up
	if(inWasKeyPressed(PSPKEY_UP))
	{
		int oldSelY = selY;
		--selY;
		selX = 0;
		if(selY < 0) selY = 0;
		if(pspFindNextWord()) pspFindPreviousWord();
		// If this is true, that means that the "next word" found was actually
		// the first word on this line, so try this again, but using
		// pspFindPreviousWord first.
		if(selY == oldSelY)
		{
			--selY;
			selX = 0;
			if(selY < 0) selY = 0;
			if(pspFindPreviousWord()) pspFindNextWord();
		}
	}
	// Line down
	if(inWasKeyPressed(PSPKEY_DOWN))
	{
		++selY;
		selX = 0;
		if(selY >= PSP_SCREEN_HEIGHT) selY = PSP_SCREEN_HEIGHT - 1;
		if(pspFindNextWord()) pspFindPreviousWord();
	}

	// All input handlers above re-calculate the selection on-demand. The
	// handlers below work on the selection, so go ahead and put the selection
	// in a buffer.
	for(i = 0; i < selLength; ++i)
	{
		wordBuffer[i] = pspScreenBuffer[selY][selX + i].charCode;
	}
	wordBuffer[selLength] = '\0';

	// Push selected word to input buffer
	if(inWasKeyPressed(PSPKEY_CROSS))
	{
		pspInsertInput(wordBuffer);
	}
	// Add selected word to quick words
	if(inWasKeyPressed(PSPKEY_TRIANGLE))
	{
		pspQuickTextAdd(wordBuffer);
	}

	// Render the selection
	for(i = selX; i < selX + selLength; ++i)
	{
		if(pspScreenBuffer[selY][i].style & REVERSE_STYLE)
		{
			pspFloatChar(PSP_SCREEN_OFSX + i, PSP_SCREEN_OFSY + selY,
				pspColorBG, pspColorFG, pspScreenBuffer[selY][i].charCode);
		}
		else
		{
			pspFloatChar(PSP_SCREEN_OFSX + i, PSP_SCREEN_OFSY + selY,
				pspColorFG, pspColorBG,pspScreenBuffer[selY][i].charCode);
		}
	}
}
