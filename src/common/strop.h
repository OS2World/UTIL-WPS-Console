/******************************************************************************\
|*                                                                            *|
|* String manipulation routines: header file                                  *|
|* Copyright (C) 1997 by FRIENDS software                                     *|
|* All Rights Reserved                                                        *|
|* Portability: Any OS with a C compiler                                      *|
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

#ifndef __STROP_H__
#define __STROP_H__

#include "stype.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define CR    '\r'
#define LF    '\n'
#define TAB   '\t'
#define SPACE ' '

#ifdef __EMX__
#define PATHSEP "\\/"
#else
#define PATHSEP "/"
#endif

/* Convert a character to upper/lower case */
char upCase(char Ch);
char loCase(char Ch);

/* Extract directory from full pathname */
char *extractDir(char *pathName);

/* Extract filename from full pathname */
char *extractName(char *pathName);

/* Remove spaces/tabs at the beginning of string */
char *removeStartSpaces(char * sz);
/* Remove spaces/tabs at the ending of string */
char *removeLastSpaces(char *sz);

/* Identifies first word in sz with a token from wordlist (delimted by \0) */
/* and returns its ordinal (0 = not found). Then token is removed from sz. */
int Token(char * sz, char * wordlist);

/* Returns a pointer to token number no in wordlist */
char *GetToken(char *wordlist, int no);

/* Same as atoi() but removes number and stops at first non-decimal char */
long decVal(char *s);
/* Same as decVal but works in hexadecimal */
ulong hexVal(char *s);

/* Converts a ulong to string */
char *decStr(ulong val, int nch, char *buff);
/* Same as decStr but works in hexadecimal */
char *hexStr(ulong val, int nch, char *buff);

/* Converts Val into hexadecimal character (0-F) */
char hexChar(char Val);

/* Sets string S to a string of Num characters Ch */
char *Strg(char Ch, int Num, char *S);

/* Return position of first character Ch in string S */
int strfirst(char Ch, char *S);
/* Return position of last character Ch in string S */
int strlast(char Ch, char *S);

/* Return length of first word in string (delimited by SPACE or TAB) */
int firstwordlen(char *S);

void strdel(char *S, int start, int count);

#ifdef __cplusplus
 };
#endif

#endif __STROP_H__
