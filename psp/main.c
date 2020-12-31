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


#include <pspkernel.h>
#include <pspdebug.h>
#include <pspiofilemgr.h>
#include <psppower.h>
#include "psp_frotz.h"
#include "pspetext.h"
#include "basecon.h"

PSP_MODULE_INFO("Hello World", 0, 1, 1);
PSP_MAIN_THREAD_STACK_SIZE_KB(64);
PSP_HEAP_SIZE_MAX();

// Exit callback
int ExitCallback(int Arg1, int Arg2, void *Common)
{
    sceKernelExitGame();
    return 0;
}

// Power Callback
extern FILE *story_fp;
extern char *story_name;
int PowerCallback(int unknown, int pwrflags, void *common)
{
	static ssize_t story_fp_cursor;

	// Suspending, close the story file.
	if(pwrflags & PSP_POWER_CB_POWER_SWITCH ||
		pwrflags & PSP_POWER_CB_SUSPENDING)
	{
		// Record the cursor position
		story_fp_cursor = ftell(story_fp);
		fclose(story_fp);
	}
	// Resuming from suspend, re-open the story file
	else if (pwrflags & PSP_POWER_CB_RESUMING)
	{
		// Re-open the file, then seek back to the saved cursor position.
		sceKernelDelayThread(1000000);
		story_fp = fopen(story_name, "rb");
		if(story_fp == NULL)
			pspDebugMsg("Failed to re-open story file after resume.\n");
		fseek(story_fp, story_fp_cursor, SEEK_SET);
	}
	return 0;
}


// Callback thread
int CallbackThread(SceSize Args, void *Argp)
{
    int CallbackId;

    CallbackId = sceKernelCreateCallback("Exit Callback", ExitCallback, NULL);
    sceKernelRegisterExitCallback(CallbackId);
    CallbackId = sceKernelCreateCallback("Power Callback", PowerCallback, NULL);
    scePowerRegisterCallback(0, CallbackId);
    sceKernelSleepThreadCB();

    return 0;
}

// Sets up the callback thread and returns its thread id
int SetupCallbacks(void)
{
    int ThreadId = 0;

    ThreadId = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (ThreadId >= 0)
    {
        sceKernelStartThread(ThreadId, 0, 0);
    }

    return ThreadId;
}
int frotz_main (int argc, char *argv[]);
int main(int argc, char ** argv)
{
	SceIoStat stat;
    pspDebugScreenInit();
    SetupCallbacks();
	char *fakeArgv[] = {
		"bogus.exe",
		NULL
	};
/*
    bcInit(0);
	int i;
	for(i = 0; i < 256; ++i)
	{
		bcRenderChar(i % 16, i / 16, i);
	}
    bcFlush();
    sceKernelSleepThread();
*/
	// Check to see if the insanely-named debug story file is present, and if
	// so just load it.
	if(sceIoGetstat(PSP_DEBUG_STORY_FILE, &stat) == 0)
	{
		fakeArgv[1] = PSP_DEBUG_STORY_FILE;
	}
	// Otherwise, prompt the user for a file name.
	else
	{
		// Get the story file name
		bcInit(0);
		while(petGetFileName() != 0){}
		fakeArgv[1] = petScratch;
	}
	// Main frotz loop
	while(1)
    {
		frotz_main(2, fakeArgv);
    }

    sceKernelSleepThread();

    return 0;
}
