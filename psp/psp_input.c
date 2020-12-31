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
#include <pspctrl.h>
#include "basecon.h"
#include "input.h"

extern char *story_name;

zchar os_read_key(int timeout, bool show_cursor)
{
	pspUIProcReturn *uiRet = pspMainUIProc(timeout, TRUE);
	if(uiRet->timedOut) return ZC_TIME_OUT;
	return uiRet->line[0];
}
zchar os_read_line(int max, zchar *buf, int timeout, int width, int continued)
{
	// Get the line input and copy it into the buffer
	pspUIProcReturn *uiRet = pspMainUIProc(timeout, FALSE);
	if(uiRet->timedOut) return ZC_TIME_OUT;
	strncpy((char*)buf, (char*)uiRet->line, max);

	// Copy input to screen
	os_display_string((zchar*)uiRet->line);

	// Truncate the input line and re-display
	uiRet->line[0] = '\0';
	uiState.inCaret = 0;
	pspRenderInputLine();
	return ZC_RETURN;
}
int os_read_file_name(char *file_name, const char *default_name, int flag)
{
	strcpy(file_name, story_name);
	switch(flag)
	{
	case FILE_SAVE:
	case FILE_RESTORE:
		strcat(file_name, ".sav");
		break;
	case FILE_SAVE_AUX:
	case FILE_LOAD_AUX:
		strcat(file_name, ".aux");
		break;
	default: return 0; break;
	}
	return 1;
}
void os_more_prompt(void)
{
	// Display the more prompt
	pspFloatString(((PSP_SCREEN_WIDTH - 8) / 2) + PSP_SCREEN_OFSX,
		(PSP_SCREEN_HEIGHT + PSP_SCREEN_OFSY),
		pspColorBG, pspColorFG,
		"MORE (X)");

	// Wait for the X key
	for(;;)
	{
		bcFlush();
		inPumpInputEvents(PSP_INASYNC);
		if(inWasKeyPressed(PSPKEY_CROSS)) break;
	}

	// Erase the more prompt
	pspFloatString(((PSP_SCREEN_WIDTH - 8) / 2) + PSP_SCREEN_OFSX,
		(PSP_SCREEN_HEIGHT + PSP_SCREEN_OFSY),
		pspColorBG, pspColorFG,
		"        ");

}
zword os_read_mouse(void) {return 0;}
