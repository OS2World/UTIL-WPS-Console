/******************************************************************************\
|*                                                                            *|
|* String manipulation routines                                               *|
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strop.h"

char upCase(char Ch)
{
 if ((Ch >= 'a') && (Ch <= 'z'))
       return Ch & 0xDF;
  else return Ch;
}

char loCase(char Ch)
{
 if ((Ch >= 'A') && (Ch <= 'Z'))
       return Ch | 0x20;
  else return Ch;
}

char *extractDir(char *pathName)
{
 char *dir;
 int len = strlen(pathName);
 while ((len-- > 0) &&
        (pathName[len] != '/') &&
        (pathName[len] != '\\') &&
        (pathName[len] != ':')) ;
 dir = malloc(++len + 1);
 if (dir)
 {
  strncpy(dir, pathName, len);
  dir[len] = 0;
 }
 return dir;
}

char *extractName(char *pathName)
{
 char *name;
 int len = strlen(pathName);
 int slen = len;
 while ((len > 0) &&
        (pathName[len-1] != '/') &&
        (pathName[len-1] != '\\') &&
        (pathName[len-1] != ':')) len--;
 name = malloc(slen - len + 2);
 if (name)
  strncpy(name, &pathName[len], slen - len + 1);
 return name;
}

char *removeStartSpaces(char *sz)
{
 int i = 0;
 char ch;
 while (((ch = sz[i]) != 0) &&
        ((ch == SPACE) || (ch == TAB) || (ch == LF) || (ch == CR))) i++;
 if (i != 0) strcpy(sz, &sz[i]);
 return sz;
}

char *removeLastSpaces(char *sz)
{
 int i = strlen(sz) - 1;
 char ch;
 while ((i >= 0) &&
        (((ch = sz[i]) == SPACE) || (ch == TAB) ||
          (ch == LF) || (ch == CR))) sz[i--] = 0;
 return sz;
}

int Token(char *sz, char *wordlist)
{
 int sp = 0;
 int tp = 0;
 int tl = 0;
 int wn = 1;

 while ((wordlist[tl] | wordlist[tl + 1]) != 0) tl++;

 while (tp <= tl)
  {
   if (wordlist[tp] == 0)
    {
     strcpy(sz, &sz[sp]);
     return wn;
    }
   if ((sz[sp] != 0) && (loCase(sz[sp]) == loCase(wordlist[tp])))
         { sp++; tp++; }
    else {
          sp = 0; wn++;
          while ((wordlist[tp++] != 0) && (tp < tl)) ;
         }
  }
 return 0;
}

char *GetToken(char *wordlist, int no)
{
 int i;
 int sp = 0;

 for (i = 1; i < no; i++)
  while (wordlist[sp++] != 0) ;

 return &wordlist[sp];
}

long decVal(char *s)
{
 long  Value = 0;
 char  ch;
 int   sl = strlen(s);
 int   sign = 0;

 switch (s[0])
 {
  case '-': sign = 1;
  case '+': strncpy(s, &s[1], sl--); break;
 }
 while (((ch = s[0]) >= '0') && (ch <= '9'))
  {
   Value = Value * 10 + (ch - '0');
   strncpy(s, &s[1], sl--);
  }
 return (sign ? -Value : Value);
}

char *decStr(ulong val, int nch, char *buff)
{
 char   fch = 1;
 ldiv_t dr;
 while (nch && (val | fch))
  {
   dr = ldiv(val, 10);
   buff[--nch] = (char)(dr.rem + '0');
   val = dr.quot;
   fch = 0;
  }
 return &buff[nch];
}

char hexChar(char Val)
{
 return (Val > 9? Val - 10 + 'A' : Val + '0');
}

ulong hexVal(char *s)
{
 ulong Value = 0;
 char  ch;
 int   sl = strlen(s);

 while ((((ch = upCase(s[0])) >= '0') && (ch <= '9')) ||
        ((ch >= 'A') && (ch <= 'F')))
  {
   Value = (Value << 4) + (ch > '9'? ch - 'A' + 10 : ch - '0');
   strncpy(s, &s[1], sl--);
  }
 return Value;
}

char *hexStr(ulong val, int nch, char *buff)
{
 char   fch = 1;

 while (nch && (val | fch))
  {
   buff[--nch] = hexChar(val & 0x0F);
   val >>= 4;
   fch = 0;
  }
 return &buff[nch];
}

char *Strg(char Ch, int Num, char *S)
{
 memset(S, Ch, Num);
 S[Num] = 0;
 return S;
}

int strfirst(char Ch, char *S)
{
 int pos = 0;
 while ((S[pos]) && (S[pos] != Ch)) pos++;
 if (!S[pos]) return -1; else return pos;
}

int strlast(char Ch, char *S)
{
 int pos = strlen(S) - 1;
 while ((pos >= 0) && (S[pos] != Ch)) pos--;
 return pos;
}

int firstwordlen(char *S)
{
 int pos = 0;
 while ((S[pos]) && (S[pos] != SPACE) && (S[pos] != TAB)) pos++;
 return pos;
}

void strdel(char *S, int start, int count)
{
 int len = strlen(S);
 if (start < len)
 {
  if (start + count < len)
   strcpy((char *)&S[start], (char *)&S[start + count]);
  else
   S[start] = 0;
 }
}
