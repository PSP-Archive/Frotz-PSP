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

char historyList[PSP_MAX_HISTORY][PSP_MAX_INPUT + 1];
int historyOffset = -1;

void pspHistoryInit()
{
	int i;
	for(i = 0; i < PSP_MAX_HISTORY; ++i)
	{
		historyList[i][0] = '\0';
	}
}

void pspHistoryPush(char *line)
{
	int i;
	for(i = PSP_MAX_HISTORY - 1; i > 0; --i)
	{
		strncpy(historyList[i], historyList[i - 1], PSP_MAX_INPUT);
	}
	strncpy(historyList[0], line, PSP_MAX_INPUT);
}

char *pspHistoryByOffset(int ofs)
{
	historyOffset += ofs;
	if(historyOffset < 0) historyOffset = 0;
	if(historyOffset >= PSP_MAX_HISTORY) historyOffset = PSP_MAX_HISTORY - 1;
	return historyList[historyOffset];
}

void pspHistoryResetOffset()
{
	historyOffset = -1;
}
