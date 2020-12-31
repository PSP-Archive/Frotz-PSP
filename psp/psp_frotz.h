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

#include "../common/frotz.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <psptypes.h>
#include <pspctrl.h>

// Configuration definitions
#define PSP_TICKS_PERTENTH		6
#define	PSP_DEBUG_STORY_FILE	"ms0:/story.file"

// Screen locations and sizes
#define PSP_FBUF_WIDTH			60
#define PSP_FBUF_HEIGHT			22
#define PSP_SCREEN_OFSX			0
#define PSP_SCREEN_OFSY			0
#define PSP_SCREEN_WIDTH		43
#define PSP_SCREEN_HEIGHT		(PSP_FBUF_HEIGHT - PSP_KEYBOX_HEIGHT - 1)
#define PSP_KEYBOX_OFSX			(PSP_SCREEN_OFSX)
#define PSP_KEYBOX_OFSY			(PSP_SCREEN_HEIGHT + PSP_SCREEN_OFSY + 1)
#define PSP_KEYBOX_WIDTH		(PSP_SCREEN_WIDTH)
#define PSP_KEYBOX_HEIGHT		3
#define	PSP_QUICK_OFSX			(PSP_SCREEN_OFSX + PSP_SCREEN_WIDTH + 1)
#define	PSP_QUICK_OFSY			(PSP_SCREEN_OFSY)
#define	PSP_QUICK_WIDTH			(PSP_FBUF_WIDTH - (PSP_SCREEN_WIDTH - 1))
#define	PSP_QUICK_HEIGHT		(PSP_FBUF_HEIGHT)

// Input options
#define PSP_MAX_INPUT			128
#define PSP_MAX_HISTORY			10

// Color variables
extern u32 pspColorFG;
extern u32 pspColorBG;

// Function prototypes

// Main functions
typedef struct
{
	bool timedOut;
	char *line;
} pspUIProcReturn;
void pspOutputInit();
pspUIProcReturn *pspMainUIProc();

// Debug and utility
void pspDebugMsg(const char *fmt, ...);

// Output system
typedef struct
{
	u8 charCode;
	u8 style;
} pspScreenCell;
extern pspScreenCell pspScreenBuffer[PSP_SCREEN_HEIGHT][PSP_SCREEN_WIDTH];
void pspRenderCell(int col, int row);
void pspFloatChar(int x, int y, u32 bg, u32 fg, u8 chr);
void pspFloatString(int x, int y, u32 bg, u32 fg, const char *str);
void pspRenderScreen();
void pspRenderAll();

// UI header info
enum pspUIModes {
	PSPUIMODE_KEYBOX = 0,
	PSPUIMODE_SCREENBUFFER,
	PSPUIMODE_QUICKTEXT,
	PSPUIMODE_COMMONWORD,
	PSPUIMODE_LAST};
typedef struct
{
	int activeMode;
	int ticks;
	bool keyBoxUsable;
	bool screenBufferUsable;
	bool copyBoxUsable;
	bool charAdded;
	char inStream[PSP_MAX_INPUT + 1];
	int inCaret;
} pspUIState;
extern pspUIState uiState;

// General input functions
void pspInsertInput(char *c);
void pspDeleteInput();
void pspRenderInputLine();

// Key box input area
void pspRenderKeyBox();
void pspKeyBoxProc();

// Quick text input area
void pspQuickTextInit();
void pspQuickTextAdd(char *c);
void pspQuickTextRender();
void pspQuickTextProc();

// History system
void pspHistoryInit();
void pspHistoryPush(char *line);
char *pspHistoryByOffset(int ofs);
void pspHistoryResetOffset();

// Screen buffer input area
void pspScreenBufferProc(bool deactivate);

// Common word input area
void pspCommonWordProc(bool deactivate);
