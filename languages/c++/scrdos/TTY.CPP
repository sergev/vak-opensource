//
// Terminal device handling functions for Unix.
//
// Copyright (C) 1994 Cronyx Ltd.
// Author: Serge Vakulenko, <vak@zebub.msk.su>
//
// This software is distributed with NO WARRANTIES, not even the implied
// warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Authors grant any other persons or organisations permission to use
// or modify this software as long as this message is kept with the software,
// all derivative works or modified versions.
//
// $Id: tty.cpp,v 1.3 2006/04/19 17:26:54 ly Exp $
//

extern "C" {
#ifdef TERMIOS
#include <sys/termios.h>
#define TERMIO
#define termio termios
#ifdef sun
#define TCGETA  TCGETS
#define TCSETA  TCSETS
#define TCSETAW TCSETSW
#else
#ifndef linux
#include <sys/ioctl.h>
#define TCGETA  TIOCGETA
#define TCSETA  TIOCSETA
#define TCSETAW TIOCSETAW
#endif
#endif
#ifndef OLCUC
#define OLCUC 0
#endif
#ifndef IUCLC
#define IUCLC 0
#endif
#ifndef OCRNL
#define OCRNL 0
#endif
#ifndef XCASE
#define XCASE 0
#endif
#else
#ifdef TERMIO
#include <termio.h>
#define tcgetattr(fd, addr)      ioctl(fd, TCGETA, (char *)addr)
#define tcsetattr(fd, opt, addr) ioctl(fd, TCSETAW, (char *)addr)
#else
#include <sgtty.h>
#endif
#endif
};

#include "screen.h"

const int CHANNEL = 2; // output file descriptor
const int NOCHAR  = 0;

struct TtyPrivate {
#if defined(TERMIO) || defined(TERMIOS)
    struct termio oldtio, newtio;
#else
    struct sgttyb tty;
    int ttyflags;
#ifdef TIOCSETC
    struct tchars oldtchars, newtchars;
#endif
#endif

#ifdef NEEDLITOUT
    int oldlocal, newlocal;
#endif

#ifdef TIOCSLTC
    struct ltchars oldchars, newchars;
#endif
};

void ScreenImp::SetTty()
{
    if (!ttydata)
        ttydata = new TtyPrivate;
    if (!ttydata)
        return;
#ifdef TERMIO
    if (tcgetattr(CHANNEL, &ttydata->oldtio) < 0)
        return;
    if (ttydata->oldtio.c_oflag & OLCUC)
        UpperCaseMode = 1; // uppercase on output
    ttydata->newtio = ttydata->oldtio;
    ttydata->newtio.c_iflag &= ~(INLCR | ICRNL | IGNCR | ISTRIP | IUCLC);
    ttydata->newtio.c_oflag &= ~(OLCUC | OCRNL | OPOST);
    ttydata->newtio.c_lflag &= ~(ECHO | ICANON | XCASE);
    ttydata->newtio.c_cc[VMIN]  = 1; // break input after each character
    ttydata->newtio.c_cc[VTIME] = 1; // timeout is 100 msecs
    ttydata->newtio.c_cc[VINTR] = 3 /*NOCHAR*/; // ^C
    ttydata->newtio.c_cc[VQUIT] = NOCHAR;
#ifdef VSWTCH
    ttydata->newtio.c_cc[VSWTCH] = NOCHAR;
#endif
#ifdef VDSUSP
    ttydata->newtio.c_cc[VDSUSP] = NOCHAR;
#endif
#ifdef VLNEXT
    ttydata->newtio.c_cc[VLNEXT] = NOCHAR;
#endif
#ifdef VDISCARD
    ttydata->newtio.c_cc[VDISCARD] = NOCHAR;
#endif
    tcsetattr(CHANNEL, TCSADRAIN, &ttydata->newtio);
#else
    if (gtty(CHANNEL, &ttydata->tty) < 0)
        return;
    if (ttydata->tty.sg_flags & LCASE)
        UpperCaseMode = 1; // uppercase on output
    ttydata->ttyflags = ttydata->tty.sg_flags;
    ttydata->tty.sg_flags &= ~(XTABS | ECHO | CRMOD | LCASE);
#ifdef CBREAK
    ttydata->tty.sg_flags |= CBREAK;
#endif
#ifdef NEEDLITOUT
    ttydata->tty.sg_flags |= LITOUT;
#endif
    stty(CHANNEL, &ttydata->tty);
#ifdef TIOCSETC
    ioctl(CHANNEL, TIOCGETC, (char *)&ttydata->oldtchars);
    ttydata->newtchars         = ttydata->oldtchars;
    ttydata->newtchars.t_intrc = NOCHAR;
    ttydata->newtchars.t_quitc = NOCHAR;
    ttydata->newtchars.t_eofc  = NOCHAR;
    ttydata->newtchars.t_brkc  = NOCHAR;
    ioctl(CHANNEL, TIOCSETC, (char *)&ttydata->newtchars);
#endif
#endif // TERMIO
#ifdef NEEDLITOUT
    ioctl(CHANNEL, TIOCLGET, (char *)&ttydata->oldlocal);
    ttydata->newlocal = ttydata->oldlocal | LLITOUT;
    ioctl(CHANNEL, TIOCLSET, (char *)&ttydata->newlocal);
#endif
#ifdef TIOCSLTC
    ioctl(CHANNEL, TIOCGLTC, (char *)&ttydata->oldchars);
    ttydata->newchars          = ttydata->oldchars;
    ttydata->newchars.t_lnextc = NOCHAR;
    ttydata->newchars.t_rprntc = NOCHAR;
    ttydata->newchars.t_dsuspc = NOCHAR;
    ttydata->newchars.t_flushc = NOCHAR;
    ioctl(CHANNEL, TIOCSLTC, (char *)&ttydata->newchars);
#endif
}

void ScreenImp::ResetTty()
{
    if (!ttydata)
        return;
#ifdef TERMIO
    tcsetattr(CHANNEL, TCSADRAIN, &ttydata->oldtio);
#else
    ttydata->tty.sg_flags = ttydata->ttyflags;
    stty(CHANNEL, &ttydata->tty);
#ifdef TIOCSETC
    ioctl(CHANNEL, TIOCSETC, (char *)&ttydata->oldtchars);
#endif
#endif
#ifdef NEEDLITOUT
    ioctl(CHANNEL, TIOCLSET, (char *)&ttydata->oldlocal);
#endif
#ifdef TIOCSLTC
    ioctl(CHANNEL, TIOCSLTC, (char *)&ttydata->oldchars);
#endif
}

void ScreenImp::FlushTtyInput()
{
    if (!ttydata)
        return;
#ifdef TCFLSH
    ioctl(CHANNEL, TCFLSH, 0);
#else
#ifdef TIOCFLUSH
    int p = 1;

    ioctl(CHANNEL, TIOCFLUSH, (char *)&p);
#endif
#endif
}
