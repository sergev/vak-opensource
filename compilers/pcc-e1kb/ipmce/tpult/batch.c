# include <stdio.h>
# ifdef MSDOS
# include <stdlib.h>
# else
# include <sys/types.h>
# include <macros.h>
# endif
# include <string.h>
# include <ctype.h>
# include "choice.h"
# include "scr.h"
# include "key.h"
# include "ut.h"
# include "wkb.h"
# include "batch.h"

union  word batch_w[MAX_BATCH];

static editval1(), editstring();
static void printval(), drawhdr();

char *r_w[] = {
	"Запись",
	"Чтение",
	NULL,
};

char *den[] = {
	"Низкая",
	"Высокая",
	NULL,
};

char *yes[] = {
	"Нет",
	"Да",
	NULL,
};

char *batch_name[] = {
	"Обмен с СВС",
	"Обмен с диском",
	"Обмен с лентой",
	"Обмен с разделом",
	"Вызов ДВС",
	"Роспись памяти",
	NULL,
};

int max_batch_n = (sizeof (batch_name) / sizeof (batch_name[0])) - 1;

int batch_n = 0;

struct b_field svs[] = {
	{ NULL             , 0 , 0, 0 , 0 , 0 , 0 , 0 , HEX , 0 , 0, },
};

struct b_field disk[] = {
	{ "Операция"       , 1 , 1, -1, -1, -1, -1, -1, MENU, r_w,  -1l, },
	{ "Канал"          , 1 , 2, -1, -1, -1, -1, -1, HEX , 0  , 0x3l, },
	{ "Устройство"     , 1 , 3, -1, -1, -1, -1, -1, HEX , 0  , 0xFl, },
	{ "Том"            , 8 , 4,  0,  7, -1, -1, -1, STR , 0  ,  -1l, },
	{ "Начальный адрес", 6 , 5, -1, -1, -1, -1, -1, HEX , 0  , 0x7FFFFFl, },
	{ "Начальный блок" , 5 , 6, -1, -1, -1, -1, -1, HEX , 0  , 0xFFFFFl,  },
	{ "Количество слов", 6 , 7, -1, -1, -1, -1, -1, HEX , 0  , 0x7FFFFFl, },
	{ NULL             , 0 , 0, 0 , 0 , 0 , 0 , 0 , HEX , 0  , 0, },
};

struct b_field tape[] = {
	{ "Операция"       , 10, 1, -1, -1, -1, -1, -1, MENU, r_w,  -1l, },
	{ "Канал"          , 1 , 2, -1, -1, -1, -1, -1, HEX , 0  , 0x3l, },
	{ "Устройство"     , 2 , 3, -1, -1, -1, -1, -1, HEX , 0  , 0xFFl,},
	{ "Том"            , 8 , 4,  0,  7, -1, -1, -1, STR , 0  ,  -1l, },
	{ "Начальный адрес", 6 , 5, -1, -1, -1, -1, -1, HEX , 0  , 0x7FFFFFl, },
	{ "Номер файла"    , 4 , 6, -1, -1, -1, -1, -1, DEC , 0  , 8192l,},
	{ "Количество слов", 6 , 7, -1, -1, -1, -1, -1, HEX , 0  , 0xFFFFFl,  },
	{ "Плотность"      , 10, 8, -1, -1, -1, -1, -1, MENU, den,  -1l, },
	{ "С тегами"       , 10, 9, -1, -1, -1, -1, -1, MENU, yes,  -1l, },
	{ "Контрольная сумма",10,10, -1, -1, -1, -1, -1, MENU, yes, -1l, },
	{ "Длина блока"    , 2 ,11, -1, -1, -1, -1, -1, HEX , 0	,  0x80l, },
	{ NULL             , 0 , 0, 0 , 0 , 0 , 0 , 0 , HEX , 0  ,  0, },
};

struct b_field partn[] = {
	{ "Операция"       , 10, 1, -1, -1, -1, -1, -1, MENU, r_w,  -1l, },
	{ "Маска устройств",16 , 2, -1, -1, -1, -1, -1,MASKA, 0  ,  -1l, },
	{ "Раздел"         ,12 , 3,  0,  7,  4,  4,  7, STR , 0  ,  -1l,},
	{ "Поколение"	   , 3 , 4,  3,  3, -1, -1, -1, DEC , 0	 ,  255l, },
	{ "Начальный адрес", 6 , 5, -1, -1, -1, -1, -1, HEX , 0  ,  0x7FFFFFl, },
	{ "Начальный блок" , 5 , 6, -1, -1, -1, -1, -1, HEX , 0	 ,  0xFFFFFl,  },
	{ "Количество слов", 6 , 7, -1, -1, -1, -1, -1, HEX , 0  ,  0x7FFFFFl, },
	{ "Том"            , 8 , 8,  0,  7, -1, -1, -1, STR , 0  ,  -1l, },
	{ NULL             , 0 , 0, 0 , 0 , 0 , 0 , 0 , HEX , 0  ,  0, },
};

struct b_field boot[] = {
	{ "Вариант конф."  , 8 , 1,  0,  7, -1, -1, -1, STR , 0  ,  -1l, },
	{ "Маска устройств",16 , 2, -1, -1, -1, -1, -1,MASKA, 0  ,  -1l, },
	{ "Раздел кода"    ,12 , 3,  0,  7,  4,  4,  7, STR , 0  ,  -1l,},
	{ "Поколение кода" , 3 , 4,  3,	 3, -1, -1, -1, DEC , 0	 ,  255l, },
	{ "Раздел конф."   ,12 , 5,  0,  7,  6,  4,  7, STR , 0  ,  -1l,},
	{ "Поколение конф.", 3 , 6,  3,	 3, -1, -1, -1, DEC , 0	 ,  255l, },
	{ "Том"            , 8 , 7,  0,  7, -1, -1, -1, STR , 0  ,  -1l, },
	{ NULL             , 0 , 0, 0 , 0 , 0 , 0 , 0 , HEX , 0  ,  0, },
};

struct b_field mfil[] = {
	{ "Начальный адрес", 6 , 1, -1, -1, -1, -1, -1, HEX , 0  ,  0x7FFFFFl, },
	{ "Конечный адрес" , 6 , 2, -1, -1, -1, -1, -1, HEX , 0  ,  0x7FFFFFl, },
	{ "Константа"      ,16 , 3, -1, -1, -1, -1, -1,MASKA, 0  ,  -1l, },
	{ NULL             , 0 , 0, 0 , 0 , 0 , 0 , 0 , HEX , 0  ,  0, },
};

struct b_field *batch_entry[] = {
	svs,
	disk,
	tape,
	partn,
	boot,
	mfil,
};

static int     lines, cols, wid_h, wid_e, r, c;
BOX *right_box = 0;

void big_menu (m) {

	struct b_field *b;
	register i, j = 1;
	BOX     *left_box;

	b = batch_entry[batch_n]; lines = cols = wid_h = wid_e = 0;
	for (i = 0; b[i].header != NULL; i++) {
		lines++;
		wid_h = max( wid_h, (int) strlen( b[i].header ));
		wid_e = max( wid_e, b[i].width );
	}
	cols = wid_h + 2 + wid_e;
	cols = max (cols, (int) strlen(batch_name[batch_n]));

	r = (LINES - lines) - 5;
	c = m ? 3 : (COLS - cols) - 5;
	endmesg ();
	if (m) {
		if (lines == 0)
			return;
		left_box = VGetBox (r - 1, c - 1, lines + 4, cols + 4);
	}
	else {
		if (right_box != 0) {
			VUngetBox (right_box);
			VFreeBox (right_box);
		}
		right_box = VGetBox (r - 1, c - 1, lines + 4, cols + 4);
	}

	VStandOut ();
	VFillBox (r - 1, c - 1, lines + 4, cols + 4, ' ');
	VStandEnd ();
	VDrawBox (r, c, lines+2, cols+2);
	VMPutString (r, (c + (cols + 2 - strlen(batch_name[batch_n])) / 2),
		     batch_name[batch_n]);
	if (lines == 0)
		return;
	drawhdr (b);
	if (m) {
		for (i = 0; j != 0; i += j) {
			if (i >= lines)
				i = 0;
			else if (i < 0)
				i = lines - 1;
			j = editval1 (b + i, i);
			VStandOut ();
			printval (b + i, i);
			VStandEnd ();
		}
		VUngetBox (left_box);
		VFreeBox (left_box);
	}
	VSync ();
}

static void drawhdr (b)
struct b_field *b;
{
	register i;

	VStandOut ();

	for (i = 0; i < lines; i++) {
		VMove (r + 1 + i, c + 1);
		Vprintf("%-*s :", wid_h, b[i].header);
		printval (b + i, i);
	}
	VStandEnd ();
}

static void printval (b, i)
struct b_field *b;
register i;

{
	register j;
	unsigned char cc;

	VMove (r + 1 + i, c + 1 + wid_h + 2);

	switch (b->type) {
	case STR:
		for (j = b->e_byte; j >= b->b_byte; j--)
			VPutChar (toalt ((cc = batch_w [b->n_word].c[j]) ? cc : ' '));

		if (b->cn_word != -1)
			for (j = b->ce_byte; j >= b->cb_byte; j--)
				VPutChar (toalt ((cc = batch_w [b->cn_word].c[j]) ? cc : ' '));

		break;
	case DEC:
	case HEX:
		Vprintf ((b->type == DEC) ? "%-*.*ld" : "%-*.*lX", b->width, b->width,
			 b->b_byte == -1 ? batch_w [b->n_word].l[0] :
				    (long) batch_w [b->n_word].c[b->b_byte]);
		break;
	case MASKA:
		Vprintf ("%-8.8lX%-8.8lX", batch_w [b->n_word].l[1], batch_w [b->n_word].l[0]);
		break;
	case MENU:
		Vprintf ("%-*s", wid_e, b->menu [batch_w[b->n_word].i[0]]);
		break;
	}
}

static ctox (c)
char c;
{
	return isxdigit (c) ?
	       (isdigit (c) ? c - '0' :
			      toupper (c) - 'A' + 10) : -1;
}

static void get_llong (s, l)
register char *s;
register long l[2];
{
	register int len, i, j;

	for (l[0] = 0, l[1] = 0, i = 0, len = strlen (s); i < len; i++) {
		j = ctox (s[i]);
		if (j < 0)
			return;
		l[i < 8] |= (long) j << (28 - ((i % 8) * 4));
	}
}

static editval1 (b, i)
struct b_field *b;
register i;

{
	register j, k;
	int      ret, len;
	unsigned char cc;
	unsigned char s[80], fmt[10];
	long     l;

	switch (b->type) {
	case STR:
		for (j = b->e_byte, k = 0; j >= b->b_byte; j--)
			if (cc = batch_w [b->n_word].c[j])
				s[k++] = toalt (cc);
			else
				break;
		if (b->cn_word != -1)
			for (j = b->ce_byte; j >= b->cb_byte; j--)
				if (cc = batch_w [b->cn_word].c[j])
					s[k++] = toalt (cc);
				else
					break;
		s [k] = '\000';
		ret = editstring (r + 1 + i, c + 1 + wid_h + 2, b->width, s, 0);
		if (ret != 0) {
			len = strlen (s);
			for (j = b->e_byte, k = 0; j >= b->b_byte; j--)
				batch_w [b->n_word].c[j] = (len-- > 0) ?
						       tokoi8 (s[k++]) : (unsigned char) '\000';
			if (b->cn_word != -1)
				for (j = b->ce_byte; j >= b->cb_byte; j--)
					batch_w [b->cn_word].c[j] = (len-- > 0) ?
								tokoi8 (s[k++]) : (unsigned char) '\000';
		}
		return ret;
	case DEC:
	case HEX:
		sprintf (s, (b->type == DEC) ? "%-*ld" : "%-*lX", b->width,
			     b->b_byte == -1 ? batch_w [b->n_word].l[0] :
					(long) batch_w [b->n_word].c[b->b_byte]);
		ret = editstring (r + 1 + i, c + 1 + wid_h + 2, b->width, s, 0);
		sprintf (fmt, (b->type == DEC) ? "%%%ild" : "%%%ilX", b->width);
		l = -1l;
		j = sscanf (s, fmt, &l);
		if (l > -1 && j > 0 && (b->max_val == -1 || b->max_val >= l)) {
			if (b->b_byte == -1)
				batch_w [b->n_word].l[0] = l;
			else
				batch_w [b->n_word].c[b->b_byte] = (unsigned char) l;
		}
		return ret;
	case MASKA:
		sprintf (s, "%-8.8lX%-8.8lX", batch_w [b->n_word].l[1], batch_w [b->n_word].l[0]);
		ret = editstring (r + 1 + i, c + 1 + wid_h + 2, b->width, s, 0);
		get_llong (s, batch_w [b->n_word].l);
		return ret;
	case MENU:
		len = batch_w[b->n_word].i[0]; /* индекс */
		len = select (r + 1 + i, c + 1 + wid_h + 2, wid_e, b->menu, len, &ret);
		if (ret != 0)
			batch_w[b->n_word].i[0] = len;
		return ret;
	default:
		return 1;
	}
}

static editstring (r, c, w, str, cp)
int r, c;
char *str;
register cp;                                    /* cursor position */
{

# define SWAP(a,b)      { register char t; t = (char) a; a = b; b = t; }

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
			return (0);
		case cntrl ('M'):
		case cntrl ('J'):
		case meta  ('d'):
			return (1);
		case meta  ('u'):
			return (-1);
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
		case cntrl ('G'):               /* delete */
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

select (r, c, w, p, i_val, ret)
int r, c;
char **p;
int *ret;
{
	register key, i, j = i_val, entry;

	for (i = entry = 0; p[i] != NULL; i++)
		entry++;

	for ( ; ; ) {
		VClearBox (r, c, 1, w);
		VMPutString (r, c, p[j]);
		VMove (r, c);
		VSync ();
		switch (key = KeyGet ()) {
		default:
			if (key<' ' || key>'~') {
				VBeep ();
				continue;
			}
		case meta ('r'):        /* right */
		case       ' ' :        /* Space */
		case cntrl('I'):        /* Tab */
			if (++j >= entry)
				j = 0;
			continue;
		case cntrl ('H'):       /* Back Space */
		case meta  ('l'):       /* left */
		case meta  ('B'):       /* Back Tab */
			if (--j < 0)
				j = entry - 1;
			continue;
		case cntrl (']'):       /* redraw screen */
			VRedraw ();
			continue;
		case cntrl ('C'):
		case cntrl ('['):       /* ESC */
		case meta  ('0'):       /* f0 */
			*ret = 0;
			return j;
		case cntrl ('M'):
		case cntrl ('J'):
		case meta  ('d'):
			*ret = 1;
			return j;
		case meta  ('u'):
			*ret = -1;
			return j;
		case meta ('h'):        /* home */
			j = 0;
			continue;
		case meta ('H'):        /* end */
			j = lines - 1;
			continue;
		}
	}
}

