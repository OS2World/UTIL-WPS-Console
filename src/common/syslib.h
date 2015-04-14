/******************************************************************************\
|*                                                                            *|
|* Operating system-related functions: header file                            *|
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

#ifndef __SYSLIB_H__
#define __SYSLIB_H__

#ifdef __cplusplus
 extern "C" {
#endif

// for debugging
#define brk __asm__("int $3")

/* Each of the following routines returns a pre-allocated pointer to char[] */
/* You should take care to free it yourself after you`re done with results  */

/* Return a pointer to raw environment */
char *Environment();

/* Return fully-qualified program pathname */
char *SourceName();

/* Return fully-qualified program path */
char *SourcePath();

#ifdef __cplusplus
 }
#endif

#endif __SYSLIB_H__
