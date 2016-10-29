/*
 *      TtySet ()
 *
 *              - set terminal CBREAK mode.
 *
 *      TtyReset ()
 *
 *              - restore terminal mode.
 *
 *      TtyFlush ()
 *
 *              - flush input queue.
 */

# ifdef TERMIO
#    include <termio.h>
# else
#    include <sgtty.h>
# endif

# ifdef VENIX
#    undef TIOCSETC                     /* such a marasmatic system */
# endif

# define CHANNEL 2                      /* output file descriptor */

# ifdef TERMIO
static struct termio oldtio, newtio;
# else
static struct sgttyb tty;               /* режимы работы терминала */
static ttyflags;
# ifdef TIOCSETC
static struct tchars oldtchars, newtchars;
# endif
# endif
# ifdef TIOCLSET
static oldlocal, newlocal;
# endif
# ifdef TIOCSLTC
static struct ltchars oldchars, newchars;
# endif

int TtyUpperCase;

TtySet ()
{
# ifdef TERMIO
	if (ioctl (CHANNEL, TCGETA, (char *) &oldtio) < 0)
		return;
	if (oldtio.c_oflag & OLCUC)
		TtyUpperCase = 1;       /* uppercase on output */
	newtio = oldtio;
	newtio.c_iflag &= ~(INLCR | ICRNL | IGNCR | ISTRIP | IUCLC);
	newtio.c_oflag &= ~OPOST;       /* no output postprocessing */
	newtio.c_lflag &= ~(ECHO | ICANON | XCASE);
	newtio.c_cc [VMIN] = 1;         /* break input after each character */
	newtio.c_cc [VTIME] = 1;        /* timeout is 100 msecs */
	newtio.c_cc [VINTR] = -1;
	newtio.c_cc [VQUIT] = -1;
	newtio.c_cc [VSWTCH] = -1;
	ioctl (CHANNEL, TCSETA, (char *) &newtio);
# else
	if (gtty (CHANNEL, &tty) < 0)
		return;
	if (tty.sg_flags & LCASE)
		TtyUpperCase = 1;       /* uppercase on output */
	ttyflags = tty.sg_flags;
	tty.sg_flags &= ~(XTABS | ECHO | CRMOD | LCASE);
#    ifdef CBREAK
	tty.sg_flags |= CBREAK;
#    endif
	stty (CHANNEL, &tty);
#    ifdef TIOCSETC
	ioctl (CHANNEL, TIOCGETC, (char *) &oldtchars);
	newtchars = oldtchars;
	newtchars.t_intrc = -1;
	newtchars.t_quitc = -1;
	newtchars.t_eofc = -1;
	newtchars.t_brkc = -1;
	ioctl (CHANNEL, TIOCSETC, (char *) &newtchars);
#    endif
# endif /* TERMIO */
# ifdef TIOCLSET
	ioctl (CHANNEL, TIOCLGET, (char *) &oldlocal);
	newlocal = oldlocal | LLITOUT;
	ioctl (CHANNEL, TIOCLSET, (char *) &newlocal);
# endif
# ifdef TIOCSLTC
	ioctl (CHANNEL, TIOCGLTC, (char *) &oldchars);
	newchars = oldchars;
	newchars.t_lnextc = -1;
	newchars.t_rprntc = -1;
	newchars.t_dsuspc = -1;
	newchars.t_flushc = -1;
	ioctl (CHANNEL, TIOCSLTC, (char *) &newchars);
# endif
}

TtyReset ()
{
# ifdef TERMIO
	ioctl (CHANNEL, TCSETA, (char *) &oldtio);
# else
	tty.sg_flags = ttyflags;
	stty (CHANNEL, &tty);
# ifdef TIOCSETC
	ioctl (CHANNEL, TIOCSETC, (char *) &oldtchars);
# endif
# endif
# ifdef TIOCLSET
	ioctl (CHANNEL, TIOCLSET, (char *) &oldlocal);
# endif
# ifdef TIOCSLTC
	ioctl (CHANNEL, TIOCSLTC, (char *) &oldchars);
# endif
}

TtyFlush ()
{
# ifdef TIOCFLUSH
	int p = 1;

	ioctl (CHANNEL, TIOCFLUSH, (char *) &p);
# endif
# ifdef TCFLSH
	ioctl (CHANNEL, TCFLSH, 0);
# endif
}
