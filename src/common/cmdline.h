/******************************************************************************\
|*                                                                            *|
|* Command-line parsing library: header file                                  *|
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

#ifndef __CMDLINE_H__
#define __CMDLINE_H__

#include "stype.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Allocate and return a pointer to a string containing original command line */
char *CmdLine(int argc, char *argv[]);

/* Parse the line passed to procedure calling either Opt or Str handlers */
void ParseCmdLine(char *Line, int (*Opt)(char *), int (*Str)(char *));

/* Return 1 if Switch in [-+], 0 otherwise. Set State to 0 if '-', 1 otherwise */
int OptState(char Switch, boolean *State);

#ifdef __cplusplus
 }
#endif

#endif __CMDLINE_H__
