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
#include "pspetext.h"
#include "basecon.h"
#include "input.h"

// Main UI proc function pointers
void (*pspUIProcFuncs[])(bool) = {
	pspKeyBoxProc,
	pspScreenBufferProc,
	pspQuickTextProc,
	pspCommonWordProc
};
pspUIProcReturn uiRet;
pspUIState uiState = {
	PSPUIMODE_KEYBOX,
	0,
	TRUE,
	TRUE,
	TRUE,
	FALSE,
	"",
	0
};

void pspCommonWordProc(bool deactivate)
{
}
void pspInsertInput(char *c)
{
	int inLen = strlen(uiState.inStream);
	int cLen = strlen(c);
	if(inLen + cLen >= PSP_MAX_INPUT) return;

	// If we already have some input, move it over
	int pos;
	for(pos = inLen; pos >= uiState.inCaret; --pos)
	{
		uiState.inStream[pos + cLen] = uiState.inStream[pos];
	}

	// And now insert our new string
	if(cLen > 0)
	{
		for(pos = 0; pos < cLen; ++pos)
		{
			uiState.inStream[pos + uiState.inCaret] = c[pos];
		}
		uiState.charAdded = TRUE;
	}

	// Advance the caret and exit
	uiState.inCaret += cLen;
	return;
}
void pspDeleteInput()
{
	int inLen = strlen(uiState.inStream);
	if(inLen <= 0) return;

	// At end of input, do a backspace
	if(inLen == uiState.inCaret)
	{
		uiState.inStream[inLen - 1] = '\0';
		uiState.inCaret--;
		return;
	}

	// Do a logical delete, move the buffer in front of the carent back one
	int pos;
	for(pos = uiState.inCaret; pos < inLen; ++pos)
	{
		uiState.inStream[pos] = uiState.inStream[pos + 1];
	}
	return;
}
void pspRenderInputLine()
{
	int i, outIdx = 0;
	char outChar = ' ';

	// TODO Figure out what portion of the input line to display

	// Render out the input line
	bool endOfString = FALSE;
	for(i = 0; i < PSP_SCREEN_WIDTH; ++i)
	{
		if(uiState.inStream[i + outIdx] == '\0')
			endOfString = TRUE;
		if(endOfString == TRUE)
			outChar = ' ';
		else
			outChar = uiState.inStream[i + outIdx];
		if(uiState.inCaret == i + outIdx)
			pspFloatChar(PSP_SCREEN_OFSX + i,
				PSP_SCREEN_HEIGHT + PSP_SCREEN_OFSY,
				pspColorFG, pspColorBG, outChar);
		else
			pspFloatChar(PSP_SCREEN_OFSX + i,
				PSP_SCREEN_HEIGHT + PSP_SCREEN_OFSY,
				pspColorBG, pspColorFG, outChar);
	}
}
pspUIProcReturn *pspMainUIProc(int timeout, bool singleKey)
{
	uiRet.line = uiState.inStream;
	uiRet.timedOut = FALSE;
	uiState.ticks = 0;
	uiState.keyBoxUsable = TRUE;
	uiState.screenBufferUsable = TRUE;
	uiState.copyBoxUsable = TRUE;

	// Throw away previous input buffer, history, etc
	uiState.inStream[0] = '\0';
	uiState.inCaret = 0;
	pspRenderInputLine();
	pspHistoryResetOffset();

	// Loop getting input
	for(;;)
	{
		uiState.charAdded = FALSE;
        bcFlush();
		inPumpInputEvents(PSP_INASYNC);

		// Common input procs
		// Switch to menu mode
		if(inIsKeyDown(PSPKEY_SELECT))
		{
			// Temp shunt to text reader.
			// NOTE: bcInit() must be called after this exits!
			petMain();
			bcInit(1);
			pspRenderAll();
			continue;
		}
		// Caret and history mode
		else if(inIsKeyDown(PSPKEY_RTRIGGER))
		{
			char *historyLine = NULL;
			// Caret left
			if(inWasKeyPressed(PSPKEY_LEFT) &&
				uiState.inCaret > 0)
			{
				uiState.inCaret--;
			}
			// Caret right
			if(inWasKeyPressed(PSPKEY_RIGHT) &&
				uiState.inCaret < strlen(uiState.inStream))
			{
				uiState.inCaret++;
			}
			// History up
			if(inWasKeyPressed(PSPKEY_UP))
			{
				historyLine = pspHistoryByOffset(1);
			}
			// History down
			if(inWasKeyPressed(PSPKEY_DOWN))
			{
				historyLine = pspHistoryByOffset(-1);
			}

			// Handle history
			if(historyLine != NULL)
			{
				strncpy(uiState.inStream, historyLine, PSP_MAX_INPUT);
				uiState.inCaret = 0;
			}
		}
		// Normal mode
		else
		{
			// Mode select mode
			int lastMode = uiState.activeMode;
			if(inIsAnalogDir(PSPKEY_UP))
				uiState.activeMode = PSPUIMODE_SCREENBUFFER;
			if(inIsAnalogDir(PSPKEY_DOWN))
				uiState.activeMode = PSPUIMODE_KEYBOX;
			if(inIsAnalogDir(PSPKEY_LEFT))
				uiState.activeMode = PSPUIMODE_COMMONWORD;
			if(inIsAnalogDir(PSPKEY_RIGHT))
				uiState.activeMode = PSPUIMODE_QUICKTEXT;

			// Dispatch the deactivation proc if needed
			if(lastMode != uiState.activeMode)
			{
				pspUIProcFuncs[lastMode](TRUE);
			}

			// End of line
			if(inWasKeyPressed(PSPKEY_LTRIGGER) ||
				inWasKeyPressed(PSPKEY_START))
			{
				// Commit to history
				pspHistoryPush(uiState.inStream);
				return &uiRet;
			}
			// Insert space
			if(inWasKeyPressed(PSPKEY_SQUARE))
			{
				pspInsertInput(" ");
			}
			// Delete character
			if(inWasKeyPressed(PSPKEY_CIRCLE))
			{
				pspDeleteInput();
			}

			// Dispatch the UI proc handler
			pspUIProcFuncs[uiState.activeMode](FALSE);
		}

		// If a character was added and this is a singleKey call, return
		if(uiState.charAdded == TRUE && singleKey == TRUE) return &uiRet;

		// Check for timeout
		if(timeout > 0)
		{
			uiState.ticks ++;
			if((uiState.ticks / PSP_TICKS_PERTENTH) >= timeout)
			{
				uiRet.timedOut = TRUE;
				pspRenderInputLine();
				return &uiRet;
			}
		}
		pspRenderInputLine();
	}
}
