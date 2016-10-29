/*
 *      int KeyInit (map, flushproc)
 *      struct KeyMap *map;
 *      int (*flushproc) ();
 *
 *              - set key map table.
 *              Flushproc is called before actual reading from terminal.
 *              Returns 1 if ok else 0.
 *
 *      int KeyGet ()
 *
 *              - reads key from terminal.
 *              Returns key (always >= 0).
 *
 *      int KeyGetChar ()
 *
 *              - reads char from terminal.
 *              Returns char (always >= 0).
 */

# include "key.h"
# include "cap.h"

# define NKEY   40

# define CAP(a,b) ((a)<<8|(b))

# define KF1    0
# define KF2    1
# define KF3    2
# define KF4    3
# define KF5    4
# define KF6    5
# define KF7    6
# define KF8    7
# define KF9    8
# define KF10   9
# define KL     10
# define KR     11
# define KU     12
# define KD     13
# define KH     14
# define K1     15
# define KP     16
# define KN     17
# define K0     18
# define K_     19

static struct KeyMap *km;
static int (*flush) ();
static keyback;

# ifdef __MSDOS__
static short fkey [20];
static remap [041];
# else
static char tspace [128];
# endif

extern unsigned alarm ();

KeyInit (map, flushproc)
struct KeyMap *map;
int (*flushproc) ();
{
# ifdef __MSDOS__
	register struct KeyMap *kp;
	register i;
# else
	struct CapTab tab [NKEY];
	register struct KeyMap *kp;
	register struct CapTab *t;
# endif

	km = map;
	flush = flushproc;
# ifdef __MSDOS__
	for (i=0; i<sizeof(remap); ++i)
		remap[i] = i;
	remap[040] = '\177';
	for (i=0; i<sizeof(fkey)/sizeof(fkey[0]); ++i)
		fkey[i] = 0;
	for (kp= &km[0]; kp->val; ++kp)
		if (kp->tcap) switch (CAP (kp->tcap[0], kp->tcap[1])) {
		case CAP ('f', '1'):    fkey [KF1] = kp->val;   break;
		case CAP ('f', '2'):    fkey [KF2] = kp->val;   break;
		case CAP ('f', '3'):    fkey [KF3] = kp->val;   break;
		case CAP ('f', '4'):    fkey [KF4] = kp->val;   break;
		case CAP ('f', '5'):    fkey [KF5] = kp->val;   break;
		case CAP ('f', '6'):    fkey [KF6] = kp->val;   break;
		case CAP ('f', '7'):    fkey [KF7] = kp->val;   break;
		case CAP ('f', '8'):    fkey [KF8] = kp->val;   break;
		case CAP ('f', '9'):    fkey [KF9] = kp->val;   break;
		case CAP ('f', '0'):    fkey [KF10] = kp->val;  break;
		case CAP ('k', 'l'):    fkey [KL] = kp->val;    break;
		case CAP ('k', 'r'):    fkey [KR] = kp->val;    break;
		case CAP ('k', 'u'):    fkey [KU] = kp->val;    break;
		case CAP ('k', 'd'):    fkey [KD] = kp->val;    break;
		case CAP ('k', 'h'):    fkey [KH] = kp->val;    break;
		case CAP ('k', '1'):    fkey [K1] = kp->val;    break;
		case CAP ('k', 'N'):    fkey [KN] = kp->val;    break;
		case CAP ('k', 'P'):    fkey [KP] = kp->val;    break;
		case CAP ('k', '0'):    fkey [K0] = kp->val;    break;
		case CAP ('k', '.'):    fkey [K_] = kp->val;    break;
		} else if (kp->str)
			if (kp->str[0]=='\177' || kp->str[0]>=1 && kp->str[0]<=037)
				remap [040] = kp->val;
			else if (kp->str[0]>=1 && kp->str[0]<=037)
				remap [kp->str[0]] = kp->val;
# else
	for (t=tab, kp= &km[0]; kp->val; ++kp) {
		if (! kp->tcap)
			continue;
		t->tname[0] = kp->tcap[0];
		t->tname[1] = kp->tcap[1];
		t->ttype = CAPSTR;
		t->tdef = 0;
		t->tc = 0;
		t->ti = 0;
		t->ts = &kp->str;
		if (++t >= tab+NKEY-1)
			break;
	}
	t->tname[0] = 0;
	CapGet (tab, tspace);
# endif
}

# ifdef __MSDOS__

KeyGet ()
{
	register c;

	if (flush)
		(*flush) ();

	if (c = getch ()) {
		if (c == '\177')
			return (remap [040]);
		else if (c>=1 && c<=037)
			return (remap [c]);
		return (c);
	}
	switch (c = getch ()) {
	case ';':       return (fkey [KF1]);
	case '<':       return (fkey [KF2]);
	case '=':       return (fkey [KF3]);
	case '>':       return (fkey [KF4]);
	case '?':       return (fkey [KF5]);
	case '@':       return (fkey [KF6]);
	case 'A':       return (fkey [KF7]);
	case 'B':       return (fkey [KF8]);
	case 'C':       return (fkey [KF9]);
	case 'D':       return (fkey [KF10]);
	case 'K':       return (fkey [KL]);
	case 'M':       return (fkey [KR]);
	case 'H':       return (fkey [KU]);
	case 'P':       return (fkey [KD]);
	case 'G':       return (fkey [KH]);
	case 'O':       return (fkey [K1]);
	case 'Q':       return (fkey [KN]);
	case 'I':       return (fkey [KP]);
	case 'R':       return (fkey [K0]);
	case 'S':       return (fkey [K_]);
	}
	return (0);
}

# else /* MSDOS */

# include <setjmp.h>
# include <signal.h>

static jmp_buf wakeup;

KeyGetChar ()
{
	char c;

	while (read (0, &c, 1) != 1)
		if (! isatty (0))
			exit (0);
	return (c & 0377);
}

static badkey ()
{
	alarm (0);
	longjmp (wakeup, -1);
}

static nmgetch (c)
register c;
{
	register struct KeyMap *kp;
	register match;

	for (kp=km; kp->val; ++kp)
		kp->index = 0;
	for (;;) {
		match = 0;
		for (kp=km; kp->val; ++kp)
			if (kp->str && kp->index >= 0)
				if ((char) c == kp->str [kp->index]) {
					if (! kp->str [++kp->index])
						return (kp->val);
					match = 1;
				} else
					kp->index = -1;
		if (! match) {
			alarm (0);
			longjmp (wakeup, 1);
		}
		c = KeyGetChar ();
	}
}

KeyGet ()
{
	register struct KeyMap *kp;
	register c, j;

	if (keyback) {
		c = keyback;
		keyback = 0;
		return (c);
	}
	if (flush)
		(*flush) ();
	c = KeyGetChar ();
	for (kp=km; kp->val; ++kp)
		if (kp->str && (char) c == kp->str[0])
			break;
	if (! kp->val) {
# ifdef DOC
		if (c == cntrl ('_'))
			_prscreen ();
# endif
		return (c);
	}
	/* look for escape sequence */
	if (j = setjmp (wakeup)) {
		/* time out or unknown escape sequence */
		TtyFlush ();
		return (j<0 ? c : 0);
	}
	signal (SIGALRM, badkey);
	alarm (2);
	c = nmgetch (c);
	alarm (0);
	return (c);
}

KeyUnget (key)
{
	keyback = key;
}

# endif /* MSDOS */
