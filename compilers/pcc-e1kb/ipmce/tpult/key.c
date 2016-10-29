/*
 *	void KeyInit (map, flushproc)
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
# ifdef MSDOS
# include <dos.h>
# endif
# include "intr.h"

# define NKEY   40

# define CAP(a,b) ((a)<<8|(b))

# define KL     0
# define KR     1
# define KU     2
# define KD     3
# define KN     4
# define KP     5
# define KH     6
# define KHH    7
# define KS     8
# define KDD    9
# define KI     10
# define K1     11
# define K2     12
# define K3     13
# define KF1    14
# define KF2    15
# define KF3    16
# define KF4    17
# define KF5    18
# define KF6    19
# define KF7    20
# define KF8    21
# define KF9    22
# define KF10   23
# define KF11   24
# define KF12   25
# define KB     26

static struct KeyMap *km;
static int (*flush) ();

# ifdef MSDOS
static short fkey [26];
static remap [041];
# else
static keyback;
# endif

extern unsigned alarm ();

void KeyInit (map, flushproc)
struct KeyMap *map;
int (*flushproc) ();
{
# ifdef MSDOS
	register struct KeyMap *kp;
	register i;
# else
	struct CapTab tab [NKEY];
	register struct KeyMap *kp;
	register struct CapTab *t;
# endif

	km = map;
	flush = flushproc;
# ifdef MSDOS
	for (i=0; i<sizeof(remap)/sizeof(remap[0]); ++i)
		remap[i] = i;
	remap[040] = '\177';
	for (i=0; i<sizeof(fkey)/sizeof(fkey[0]); ++i)
		fkey[i] = 0;
	for (kp= &km[0]; kp->val; ++kp)
		if (kp->tcap) switch (CAP (kp->tcap[0], kp->tcap[1])) {
		case CAP ('k', 'l'):    fkey [KL] = kp->val;    break;
		case CAP ('k', 'r'):    fkey [KR] = kp->val;    break;
		case CAP ('k', 'u'):    fkey [KU] = kp->val;    break;
		case CAP ('k', 'd'):    fkey [KD] = kp->val;    break;
		case CAP ('k', 'N'):    fkey [KN] = kp->val;    break;
		case CAP ('k', 'P'):    fkey [KP] = kp->val;    break;
		case CAP ('k', 'h'):    fkey [KH] = kp->val;    break;
		case CAP ('k', 'H'):    fkey [KHH] = kp->val;   break;
		case CAP ('k', 'S'):    fkey [KS] = kp->val;    break;
		case CAP ('k', 'I'):    fkey [KI] = kp->val;    break;
		case CAP ('k', 'D'):    fkey [KDD] = kp->val;   break;
		case CAP ('k', '1'):    fkey [K1] = kp->val;    break;
		case CAP ('k', '2'):    fkey [K2] = kp->val;    break;
		case CAP ('k', '3'):    fkey [K3] = kp->val;    break;
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
		case CAP ('f', '.'):    fkey [KF11] = kp->val;  break;
		case CAP ('f', '-'):    fkey [KF12] = kp->val;  break;
		case CAP ('k', 'B'):    fkey [KB]  = kp->val;   break;
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
	CapGet (tab);
# endif
}

# ifdef MSDOS

KeyGet ()
{
	register c;

	if (flush)
		(*flush) ();

	while (! dispatch ())
		;

	if (c = getch ()) {
		if (c == '\177')
			return (remap [040]);
		else if (c>=1 && c<=037)
			return (remap [c]);
		return (c);
	}
	switch (c = getch ()) {
	case 'K':       return (fkey [KL]);
	case 'M':       return (fkey [KR]);
	case 'H':       return (fkey [KU]);
	case 'P':       return (fkey [KD]);
	case 'Q':       return (fkey [KN]);
	case 'I':       return (fkey [KP]);
	case 'G':       return (fkey [KH]);
	case 'O':       return (fkey [KHH]);
	case 'R':       return (fkey [KI]);
	case 'S':       return (fkey [KDD]);
	case '^':       return (fkey [K1]);
	case '_':       return (fkey [K2]);
	case '`':       return (fkey [K3]);
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
	case 'T':       return (fkey [KF11]);
	case 'U':       return (fkey [KF12]);
	case '\xF':     return (fkey [KB]);
# ifdef NDEBOG
	default :
		{
# include "scr.h"
# include "ut.h"
		CURSOR aa;
		aa = VGetCursor();
		VMove( LINES - 1, 60 );
		Vprintf("Key = %c, %X", c, (int) c);
		VSetCursor( aa );
		}
# endif /* NDEBUG */
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
	alarm (1);
	c = nmgetch (c);
	alarm (0);
	return (c);
}

void KeyUnget (key)
{
	keyback = key;
}

# endif /* MSDOS */
