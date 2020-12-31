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


#include "input.h"
#include <pspctrl.h>
#include <psptypes.h>
#include <pspthreadman.h>

// Maps a key enum to the PspCtrlButtons value for that key
int pspKeyName_pspCtrlButton[] = {
	PSP_CTRL_SELECT,
	PSP_CTRL_START,
	PSP_CTRL_UP,
	PSP_CTRL_RIGHT,
	PSP_CTRL_DOWN,
	PSP_CTRL_LEFT,
	PSP_CTRL_LTRIGGER,
	PSP_CTRL_RTRIGGER,
	PSP_CTRL_TRIANGLE,
	PSP_CTRL_CIRCLE,
	PSP_CTRL_CROSS,
	PSP_CTRL_SQUARE
};

struct
{
	u16 ticks;		// The number of ticks that the key has been held down for, 1 on the first tick.
} keyStates[12] = {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}};

u32 buttonFlags = 0;
u8 analogX = 127;
u8 analogY = 127;

void inPumpInputEvents(int flags)
{
	int i, lastSampleMode;
	SceCtrlData pad;
	u32 lastFlags = buttonFlags;

	//sceCtrlSetSamplingCycle(0);
	//sceCtrlSetSamplingMode(1);

	while(1)
	{
		buttonFlags = 0;
		sceCtrlGetSamplingMode(&lastSampleMode);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
		sceKernelDelayThread(PSP_MICRO_PER_FRAME);
		sceCtrlPeekBufferPositive(&pad, 1);
		sceCtrlSetSamplingMode(lastSampleMode);
		analogX = pad.Lx;
		analogY = pad.Ly;

		// Itterate through all key names
		for(i = 0; i < PSPKEY_LAST; i ++)
		{
			// Key is down, increment ticks
			if(pad.Buttons & pspKeyName_pspCtrlButton[i])
			{
				keyStates[i].ticks ++;
				// If this is the first frame generate a keystroke
				if(keyStates[i].ticks == 1) buttonFlags |= (1 << i);
				// If the key has been down long enough to repeat...
				if(keyStates[i].ticks >= PSP_REPEAT_DELAY)
				{
					// And we are on an even multiple of the ticks delay, generate a keystroke
					if((keyStates[i].ticks - PSP_REPEAT_DELAY) % PSP_REPEAT_TICKS == 0)
					{
						buttonFlags |= (1 << i);
					}
				}
			}
			// Key is not down, clear ticks
			else
			{
				keyStates[i].ticks = 0;
			}
		}

		// If a key was pressed or this is an async call return
		if(lastFlags != buttonFlags || flags & PSP_INASYNC) break;
	}
}
int inIsKeyDown(int key)
{
	// Bad index
	if(key < 0 || key >= PSPKEY_LAST) return 0;

	if(keyStates[key].ticks > 0) return 1;

	return 0;
}
int inWasKeyPressed(int key)
{
	// Bad index
	if(key < 0 || key >= PSPKEY_LAST) return 0;

	if(buttonFlags & (1 << key)) return 1;

	return 0;
}
int inIsAnalogDir(int dir)
{
	switch(dir)
	{
	case PSPKEY_UP: return analogY < 64 ? 1 : 0; break;
	case PSPKEY_DOWN: return analogY > 191 ? 1 : 0; break;
	case PSPKEY_LEFT: return analogX < 64 ? 1 : 0; break;
	case PSPKEY_RIGHT: return analogX > 191 ? 1 : 0; break;
	}
	return 0;
}
