# include <stdio.h>
# include <string.h>
# ifdef MSDOS
# include <conio.h>
# include <stdlib.h>
# include <dos.h>
# else
# include <sys/types.h>
# include <macros.h>
# include <memory.h>
# endif
# include <time.h>
# include "wkb.h"
# include "scr.h"
# include "key.h"
# include "intr.h"
# include "menu.h"
# include "ut.h"

struct ioregs regs;

static long ymask1 [16] = {
	0x00000001l,	0x00000100l,	0x00010000l,	0x01000000l,
	0x00000002l,	0x00000200l,	0x00020000l,	0x02000000l,
	0x00000004l,	0x00000400l,	0x00040000l,	0x04000000l,
	0x00000008l,	0x00000800l,	0x00080000l,	0x08000000l,
};

static long ymask2 [16] = {
	0x00000010l,	0x00001000l,	0x00100000l,	0x10000000l,
	0x00000020l,	0x00002000l,	0x00200000l,	0x20000000l,
	0x00000040l,	0x00004000l,	0x00400000l,	0x40000000l,
	0x00000080l,	0x00008000l,	0x00800000l,	0x80000000l,
};

static long ypat1 [16] = {
	0x00000001l,	0x00000002l,	0x00000004l,	0x00000008l,
	0x00008000l,	0x00004000l,	0x00002000l,	0x00001000l,
	0x00010000l,	0x00020000l,	0x00040000l,	0x00080000l,
	0x80000000l,	0x40000000l,	0x20000000l,	0x10000000l,
};

static long ypat2 [16] = {
	0x00000010l,	0x00000020l,	0x00000040l,	0x00000080l,
	0x00000800l,	0x00000400l,	0x00000200l,	0x00000100l,
	0x00100000l,	0x00200000l,	0x00400000l,	0x00800000l,
	0x08000000l,	0x04000000l,	0x02000000l,	0x01000000l,
};

void Load_Regs ();

void State_B ()
{
	BOX *RBox = 0;
	register int i;
		 int r = 5, c = 3, lines = 16, cols = 73;
	char *msg = "Нажмите <Esc> для выхода";

	RBox = VGetBox (r - 1, c - 1, lines + 2, cols + 2);
	VFillBox (r - 1, c - 1, lines + 2, cols + 2, ' ');
	VStandOut ();
	VDrawBox (r, c, lines, cols);
	VStandEnd ();
	VMPutString (r + lines, (c + (cols + 2 - strlen(msg)) / 2), msg);
	do {
	Load_Regs (&regs);
	VMove (r + 3, c + 2);
	Vprintf ("СЧАС %03X %04X", (int) (regs.pc>>16 & 0xfff),
		(int) (regs.pc & 0xffff));
	VMove (r + 4, c + 2);
	if (((regs.cr >> 20) & 0xff) == 0x3f)
		Vprintf ("  РК %X %02X %02X %03X", (int) (regs.cr>>28 & 0xf),
			(int) (regs.cr>>20 & 0xff),
			(int) (regs.cr>>12 & 0xff),
			(int) (regs.cr & 0xfff));
	else
		Vprintf ("  РК %X %02X %05lX", (int) (regs.cr>>28 & 0xf),
			(int) (regs.cr>>20 & 0xff),
			      (regs.cr & 0xfffffl));
	for (i = 0; i < 8; i++) {
		VMove (r + 2 + i, 30);
		if (i == 0)
		Vprintf (" ИС8 %03X %04X",
			(int) (regs.r[i]>>16 & 0x7ff),
			(int) (regs.r[i] & 0xffff));
		else
		Vprintf ("  R%X %03X %04X", i,
			(int) (regs.r[i]>>16 & 0x7ff),
			(int) (regs.r[i] & 0xffff));
		Vprintf ("  R%X %03X %04X", i+8,
			(int) (regs.r[i+8]>>16 & 0x7ff),
			(int) (regs.r[i+8] & 0xffff));
		Vprintf ("  ИС%X %03X %04X", i,
			(int) (regs.s[i]>>16 & ((i == 4 || i == 5) ? 0xfff : 0x7ff)),
			(int) (regs.s[i] & 0xffff));
	}
	VMove (r + 9, c + 4);
	Vprintf ("ВР  %04X %04X %04X %04X", regs.i.i[3], regs.i.i[2],
					    regs.i.i[1], regs.i.i[0]);
	VMove (r + 11, c + 4);
	Vprintf ("СМ  %04X %04X %04X %04X", regs.a.i[3], regs.a.i[2],
					    regs.a.i[1], regs.a.i[0]);
	VMove (r + 11, c + 29);
	Vprintf ("РМР  %04X %04X %04X %04X", regs.y.i[3], regs.y.i[2],
					     regs.y.i[1], regs.y.i[0]);
	VMove (r + 13, c + 7);
	Vprintf ("    6         5         4         3         2         1         ");
	VMove (c + 14, c + 7);
	Vprintf ("4321098765432109876543210987654321098765432109876543210987654321");
	VMove (c + 15, c + 4);
	Vprintf ("СМ ");
	for (i = 31; i >= 0; i--)
		VPutChar (regs.a.l[1] >> i & 1 ? '1' : '0');
	for (i = 31; i >= 0; i--)
		VPutChar (regs.a.l[0] >> i & 1 ? '1' : '0');
	hidecursor ();
	VSync ();
	} while (KeyGet () != cntrl ('['));
	VUngetBox (RBox);
	VFreeBox (RBox);
}

static union word Get_Inp ()
{
	union word i;

	/* load input register */
	i.l[0]  = (long) (pt_getb (0, 040, 8) & 0xF0) >> 4 ;
	i.l[0] |= (long) (pt_getb (0, 041, 8) & 0xF0)      ;
	i.l[0] |= (long) (pt_getb (0, 042, 8) & 0xF0) << 4 ;
	i.l[0] |= (long) (pt_getb (0, 043, 8) & 0xF0) << 8 ;
	i.l[0] |= (long) (pt_getb (0, 044, 8) & 0xF0) << 12;
	i.l[0] |= (long) (pt_getb (0, 045, 8) & 0xF0) << 16;
	i.l[0] |= (long) (pt_getb (0, 046, 8) & 0xF0) << 20;
	i.l[0] |= (long) (pt_getb (0, 047, 8) & 0xF0) << 24;
	i.l[1]  = (long) (pt_getb (0, 050, 8) & 0xF0) >> 4 ;
	i.l[1] |= (long) (pt_getb (0, 051, 8) & 0xF0)      ;
	i.l[1] |= (long) (pt_getb (0, 052, 8) & 0xF0) << 4 ;
	i.l[1] |= (long) (pt_getb (0, 053, 8) & 0xF0) << 8 ;
	i.l[1] |= (long) (pt_getb (0, 054, 8)       ) << 16;
	i.l[1] |= (long) (pt_getb (0, 055, 8)       ) << 24;
	return i;
}

static union word Get_Acc ()
{
	union word a;
	register unsigned char c;

	/* load accumulator */
	a.l[0] = pt_getb (0, 0, 6);
	a.l[0] |= (long) pt_getb (0, 1, 6) << 6;
	a.l[0] |= (long) pt_getb (0, 2, 6) << 12;
	a.l[0] |= (long) pt_getb (0, 3, 6) << 18;
	a.l[0] |= (long) pt_getb (0, 4, 6) << 24;
	a.l[0] |= (long) (c = pt_getb (0, 5, 6)) << 30;
	a.l[1] = c >> 2;
	a.l[1] |= (long) pt_getb (0, 6, 6) << 4;
	a.l[1] |= (long) pt_getb (0, 7, 6) << 10;
	a.l[1] |= (long) pt_getb (0, 033, 5) << 16;
	a.l[1] |= (long) pt_getb (0, 075, 3) << 21;
	a.l[1] |= (long) pt_getb (0, 076, 8) << 24;
	return a;
}

union word Get_Young ()
{
	union word y;
	register long a, b;
	register i;

	y.l[0] = y.l[1] = 0;
	a = pt_getl (0, 060);
	b = pt_getl (0, 064);
	for (i = 0; i < 16; i++) {
		if (a & ymask1 [i])
			y.l[0] |= ypat1 [i];
		if (a & ymask2 [i])
			y.l[1] |= ypat1 [i];
		if (b & ymask1 [i])
			y.l[0] |= ypat2 [i];
		if (b & ymask2 [i])
			y.l[1] |= ypat2 [i];
	}
	return y;
}

void Load_Regs (r)
register struct ioregs *r;
{
	register i;
	register long h;

	/* enable access to cpu registers */
	stop ();
	pt_enable ();
	/* load registers */
	for (i = 0; i < 16; i++)
		r->r[i] = pt_getl (i, 0130);
	/* load special registers */
	for (i = 0; i < 8; i++)
		r->s[i] = pt_getl (i + 030, 0130);
	/* load input register */
	r->i = Get_Inp ();
	/* load accumulator */
	r->a = Get_Acc ();
	/* load young */
	r->y = Get_Young ();
	/* load command register */
	r->cr = pt_getl (0, 0120);
	/* load address of next command */
	h = pt_getl (0, 0150);
	r->pc = h & 0177 | h >> 1 & 037600l | h >> 2 & 07740000l | h >> 3 & 01770000000l;
	/* disable access to cpu registers */
	if (stopped == 0)
		run ();
}

extern char *batch_name[];
static wid_bs, row_bs, col_bs;
BOX     *state_box;

static  char *cause[] = {
/* 0 */ "Выполнение\04000:00:00",
/* 1 */	"Неизвестный останов",
/* 2 */	"У0",
/* 3 */	"Останов по ЗП",
/* 4 */	"Останов по СЧ",
/* 5 */	"Сбой опр. ост. РР",
/* 6 */ "Останов по КРА",
/* 7 */	"Одиночный режим",
/* 8 */ "Команда Стоп",
/* 9 */	"Сбой опр. ост. РК",
	NULL,
};

extern unsigned char CAC_Name [];
extern int loaded_sas, max_batch_n;

/* k - Индекс по массиву строк с именами заданий, если 0 - то
   по содержимому 0 и 0x3e000 ячеек ВКБ */

static void Change_NameB (k) {

	char   DVS_Name [9], *s, c;
	register bad_mem = 0;

	VStandEnd ();
	VSetDim ();
	VDrawBox (row_bs, col_bs, 3, wid_bs + 2);
	VSetNormal ();

BEGIN :

	if (k >= max_batch_n) {
		bad_mem = 1;
		s = " ??? ";
	}
	else if (k >= 0) {
		s = batch_name[k];
	}
	else {
		union  word W_0, batch_wb;
		register int i;

		if (pt_load (&W_0.l[1], &W_0.l[0], 0x36l, &c, 1)) {
			/* error ("Ошибка при чтении адреса: %5lX", 0x36l);
			return ; */
			bad_mem = 1;
		} /* else */ {
			if (memcmp (CAC_Name, W_0.c, 8) != 0) {
				for (i = 0; i < 8; i++)
					DVS_Name[i] = toalt (W_0.c[7 - i]);
				DVS_Name [8] = '\0';
				s = DVS_Name;
			} else if (loaded_sas) {
				/* выполняется САС */
				if (pt_load (&batch_wb.l[1], &batch_wb.l[0], 0x3E000l, &c, 1)) {
					/* error ("Ошибка при чтении адреса: %5lX", 0x3E000l);
					return ; */
					k = max_batch_n;
					goto BEGIN ;
				} else {
					k = batch_wb.i[0];
					goto BEGIN ;
				}
			} else
				s = NULL;
		}
	}
	if (s != NULL) {
		if (bad_mem) {
			VStandOut ();
			VSetDim ();
		}
		VMPutString (row_bs, (col_bs + 1 + (wid_bs - strlen(s)) / 2), s);
	}
}

extern long start_time;

static void Change_StateB () {

	register char   *state;
	long	 cur_time, work_time;
	register hour, mins, sec;

	if (stopped) {
		VSetBold ();
		VBeep ();
	}
	else
		VSetDim ();
	VStandOut ();
	VFillBox (row_bs + 1, col_bs + 1, 1, wid_bs, ' ');
	state = cause[stopped];
	if (stopped == 0) {
		/* Выдача времени работы процессора */
		time (& cur_time);
		work_time = cur_time - start_time;
		mins = work_time / 60;
		hour = mins / 60;
		mins %= 60;
		sec  = work_time % 60;
		sprintf( &state[10], "%3d:%02d:%02d", hour, mins, sec);
	}
	VMPutString (row_bs + 1, (col_bs + 1 + (wid_bs - strlen(state)) / 2), state);
	VSetNormal ();
	VStandEnd ();
}

extern void getboxes (), drawmenu ();

void Upd_StateB (k) {

	CURSOR Old; short  Attr;
	register struct menu *m;

	Old = VGetCursor (); Attr = VGetAttr ();

	m = &menu[nmenu];
	if (m -> box_on) {
		VUngetBox (m->box);
	}
	Change_NameB (k);
	Change_StateB ();
	if (m -> box_on) {
		getboxes ();
		drawmenu (m);
	}
	VSetCursor (Old); VSetAttr (Attr);
	VSync ();
}

void Draw_StateB () {

	register i;

	wid_bs = 0;

	for (i = 0; batch_name[i] != NULL; i++)
		wid_bs = max (wid_bs, (int) strlen (batch_name[i]));

	for (i = 0; cause[i] != NULL; i++)
		wid_bs = max (wid_bs, (int) strlen (cause[i]));

	row_bs = 1;
	col_bs = COLS - wid_bs - 3;
	state_box = VGetBox (row_bs, col_bs, 3, wid_bs + 2);
	Change_NameB (-1);
	time (& start_time);
	Change_StateB ();
# ifdef MSDOS
	_disable ();
	int_tab [0].ticks = TICK_CHK;
	_enable ();
# endif
}

