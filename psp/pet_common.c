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


#include <pspthreadman.h>
#include "pspetext.h"
#include "basecon.h"
#include "input.h"

// Various globals
char petParm[PET_FBPARM_LEN];
char petScratch[PET_FBSCRATCH_LEN];
char petListStrings[PET_FBLISTSTR_COUNT*PET_FBLISTSTR_LEN];
u8 petFileBuf[PET_FBFILEBUF_LEN];
char petFilePath[PET_FPATH_LEN] = "\0";

u32 petFGColor = PET_DEFAULT_FG;
u32 petBGColor = PET_DEFAULT_BG;
u32 petFGHLColor = PET_DEFAULT_FGHL;
u32 petBGHLColor = PET_DEFAULT_BGHL;
u32 petFGERColor = PET_DEFAULT_FGER;
u32 petBGERColor = PET_DEFAULT_BGER;

// Help screen text
FILEBUF helpScreenBuffer;
char helpScreenText[] = "\
********************* Common Controls *********************\n\
[DPAD Up]    Move one line up\n\
[DPAD Down]  Move one line down\n\
[DPAD Left]  Move one page up\n\
[DPAD Right] Move one page down\n\
[Select]     Display this help file\n\
[Start]      Exit\n\
******************* File View Controls ********************\n\
[Triangle]   Open a new file\n\
****************** File Select Controls *******************\n\
[Triangle]   Got to parent directory\n\
[Cross]      Select file or directory\n\
***********************************************************\n";

int errorPrint(const char *str)
{
	bcSetPos(0, 0);
	bcSetFG(petFGERColor);
	bcSetBG(petBGERColor);
	bcPrintString((u8*)str, BC_PRINT_NONE);
	bcClearToEndOfLine();
	sceKernelDelayThread(1000000);
	bcSetPos(0, 0);
	bcSetFG(petFGColor);
	bcSetBG(petBGColor);
	bcClearToEndOfLine();
	sceKernelDelayThread(250000);
	return 1;
}
/* Handle common input events.
 * returns 0 if the callee should continue input processing.
 * returns -1 if the callee should pump input events again.
 * returns -2 if the callee should exit.
 * returns > 0 on error.
 */
int petCommonInputHandler(int isHelpScreen)
{
	// Global change mode
	if(inIsKeyDown(PSPKEY_SQUARE))
	{
		return -1;
	}
	// Display help screen
	if(inWasKeyPressed(PSPKEY_SELECT) &&
		isHelpScreen == 0)
	{
		fbFromBuffer(&helpScreenBuffer, (u8*)helpScreenText,
			strlen(helpScreenText), "Help Screen");
		petBufferViewer(&helpScreenBuffer, NULL, NULL, 1);
		return -1;
	}
	// Exit when start is pressed
	if(inWasKeyPressed(PSPKEY_START))
	{
		return -2;
	}
	return 0;
}
void petDisplayList(int hlit, int count)
{
	int sHeight, outLine, entry;
	sHeight = bcGetHeight();

	bcSetBG(petBGColor);
	// Display directory contents
	for(outLine = 1; outLine < sHeight; ++outLine)
	{
		bcSetPos(0, outLine);
		entry = (outLine - 1);
		if(entry >= count)
		{
			bcSetFG(petFGColor);
			bcSetBG(petBGColor);
			bcClearToEndOfLine();
			continue;
		}
		if(hlit == entry)
		{
			bcSetFG(petFGHLColor);
			bcSetBG(petBGHLColor);
			bcPrintString((u8*)"\020", BC_PRINT_NONE);
		}
		else
		{
			bcSetFG(petFGColor);
			bcSetBG(petBGColor);
			bcPrintString((u8*)" ", BC_PRINT_NONE);
		}
		bcPrintString((u8*)&petListStrings[entry * PET_FBLISTSTR_LEN],
			BC_PRINT_NOSCROLL);
		bcClearToEndOfLine();
	}
	return;
}
/* List display proc, reads all list items from the specail petListStrings
 * pointer. The refreshList function should take one parameter, the new top
 * list index, and return the number of list items that are now valid.
 */
int petListProc(const char *statusLine, int(*refreshList)(int))
{
	// Init list UI
	int sel = 0;
	int top = 0;
	int onScreenCount = 0;

	// Print status line
	bcSetFG(petFGHLColor);
	bcSetBG(petBGHLColor);
	bcSetPos(0, 0);
	bcPrintString((u8*)statusLine, BC_PRINT_NOSCROLL);
	bcClearToEndOfLine();

	// Get the inital list
	onScreenCount = refreshList(top);
	if(onScreenCount <= 0) return -126;
	petDisplayList(sel - top, onScreenCount);

	// Input loop
	while(1)
	{
		inPumpInputEvents(PSP_INNONE);
		// Common events
		int ret = petCommonInputHandler(0);
		if(ret == -1)
		{
			petDisplayList(sel - top, onScreenCount);
			continue;
		}
		if(ret == -2 || ret > 0) return -2;
		// Line down
		if(inWasKeyPressed(PSPKEY_DOWN))
		{
			// Down in middle of list
			if(sel - top < onScreenCount - 1)
			{
				++sel;
				petDisplayList(sel - top, onScreenCount);
			}
			// Down at bottom of list when there is more list left
			else if(sel - top == onScreenCount - 1 &&
				onScreenCount == PET_FBLISTSTR_COUNT)
			{
				top = sel;
				onScreenCount = refreshList(top);
				if(onScreenCount <= 0) return -126;
				petDisplayList(sel - top, onScreenCount);
			}
		}
		// Line up
		if(inWasKeyPressed(PSPKEY_UP))
		{
			// Up in middle of list
			if(sel - top > 0)
			{
				--sel;
				petDisplayList(sel - top, onScreenCount);
			}
			// Up at tome of list when there is more list left
			else if(sel == top && top > 0)
			{
				top = top - (PET_FBLISTSTR_COUNT - 1);
				if(top < 0) top = 0;
				onScreenCount = refreshList(top);
				if(onScreenCount <= 0) return -126;
				petDisplayList(sel - top, onScreenCount);
			}
		}
		// Page down
		if(inWasKeyPressed(PSPKEY_RIGHT))
		{
			// Not and end of list
			if(onScreenCount == PET_FBLISTSTR_COUNT)
			{
				top += PET_FBLISTSTR_COUNT - 1;
				sel = top;
				onScreenCount = refreshList(top);
				if(onScreenCount <= 0) return -126;
				petDisplayList(sel - top, onScreenCount);
			}
			// At end of list
			else
			{
				sel = top + onScreenCount - 1;
				petDisplayList(sel - top, onScreenCount);
			}
		}
		// Page up
		if(inWasKeyPressed(PSPKEY_LEFT))
		{
			// Not and top of list
			if(top != 0)
			{
				top -= PET_FBLISTSTR_COUNT - 1;
				if(top < 0) top = 0;
				sel = top;
				onScreenCount = refreshList(top);
				if(onScreenCount <= 0) return -126;
				petDisplayList(sel - top, onScreenCount);
			}
			// At top of list
			else
			{
				sel = top;
				petDisplayList(sel - top, onScreenCount);
			}
		}
		// Item selected
		if(inWasKeyPressed(PSPKEY_CROSS))
		{
			return sel;
		}
		// Exit with triangle
		if(inWasKeyPressed(PSPKEY_TRIANGLE))
		{
			return -1;
		}
		// Exit with circle
		if(inWasKeyPressed(PSPKEY_CIRCLE))
		{
			return -2;
		}

		// Rerender display
		petDisplayList(sel - top, onScreenCount);
	}

	// Should never execute
	return -126;
}
