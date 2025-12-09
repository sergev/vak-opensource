
  This directory contains the sources of the visual screen and keyboard
  handling library for MSDOS and Unix.  It also supports menu system,
  dialog boxes etc.

  File		Description
  €€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€
  SCRDOS.CC	- low level routines for MSDOS
  SCREEN.CC	- low level routines for Unix
  TTY.CC	- terminal driver modes handling for Unix
  KEY.CC	- keyboard handling for Unix
  MENU.CC	- menu subsystem
  POPUP.CC	- popup windows subpackage
  DIALOG.CC	- dialog windows subpackage
  BOX.CC	- dialog boxes support
  TERMCAP.CC	- terminal database (termcap) parser for Unix
  TERMLIB.CC	- termlib compatibility for Unix
  TERMGOTO.CC	- direct cursor addressing support (for termcap)
  CURSES.CC	- curses compatibility
  HV.CC		- an example of using the package: hexadecimal file viewer
  HELLO.CC	- a sample program: hello, world
  HELLCURS.C	- an example of curses compatibility
  CTYPE.C	- cyrillisation file
  SCREEN.H	- the main definitions
  MENU.H	- menu subsystem definitions
  POPUP.H	- popup windows definitions
  DIALOG.H	- dialog windows definitions
  TERMCAP.H	- termcap subsystem definitions
  EXTERN.H	- common defines
  EXTDOS.H	- MSDOS compatibility defines
  CURSES.H	- curses compatimility definitions
  CTYPE.H	- definitions for cylillisation
  MAKEFILE	- compilation management file for MSDOS
  MAKEFILE.UNX	- compilation management file for Unix
  CURSES-R.TXT	- description of curses functions in Russian
  README	- this file
  README-R	- the same in Russian
  ___________________________________
  Copyright (C) 1993-1995 Cronyx Ltd.
  $Id: readme.txt,v 1.1 2002/07/30 13:37:01 rik Exp $