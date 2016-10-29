# include <stdio.h>
# include <string.h>
# ifdef MSDOS
# include <stdlib.h>
# else
# include <sys/types.h>
# include <macros.h>
# endif
# include "choice.h"
# include "scr.h"
# include "key.h"
# include "ut.h"

# define SWAP(a,b)      { register t; t = a; a = b; b = (char) t; }

struct choice {
	char *name;
	int namlen;
	int r, c;
};

static cnum;
static void (*attron) (), (*attroff) ();
static menuchoice();
static void initchoice(), drawchoice();
static BOX *mbox = 0;

void endmesg ()
{
	if (mbox) {
		VUngetBox (mbox);
		VFreeBox  (mbox);
	}
	mbox = 0;
}

/* VARARGS 1 */
void error (s, aa, bb, cc, dd)
char *s;
long aa, bb, cc, dd;
{
	char buf [100];

	sprintf (buf, s, aa, bb, cc, dd);
	getchoice (1, " Ошибка ", buf, NULL, " Да ", NULL, NULL);
}

/* VARARGS 1 */
void pmessage (name, s, aa, bb, cc, dd)
char *name, *s;
long aa, bb, cc, dd;
{
	char buf [100];

	sprintf (buf, s, aa, bb, cc, dd);
	getchoice (0, name, buf, NULL, " Да ", NULL, NULL);
}

/* VARARGS 1 */
void bmessage (name, s, aa, bb, cc, dd)
char *name, *s;
long aa, bb, cc, dd;
{
	char buf [100];

	endmesg ();
	sprintf (buf, s, aa, bb, cc, dd);
	getchoice (1, name, buf, NULL, NULL, NULL, NULL);
}

/* VARARGS 1 */
void message (name, s, aa, bb, cc, dd)
char *name, *s;
long aa, bb, cc, dd;
{
	char buf [100];

	endmesg ();
	sprintf (buf, s, aa, bb, cc, dd);
	getchoice (0, name, buf, NULL, NULL, NULL, NULL);
}

ask_are_you_shure (message) /* Запрос "Вы уверены" */
register char * message ;
{
	return getchoice(1, message, "Вы уверены ?\007", NULL, " Да ", " Нет ", NULL) == 0;
}

getchoice (bold, head, mesg, mesg2, c1, c2, c3)
char *head, *mesg, *mesg2, *c1, *c2, *c3;
{
	int len, ch;
	int isattr;
	register r, c, w, h;
	BOX *box;
	struct choice tab [3];

	w = strlen (mesg);
	if (mesg2) {
		len = strlen (mesg2);
		w = max(w, len);
	}
	len = strlen (head);
	w = max(w, len);
	len = 0;
	if (c1)
		len += strlen (c1);
	if (c2)
		len += strlen (c2);
	if (c3)
		len += strlen (c3);
	w = max(w, len) +10;
	h = 5 + (c1 != NULL);
	if (bold)
		r = LINES/2;
	else
		r = LINES/4;
	c = (80 - w) / 2;

	box = VGetBox (r-1, c-2, h+2, w+4);             /* save box */
	isattr = VStandOut ();
	if (bold)
		isattr &= VSetBold ();
	else
		isattr &= VSetDim ();
	VFillBox (r-1, c-2, h+2, w+4, ' ');             /* clear */
	VDrawBox (r, c, h, w);                          /* draw margins */
	VMPutString (r, c + (w-strlen(head)) / 2, head); /* head */
	if (mesg2) {
		VMPutString (r+1, c + (w-strlen(mesg)) / 2, mesg);
		VMPutString (r+2, c + (w-strlen(mesg2)) / 2, mesg2);
	} else
		VMPutString (r+2, c + (w-strlen(mesg)) / 2, mesg);

	if (c1) {
		/* menu */
		if (isattr) {
			attron = bold ? VSetDim : VStandEnd;
			attroff = bold ? VSetBold : VStandOut;
		} else
			attron = attroff = 0;

		initchoice (r+4, c+w/2, c1, c2, c3, tab);

		ch = menuchoice (tab);
		VStandEnd ();
		VSetNormal ();
		VUngetBox (box);
		VFreeBox (box);
		return (ch);
	} else {
		/* message */
		VStandEnd ();
		VSetNormal ();
		mbox = box;
		hidecursor ();
		VSync ();
		return (0);
	}
}

static void initchoice (row, col, c1, c2, c3, tab)
char *c1, *c2, *c3;
register struct choice *tab;
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

static menuchoice (tab)
register struct choice *tab;
{
	int ch = 0;

	for (;;) {
		drawchoice (ch, tab);
		hidecursor ();
		VSync ();
		switch (KeyGet ()) {
		default:
			VBeep ();
			continue;
		case cntrl (']'):       /* redraw screen */
			VRedraw ();
			continue;
		case cntrl ('['):       /* ESC */
		case cntrl ('C'):
		case meta  ('0'):       /* f0 */
			return (-1);
		case cntrl ('M'):       /* Enter */
		case cntrl ('J'):
			return (ch);
		case        ' ':        /* Space */
		case cntrl ('I'):       /* Tab */
		case meta  ('r'):       /* right */
			if (++ch >= cnum)
				ch = 0;
			continue;
		case cntrl ('H'):       /* BS */
		case meta  ('B'):       /* Back Tab */
		case meta  ('l'):       /* left */
			if (--ch < 0)
				ch = cnum - 1;
			continue;
		case meta  ('h'):       /* home */
			ch = 0;
			continue;
		case meta  ('H'):       /* end */
			ch = cnum - 1;
			continue;
		}
	}
}

static void drawchoice (ch, tab)
register struct choice *tab;
{
	register i;

	for (i=0; i<cnum; ++i) {
		if (i == ch) {
			if (attron)
				(*attron) ();
			VMPutString (tab[i].r, tab[i].c, tab[i].name);
			if (! attron) {
				VMPutChar (tab[i].r, tab[i].c, '[');
				VMPutChar (tab[i].r, tab[i].c + strlen (tab[i].name) - 1, ']');
			}
			if (attroff)
				(*attroff) ();
		} else
			VMPutString (tab[i].r, tab[i].c, tab[i].name);
	}
}

static char *editstring (r, c, w, str, cp)
char *str;
register cp;                                    /* cursor position */
{
	register key, k;
	int firstkey = 1;

	if (cp) {
		for (cp=0; str[cp]; ++cp)
			;
		firstkey = 0;
	}
	for (; ; firstkey=0) {
		VClearBox (r, c, 1, w);
		VMPutString (r, c, str);
		VMove (r, c+cp);
		VSync ();
		switch (key = KeyGet ()) {
		default:
			if (key < ' ' || key == '\377') {
				VBeep ();
				continue;
			}
			if (firstkey) {
				str[0] = (char) key;
				str[1] = 0;
				cp = 1;
				continue;
			}
			for (k=cp; str[k]; ++k)
				SWAP (key, str[k]);
			str [k] = (char) key;
			str [w] = str [k+1] = 0;
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
		case cntrl (']'):       /* redraw screen */
			VRedraw ();
			continue;
		case cntrl ('C'):
		case cntrl ('['):
		case meta ('0'):        /* f0 */
			return (NULL);
		case cntrl ('M'):
		case cntrl ('J'):
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
		case meta ('H'):        /* end */
			while (str [cp])
				++cp;
			continue;
		case cntrl ('H'):               /* back space */
			if (cp < 1)
				continue;
			for (k=cp--; str[k]; ++k)
				str[k-1] = str[k];
			str [k-1] = 0;
			continue;
		case meta  ('u'):		/* up */
		case meta  ('d'):		/* down */
			continue;
		case cntrl ('G'):		/* delete */
		case meta  ('D'):
			if (! str [cp])
				continue;
			for (k=cp+1; str[k]; ++k)
				str[k-1] = str[k];
			str [k-1] = 0;
			continue;
		case cntrl ('Y'):               /* clear line */
			str [cp = 0] = 0;
			continue;
		}
	}
}

char *getstring (w, str, head, mesg)
char *str, *head, *mesg;
register w;
{
	register r, c, h;
	int len;
	BOX *box;
	static char buf [81];

	len = strlen (mesg);
	w = max (w, len);
	len = strlen (head);
	w = max (w, len);
	len = strlen (str);
	w = max (w, len);
	h = 4;
	w += 4;
	r = LINES/4;
	c = (78 - w) / 2;

	box = VGetBox (r-1, c-2, h+2, w+4);             /* save box */
	VStandOut ();
	VSetDim ();
	VFillBox (r-1, c-2, h+2, w+4, ' ');             /* clear */
	VDrawBox (r, c, h, w);                          /* draw margins */
	VMPutString (r, c + (w-strlen(head)) / 2, head); /* head */
	VMPutString (r+1, c+2, mesg);                   /* message */
	VStandEnd ();
	VSetNormal ();

	strncpy (buf, str != NULL ? str : "", 80);

	str = editstring (r+2, c+2, w-4, buf, 0);
	VUngetBox (box);
	VFreeBox (box);
	return (str);
}

long editval (w, head, mesg, fmt, init_val)
long init_val;
char * head, * mesg, * fmt;
{
#       define MAX_STR  60
	char     str [MAX_STR];
	register char *s;
	long     val;

	sprintf (str, fmt, init_val);

	w = min (w, MAX_STR);
	do {
		if ((s = getstring (w, str, head, mesg)) == NULL)
			return -1l;
	} while (sscanf (s, fmt, &val) != 1);
	return val;
}
