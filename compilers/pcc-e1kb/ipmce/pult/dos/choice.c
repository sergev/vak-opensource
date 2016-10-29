# include "jlib.h"

# ifndef NULL
#    define NULL        ((char *) 0)
# endif

# define LINES		25

# define SWAP(a,b)      { register t; t = a; a = b; b = t; }

struct choice {
	char *name;
	int namlen;
	int r, c;
};

static struct choice tab [3];
static cnum;
static (*attron) (), (*attroff) ();
static JBOX *mbox;

extern Jdim (), Jbold (), Jrev (), Junrev ();
extern char *strncpy ();

/* VARARGS 1 */
error (s, aa, bb, cc, dd)
char *s;
long aa, bb, cc, dd;
{
	char buf [100];

	sprintf (buf, s, aa, bb, cc, dd);
	getchoice (1, " Ошибка ", buf, NULL, " Ok ", NULL, NULL);
}

/* VARARGS 1 */
message (name, s, aa, bb, cc, dd)
char *name, *s;
long aa, bb, cc, dd;
{
	char buf [100];

	sprintf (buf, s, aa, bb, cc, dd);
	getchoice (0, name, buf, NULL, NULL, NULL, NULL);
}

endmesg ()
{
	if (mbox)
		Jungetbox (mbox);
	mbox = 0;
}

getchoice (bold, head, mesg, mesg2, c1, c2, c3)
char *head, *mesg, *mesg2, *c1, *c2, *c3;
{
	int len, ch;
	register r, c, w, h;
	JBOX *box;

	w = strlen (mesg);
	if (mesg2) {
		len = strlen (mesg2);
		if (len > w)
			w = len;
	}
	len = strlen (head);
	if (len > w)
		w = len;
	len = 0;
	if (c1)
		len += strlen (c1);
	if (c2)
		len += strlen (c2);
	if (c3)
		len += strlen (c3);
	if (len > w)
		w = len;
	h = 6;
	w += 10;
	if (bold)
		r = LINES/2;
	else
		r = LINES/4;
	c = (80 - w) / 2;

	box = Jgetbox (r-1, c-2, h+2, w+4);             /* save box */
	if (bold)
		Jrbold ();
	else
		Jrdim ();
	Jfillbox (r-1, c-2, h+2, w+4, ' ');             /* clear */
	Jdrawbox (r, c, h, w);                          /* draw margins */
	Jmputstr (r, c + (w-strlen(head)) / 2, head);					/* head */
	if (mesg2) {
		Jmputstr (r+1, c + (w-strlen(mesg)) / 2, mesg);
		Jmputstr (r+2, c + (w-strlen(mesg2)) / 2, mesg2);
	} else
		Jmputstr (r+2, c + (w-strlen(mesg)) / 2, mesg);

	if (c1) {
		attron = bold ? Jrdim : Junrev;
		attroff = bold ? Jrbold : Jrev;

		initchoice (r+4, c+w/2, c1, c2, c3);

		ch = menuchoice ();
		Junrev ();
		Jnorm ();
		Jungetbox (box);
		Jfreebox (box);
		return (ch);
	} else {
		/* message */
		Junrev ();
		Jnorm ();
		mbox = box;
		return (0);
	}
}

static initchoice (row, col, c1, c2, c3)
char *c1, *c2, *c3;
{
	register i, w;

	cnum = c2 ? (c3 ? 3 : 2) : 1;
	tab[0].name = c1;
	tab[1].name = c2;
	tab[2].name = c3;
	w = cnum-1;
	for (i=0; i<cnum; ++i) {
		w += tab[i].namlen = strlen (tab[i].name);
		tab[i].r = row;
	}
	tab[0].c = col-w/2;
	tab[1].c = tab[0].c + tab[0].namlen + 1;
	tab[2].c = tab[1].c + tab[1].namlen + 1;
}

static menuchoice ()
{
	int ch = 0;

	for (;;) {
		drawchoice (ch);
		switch (getkey ()) {
		default:
			beep ();
			continue;
		case cntrl ('['):
			return (-1);
		case cntrl ('M'):
			return (ch);
		case ' ':
		case cntrl ('I'):
		case meta ('r'):        /* right */
			if (++ch >= cnum)
				ch = 0;
			continue;
		case cntrl ('H'):
		case meta ('l'):        /* left */
			if (--ch < 0)
				ch = cnum-1;
			continue;
		}
	}
}

static drawchoice (ch)
{
	register i;

	for (i=0; i<cnum; ++i) {
		if (i == ch) {
			if (attron)
				(*attron) ();
			Jmputstr (tab[i].r, tab[i].c, tab[i].name);
			if (! attron) {
				Jmputch (tab[i].r, tab[i].c, '[');
				Jmputch (tab[i].r, tab[i].c + strlen (tab[i].name) - 1, ']');
			}
			if (attroff)
				(*attroff) ();
		} else
			Jmputstr (tab[i].r, tab[i].c, tab[i].name);
	}
}

static char *editstring (r, c, w, str, cp)
char *str;
register cp;                                    /* cursor position */
{
	register key, k;
	int firstkey = 1;

	if (cp) {
		for (cp=0; str[cp]; ++cp);
		firstkey = 0;
	}
	for (; ; firstkey=0) {
		Jclearbox (r, c, 1, w);
		Jmputstr (r, c, str);
		Jsetcursor (r*80 + c + cp);
		key = getkey ();
		Jsetcursor (-1);
		switch (key) {
		default:
			if (key<' ' || key>=0xff) {
				beep ();
				continue;
			}
			if (firstkey) {
				str[0] = key;
				str[1] = 0;
				cp = 1;
				continue;
			}
# ifdef INSERTMODE
			for (k=cp; str[k]; ++k)
				SWAP (key, str[k]);
			str [k] = key;
			str [w] = str [k+1] = 0;
# else
			if (! str [cp])
				str [cp+1] = 0;
			str [cp] = key;
			str [w] = 0;
# endif
			/* fall through */
		case meta ('r'):        /* right */
			if (str [cp]) {
				++cp;
				if (cp >= w)
					cp = w-1;
			}
			continue;
		case meta ('l'):        /* left */
			if (--cp < 0)
				cp = 0;
			continue;
		case cntrl ('['):
			return (0);
		case cntrl ('M'):
			return (str);
		case cntrl ('I'):
			if (str [cp])
				while (str [++cp]);
			else
				cp = 0;
			continue;
		case meta ('h'):        /* home */
			cp = 0;
			continue;
		case meta ('e'):        /* end */
			while (str [cp])
				++cp;
			continue;
		case cntrl ('H'):		/* back space */
			if (cp) {
				for (k=cp--; str[k]; ++k)
					str[k-1] = str[k];
				str [k-1] = 0;
			}
			continue;
		case meta ('.'):		/* delete */
			if (! str [cp])
				continue;
			for (k=cp+1; str[k]; ++k)
				str[k-1] = str[k];
			str [k-1] = 0;
			continue;
		case cntrl ('Y'):		/* clear line */
			str [cp = 0] = 0;
			continue;
		}
	}
}

char *getstring (ws, str, head, mesg)
char *str, *head, *mesg;
{
	register r, c, h, w;
	int len;
	JBOX *box;
	static char buf [81];

	w = ws;
	len = strlen (mesg);
	if (len > w)
		w = len;
	len = strlen (head);
	if (len > w)
		w = len;
	h = 4;
	w += 4;
	r = LINES/4;
	c = (78 - w) / 2;

	box = Jgetbox (r-1, c-2, h+2, w+4);             /* save box */
	Jrbold ();
	Jfillbox (r-1, c-2, h+2, w+4, ' ');             /* clear */
	Jdrawbox (r, c, h, w);                          /* draw margins */
	Jmputstr (r, c + (w-strlen(head)) / 2, head); /* head */
	Jmputstr (r+1, c+2, mesg);                   /* message */
	Jbold ();

	strncpy (buf, str ? str : "", 80);

	str = editstring (r+2, c + (w-ws)/2, ws, buf, 0);
	Jnorm ();
	Jungetbox (box);
	Jfreebox (box);
	return (str);
}

getkey ()
{
	register c;

	c = getch ();
	if (c)
		return (c & 0xff);
	switch (c = getch ()) {
	case 'K':       return (meta ('l'));
	case 'M':       return (meta ('r'));
	case 'H':       return (meta ('u'));
	case 'P':       return (meta ('d'));
	case 'G':       return (meta ('h'));
	case 'O':       return (meta ('e'));
	case 'Q':       return (meta ('n'));
	case 'I':       return (meta ('p'));
	case 'R':       return (meta ('i'));
	case 'S':       return (meta ('.'));
	}
	return (0);
}
