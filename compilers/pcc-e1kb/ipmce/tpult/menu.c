# include <stdio.h>
# include <string.h>
# ifdef MSDOS
# include <stdlib.h>
# include <dos.h>
# else
# include <sys/types.h>
# include <macros.h>
# endif
# include "key.h"
# include "scr.h"
# include "menu.h"
# include "choice.h"
# include "ut.h"

static void drawhead(), upmenu(), downmenu(), homemenu(), endmenu ();
       void drawmenu();

# define clrmenu(m) { VUngetBox ((m).box); (m).box_on = 0; }

static void initmenu ()
{
	register struct menu *m;
	register struct submenu *sm;
	int col, w;

	col = 3;
	for (m=menu; m->name; ++m) {
		m->len = 4 + strlen (m->name);
		m->col = col;
		col += m->len;
		m->hgh = 0;
		m->wid = 0;
		for (sm=m->submenu; sm->name; ++sm) {
			sm->row = m->hgh;
			w = 4 + strlen (sm->name);
			if (w > m->wid)
				m->wid = w;
			++m->hgh;
		}
	}
}

void getboxes ()
{
	register struct menu *m;

	for (m=menu; m->name; ++m) {
		if (m->box)
			VFreeBox (m->box);
		m->box = VGetBox (1, m->col-1, m->hgh+3, m->wid+4);
	}
}

getmenu (nkey)
{
	if (! menu[0].len)
		initmenu ();

	getboxes ();
	/*
	VMPutString (LINES-1, 0, "\0011\16      \17 2\16      \17 3\16      \17 4\16      \17 5\16      \17 6\16      \17 7\16      \17 8\16      \17 9\16      \01710\16Quit \17\2");
	*/
	drawhead (nmenu);
	for ( ;; ) {
		if (nkey != 0)
			drawmenu (&menu[nmenu]);
CONTINUE :
		hidecursor ();
		VSync ();
		switch (KeyGet ()) {
		default:
			VBeep ();
			continue;
		case cntrl (']'):          /* redraw screen */
			VRedraw ();
			continue;
		case cntrl ('M'):
		case cntrl ('J'):
			if (nkey == 0)
				return getmenu (1);
			clrmenu (menu[nmenu]);
			return (CHOICE (menu[nmenu].key, menu[nmenu].submenu[menu[nmenu].nsm].key));
		case cntrl ('C'):
		case cntrl ('['):
		case meta ('J'):        /* f0 */
			clrmenu (menu[nmenu]);
			return (0);
		case meta ('r'):        /* right */
			clrmenu (menu[nmenu]);
			if (! menu[++nmenu].name)
				nmenu = 0;
			nkey++;
			break;
		case meta ('l'):        /* left */
			clrmenu (menu[nmenu]);
			if (--nmenu < 0) {
				for (nmenu=0; menu[nmenu].name; ++nmenu);
					--nmenu;
			}
			nkey++;
			break;
		case meta ('u'):        /* up */
			if (nkey == 0)
				continue;
			upmenu (&menu[nmenu]);
			goto CONTINUE;
		case meta ('h'):        /* home */
			if (nkey == 0)
				continue;
			homemenu(&menu[nmenu]);
			break;
		case meta ('d'):        /* down */
			if (nkey == 0)
				return getmenu (1);
			downmenu (&menu[nmenu]);
			goto CONTINUE;
		case meta ('H'):        /* end */
			if (nkey == 0)
				continue;
			endmenu(&menu[nmenu]);
			break;
		}
		drawhead (nmenu);
	}
}

static void drawhead (nm)
{
	register i, reverse;

	VSetDim ();
	reverse = VStandOut ();
	VMove (0, 0);
	for (i=0; i<80; ++i)
		VPutChar (' ');
	VMove (0, 2);
	for (i=0; menu[i].name; ++i) {
		if (i == nm) {
			VStandEnd ();
			VSetBold ();
		}
		VPutChar (' ');
		VPutChar (i==nm && !reverse ? '[' : ' ');
		VPutString (menu[i].name);
		VPutChar (i==nm && !reverse ? ']' : ' ');
		VPutChar (' ');
		if (i == nm) {
			VStandOut ();
			VSetDim ();
		}
	}
	VSetNormal ();
	VStandEnd ();
}

void drawmenu (m)
register struct menu *m;
{
	register i, reverse;

	VClearBox (1, m->col-1, m->hgh+3, m->wid+4);
	VSetBold ();
	VDrawBox (1, m->col, m->hgh+2, m->wid+2);
	for (i=0; m->submenu[i].name; ++i)
		if (! m->submenu[i].name[0])
			VHorLine (i+2, m->col+2, m->wid-2);
	VSetNormal ();
	for (i=0; m->submenu[i].name; ++i) {
		if (! m->submenu[i].name[0])
			continue;
		if (i == m->nsm)
			reverse = VStandOut ();
		else if (! m->submenu[i].active)
			VSetDim ();
		VMove (2+i, m->col+1);
		VPutChar (i==m->nsm && !reverse ? '[' : ' ');
		Vprintf ("%c %-*s", m->submenu[i].tag ? '*' : ' ',
			 m->wid-4, m->submenu[i].name);
		VPutChar (i==m->nsm && !reverse ? ']' : ' ');
		if (i == m->nsm)
			VStandEnd ();
		else if (! m->submenu[i].active)
			VSetNormal ();
	}
	m -> box_on = 1;
}

static void upmenu (m)
register struct menu *m;
{

	register i = m->nsm, reverse;

	VSetNormal ();
	if (! m->submenu[i].active)
		VSetDim ();
	VMove (2 + i, m->col + 1);
	Vprintf (" %c %-*s ", m->submenu[i].tag ? '*' : ' ',
		m->wid-4,     m->submenu[i].name);
	if (! m->submenu[i].active)
		VSetNormal ();
	for (;;) {
		if (--i < 0) {
			for (i = 0; m->submenu[i].name; ++i);
				--i;
		}
		if (m->submenu[i].active)
			break;
	}
	m->nsm = i;
	reverse = VStandOut ();
	VMove (2 + i, m->col+1);
	VPutChar (!reverse ? '[' : ' ');
	Vprintf ("%c %-*s", m->submenu[i].tag ? '*' : ' ',
		 m->wid-4,  m->submenu[i].name);
	VPutChar (!reverse ? ']' : ' ');
	VStandEnd ();
}

static void homemenu (m)
register struct menu *m;
{

	register i;

	i = -1;
	for (;;) {
		if (! m->submenu[++i].name)
			i = 0;
		if (m->submenu[i].active)
			break;
	}
	m->nsm = i;
}

static void endmenu (m)
register struct menu *m;
{

	register i;

	i = -1;
	for (;;) {
		if (! m->submenu[++i].name)
			i = 0;
		if (m->submenu[i].active)
			break;
	}
	for (;;) {
		if (--i < 0) {
			for (i = 0; m->submenu[i].name; ++i);
				--i;
		}
		if (m->submenu[i].active)
			break;
	}
	m->nsm = i;
}

static void downmenu (m)
register struct menu *m;
{

	register i = m->nsm, reverse;

	VSetNormal ();
	if (! m->submenu[i].active)
		VSetDim ();
	VMove (2 + i, m->col + 1);
	Vprintf (" %c %-*s ", m->submenu[i].tag ? '*' : ' ',
		m->wid-4,     m->submenu[i].name);
	if (! m->submenu[i].active)
		VSetNormal ();
	for (;;) {
		if (! m->submenu[++i].name)
			i = 0;
		if (m->submenu[i].active)
			break;
	}
	m->nsm = i;
	reverse = VStandOut ();
	VMove (2 + i, m->col+1);
	VPutChar (!reverse ? '[' : ' ');
	Vprintf ("%c %-*s", m->submenu[i].tag ? '*' : ' ',
		 m->wid-4,  m->submenu[i].name);
	VPutChar (!reverse ? ']' : ' ');
	VStandEnd ();
}

int  small_menu (header, names, n_name)
char *header;
register char **names;
{
	register i, j, lines, cols, wid, r, c, key;
	BOX     *box;

	lines = cols = wid = 0;
	for (i = 0; names[i] != NULL; i++) {
		lines++;
		wid = max( wid, (int) strlen( names[i] ));
	}
	cols = wid;
	cols = max (cols, (int) strlen(header));

	r = LINES / 3;
	c = COLS / 3;
	endmesg ();
	if (lines == 0)
		return 0;
	box = VGetBox (r - 1, c - 1, lines + 4, cols + 4);

	VStandOut ();
	VFillBox (r - 1, c - 1, lines + 4, cols + 4, ' ');
	VStandEnd ();
	VDrawBox (r, c, lines+2, cols+2);
	VSetBold ();
	VMPutString (r, (c + (cols + 2 - strlen(header)) / 2),
		     header);
	VSetNormal ();
	VStandOut ();
	for (i = 0; i < lines; i++) {
		VMove (r + 1 + i, c + 1);
		Vprintf("%-*s", wid, names[i]);
	}
	VStandEnd ();
	for (i = j = n_name ;; i = j) {
		if (i >= lines)
			i = 0;
		else if (i < 0)
			i = lines - 1;
		VMove (r + 1 + i, c + 1);
		Vprintf("%-*s", wid, names[i]);
		hidecursor ();
		VSync ();
		switch (key = KeyGet ()) {
		default:
			if (key<' ' || key>'~') {
				VBeep ();
				continue;
			}
		case cntrl (']'):       /* redraw screen */
			VRedraw ();
			continue;
		case cntrl ('C'):
		case cntrl ('['):       /* ESC */
		case meta  ('0'):       /* f0 */
			i = -1;
			goto ret;
		case cntrl ('M'):
		case cntrl ('J'):
			goto ret;
		case meta  ('d'):
		case cntrl ('I'):
		case        ' ' :       /* Space */
			j = i + 1;
			break;
		case cntrl ('H'):       /* Back Space */
		case meta  ('B'):       /* Back Tab */
		case meta  ('u'):
			j = i - 1;
			break;
		case meta ('h'):        /* home */
			j = 0;
			break;
		case meta ('H'):        /* end */
			j = lines - 1;
			break;
		}
		VStandOut ();
		VMove (r + 1 + i, c + 1);
		Vprintf("%-*s", wid, names[i]);
		VStandEnd ();
	}
ret:
	VUngetBox (box);
	VFreeBox (box);
	return i;
}
# ifdef UNDEFINED
/*
 * Выбор имени файла из текущего каталога по маске.
 */
# define MAX_NAMES 256
# define POS_PER_NAME 16
# define FILES_PER_COL 16
#pragma optimize("e",off)
char * select_file (file_mask)
register char * file_mask;
{
	struct find_t f_info;
	static char header[] = " Выбор файла ";
	char * names[MAX_NAMES];
	int i = -1, j, k = 0, lines, cols, r, c, key, files,
		 columns, displayed;
	unsigned err_code;
	static char name [14], *ret = name;
	BOX     *box;

	if (strlen (file_mask) == 0) /* Имя не задано */
		return NULL;
	if (strpbrk(file_mask, "*?") == NULL) /* No wildcard's */
		return file_mask;
	if ((err_code = _dos_findfirst (file_mask, _A_NORMAL, & f_info)) != 0) {
		error ("%s : %s", file_mask, sys_errlist[errno]);
		return NULL;
	} else {
		do {
			if (++i >= MAX_NAMES) {
				error ("select_file : %s", "Слишком много файлов в каталоге");
				doquit ();
			}
			if ((names[i] = strdup (f_info . name)) == NULL) {
				error ("strdup : %s", "Неожиданно кончилась память");
				doquit ();
			}
		}
		while (! _dos_findnext(& f_info));
	}
	files = i + 1;
	j = files / FILES_PER_COL;
	if (j == 0) j = 1;
	columns = min (j, 4);
	lines = min (files, FILES_PER_COL);
	cols  = POS_PER_NAME * columns;
	displayed = min ((lines * columns), files);
	r = 3;
	c = 3;

	endmesg ();
	box = VGetBox (r - 1, c - 1, lines + 4, cols + 4);

	VStandOut ();
	VFillBox (r - 1, c - 1, lines + 4, cols + 4, ' ');
	VStandEnd ();
	VDrawBox (r, c, lines+2, cols+2);
	VSetBold ();
	VMPutString (r, (c + (cols + 2 - strlen(header)) / 2), header);
	VSetNormal ();
	i = 0;

REDRAW :

	VStandOut ();
	for (j = 0; j < displayed; j++) {
		VMove (r + 1 +	j % FILES_PER_COL,
		       c + 1 + (j / FILES_PER_COL) * POS_PER_NAME);
		Vprintf("%-16.16s", names[j + k]);
	}
	VStandEnd ();

	for (k += i ;; ) {

		int i1, k1;

		/* i - позиция в окне, k - индекс по массиву файлов */
		VMove (r + 1 +	i % FILES_PER_COL,
		       c + 1 + (i / FILES_PER_COL) * POS_PER_NAME);
		Vprintf("%-16.16s", names[k]);
		hidecursor ();
		VSync ();
		i1 = i; k1 = k;
		switch (key = KeyGet ()) {
		default:
			if (key<' ' || key>'~') {
				VBeep ();
				continue;
			}
		case cntrl (']'):	/* Redraw Screen */
			VRedraw ();
			continue;
		case cntrl ('C'):
		case cntrl ('['):       /* ESC */
		case meta  ('0'):	/* F0 */
			ret = NULL;
			goto RETURN ;
		case cntrl ('M'):
		case cntrl ('J'):
			strncpy (name, names[k], sizeof(name));
			goto RETURN;
		case meta  ('d'):	/* Down */
			if (k == files - 1)
				continue;
			i++;
			break;
		case meta  ('P'):	/* Page Up */
			if (k == 0)
				continue;
			i -= columns * FILES_PER_COL;
			break;
		case meta  ('N'):	/* Page Down */
			if (k == files - 1)
				continue;
			i += columns * FILES_PER_COL;
			break;
		case meta  ('u'):	/* Up */
			if (k == 0)
				continue;
			i--;
			break;
		case meta ('l'):	/* Left */
			if (k == 0)
				continue;
			i -= FILES_PER_COL;
			break;
		case meta ('r'):	/* Right */
			if (k == files - 1)
				continue;
			i += FILES_PER_COL;
			break;
		case meta ('h'):	/* Home */
			k = i = 0;
			goto REDRAW;
		case meta ('H'):	/* End */
			k = files - displayed, i = displayed - 1;
			goto REDRAW;
		}
		VStandOut ();
		VMove (r + 1 +	i1 % FILES_PER_COL,
		       c + 1 + (i1 / FILES_PER_COL) * POS_PER_NAME);
		Vprintf("%-16.16s", names[k1]);
		VStandEnd ();
		k -= (i1 - i);
		if (i >= displayed)
			i = FILES_PER_COL;
		else if (i < 0)
			i = displayed - 1 - FILES_PER_COL;
		else
			continue; /* В пределах окна */
		k -= i; /* для REDRAW */
		if (k >= files)
			k = files - displayed;
		else if (k < 0)
			k = 0;
		goto REDRAW;
	}

RETURN :

	for (j = 0; j < files; j++)
		free (names[j]);
	VUngetBox (box);
	VFreeBox (box);
	return ret;
}
# endif

