# include <stdio.h>
# ifdef MSDOS
# include <dos.h>
# include <conio.h>
# else
# include <memory.h>
# endif
# include <string.h>
# include "intr.h"
# include "key.h"
# include "scr.h"
# include "choice.h"
# include "wkb.h"
# include "menu.h"
# include "ut.h"

# define CHK_COUNT 10

# ifdef MSDOS
# pragma check_stack (off)
# pragma check_pointer (off)
long far *pcur_tick = (long far *)0x0000046cL;
void (interrupt far *old_timer)( void );
# endif
int stopped;
extern void State_B (), Upd_StateB ();

void a_stop (), a_reset_U_Chan_done ();

struct int_h int_tab [] = {
	{0, 0, 0l, a_stop,},
	{0, 0, 0l, a_reset_U_Chan_done,},
};

# ifdef MSDOS
void interrupt far timer_int () {

	static struct int_h *p;
	static long cur_tick;
	static i;

	for (cur_tick = *pcur_tick, p = int_tab, i = 0; i < MAX_H; p++, i++)
		if (cur_tick > p -> wait_tick && p -> ticks) {
			_disable ();
			p -> flag = 1;
			_enable ();
			p -> wait_tick = cur_tick + p -> ticks;
		}
}
# endif

void inst_timer () {
# ifdef MSDOS
	old_timer = _dos_getvect (0x1C);
	_dos_setvect (0x1C, timer_int);
# pragma check_stack ()
# pragma check_pointer ()
# endif
}

extern void drawmenu (), Load_Regs ();
extern int max_batch_n;
extern char *batch_name[];
extern struct ioregs regs;
extern unsigned char CAC_Name [];

void a_reset_U_Chan_done () {

	union word w_24;
	CURSOR Old; short  Attr;
	char c;

	pt_load (&w_24.l[1], &w_24.l[0], 0x24l, &c, 0);

	if (w_24.i[0] == 0) {
		Old = VGetCursor (); Attr = VGetAttr ();
# ifdef MSDOS
		_disable ();
		int_tab[1].ticks   = 0;
		int_tab[1].flag	   = 0;
		_enable ();
# endif
		pmessage ("Сброс универсального канала", "%s", "ВЫПОЛНЕНО");
		VSetCursor (Old); VSetAttr (Attr);
		VSync ();
	}
}

void a_stop () {

	static int old_stopped = 0;
	static unsigned long old_rk = 0, old_sthas = 0, old_rr = 0;
	unsigned long addr, rk, rr, sthas, athp;
	unsigned int  reg, op;
		 int  altf = 0;
	unsigned tblwk, tostrr, wklath, zpath, zpprr, sthprr,
		 tostrk, stprk, tor, wyklo, i;

	/* Проверка СКП */
	if ((pt_getb (0, 0202, 7) & 0100) == 0)
		stopped = 0;

	for (i = 0, old_stopped = stopped; i < CHK_COUNT; i++) {
		rk = pt_getl (0, 0120);
		rr = pt_getl (0, 0160) & 0xfffffffl;
		sthas = pt_getl (0, 0150) & 0xfffffffl;

		if ((old_rk != rk) || (old_sthas != sthas) || (old_rr != rr)) {
			old_sthas = sthas, old_rk = rk, old_rr = rr;
			stopped = 0;
			break;
		} else
			stopped = 1;
	}

	/* Проверка СКП */
	if ((pt_getb (0, 0202, 7) & 0100) == 0)
		stopped = 0;

# define sbr(a)	(a & 0177 | a >> 1 & 037600l | a >> 2 & 07740000l |\
a >> 3 & 01770000000l)

	/* Разбор причины останова */
	if (stopped > 0) {
		athp = pt_getl (0, 0144) & 0xfffffffl;
		sthas= sbr (sthas);
		rr   = sbr (rr);
		athp = sbr (athp);
		addr = rk & 0xFFFFFl; /* исполнителный адрес */
		op   = (int) (rk >> 20) & 0xFF; /* код операции */
		reg  = (int) (rk >> 28) & 0xF; /* индексный регистр */
		if (op == 0x3F) {
			altf = 1;      /* команда дополнительного формата */
			op   = (int) (addr >> 12) & 0xFF;
			addr &= 0xFFFl;
		}
		i = pt_getb (0, 0176, 8);
# define	bit(w,shift)	(((w) >> (shift)) & 1)
		tblwk = bit (i, 4);
		tostrr = bit (i, 5);
		i = pt_getb (0, 0352, 6);
		wklath = bit (i, 1);
		wyklo  = bit (i, 3);
		zpath  = bit (i, 2);
		i = pt_getb (0, 0174, 8);
		sthprr = bit (i, 7);
		zpprr  = bit (i, 6);
		i = pt_getb (0, 0111, 8);
		tostrk = bit (i, 6);
		i = pt_getb (0, 0113, 7);
		stprk  = bit (i, 2);
		i = pt_getb (0, 0351, 8);
		tor    = bit (i, 0);
		/* 1 - Неизвестный останов */
		if (tblwk /* ТБЛВК */ && sthas == 1 /* СЧАС */)
			stopped = 2;
		else if (tostrr /* ТОСТР */) {
			if (wklath /* ВКЛАЧ */) {
				if (rr /* РР */ != athp /* АЧП */)
					goto BAD_RR;
				if (zpath /* ЗПАЧ */ && zpprr /* ЗППРР */)
					stopped = 3;
				else if (! zpath /* ЗПАЧ */ && sthprr /* СЧПРР */)
					stopped = 4;
				else
					goto BAD_RR;
			} else
BAD_RR: 			stopped = 5;
		} else if (tostrk /* ТОСТРК */) {
			if (stprk /* СТПРК */)
				stopped = 6;
			else if (tor /* ТОР */)
				stopped = 7;
			else if (wyklo /* ВЫКЛО */ == 0)
				if (altf && op == 0x07)
					stopped = 8;
				else
					stopped = 9;
			else
				stopped = 9;
		}
	}

	if (old_stopped != stopped || stopped == 0)
		Upd_StateB (-1);

	if (stopped > 0) {

		char buf [100];
		int i;
		CURSOR Old; short Attr;

# ifdef MSDOS
		_disable ();
		int_tab[0].ticks   = 0;
		int_tab[0].flag	   = 0;
		_enable ();
# endif

		Old = VGetCursor (); Attr = VGetAttr ();

		if ( stopped == 8 ) {

		char buf1[100], c, name[20], *s;
		unsigned n;
		union  word batch_wb, W_0;

		if (pt_load (&W_0.l[1], &W_0.l[0], 0x36l, &c, 1)) {
			error ("Ошибка при чтении адреса: %5lX", 0x36l);
			return ;
		} else if (memcmp (CAC_Name, W_0.c, 8) == 0) {
			/* САС остановил процессор */
			if (pt_load (&batch_wb.l[1], &batch_wb.l[0], 0x3E000l, &c, 1)) {
				error ("Ошибка при чтении адреса: %5lX", 0x3E000l);
				n = max_batch_n;
			} else {
				n = batch_wb.i[0];
				if ((unsigned) n >= (unsigned) max_batch_n) {
					error ("Неверный номер задания.");
					n = max_batch_n;
				}
			}
		}
		s = batch_name [n];
		if (s == NULL)
			s = " ??? ";
		Upd_StateB (n);
		VSync ();
		switch ((int) addr) {
		case 0xCAC :
			sprintf (buf, " %s : ВЫПОЛНЕНО.", s);
			i = getchoice (0, " Останов САС ", buf, NULL, " Да ", NULL, NULL);
			goto RETURN ;
		case 0xBAD :
			sprintf (buf, " %s : НЕ ВЫПОЛНЕНО.", s);
			Load_Regs (&regs);
			if (regs.i.i[0] == 0) {
				sprintf (buf1, "Внутренняя ошибка 0 (СМ = %04X %04X %04X %04X)",
						regs.a.i[3], regs.a.i[2], regs.a.i[1], regs.a.i[0]);
			} else {
				switch (regs.i.i[0]) {
				case 0xFFFF:
					sprintf (buf1, "Неверный номер работы (N = %1i)", regs.a.i[0]);
					break;
				case 0x11 :
				case 0x12 :
				case 0x13 :
				case 0x14 :
				case 0x15 :
				case 0x16 :
				case 0x17 :
				case 0x18 :
				case 0x19 :
				case 0x1A :
				case 0x1B :
				case 0x1C :
				case 0x1D :
				case 0x1E :
				case 0x1F :
					sprintf (buf1, "Ошибка при обмене с МД (БЛОК = %1lXH)", regs.a.l[0]);
					break;
				case 0x10 :
				case 0x20 :
				case 0x30 :
					sprintf (buf1, "Неверная операция (ОП = %1i)", regs.a.i[0]);
					break;
				case 0x21 :
					sprintf (buf1, "Плохой размер блока (L = %1lXH)", regs.a.l[0]);
					break;
				case 0x22 :
					sprintf (buf1, "Плохое ССК (ССК %04X %04X %04X %04X)",
						regs.a.i[3], regs.a.i[2], regs.a.i[1], regs.a.i[0]);
					break;
				case 0x23 :
					sprintf (buf1, "Плохой байт состояния (ССК %04X %04X %04X %04X)",
						regs.a.i[3], regs.a.i[2], regs.a.i[1], regs.a.i[0]);
					break;
				case 0x24 :
					sprintf (buf1, "Ошибка при перемотке (ССК %04X %04X %04X %04X)",
						regs.a.i[3], regs.a.i[2], regs.a.i[1], regs.a.i[0]);
					break;
				case 0x25 :
					sprintf (buf1, "Ошибка при файл вперед (ССК %04X %04X %04X %04X)",
						regs.a.i[3], regs.a.i[2], regs.a.i[1], regs.a.i[0]);
					break;
				case 0x26 :
					sprintf (buf1, "Ошибка обмена (БУС | аддр_оп %04X %04X %04X %04X)",
						regs.a.i[3], regs.a.i[2], regs.a.i[1], regs.a.i[0]);
					break;
				case 0x27 :
					sprintf (buf1, "Ошибка при установке плотности (ССК %04X %04X %04X %04X)",
						regs.a.i[3], regs.a.i[2], regs.a.i[1], regs.a.i[0]);
					break;
				case 0x28 :
					sprintf (buf1, "Ошибка при стирании (ССК %04X %04X %04X %04X)",
						regs.a.i[3], regs.a.i[2], regs.a.i[1], regs.a.i[0]);
					break;
				case 0x31 :
					sprintf (buf1, "Плохая версия метки тома (МЕТКА = %i)", regs.a.i[0]);
					break;
				case 0x32 :
				case 0x41 :
					strcpy (name, "        ????");
					for (i = 0; i < 8; i++)
						name [i] = toalt (regs.a.c [7 - i]);
					sprintf (buf1, "Раздел не найден (ИМЯ: %s)", name);
					break;
				case 0x33 :
					sprintf (buf1, "Длина раздела (ДЛИНА = %i) меньше заданного", regs.a.i[0]);
					break;
				default:
					sprintf (buf1, "ВР: %04X %04X %04X %04X СМ: %04X %04X %04X %04X",
							regs.i.i[3], regs.i.i[2], regs.i.i[1], regs.i.i[0],
							regs.a.i[3], regs.a.i[2], regs.a.i[1], regs.a.i[0]);
				}
			}
			i = getchoice (1, " Останов САС ", buf, buf1, " Да ", NULL, NULL);
			goto RETURN ;
		}
		} else if (stopped == 2)
			goto RETURN ;

		/* сам встал */
		sprintf (buf, altf ? "%s %1.1X 3F %2.2X %3.3lX" :
				     "%s %1.1X %2.2X %5.5lX"    ,
			  "Процессор остановлен РК: ", reg, op, addr);

		if (getchoice (1, " Останов ", buf, "Будете смотреть регистры ?", " Да ", " Нет ", NULL) == 0)
			State_B ();
RETURN :
		VSetCursor (Old); VSetAttr (Attr);
		VSync ();
	}
}

# ifdef MSDOS
int dispatch () {

	register struct int_h *p;
	register i;

	if (kbhit ())
		return 1;

	for (p = int_tab, i = 0; i < MAX_H; p++, i++)
		if (p -> flag) {
			_disable ();
			p -> flag    = 0;
			_enable ();
			(*p -> exec) ();
		}
	return 0;
}
# endif

