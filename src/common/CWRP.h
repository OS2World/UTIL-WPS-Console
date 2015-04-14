/******************************************************************************\
|*                                                                            *|
|* Console Window Request Processor: header file                              *|
|* Copyright (C) 1997 by FRIENDS software                                     *|
|* All Rights Reserved                                                        *|
|* Portability: OS/2                                                          *|
|*                                                                            *|
|* This program is free software; you can redistribute it and/or modify       *|
|* it under the terms of the GNU General Public License as published by       *|
|* the Free Software Foundation; either version 2 of the License, or          *|
|* (at your option) any later version.                                        *|
|*                                                                            *|
|* This program is distributed in the hope that it will be useful,            *|
|* but WITHOUT ANY WARRANTY; without even the implied warranty of             *|
|* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *|
|* GNU General Public License for more details.                               *|
|*                                                                            *|
|* You should have received a copy of the GNU General Public License          *|
|* along with this program; if not, write to the Free Software                *|
|* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA  *|
|*                                                                            *|
\******************************************************************************/

typedef struct
{
 HWND manager;				/* CWRP window handle */
 HMTX inuse;				/* In-Use mutex semaphore */
 HEV ready;				/* Request Ready event semaphore */
 SWP swp;				/* cwrqSetPos arg */
 ULONG fNo;				/* cwrqSetFont arg */
 BOOL Show;				/* cwrqShow arg */
 int ret;				/* CWRP return value */
} tConsoleManagerSharedMem;

#define arg0_magic "run cwrp daemon"

#define cwrpSharedMem "\\SHAREMEM\\CONSOLE\\CWRP"

#define WM_CONREQUEST 0xFEED		/* Console request message ID # */

#define cwrqLockOutput		1
#define cwrqUnlockOutput	2
#define cwrqSetFont		3
#define cwrqSetPos		4
#define cwrqShow		5
#define cwrqQueryFont		6
#define cwrqQueryBorder		7
