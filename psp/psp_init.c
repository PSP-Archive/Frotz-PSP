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
#include "pspetext.h"
#include <pspthreadman.h>
#include <stdio.h>
#include <stdarg.h>

// Globals
f_setup_t f_setup;

// We expect no arguments because this is a PSP executable
void os_process_arguments(int argc, char *argv[])
{
	if(story_name == NULL)
		story_name = (char*)malloc(FILENAME_MAX);
	strncpy(story_name, argv[1], FILENAME_MAX);
}
void os_init_screen(void)
{
	//PSPDebugMsg("os_init_screen()");
	h_screen_rows = PSP_SCREEN_HEIGHT;
	h_screen_cols = PSP_SCREEN_WIDTH;
	h_interpreter_number = INTERP_MSDOS;
	h_interpreter_version = 'F';
	pspOutputInit();
}
int os_random_seed(void) {return time(0);}
void os_restart_game(int stage) {}
char textBuf[1024];
void pspDebugMsg(const char *fmt, ...)
{
	va_list		ap;

	if(fmt == NULL) return;
	va_start(ap, fmt);
		vsprintf(textBuf, fmt, ap);
	va_end(ap);

	bcSetFG(0xff000000);
	bcSetBG(0xff0000ff);
	bcSetPos(0, 0);
	bcPrintString((u8*)textBuf, BC_PRINT_NONE);
	bcFlush();
	sceKernelDelayThread(2000000);
	bcSetPos(0, 0);
	bcPrintString((u8*)"          ", BC_PRINT_NONE);
	bcFlush();
}
void os_fatal(const char *s)
{
	pspDebugMsg(s);
	sceKernelSleepThread();
}
FILE *os_path_open(const char *name, const char *mode)
{
	FILE *fp = fopen(name, mode);
	return fp;
}
void os_init_setup(void)
{
	f_setup.attribute_assignment = 0;
	f_setup.attribute_testing = 0;
	f_setup.context_lines = 0;
	f_setup.object_locating = 0;
	f_setup.object_movement = 0;
	f_setup.left_margin = 0;
	f_setup.right_margin = 0;
	f_setup.ignore_errors = 0;
	f_setup.piracy = 0;
	f_setup.undo_slots = MAX_UNDO_SLOTS;
	f_setup.expand_abbreviations = 0;
	f_setup.script_cols = 58;
	f_setup.save_quetzal = 1;
	f_setup.sound = 1;
	f_setup.err_report_mode = ERR_DEFAULT_REPORT_MODE;
}

// SOUND
void os_prepare_sample(int a) {}
void os_finish_with_sample(int a) {}
void os_start_sample(int a, int b, int c, zword d) {}
void os_stop_sample(int a) {}

// PIC
bool os_picture_data(int num, int *height, int *width) {return FALSE;}
void os_draw_picture(int num, int row, int col) {}
int os_peek_colour(void) {return BLACK_COLOUR;}
