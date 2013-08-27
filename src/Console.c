/******************************************************************************\
|*                                                                            *|
|* Console window manager                                                     *|
|* Copyright (C) 1997 by FRIENDS software                                     *|
|* All Rights Reserved                                                        *|
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
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <malloc.h>
#include <process.h>
#include <string.h>
#include <alloca.h>
#include <io.h>
#include "syslib.h"
#include "cmdline.h"
#include "strop.h"
#include "os2con.h"

char *version = "0.1.0";

char *os2iniMaximize = "fMaximize";	// Entry with "maximize window" flag
char *os2iniShape = "sInitialShape";	// Entry with window initial shape
char *os2iniFontSize;			// Entry with VIO font size

// Shield:sInitialShape structure
// Does somebody know what those fields means?
struct
{
 USHORT unknown1;
 USHORT WindowH;
 USHORT WindowW;
 USHORT WindowY;
 USHORT WindowX;
 USHORT unknown2;
 USHORT unknown3;
 USHORT unknown4;
 USHORT unknown5;
} DefShape;

struct
{
 signed char W;
 signed char H;
} Font, DefFont;

struct
{
 int W;
 int H;
} Console, Border;

struct
{
 int X,Y,W,H;
} ConsolePos;

// Command-line driven flags
boolean fAutoSize = TRUE;
boolean fConsoleSize = FALSE;
boolean fQueryCommand = FALSE;
boolean fDelay = FALSE;
boolean fExplicitPos = FALSE;
boolean fExplicitSize = FALSE;
boolean fFontSet = FALSE;
boolean fNewSession = FALSE;
boolean fQuiet = FALSE;
boolean fResizeWindowToMax = FALSE;
boolean fMaximizeConsole = FALSE;
boolean fRestoreConsole = FALSE;
boolean fMinimizeConsole = FALSE;
boolean fShowConsole = FALSE;
boolean fHideConsole = FALSE;
boolean fForegroundConsole = FALSE;
boolean fSetBorder = FALSE;
boolean fFlashWindow = FALSE;
boolean fSetFlash = FALSE;

USHORT fMaximize;
STARTDATA sd;
char *cmdLine;
char *progname = NULL;
char *progargs = NULL;
char *progtitle = NULL;
int optCount;

HMTX ConsoleSem;

static long INIpresent = 0;
#define iniShape    0x00000001
#define iniMaximize 0x00000002
#define iniFontsize 0x00000004

static void ReleaseINI(void)
{
 DosReleaseMutexSem(ConsoleSem);
 DosCloseMutexSem(ConsoleSem);
}

void doHeader()
{
 static char AlreadyPrinted = 0;

 if (fQuiet || AlreadyPrinted) return;
 AlreadyPrinted = 1;
 SetColor(colWHITE, colSAME);
 printf("Ú[ Console ]ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ[ Version %s ]¿\n", version);
 printf("Ã Copyright (C) 1997 by FRIENDS software Ä All Rights Reserved Ù\n");
}

void doHelp()
{
 char *HelpText[] =
 {
  "³ This utility will start a windowed session (DOS or OS/2) optionaly",
  "³ placing window at a specific location, changing its font and resizing",
  "³ You can place legal switches in the CONSOLE environment variable",
  "Ã Usage: Console {options} {program} {...program options...}",
  "³  Some options works only when starting a new session (these options are",
  "³  marked with a /N/ sign), some applies to current window, these are marked",
  "³  with /C/. Those which applies to both cases are marked with /CN/.",
  "Ã Available options:",
  "Ã -b{w,h} /CN/",
  "³  Set window border [W]idth and [H]eight",
  "Ã -f{h,w} /CN/",
  "³  Set window font [H]eight and [W]idth (enables -or+)",
  "Ã -o{a|b|c|d|f|m|n|r|s|v|x}{+|-}",
  "³  Enable (+) or disable (-) one of following [O]ptions:",
  "³  a /CN/ - Turn on (+)/off (-) [A]NSI colored output",
  "³  b /CN/ - Start a [B]ackground session (+) or foreground (-)",
  "³  c /N/  - auto[C]lose on termination (+) or leave window until user closes it",
  "³  d /N/  - Enable (+) or disable (-) a small [D]elay after running command",
  "³           Use this if you experience problems with -p starting a new session",
  "³  f /CN/ - Start window title [F]lashing (+) or stop flashing (-)",
  "³  m /CN/ - Start session [M]inimized (+) or non-minimized (-)",
  "³  n /N/  - Start a [N]ew session (+) or change same window (-)",
  "³  r /C/  - [R]esize window to max possible size (+) or leave as-is (-)",
  "³  s /N/  - auto[S]ize window: compute the size of new window based on known",
  "³           console window size and console font size (overriden by -p,,#,#)",
  "³  v /CN/ - Start a [V]isible console window (+) or invisible (-)",
  "³  x /CN/ - Start session ma[X]imized (+) or non-maximized (-)",
  "Ã -p{{x}{,{y}{,{w}{,h}}}} /CN/",
  "³  Set window position (X, Y) and size (W, H) (in pixels)",
  "³  Defining width and height implies -or-",
  "Ã -s{w,h} /CN/",
  "³  re[S]ize window using \"MODE W,H\" command. (enables -or+ and -wo)",
  "Ã -q /CN/",
  "³  Display a sample command line to start a window with current",
  "³  size/shape (possibly modified by command-line options)",
  "Ã -t\"...\" /CN/",
  "³  Set console [T]itle",
  "Ã -w{a|d|o} /N/",
  "³  [W]indow type: [D]OS/[O]S2/[A]utodetect; default = OS/2",
  "Ã´ Default: -oa+ -oc+ -od- -of- -om- -on- -or- -os+ -ov+ -ox- -wo",
  "Ã´ Example: Console -wo -f18,10 -p200,100 -on t-mail.exe",
  "Ã´          Console -s40,25 -p,,300,100 -f8,8",
  "Ã´          Console -s60,20 -p10,10,488,186 -f8,8 -on -oc- -ob",
  NULL
 };

 int i, linecnt, consoleH;

 GetConsoleSize(&i, &consoleH);
 linecnt = consoleH - 2;

 ReleaseINI();
 doHeader();
 for (i = 0; i < 999; i++)
 {
  char *Line = HelpText[i];
  if (Line == NULL) break;
  switch (Line[0])
  {
   case '³':
    if (Line[2] == ' ') SetColor(colLBLUE, colSAME); else SetColor(colYELLOW, colSAME);
    break;
   case 'Ã':
    if (Line[1] == ' ') SetColor(colLCYAN, colSAME); else SetColor(colBLUE, colSAME);
    break;
  }
  printf("%s\n", Line);
  if ((--linecnt <= 1) && (_isterm(1)) && (_isterm(0)))
  {
   SetColor(colWHITE, colSAME);
   printf("-- [press any key] --");
   fflush(stdout);
   getch();
   printf("\r                     \r");
   linecnt = consoleH;
  }
 }
 fflush(stdout);
}

void Stop(int code, char *msg)
{
 int cl;
 int mcl;

 doHeader();
 switch (code)
 {
  case 1:
   doHelp();
   break;
  case 2:
   cl = strlen(cmdLine);
   mcl = strlen(msg);
   cmdLine[cl - mcl] = 0;
   SetColor(colYELLOW, colSAME); printf("Ã Error in command-line option:\nÃ %s", cmdLine);
   SetColor(colLRED, colSAME); printf("%s\n", msg);
   break;
  case 3:
   SetColor(colLRED, colSAME); 
   printf("Cannot open mutex semaphore \\SEM32\\CONSOLE");
   break;
 }
 SetColor(colBLUE, colSAME);
 printf("À´ Done\n");
 SetColor(colCYAN, colSAME);
 exit(code);
}

int isdigit(char c)
{
 return (((c >= '0') && (c <= '9')) || (c == '+') || (c == '-'));
}

void doInitialize()
{
 ULONG Sz;
 ULONG bsz = 0;

 // Initialize variables
 memset(&sd, 0, sizeof(sd));
 Font.W = -1;
 Console.W = -1;
 sd.PgmControl = SSF_CONTROL_SETPOS;
 sd.Related = SSF_RELATED_INDEPENDENT;
 sd.SessionType = SSF_TYPE_WINDOWABLEVIO;

 // Look inside OS2.INI and find the appropiate "~Font Size..." entry
 if (PrfQueryProfileSize(HINI_USERPROFILE, "Shield", NULL, &bsz))
 {
  PSZ buff = alloca(bsz);
  if (PrfQueryProfileData(HINI_USERPROFILE, "Shield", NULL, buff, &bsz))
  {
   char *name = buff;
   int len;
   while (*name)
   {
    len = strlen(name) + 1;
    if (strstr(name, "..."))
     if ((os2iniFontSize = malloc(len)))
      strcpy(os2iniFontSize, name);
    name += len;
   }
  }
 }
 if (os2iniFontSize == NULL)
  os2iniFontSize = "~Font Size...";

 // Query default window creation attributes
 Sz = sizeof(DefShape);
 if (PrfQueryProfileData(HINI_USERPROFILE, "Shield", os2iniShape,
     (PVOID)&DefShape, &Sz))
 {
  INIpresent |= iniShape;
  ConsolePos.X = DefShape.WindowX;
  ConsolePos.Y = DefShape.WindowY;
  ConsolePos.W = DefShape.WindowW;
  ConsolePos.H = DefShape.WindowH;
 }

 Sz = sizeof(fMaximize);
 if (PrfQueryProfileData(HINI_USERPROFILE, "Shield", os2iniMaximize,
     (PVOID)&fMaximize, &Sz))
 {
  INIpresent |= iniMaximize;
  if (fMaximize)
   sd.PgmControl |= SSF_CONTROL_MAXIMIZE;
 }

 Sz = sizeof(DefFont);
 if (PrfQueryProfileData(HINI_USERPROFILE, "Shield", os2iniFontSize,
     (PVOID)&DefFont, &Sz))
 {
  INIpresent |= iniFontsize;
  Font = DefFont;
 }

 // Query console size, console border size
 GetConsoleSize(&Console.W, &Console.H);
 GetConsoleBorderSize(&Border.W, &Border.H);
}

int OptH(char *Opt)
{
 int optLen = 2;
 boolean State;

 optCount++;
 switch (Opt[1])
 {
  case 'b':
   fSetBorder = TRUE;
   strdel(Opt, 0, 2); Border.W = decVal(Opt);
   if (Opt[0] != ',') Stop(2, Opt);
   strdel(Opt, 0, 1); Border.H = decVal(Opt);
   optLen = 0;
   break;
  case 'f':
   fFontSet = TRUE;
   fResizeWindowToMax = TRUE;
   strdel(Opt, 0, 2);
   if (isdigit(Opt[0]))
   {
    Font.H = decVal(Opt);
    if (Opt[0] != ',') Stop(2, Opt);
    strdel(Opt, 0, 1);
    Font.W = decVal(Opt);
   }
   else
   {
    int fw,fh;
    if (GetConsoleFontSize(&fh, &fw))
    { Font.H = fh; Font.W = fw; }
   }
   optLen = 0;
   break;
  case 'o':
   optLen += OptState(Opt[3], &State) + 1;
   switch (Opt[2])
   {
    case 'a': ANSI = State;
              break;
    case 'd': fDelay = State;
              break;
    case 'r': fResizeWindowToMax = State;
              break;
    case 'f': if (State)
              {
               fFlashWindow = TRUE;
               fSetFlash = TRUE;
              }
              else
              {
               fFlashWindow = FALSE;
               fSetFlash = TRUE;
              }
              break;
    case 'b': if (State)
              {
               sd.FgBg = SSF_FGBG_BACK;
               fForegroundConsole = FALSE;
              }
              else
              {
               sd.FgBg = SSF_FGBG_FORE;
               fForegroundConsole = TRUE;
              }
              break;
    case 'c': if (State)
               sd.PgmControl &= ~SSF_CONTROL_NOAUTOCLOSE;
              else
               sd.PgmControl |= SSF_CONTROL_NOAUTOCLOSE;
              break;
    case 'v': if (State)
              {
               sd.PgmControl &= ~SSF_CONTROL_INVISIBLE;
               fHideConsole = FALSE;
               fShowConsole = TRUE;
              }
              else
              {
               sd.PgmControl |= SSF_CONTROL_INVISIBLE;
               fHideConsole = TRUE;
               fShowConsole = FALSE;
              }
              break;
    case 'x': if (State)
              {
               sd.PgmControl |= SSF_CONTROL_MAXIMIZE;
               sd.PgmControl &= ~SSF_CONTROL_MINIMIZE;
               fAutoSize = FALSE;
               fMaximizeConsole = TRUE;
               fRestoreConsole = FALSE;
              }
              else
              {
               sd.PgmControl &= ~SSF_CONTROL_MAXIMIZE;
               fMaximizeConsole = FALSE;
               fRestoreConsole = TRUE;
              }
              break;
    case 'm': if (State)
              {
               sd.PgmControl |= SSF_CONTROL_MINIMIZE;
               sd.PgmControl &= ~SSF_CONTROL_MAXIMIZE;
               fMinimizeConsole = TRUE;
               fRestoreConsole = FALSE;
              }
              else
              {
               sd.PgmControl &= ~SSF_CONTROL_MINIMIZE;
               fMinimizeConsole = FALSE;
               fRestoreConsole = TRUE;
              }
              break;
    case 'n': fNewSession = State;
              break;
    case 's': fAutoSize = State;
              break;
     default: Stop(2, Opt);
   }
   break;
  case 'p':
   strdel(Opt, 0, 2);
   if (isdigit(Opt[0]))
   { ConsolePos.X = decVal(Opt); fExplicitPos = TRUE; }
   else if (Opt[0] != ',')
         GetPixelConsoleSize(&ConsolePos.X, &ConsolePos.Y, &ConsolePos.W, &ConsolePos.H);
   if (Opt[0] == ',')
   {
    strdel(Opt, 0, 1);
    if (isdigit(Opt[0]))
    { ConsolePos.Y = decVal(Opt); fExplicitPos = TRUE; }
    if (Opt[0] == ',')
    {
     strdel(Opt, 0, 1);
     if (isdigit(Opt[0]))
     { ConsolePos.W = decVal(Opt); fExplicitSize = TRUE; }
     if (Opt[0] == ',')
     {
      strdel(Opt, 0, 1);
      if (isdigit(Opt[0]))
      { ConsolePos.H = decVal(Opt); fExplicitSize = TRUE; }
     }
    }
   }
   optLen = 0;
   break;
  case 's':
   fConsoleSize = TRUE;
   fResizeWindowToMax = TRUE;
   strdel(Opt, 0, 2); Console.W = decVal(Opt);
   if (Opt[0] != ',') Stop(2, Opt);
   strdel(Opt, 0, 1); Console.H = decVal(Opt);
   optLen = 0;
   break;
  case 't':
  {
   int i = 3;
   int l = strlen(Opt);
   if (Opt[2] != '"') Stop(2, Opt);
   while (i < l && Opt[i] != '"') i++;
   if (Opt[i] != '"') Stop(2, Opt);
   progtitle = malloc(i - 2);
   strncpy(progtitle, &Opt[3], i - 3);
   progtitle[i - 2] = 0;
   optLen = i + 1;
   break;
  }
  case 'q':
   fQueryCommand = TRUE;
   break;
  case 'w':
   switch (Opt[2])
   {
    case 'a':
     sd.SessionType = SSF_TYPE_DEFAULT;
     break;
    case 'd':
     sd.SessionType = SSF_TYPE_WINDOWEDVDM;
     break;
    case 'o':
     sd.SessionType = SSF_TYPE_WINDOWABLEVIO;
     break;
    default:
     Stop(2, Opt);
   }
   optLen++;
   break;
  case '?':
  case 'h':
   Stop(1, NULL);
  default:
   Stop(2, Opt);
 }
 if ((Opt[optLen] != 0) && (Opt[optLen] != SPACE) && (Opt[optLen] != TAB))
  Stop(2, Opt);
 return optLen;
}

int StrH(char *Str)
{
 int len;
 int sz;

 optCount++;
 len = firstwordlen(Str);
 sz = strlen(Str) - len;
 progname = malloc(len + 1);
 Str[len] = 0;
 strcpy(progname, Str);
 if (!strcmp("*", progname)) progname[0] = 0;
 if (sz > 0)
 {
  progargs = malloc(sz + 1);
  strcpy(progargs, (char *)&Str[len + 1]);
 }
 return 99999;
}

void doShowCommand()
{
 char ch;

 SetColor(colLCYAN, colSAME);
 printf("Ã Effective command line:\nÃ´ ");
 SetColor(colWHITE, colSAME);
 printf("Console -p%d,%d,%d,%d -b%d,%d -f%d,%d -s%d,%d",
        ConsolePos.X, ConsolePos.Y, ConsolePos.W, ConsolePos.H,
        Border.W, Border.H, Font.H, Font.W, Console.W, Console.H);

 if (fNewSession)
 {
  switch (sd.SessionType)
  {
   case SSF_TYPE_DEFAULT:       ch = 'a'; break;
   case SSF_TYPE_WINDOWEDVDM:   ch = 'd'; break;
   case SSF_TYPE_WINDOWABLEVIO: ch = 'o'; break;
   default: ch = '*';
  }
  printf("-t%c ", ch);

  (sd.PgmControl & SSF_CONTROL_NOAUTOCLOSE) ? (ch = '-') : (ch = '+');
  printf(" -oc%c", ch);
 }

 if (fShowConsole || fHideConsole)
 {
  (fShowConsole) ? (ch = '-') : (ch = '+');
  printf(" -ov%c", ch);
 }
 if (fMaximizeConsole)
  printf(" -ox");
 if (fMinimizeConsole)
  printf(" -om");
 if (fRestoreConsole)
  printf(" -ox-");
 if (fForegroundConsole)
  printf(" -of");
 else if (sd.FgBg == SSF_FGBG_BACK)
  printf(" -ob");
 if (fDelay)
  printf(" -od");

 printf("\n");
}

void doAutoSize()
{
 int i;

 i = NearestFont(Font.H, Font.W);
 if (i >= 0)
 {
  Font.W = FontList[i].cx;
  Font.H = FontList[i].cy;
 }

 // If position is explicitely defined and size is not, auto compute window size
 if ((!fExplicitSize) && (Font.W > 0) && (Font.H > 0) && fAutoSize)
 {
  int bw = WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
  int bh = WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER);
  int th = WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);

  if (Console.W == -1)
  { Console.W = 80; Console.H = 25; }
  ConsolePos.W = bw * 2 + Console.W * Font.W;
  ConsolePos.H = th + bh * 2 + Console.H * Font.H;
 }
}

void doNewSession()
{
 char objbuff[255];
 ULONG sid;
 PID pid;
 USHORT fMax;
 long Sz;
 PVOID p;

 doAutoSize();
 if (fExplicitSize || fAutoSize) sd.PgmControl &= ~SSF_CONTROL_MAXIMIZE;

 // Activate our window if sd.FgBg = SSF_FGBG_FORE;
 if (sd.FgBg == SSF_FGBG_FORE)
  SelectConsole();

 // Fill in the STARTDATA structure
 sd.Length = sizeof(sd);
 sd.PgmName = progname;
 sd.PgmInputs = progargs;
 sd.PgmTitle = progtitle;
 sd.InheritOpt = SSF_INHERTOPT_PARENT;
 sd.ObjectBuffer = (PVOID)&objbuff;
 sd.ObjectBuffLen = sizeof(objbuff);
 sd.InitXPos = ConsolePos.X;
 sd.InitYPos = ConsolePos.Y;
 sd.InitXSize = ConsolePos.W;
 sd.InitYSize = ConsolePos.H;

 // Delete the shitty sInitialShape since it has a messy structure
 PrfWriteProfileData(HINI_USERPROFILE, "Shield", os2iniShape,
     NULL, 0);
 if (sd.PgmControl & SSF_CONTROL_MAXIMIZE) fMax = 1; else fMax = 0;
 PrfWriteProfileData(HINI_USERPROFILE, "Shield", os2iniMaximize,
     (PVOID)&fMax, sizeof(fMax));
 PrfWriteProfileData(HINI_USERPROFILE, "Shield", os2iniFontSize,
     (PVOID)&Font, sizeof(Font));

 switch (DosStartSession(&sd, &sid, &pid))
 {
  case NO_ERROR:
   SetColor(colLBLUE, colSAME);
   printf("Ã Session successfully started\n");
   break;
  case ERROR_SMG_START_IN_BACKGROUND:
  {
   SetColor(colLRED, colSAME);
   printf("Ã Session started in background\n");
   break;
  }
  default:
   SetColor(colLRED, colSAME);
   printf("Ã Failed because of \"%s\"\n", (char *)&objbuff);
 };
 if (fDelay) DosSleep(500);


 // Restore INI state
 Sz = sizeof(DefShape);
 if (INIpresent & iniShape) p = (PVOID)&DefShape; else p = NULL;
 PrfWriteProfileData(HINI_USERPROFILE, "Shield", os2iniShape, p, Sz);
 Sz = sizeof(fMaximize);
 if (INIpresent & iniMaximize) p = (PVOID)&fMaximize; else p = NULL;
 PrfWriteProfileData(HINI_USERPROFILE, "Shield", os2iniMaximize, p, Sz);
 Sz = sizeof(DefFont);
 if (INIpresent & iniFontsize) p = (PVOID)&DefFont; else p = NULL;
 PrfWriteProfileData(HINI_USERPROFILE, "Shield", os2iniFontSize, p, Sz);
}

void doSameSession()
{
 char *arg;
 RESULTCODES rc;
 int i,j;

 if (progtitle)
  SetConsoleTitle(progtitle);

 if (fForegroundConsole)
  SelectConsole();

 if (fConsoleSize)
  SetConsoleSize(Console.W, Console.H, colSAME);

 if (fFontSet)
 {
  RECTL r;
  LockConsoleUpdate();
  SetConsoleFontSize(Font.H, Font.W);
  if (fResizeWindowToMax && !fExplicitSize)
   SetMaxConsoleSize();

  // Workaround: NPS WPS does not redraw window shadow when window grows :-(
  GetPixelConsoleSize((int *)&r.xLeft, (int *)&r.yBottom, (int *)&r.xRight, (int *)&r.yTop);
  r.xRight += r.xLeft + 8; r.yTop += r.yBottom + 8;
  r.xLeft -= 8; r.yBottom -= 8;
  WinInvalidateRect(HWND_DESKTOP, &r, FALSE);

  UnlockConsoleUpdate();
 } else
 if (fResizeWindowToMax && !fExplicitSize)
  SetMaxConsoleSize();

 if (fSetBorder)
  SetConsoleBorderSize(Border.W, Border.H);
 if (fSetFlash)
  FlashConsole(fFlashWindow);

 if (fMinimizeConsole)
  MinimizeConsole();
 else if (fMaximizeConsole)
  MaximizeConsole();

 if (GetWindowHWND())
 {
  if ((fExplicitPos || fExplicitSize))
   SetPixelConsoleSize(ConsolePos.X, ConsolePos.Y, ConsolePos.W, ConsolePos.H,
    (fExplicitPos ? SWP_MOVE : 0) | (fExplicitSize ? SWP_SIZE : 0));
  else if (fMaximizeConsole)
   SetPixelConsoleSize(ConsolePos.X, ConsolePos.Y, 0, 0, SWP_MOVE);
 }

 if (fHideConsole)
  ShowConsole(FALSE);
 else if (fShowConsole)
  ShowConsole(TRUE);

 if (fRestoreConsole)
  RestoreConsole();

 if (!progname) return;
 if (!progname[0]) progname = getenv("COMSPEC");
 if (progname) i = strlen(progname); else i = 0;
 j = i; i++;
 if (progargs) i+= strlen(progargs);
 arg = malloc(i);

 if (progname) strcpy(arg, progname); else arg[0] = 0;
 strcat(arg, "*");
 if (progargs) strcat(arg, progargs);
 arg[j] = 0;

 ReleaseINI();
 if (DosExecPgm(NULL, 0, EXEC_SYNC, arg, NULL, &rc, progname))
 {
  SetColor(colLRED, colSAME);
  printf("Error running command %s\n", progname);
  if (_isterm(1)) getch();
  return;
 }
}

int main (int argc, char *argv[])
{
 if (!_isterm(1)) ANSI = 0;

 if (DosCreateMutexSem("\\SEM32\\CONSOLE.RUN", &ConsoleSem, DC_SEM_SHARED, TRUE) != NO_ERROR)
 {
  if (DosOpenMutexSem("\\SEM32\\CONSOLE.RUN", &ConsoleSem) != NO_ERROR)
   Stop(3, NULL);
  if (DosRequestMutexSem(ConsoleSem, SEM_INDEFINITE_WAIT) != NO_ERROR)
   Stop(3, NULL);
 }
 atexit(ReleaseINI);

 doInitialize();

 ParseCmdLine(getenv("CONSOLE"), OptH, StrH);
 optCount = 0;
 cmdLine = CmdLine(argc, argv);
 ParseCmdLine(cmdLine, OptH, StrH);
 free(cmdLine);

 if (!fNewSession && (optCount > 0) && !fQueryCommand)
  fQuiet = TRUE;

 doHeader();

 if (fQueryCommand)
  doShowCommand();

 if ((fConsoleSize || fSetBorder || (fSetFlash && fFlashWindow)) &&
     fNewSession && (sd.SessionType == SSF_TYPE_WINDOWABLEVIO))
 {
  char *p,*n;
  int nl = 64;
  if (progname) nl += strlen(progname);
  if (progargs) nl += strlen(progargs);
  p = malloc(nl);
  sprintf(p, "-s%d,%d ", Console.W, Console.H);
  if (fSetBorder)
   sprintf(&p[strlen(p)], "-b%d,%d ", Border.W, Border.H);
  if (fSetFlash && fFlashWindow)
   strcat(&p[strlen(p)], "-of ");
  if (fExplicitSize)
   sprintf(&p[strlen(p)], "-p,,%d,%d ", ConsolePos.W, ConsolePos.H);
  if ((sd.PgmControl & SSF_CONTROL_INVISIBLE) == 0)
  {
   strcat(&p[strlen(p)], "-ov ");
   sd.PgmControl |= SSF_CONTROL_INVISIBLE;
  }
  if (!progname || !progname[0]) n = getenv("COMSPEC"); else n = progname;
  sprintf(&p[strlen(p)], "%s ", n);
  if (progargs) strcat(p, progargs);
  if (progname) free(progname);
  progname = SourceName();
  progargs = p;
 }

 if (!optCount)
  doHelp();
 else
 {
  if (fNewSession)
   doNewSession();
  else
   doSameSession();
 }

 if (!fQuiet)
 {
  SetColor(colBLUE, colSAME);
  printf("À´ Done\n");
  SetColor(colCYAN, colSAME);
 }
 return 0;
}
