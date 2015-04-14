/******************************************************************************\
|*                                                                            *|
|* Operating system-related functions                                         *|
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

#define INCL_DOSPROCESS
#include <os2.h>
#include <string.h>
#include <malloc.h>
#include "strop.h"
#include "syslib.h"

char *Environment()
{
 PTIB tib;
 PPIB pib;
 DosGetInfoBlocks(&tib, &pib);
 return pib->pib_pchenv;
}

char *SourceName()
{
 int i;
 char *RetS;
 char *Env = Environment();

 do Env++; while ((Env[-1] != 0) || (Env[0] != 0));
 do Env++; while (Env[0] != 0);
 Env++;
 i = strlen(Env);
 RetS = malloc(i);
 if (RetS) strcpy(RetS, Env);
 return RetS;
}

char *SourcePath()
{
 int i;
 char *RetS;
 char *Env = Environment();

 do Env++; while ((Env[-1] != 0) || (Env[0] != 0));
 do Env++; while (Env[0] != 0);

 Env++; RetS = Env + strlen(Env);
 while ((RetS > Env) && (strlast(RetS[-1], PATHSEP) < 0)) RetS--;

 i = RetS - Env;
 RetS = malloc(i + 1);
 if (RetS)
 {
  strncpy(RetS, Env, i);
  RetS[i] = 0;
 }
 return RetS;
}
