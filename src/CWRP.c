/******************************************************************************\
|*                                                                            *|
|* Console Window Request Processor                                           *|
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

#define INCL_DOS
#define INCL_DOSERRORS
#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "CWRP.h"

static tConsoleManagerSharedMem *ShMem;
static PFNWP OldWP;
static void *share = NULL;
static PID gavepid = 0;

void error(int errno)
{
 int i;
 char *msg;

 switch (errno)
 {
  case 1:
   msg = "Cannot create shared memory -- is CWRP already running?";
   break;
  case 2:
   msg = "Cannot create console agent window";
   break;
  case 3:
   msg = "Invalid Console Window request";
   break;
  case 4:
   msg = "Cannot create some semaphores";
   break;
  case 5:
   msg = "This program is not intended to be launched manually! "
         "It is run at appropiate time automatically.";
   break;
  case 6:
   msg = "Cannot obtain font dialog window handle";
   break;
  default:
   return;
 }

 for (i = 0; i < 10; i++)
  DosBeep(i * 100 + errno * 500 + 1000, 30);

 WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, msg, "CWRP error",
               0, MB_CANCEL | MB_ERROR | MB_MOVEABLE);
}

static int lockCount = 0;

BOOL LockUpdate()
{
 lockCount++;
 WinLockWindowUpdate(HWND_DESKTOP, HWND_DESKTOP);
 return (lockCount == 1);
}

BOOL UnlockUpdate()
{
 if (--lockCount == 0)
  WinLockWindowUpdate(HWND_DESKTOP, NULLHANDLE);
 return (lockCount == 0);
}

BOOL ShareMemoryWith(PID pid)
{
 if (gavepid != pid)
 {
  gavepid = pid;
  if (DosAllocSharedMem((PPVOID)&share, NULL, 4096, OBJ_GIVEABLE | OBJ_TILE | PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
   return FALSE;
  if (DosGiveSharedMem((PVOID)share, pid, PAG_READ | PAG_WRITE) != NO_ERROR)
  {
   DosFreeMem((PVOID)share);
   return FALSE;
  }
 }
 return TRUE;
}

BOOL InitSharedMem()
{
 return (DosAllocSharedMem(
  (PVOID)&ShMem,			    /* Pointer to object pointer */
  cwrpSharedMem,				   /* Shared memory name */
  sizeof(*ShMem),			       /* Desired size of object */
  PAG_COMMIT |						/* Commit memory */
  PAG_WRITE)				/* Allocate memory as read/write */
  == NO_ERROR);
}

BOOL InitSem()
{
 if (DosCreateEventSem(
  NULL,						    /* Unnamed semaphore */
  &ShMem->ready,				     /* Semaphore handle */
  DC_SEM_SHARED,				     /* Shared semaphore */
  FALSE)						   /* Not posted */
  != NO_ERROR)
  return FALSE;

 if (DosCreateMutexSem(
  NULL,						    /* Unnamed semaphore */
  &ShMem->inuse,				     /* Semaphore handle */
  DC_SEM_SHARED,				     /* Shared semaphore */
  FALSE)						    /* Not owned */
  != NO_ERROR)
  return FALSE;

 return TRUE;
}

BOOL SetConsoleFont(HWND vioHWND)
{
 int i;
 HWND dlgHWND, listHWND;

 LockUpdate();
 WinPostMsg(vioHWND, WM_SYSCOMMAND, (MPARAM)153, MPFROM2SHORT(CMDSRC_MENU, FALSE));
 for (i = 0; i < 100; i++)
 {
  dlgHWND = WinQueryWindow(vioHWND, QW_NEXTTOP);
  listHWND = WinWindowFromID(dlgHWND, 506);
  if (dlgHWND && listHWND) break;
  DosSleep(100);
 }
 if (!dlgHWND || !listHWND) { error(6); goto failed; }
 WinShowWindow(dlgHWND, FALSE);
 UnlockUpdate();
 WinSendMsg(listHWND, LM_SELECTITEM, (MPARAM)ShMem->fNo, (MPARAM)TRUE);
 WinSendMsg(dlgHWND, WM_COMMAND, (MPARAM)501, MPFROM2SHORT(CMDSRC_PUSHBUTTON, FALSE));
 return TRUE;

failed:
 UnlockUpdate();
 return FALSE;
}

ULONG QueryConsoleFont(HWND vioHWND)
{
 int i;
 HWND dlgHWND, listHWND;

 LockUpdate();
 WinPostMsg(vioHWND, WM_SYSCOMMAND, (MPARAM)153, MPFROM2SHORT(CMDSRC_MENU, FALSE));
 for (i = 0; i < 100; i++)
 {
  dlgHWND = WinQueryWindow(vioHWND, QW_NEXTTOP);
  listHWND = WinWindowFromID(dlgHWND, 506);
  if (dlgHWND && listHWND) break;
  DosSleep(100);
 }
 if (!dlgHWND || !listHWND) { error(6); goto failed; }
 WinShowWindow(dlgHWND, FALSE);
 UnlockUpdate();

 i = (int)WinSendMsg(listHWND, LM_QUERYSELECTION, (MPARAM)LIT_FIRST, (MPARAM)0);
 WinSendMsg(dlgHWND, WM_COMMAND, (MPARAM)DID_CANCEL, MPFROM2SHORT(CMDSRC_PUSHBUTTON, FALSE));
 return i;

failed:
 UnlockUpdate();
 return -1;
}

BOOL ResizeConsole(HWND vioHWND)
{
 SWP swp;

 WinQueryWindowPos(vioHWND, &swp);
 if (swp.fl & SWP_MINIMIZE)
 {
  if (ShMem->swp.fl & SWP_MOVE)
  {
   WinSetWindowUShort(vioHWND, QWS_XRESTORE, ShMem->swp.x);
   WinSetWindowUShort(vioHWND, QWS_YRESTORE, ShMem->swp.y);
  }
  if (ShMem->swp.fl & SWP_SIZE)
  {
   WinSetWindowUShort(vioHWND, QWS_CXRESTORE, ShMem->swp.cx);
   WinSetWindowUShort(vioHWND, QWS_CYRESTORE, ShMem->swp.cy);
  }
  return TRUE;
 }
 else
  return (WinSetWindowPos(vioHWND, ShMem->swp.hwndInsertBehind,
           ShMem->swp.x, ShMem->swp.y, ShMem->swp.cx, ShMem->swp.cy, ShMem->swp.fl));
}

BOOL ShowConsole(HWND vioHWND)
{
 WinSetWindowPos(vioHWND, NULLHANDLE, 0,0,0,0, ShMem->Show ? SWP_SHOW : SWP_HIDE);
 return TRUE;
}

ULONG QueryConsoleBorder(HWND vioHWND)
{
 PID pid;
 TID tid;
 PPOINTL bs;

 WinQueryWindowProcess(vioHWND, &pid, &tid);
 if (!ShareMemoryWith(pid))
  goto fallback;

 bs = share;
 if (!WinSendMsg(vioHWND, WM_QUERYBORDERSIZE, MPFROMP(bs), (MPARAM)0))
  goto fallback;
 return (bs->x | (bs->y << 16));

fallback:
 return (WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER) |
        (WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER) << 16));
}

MRESULT ConManProc(HWND wnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 switch (msg)
 {
  case WM_CONREQUEST:
  {
#ifdef DEBUG
   char *txt = alloca(256);
   sprintf(txt, "<%08x> <%d> <%08x>", (unsigned int)wnd, (int)mp1, (int)mp2);
   WinSetWindowText(wnd, (PSZ)txt);
#endif
   switch ((ULONG)mp1)
   {
    case cwrqLockOutput:
     LockUpdate();
     break;
    case cwrqUnlockOutput:
     UnlockUpdate();
     break;
    case cwrqSetFont:
     SetConsoleFont((HWND)mp2);
     break;
    case cwrqSetPos:
     ResizeConsole((HWND)mp2);
     break;
    case cwrqShow:
     ShowConsole((HWND)mp2);
     break;
    case cwrqQueryFont:
     ShMem->ret = QueryConsoleFont((HWND)mp2);
     break;
    case cwrqQueryBorder:
     ShMem->ret = QueryConsoleBorder((HWND)mp2);
     break;
    default:
     error(3);
   }
   DosPostEventSem(ShMem->ready);
   return (MRESULT)0;
  }
  default:
   return OldWP(wnd, msg, mp1, mp2);
 }
}

int main(int argc, char *argv[])
{
 HAB ab = WinInitialize(0);
 HMQ mq = WinCreateMsgQueue(ab, 0);
 QMSG msg;

#ifndef DEBUG
 if (strcmp(argv[0], arg0_magic) != 0)
 {
  error(5);
  goto failed;
 }
#endif

 if (!InitSharedMem()) { error(1); goto failed; }
 if (!InitSem())       { error(4); goto failed; }

 ShMem->manager = WinCreateWindow(
  HWND_DESKTOP,						/* Parent window */
  WC_STATIC,						   /* Class name */
  "Console Window Request Processor",			  /* Window text */
#ifdef DEBUG
  SS_TEXT | DT_CENTER | WS_VISIBLE,			 /* Window style */
#else
  SS_TEXT,						 /* Window style */
#endif
  0, 0,						       /* Position (x,y) */
  200, 40,					  /* Size (width,height) */
  NULLHANDLE,						 /* Owner window */
  HWND_TOP,					       /* Sibling window */
  0,							    /* Window id */
  NULL,							 /* Control data */
  NULL);					      /* Pres parameters */

 if (!ShMem->manager) { error(2); goto failed; }

 OldWP = WinSubclassWindow(ShMem->manager, &ConManProc);
 while (WinGetMsg(ab, &msg, 0, 0, 0))
  WinDispatchMsg(ab, &msg);

failed:
 WinDestroyMsgQueue(mq);
 WinTerminate(ab);
 return 0;
}
