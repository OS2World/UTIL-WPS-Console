/******************************************************************************\
|*                                                                            *|
|* Command-line parsing library                                               *|
|* Copyright (C) 1997 by FRIENDS software                                     *|
|* All Rights Reserved                                                        *|
|* Portability: universal                                                     *|
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
#include <os2.h>
#include <string.h>
#include <malloc.h>
#include "strop.h"
#include "cmdline.h"

char *CmdLine(int argc, char *argv[])
{
 char *Line, *Cmd;
 PTIB tib;
 PPIB pib;

 if (DosGetInfoBlocks(&tib, &pib))
  return "";

 Cmd = pib->pib_pchcmd + strlen(pib->pib_pchcmd) + 1;
 Line = malloc(strlen(Cmd) + 1);
 strcpy(Line, Cmd);
 return Line;
}

void ParseCmdLine(char *Line, int (*Opt)(char *), int (*Str)(char *))
{
 int len;
 char *tmp;
 if (!Line) return;
 len = strlen(Line);
 memcpy(tmp = malloc(len), Line, len + 1);
 while (tmp[0])
 {
  int dc;
  dc = 0;
  while ((tmp[dc]) && ((tmp[dc] == SPACE) || (tmp[dc] == TAB))) dc++;
  strdel(tmp, 0, dc);
  if (tmp[0] == '-')
  { if (&Opt && tmp[0]) dc = Opt(tmp); else dc = 1; }
  else
  { if (&Str && tmp[0]) dc = Str(tmp); else dc = 1; }
  strdel(tmp, 0, dc);
 }
 free(tmp);
}

int OptState(char Switch, boolean *State)
{
 switch (Switch)
 {
  case '-':
   *State = FALSE; return 1;
  case '+':
   *State = TRUE; return 1;
  default:
   *State = TRUE; return 0;
 }
}
