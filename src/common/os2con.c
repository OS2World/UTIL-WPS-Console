/******************************************************************************\
|*                                                                            *|
|* OS/2 console control routines                                              *|
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

#define INCL_SUB
#define INCL_DOS
#define INCL_WIN
#define INCL_DOSERRORS
#include <os2.h>
#include <malloc.h>
#include "os2con.h"
#include "CWRP.h"

#include <conio.h>
#include <stdio.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>

// Enable (1) or disable (0) ANSI output
boolean ANSI = TRUE;

// Console font list
PVIOFONTCELLSIZE FontList;
ULONG FontCount;

HWND vioHWND = NULLHANDLE;
HWND clientHWND = NULLHANDLE;
HWND listHWND = NULLHANDLE;
HWND dlgHWND = NULLHANDLE;
HSWITCH vioHSW;

static int ANSI2VIO[17] =
{-1, 0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15};

/*
 * Note: Since this library is oriented to console programs, we
 *       SHOULD NOT use functions that requires a message queue
 *       to exist (since console program cannot own a message queue),
 *       Most WinXXX functions DO require a msgqueue to exist, so we
 *       should do as much as possible with messages, thanks God
 *       WinPostMsg does not require it (alas, WinSendMsg requires).
 *
 *       For functions that require an message queue to exist we
 *       should use an external PM agent program called CWRP.
 */

static tConsoleManagerSharedMem *ShMem;
static HMTX cwrpInUse;
static HEV cwrpReady;

HWND GetWindowHWND()
{
 if (!vioHWND)
 {
  PTIB tb;
  PPIB pb;
  SWCNTRL swc;
  if (DosGetInfoBlocks(&tb, &pb))
   return NULLHANDLE;
  if (!(vioHSW = WinQuerySwitchHandle(NULLHANDLE, pb->pib_ulpid)))
   return NULLHANDLE;
  if (WinQuerySwitchEntry(vioHSW, &swc))
   return NULLHANDLE;
  vioHWND = swc.hwnd;
  clientHWND = WinWindowFromID(vioHWND, FID_CLIENT);
 }
 return vioHWND;
}

/* Send a request to CWRP */
int CWRQ(ULONG request, void *data)
{
 int Try;
 ULONG Count;
 BOOL rc;

 if (!ShMem)
 {
  for (Try = 0; Try < 50; Try++)
   if (DosGetNamedSharedMem(
    (PVOID)&ShMem,                       /* Pointer to pointer of object */
    cwrpSharedMem,                              /* Name of shared memory */
    PAG_READ | PAG_WRITE) != NO_ERROR)              /* Read/write access */
   {
    if (Try == 0)                                   /* CWRP not running? */
    {
     RESULTCODES rc;
     char cwrp[CCHMAXPATH];

     _path((char *)&cwrp, "CWRP.exe");
     DosExecPgm(NULL, 0, EXEC_BACKGROUND, arg0_magic"\0\0", NULL, &rc, cwrp);
    }

    DosSleep(100);
   } else break;
  if (!ShMem) goto failed;
  for (Try = 0; Try < 100; Try++)
   if (!ShMem->manager) DosSleep(100);
   else break;
  if (!ShMem->manager) goto failed;
  cwrpInUse = ShMem->inuse;
  cwrpReady = ShMem->ready;
  if (DosOpenMutexSem(NULL, &cwrpInUse) != NO_ERROR) goto failed;
  if (DosOpenEventSem(NULL, &cwrpReady) != NO_ERROR) goto failed;
 }

 if (DosRequestMutexSem(cwrpInUse, SEM_INDEFINITE_WAIT) != NO_ERROR)
  return -1;
 DosResetEventSem(cwrpReady, &Count);

 switch (request)
 {
  case cwrqSetFont:
   ShMem->fNo = (LONG)data;
  case cwrqShow:
   ShMem->Show = (BOOL)data;
   break;
  case cwrqSetPos:
   ShMem->swp = *(PSWP)data;
 }

 ShMem->ret = 0;
 WinPostMsg(ShMem->manager, WM_CONREQUEST, (MPARAM)request, (MPARAM)GetWindowHWND());
 rc = DosWaitEventSem(cwrpReady, 10000);
 Try = ShMem->ret;
 DosReleaseMutexSem(cwrpInUse);
 if (rc != NO_ERROR) return -1;
 return Try;

failed:
 ShMem = 0;
 return -1;
}

void SetColor(int Fore, int Back)
{
 if (!ANSI || (Fore < colSAME) || (Fore > colWHITE) || (Back < colSAME) || (Back > colWHITE))
  return;

 Fore = ANSI2VIO[++Fore];
 Back = ANSI2VIO[++Back];

 if ((Fore != colSAME) && (Back != colSAME))
  printf("\x1b[%d;3%d;4%dm", Fore/8, Fore%8, Back%8);
 else if (Fore != colSAME)
  printf("\x1b[%d;3%dm", Fore/8, Fore%8);
 else if (Back != colSAME)
  printf("\x1b[4%dm", Back%8);
}

void SetConsoleSize(int Width, int Height, int Attr)
{
 VIOMODEINFO vi;
 vi.cb = sizeof(vi);
 VioGetMode(&vi, 0);

 vi.col = Width;
 vi.row = Height;
 vi.cb = sizeof(vi.cb) + sizeof(vi.fbType) + sizeof(vi.color) + \
         sizeof(vi.col) + sizeof(vi.row);
 VioSetMode(&vi, 0);
 ClearConsole(Attr);
}

void GetConsoleSize(int *Width, int *Height)
{
 VIOMODEINFO vi;
 vi.cb = sizeof(vi);
 VioGetMode(&vi, 0);
 *Width = vi.col;
 *Height = vi.row;
}

void SetCursorPos(int X, int Y)
{
 VioSetCurPos(Y, X, 0);
}

void ClearConsole(int Attr)
{
 static char clsAttr[2];
 VIOMODEINFO vi;

 clsAttr[0] = ' ';
 if (Attr == colSAME)
  clsAttr[1] = 0x07;
 else
  clsAttr[1] = ANSI2VIO[(Attr & 0x0f) + 1] | ANSI2VIO[(Attr >> 4) + 1] << 4;
 vi.cb = sizeof(vi);
 VioGetMode(&vi, 0);
 VioScrollUp(0, 0, vi.row, vi.col, vi.col, (PBYTE)&clsAttr, 0);

 SetCursorPos(0, 0);
}

BOOL QueryFontList()
{
 HDC hdc;
 ULONG outCount;
 PVIOSIZECOUNT vsc;
 uint i,j,k;

 if (FontList && FontCount)
  return TRUE;

 if (!GetWindowHWND()) exit(1);
 hdc = WinQueryWindowDC(clientHWND);

 outCount = sizeof(FontCount);
 if (DevEscape(hdc, DEVESC_QUERYVIOCELLSIZES, 0, NULL, &outCount, (PBYTE)&FontCount) != DEV_OK)
  return FALSE;

 outCount = sizeof(VIOSIZECOUNT) + FontCount * sizeof(VIOFONTCELLSIZE);
 vsc = malloc(outCount);
 if (DevEscape(hdc, DEVESC_QUERYVIOCELLSIZES, 0, NULL, &outCount, (PBYTE)vsc) != DEV_OK)
  return FALSE;

 FontList = (PVIOFONTCELLSIZE)&vsc[1];

 /* Sort fonts by height as in list box */
 for (i = 0; i < FontCount; i++)
  for (j = i + 1; j < FontCount; j++)
   if ((FontList[i].cy > FontList[j].cy) ||
       ((FontList[i].cy == FontList[j].cy) && (FontList[i].cx > FontList[j].cx)))
   {
    k = FontList[i].cx; FontList[i].cx = FontList[j].cx; FontList[j].cx = k;
    k = FontList[i].cy; FontList[i].cy = FontList[j].cy; FontList[j].cy = k;
   }

 return TRUE;
}

BOOL SetConsoleFontSize(int H, int W)
{
 int fno;

 if ((fno = NearestFont(H, W)) < 0)
  return FALSE;

 return (CWRQ(cwrqSetFont, (void *)fno) != -1);
}

BOOL GetConsoleFontSize(int *H, int *W)
{
 int i;

 if (!QueryFontList())
  return FALSE;

 i = CWRQ(cwrqQueryFont, NULL);

 if (i < 0)
  return FALSE;
 *W = FontList[i].cx;
 *H = FontList[i].cy;
 return TRUE;
}

BOOL MaximizeConsole()
{
 if (!GetWindowHWND()) return FALSE;
 WinPostMsg(vioHWND, WM_SYSCOMMAND, (MPARAM)SC_MAXIMIZE, MPFROM2SHORT(CMDSRC_MENU, FALSE));
 return TRUE;
}

BOOL MinimizeConsole()
{
 if (!GetWindowHWND()) return FALSE;
 WinPostMsg(vioHWND, WM_SYSCOMMAND, (MPARAM)SC_MINIMIZE, MPFROM2SHORT(CMDSRC_MENU, FALSE));
 return TRUE;
}

BOOL RestoreConsole()
{
 if (!GetWindowHWND()) return FALSE;
 WinPostMsg(vioHWND, WM_SYSCOMMAND, (MPARAM)SC_RESTORE, MPFROM2SHORT(CMDSRC_MENU, FALSE));
 return TRUE;
}

BOOL ShowConsole(BOOL Show)
{
 return (CWRQ(cwrqShow, (void *)Show) != -1);
}

BOOL SelectConsole()
{
 if (!GetWindowHWND()) return FALSE;
 // Lock window updates so that if Console is minimized it will not pop up
 WinEnableWindow(vioHWND, FALSE);       // Tricky ??? but works.
 WinSwitchToProgram(vioHSW);
 WinEnableWindow(vioHWND, TRUE);
 return TRUE;
}

BOOL FlashConsole(BOOL State)
{
 if (!GetWindowHWND()) return FALSE;
 WinPostMsg(vioHWND, WM_FLASHWINDOW, (MPARAM)State, (MPARAM)0);
 return TRUE;
}

BOOL SetConsoleBorderSize(int W, int H)
{
 if (!GetWindowHWND()) return FALSE;
 WinPostMsg(vioHWND, WM_SETBORDERSIZE, (MPARAM)W, (MPARAM)H);
 return TRUE;
}

BOOL GetConsoleBorderSize(int *W, int *H)
{
 ULONG i = CWRQ(cwrqQueryBorder, NULL);
 if (i < 0)
  return FALSE;

 *W = (i & 0xFFFF);
 *H = (i >> 16);
 return TRUE;
}

BOOL SetPixelConsoleSize(int X, int Y, int W, int H, ULONG Flags)
{
 SWP swp;

 // Fill with old values
 WinQueryWindowPos(vioHWND, &swp);
 swp.fl = Flags;
 if (((Flags & SWP_MOVE) == 0) && ((Flags & SWP_SIZE) != 0))
 {
  swp.y += swp.cy - H;
  swp.fl |= SWP_MOVE;
 }
 if (Flags & SWP_MOVE)
 { swp.x = X;  swp.y = Y; }
 if (Flags & SWP_SIZE)
 { swp.cx = W; swp.cy = H; }

 return (CWRQ(cwrqSetPos, (void *)&swp) != -1);
}

BOOL GetPixelConsoleSize(int *X, int *Y, int *W, int *H)
{
 SWP swp;

 if (!GetWindowHWND()) return FALSE;

 WinQueryWindowPos(vioHWND, &swp);
 *X = swp.x;  *Y = swp.y;
 *W = swp.cx; *H = swp.cy;
 return TRUE;
}

int NearestFont(int H, int W)
{
 ULONG i,j,fno,dist;

 if (!QueryFontList())
  return -1;

 dist = 9999; fno = 0;
 for (i = 0; i < FontCount; i++)
 {
  j = (FontList[i].cy - H) * (FontList[i].cy - H) +
      (FontList[i].cx - W) * (FontList[i].cx - W);
  if (j < dist)
  { dist = j; fno = i; }
 }
 return fno;
}

USHORT _THUNK_FUNCTION (Win16SetTitle) ();

USHORT WinSetTitle (char *title)
{
  return ((USHORT)
          (_THUNK_PROLOG (4);
           _THUNK_FLAT (title);
           _THUNK_CALL (Win16SetTitle)));
}

BOOL SetConsoleTitle(char *title)
{
 return (WinSetTitle(title));
}

BOOL SetMaxConsoleSize()
{
 int w,h;
 int bw,bh;
 int cw,ch;
 int oldx,oldy,oldw,oldh;
 int th = WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);

 if (!GetConsoleFontSize(&h, &w))
  return FALSE;

 if (!GetConsoleBorderSize(&bw, &bh))
  return FALSE;

 if (!GetPixelConsoleSize(&oldx, &oldy, &oldw, &oldh))
  return FALSE;

 GetConsoleSize(&cw, &ch);

 // Compute max window size
 cw = bw * 2 + cw * w;
 ch = th + bh * 2 + ch * h;
 oldy = (oldy + oldh) - ch;

 return SetPixelConsoleSize(oldx, oldy, cw, ch, SWP_MOVE | SWP_SIZE);
}

BOOL LockConsoleUpdate()
{
 return CWRQ(cwrqLockOutput, NULL);
}

BOOL UnlockConsoleUpdate()
{
 return CWRQ(cwrqUnlockOutput, NULL);
}
