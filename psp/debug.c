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


#include <pspiofilemgr.h>
#include <stdio.h>
#include <stdarg.h>
#include "debug.h"


// Debuging enabled
#ifdef DEBUG
	// PSPLink debugging disabled
	#ifndef PSPLINK_DEBUG
		char textBuf[DEBUG_TEXT_BUF_LENGTH];

		int appendBufferToFile(const char * path, void * buffer, int buflen)
		{
			// Written Bytes
			int written = 0;

			// Open File for Appending
			SceUID file = sceIoOpen(path, PSP_O_APPEND | PSP_O_CREAT | PSP_O_WRONLY, 0777);

			// Opened File
			if(file >= 0)
			{
				// Write Buffer
				written = sceIoWrite(file, buffer, buflen);

				// Close File
				sceIoClose(file);
			}

			// Return Written Bytes
			return written;
		}
		void debugPrintf(const char *fmt, ...)
		{
			va_list		ap;

			if(fmt == NULL) return;
			va_start(ap, fmt);
				vsprintf(textBuf, fmt, ap);
			va_end(ap);

			appendBufferToFile(DEBUG_LOGFILE, textBuf, strlen(textBuf));
		}
	#endif//PSPLINK_DEBUG
#endif//DEBUG
