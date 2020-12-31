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


#include "basecon.h"
#include "font.h"
#include <pspkernel.h>
#include <pspdisplay.h>
#include <stdio.h>

#define SKIPCACHE_MASK 0x40000000

int dMode, sWidth, sHeight, fbPitch, pFormat, fbPixCount;
static int iFont = 0;
void *pFB;
u32 bgColor = 0xff000000;
u32 fgColor = 0xffcfcfcf;
int posX, posY;

int bcInit(int font)
{
	// Set up the font
	if(font < 0 ||
		font >= FONT_NUM)
	{
		font = 0;
	}
	iFont = font;

	// Find out the screen metrics
	sceDisplayGetMode(&dMode, &sWidth, &sHeight);
	sceDisplayGetFrameBuf(&pFB, &fbPitch, &pFormat, 1);

	pFB = (void*) ((unsigned int)pFB | (unsigned int)SKIPCACHE_MASK);
	fbPixCount = fbPitch * sHeight;

	//debugPrintf("%d %d %d 0x%08X %d %d\n", dMode, sWidth, sHeight, pFB, fbPitch, pFormat);

	bcSetFG(0xffcfcfcf);
	bcSetBG(0xff000000);
	bcClear();
	bcSetPos(0, 0);

	return 0;
}
void bcFlush()
{
    sceDisplayWaitVblankStart();
}
void bcKill()
{
	bcClear();
}
int bcGetWidth()
{
	return fonts[iFont].sWidth;
}
int bcGetHeight()
{
	return fonts[iFont].sHeight;
}
void bcSetPos(int x, int y)
{
	posX = x;
	posY = y;
}
void bcPrintChar(int c, int flags)
{
	bcRenderChar(posX, posY, c);
	++posX;
	if(posX >= fonts[iFont].sWidth &&
		(flags & BC_PRINT_NOSCROLL) == 0)
	{
		++posY;
		if(posY >= fonts[iFont].sHeight) posY = 0;
		posX = 0;
	}
}
void bcPrintString(const u8 *str, int flags)
{
	while(*str)
	{
		if(*str == '\n')
		{
			bcClearToEndOfLine();
			++posY;
			if(posY >= fonts[iFont].sHeight) posY = 0;
			posX = 0;
		}
		else
		{
			bcRenderChar(posX, posY, *str);
			++posX;
			if(posX >= fonts[iFont].sWidth &&
				(flags & BC_PRINT_NOSCROLL) == 0)
			{
				++posY;
				if(posY >= fonts[iFont].sHeight) posY = 0;
				posX = 0;
			}
		}
		++str;
	}
}
void bcPrintRawString(const u8 *str, int flags)
{
	while(*str)
	{
        bcRenderChar(posX, posY, *str);
        ++posX;
        if(posX >= fonts[iFont].sWidth &&
            (flags & BC_PRINT_NOSCROLL) == 0)
        {
            ++posY;
            if(posY >= fonts[iFont].sHeight) posY = 0;
            posX = 0;
        }
		++str;
	}
}
void bcRenderChar(int x, int y, int c)
{
	int px, py;
	u32* pFB32 = (u32*)pFB;
	u16* pFB16 = (u16*)pFB;
	int iPixBit, iPixByte;
	u8 mask;
	int fbOfs;

	// Bail if offscreen or invalid character
	if(x < 0 || x >= bcGetWidth() ||
		y < 0 || y >= bcGetHeight() ||
		c < 0 || c >= 256) return;

	// Render out one character cell
	for(py = 0; py < fonts[iFont].charHeight; ++py)
	{
		for(px = 0; px < fonts[iFont].charWidth; ++px)
		{
			// Determine col byte offset
			iPixByte = ((c % 16) * fonts[iFont].charWidth + px) / 8;
			// Add row offset
			iPixByte += ((c / 16) * fonts[iFont].charHeight + py) * fonts[iFont].dataPitch;
			// Determine bit offset
			iPixBit = ((c % 16) * fonts[iFont].charWidth + px) % 8;
			mask = 1 << iPixBit;
			fbOfs = (y * fonts[iFont].charHeight + fonts[iFont].sOfsY + py) * fbPitch
					+ (x * fonts[iFont].charWidth + fonts[iFont].sOfsX + px);
			if(fonts[iFont].data[iPixByte] & mask)
			{
				if(pFormat == 3) pFB32[fbOfs] = fgColor;
				else pFB16[fbOfs] = (u16)fgColor;
			}
			else
			{
				if(pFormat == 3) pFB32[fbOfs] = bgColor;
				else pFB16[fbOfs] = (u16)bgColor;
			}
		}
	}
}
void bcClear()
{
	int x, y;
	u32 *pFB32 = (u32*)pFB;
	u16* pFB16 = (u16*)pFB;
	for(y = 0; y < sHeight; ++y)
	{
		for(x = 0; x < sWidth; ++x)
		{
			if(pFormat == 3) pFB32[y * fbPitch + x] = bgColor;
			else pFB16[y * fbPitch + x] = (u16)bgColor;
		}
	}
}
void bcSetFG(u32 c)
{
	switch(pFormat)
	{
	/*
	xxxxxxxxbbbbbxxxgggggxxxrrrrrxxx
	                abbbbbgggggrrrrr
	xxxxxxxxbbbbxxxxggggxxxxrrrrxxxx
	                aaaabbbbggggrrrr
	*/
	case 0:
		fgColor = ((c & 0x000000f8) >> 3) |
				  ((c & 0x0000fc00) >> 5) |
				  ((c & 0x00f80000) >> 8);
		break;
	case 1:
		fgColor = ((c & 0x000000f8) >> 3) |
				  ((c & 0x0000f800) >> 6) |
				  ((c & 0x00f80000) >> 9) |
				  0x8000;
		break;
	case 2:
		fgColor = ((c & 0x000000f0) >> 4) |
				  ((c & 0x0000f000) >> 8) |
				  ((c & 0x00f00000) >> 12) |
				  0xf000;
		break;
	default:
		fgColor = c;
		break;
	}
}
void bcSetBG(u32 c)
{
	u32 temp = fgColor;
	bcSetFG(c);
	bgColor = fgColor;
	fgColor = temp;
}
void bcClearToEndOfLine()
{
	int x;

	for(x = posX; x < fonts[iFont].sWidth; ++x)
	{
		bcRenderChar(x, posY, ' ');
	}
}
