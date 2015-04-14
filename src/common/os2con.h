/******************************************************************************\
|*                                                                            *|
|* OS/2 console control routines: header file                                 *|
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

#ifndef __ANSI_H__
#define __ANSI_H__

#include "stype.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define colSAME		-1
#define colBLACK	0
#define colBLUE		1
#define	colGREEN	2
#define colCYAN		3
#define colRED		4
#define colMAGENTA	5
#define colBROWN	6
#define colGRAY		7
#define colDGRAY	8
#define colLBLUE	9
#define colLGREEN	10
#define colLCYAN	11
#define colLRED		12
#define colLMAGENTA	13
#define colYELLOW	14
#define colWHITE	15

// Output ANSI color sequence (if ANSI == 1)
void SetColor(int Fore, int Back);

// Query console width & height
void GetConsoleSize(int *Width, int *Height); 

// Set console (window) width & height
void SetConsoleSize(int Width, int Height, int Attr);

// Clear console
void ClearConsole(int Attr);

// Set cursor position
void SetCursorPos(int X, int Y);

// Enable ANSI colors?
extern boolean ANSI;

// Console font size structure
typedef struct
{
 int W;
 int H;
} tFontSize, *pFontSize;

// The list of console fonts
extern PVIOFONTCELLSIZE FontList;
extern ULONG FontCount;

// VIO window handles
extern HWND vioHWND;
extern HWND listHWND;
extern HWND clientHWND;
extern HWND dlgHWND;
extern HSWITCH vioHSW;

//--- "Tricky" functions ---

// Get VIO window handle
extern HWND GetWindowHWND();

// Query the list of available VIO fonts (result placed in FontList[])
extern BOOL QueryFontList();

// Set console font size (relatively slow!)
extern BOOL SetConsoleFontSize(int H, int W);

// Query console font size (relatively slow!)
extern BOOL GetConsoleFontSize(int *H, int *W);

// Compute nearest font that is about WxH size
extern int  NearestFont(int H, int W);

// Set maximal possible console size (but not maximizing the window!)
extern BOOL SetMaxConsoleSize();

// Maximize console window
extern BOOL MaximizeConsole();

// Minimize console window
extern BOOL MinimizeConsole();

// Restore console window
extern BOOL RestoreConsole();

// Show/hide console (not same as minimizing/restoring!)
extern BOOL ShowConsole(BOOL Show);

// Make console active desktop window
extern BOOL SelectConsole();

// Start console window title+border flashing
extern BOOL FlashConsole(BOOL State);

// Resize console window (Flags consists of SWP_XXX constants )
extern BOOL SetPixelConsoleSize(int X, int Y, int W, int H, ULONG Flags);

// Get console window position & size (fast)
extern BOOL GetPixelConsoleSize(int *X, int *Y, int *W, int *H);

// Set console window border width/height
extern BOOL SetConsoleBorderSize(int W, int H);

// Query console window border width/height
extern BOOL GetConsoleBorderSize(int *W, int *H);

// Set console window title string (not always works! This is a feature of PM)
extern BOOL SetConsoleTitle(char *title);

// Lock updates on desktop
BOOL LockConsoleUpdate();

// Unlock updates on desktop; nested calls are allowed
BOOL UnlockConsoleUpdate();

#ifdef __cplusplus
 }
#endif

#endif __ANSI_H__
