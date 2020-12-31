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


#include "debug.h"
#include "pspetext.h"
#include <pspiofilemgr_fcntl.h>
#include <stdlib.h>

/* Internal file copy utility function. The PSP hates it when we do a file
 * read directly into the framebuffer area for whatever reason. So this
 * function uses the static file path buffer to read chunks of the file before
 * pushing the data into the destination buffer. The file path buffer is
 * backed up to and restored from the petParm buffer.
 */
int fbChunkedRead(SceUID fd, u8 *buf, SceSize loadLength)
{
	SceSize loaded = 0;
	SceSize toLoad = PET_FPATH_LEN;
	int retVal = 0, lastChunk = 0;

	// Short-circut just in case
	if(loadLength <= 0) return 0;

	// Back up the file path
	memcpy(petParm, petFilePath, PET_FPATH_LEN);

	// Perform all chunked reads
	while(1)
	{
		// Last chunk
		if(loaded + toLoad >= loadLength)
		{
			lastChunk = 1;
			toLoad = loadLength - loaded;
		}

		// Load the chunk
		if(sceIoRead(fd, petFilePath, toLoad) != toLoad)
		{
			debugPrintf("fbChunkedRead() sceIoRead() failed\n");
			errorPrint("File Read Failed");
			retVal = 1;
			break;
		}

		// Copy to buffer
		memcpy(&buf[loaded], petFilePath, toLoad);
		loaded += toLoad;

		// Break if last chunk loaded
		if(lastChunk) break;
	}

	// Restore the file path
	memcpy(petFilePath, petParm, PET_FPATH_LEN);

	return retVal;
}
// Set fb->bufTop before calling
int fbLoadBuffer(FILEBUF *fb)
{
	SceUID fd;
	SceSize loadLength;


	// Checking
	if(fb->bufTop > fb->length ||
		fb->bufTop < 0)
	{
		debugPrintf("fbLoadBuffer() bad parameter\n");
		return 1;
	}

	// Figure out how much to read
	if(fb->bufTop + fb->bufLength > fb->length)
	{
		loadLength = fb->length - fb->bufTop;
	}
	else loadLength = fb->bufLength;

	// Open the file
	fd = sceIoOpen(fb->path, PSP_O_RDONLY, 0777);
	if(fd < 0)
	{
		debugPrintf("fbLoadBuffer() failed to open file %s\n", fb->path);
		errorPrint("Failed to Open File");
		return 1;
	}

	// Load the buffer
	if(sceIoLseek(fd, fb->bufTop, PSP_SEEK_SET) != fb->bufTop)
	{
		debugPrintf("fbLoadBuffer() sceIoLseek() failed\n");
		errorPrint("Failed to Open File");
		sceIoClose(fd);
		return 1;
	}
	//if(fbChunkedRead(fd, fb->buf, loadLength))
	if(sceIoRead(fd, fb->buf, loadLength) != loadLength)
	{
		debugPrintf("fbChunkedRead() sceIoRead() failed\n");
		errorPrint("File Read Failed");
		sceIoClose(fd);
		return 1;
	}
	sceIoClose(fd);
	return 0;
}

int fbOpen(FILEBUF *fb, u8 *buf, SceOff bufLength)
{
	SceIoStat stat;

	if(fb->path == NULL ||
		bufLength < 16) return 1;

	// Init the struct
	fb->bufLength = bufLength;
	fb->bufTop = 0;
	fb->buf = buf;

	// Get information about the file
	if(sceIoGetstat(fb->path, &stat) < 0)
	{
		debugPrintf("fbOpen() sceIoGetstat(\"%s\") failed\n", fb->path);
		errorPrint("Failed to Stat File");
		errorPrint(fb->path);
		return 1;
	}
	fb->length = stat.st_size;

	// Load the buffer
	if(fbLoadBuffer(fb))
	{
		return 1;
	}

	return 0;
}
int fbFromBuffer(FILEBUF *out, u8 *buf, SceOff bufLength, char *name)
{
	out->path = name;
	out->bufLength = bufLength;
	out->length = bufLength;
	out->bufTop = 0;
	out->buf = buf;
	return 0;
}
void fbClose(FILEBUF *fb)
{
}
int fbReadByte(FILEBUF *fb, SceOff off)
{
	// Bound offset
	if(off < 0 ||
		off >= fb->length) return -1;

	// If the offset is within our current buffer return it
	if(off >= fb->bufTop &&
		off < fb->bufTop + fb->bufLength)
	{
		return (int)fb->buf[off - fb->bufTop];
	}
	// Otherwise try to re-window
	else
	{
		fb->bufTop = off - (fb->bufLength / 2);
		if(fb->bufTop < 0) fb->bufTop = 0;
		if(fbLoadBuffer(fb))
		{
			return -1;
		}
		return (int)fb->buf[off - fb->bufTop];
	}

	return 0;
}
