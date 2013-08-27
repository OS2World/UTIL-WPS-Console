# Makefile for GCC + GNU Make

INCDIR = ./common
EMXRT = 0
FINAL = 1
DEBUG = 0

include makeinit

ifeq ($(EMXRT),1)
PREFIX = e
else
PREFIX = s
endif

.PHONY:		all

all:		Console.exe CWRP.exe

Console.exe:	Console.obj strop.obj cmdline.obj \
		os2con.obj syslib.obj Console.def

CWRP.exe:	CWRP.obj CWRP.def

Console.obj:	strop.h os2con.h

CWRP.obj:	CWRP.h

os2con.obj:	os2con.h

clean:
		-@rm *.obj *.lib 1>nul 2>nul

distr:
		mv Console.exe ../$(PREFIX)Console.exe
		mv CWRP.exe ../$(PREFIX)CWRP.exe
