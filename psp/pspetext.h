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

#ifndef PSPETEXT_H_INCLUDED
#define PSPETEXT_H_INCLUDED

#include <psptypes.h>

// Configuration Block
#define PET_DEFAULT_FG		0xffcfcfcf
#define PET_DEFAULT_BG		0xff000000
#define PET_DEFAULT_FGHL	0xff000000
#define PET_DEFAULT_BGHL	0xffcfcfcf
#define PET_DEFAULT_FGER	0xff000000
#define PET_DEFAULT_BGER	0xff0000ff

#define PET_DEFAULT_DIR		"ms0:/"

//#define DEBUG
//#define PSPLINK_DEBUG


// Static memory buffers and thier dimensions. The FB* naming convention comes
// from when I shoved this into the first few lines of the frame buffer. I
// just don't feel like refactoring this.
#define	PET_FBPARM_LEN			256
extern char petParm[PET_FBPARM_LEN];

#define	PET_FBSCRATCH_LEN		256
extern char petScratch[PET_FBSCRATCH_LEN];

// This is NOT an array of pointers, but one big buffer.
#define	PET_FBLISTSTR_COUNT		33
#define	PET_FBLISTSTR_LEN		80
extern char petListStrings[PET_FBLISTSTR_COUNT*PET_FBLISTSTR_LEN];

#define	PET_FBFILEBUF_LEN		4096
extern u8 petFileBuf[PET_FBFILEBUF_LEN];

#define	PET_FPATH_LEN			256
extern char petFilePath[PET_FPATH_LEN];

// Globals
extern u32 petFGColor;
extern u32 petBGColor;
extern u32 petFGHLColor;
extern u32 petBGHLColor;
extern u32 petFGERColor;
extern u32 petBGERColor;

// File buffer system
#include <pspiofilemgr.h>
typedef struct
{
	const char *path;
	SceOff length;
	SceOff bufTop;
	SceOff bufLength;
	u8 *buf;
} FILEBUF;
int fbOpen(FILEBUF *fb, u8 *buf, SceOff bufLength);
int fbFromBuffer(FILEBUF *out, u8 *buf, SceOff bufLength, char *name);
void fbClose(FILEBUF *fb);
int fbReadByte(FILEBUF *fb, SceOff off);
int fbLoadBuffer(FILEBUF *fb);

// Global functions
int errorPrint(const char *str);
int petMain();
int petCommonInputHandler(int isHelpScreen);
int petGetFileName();
int petBufferViewer(FILEBUF *p_fileBuf, SceOff *p_line, SceOff *p_offset, int isHelpScreen);
SceOff scanForPrevLine(FILEBUF *buf, SceOff start);
SceOff scanForNextLine(FILEBUF *buf, SceOff start);
int petListProc(const char *statusLine, int(*refreshList)(int));

#endif // PSPETEXT_H_INCLUDED
