# include <stdio.h>
# ifdef MSDOS
# include <stdlib.h>
# include <dos.h>
# include <io.h>
# include <conio.h>
# else
# include <errno.h>
extern  char *sys_errlist[], *malloc ();
# include <sys/pult.h>
# endif
# include <string.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <time.h>
# include "wkb.h"
# include "choice.h"
# include "intr.h"
# include "ut.h"

long start_time;
unsigned pt_port, pt_port1, pt_port2;

# ifndef MSDOS
# define WORDSZ         sizeof (struct ptword)
int   f_pult;
typedef int size_t;
# else
unsigned long old_addr = -1;
# endif

# ifdef MSDOS
pt_parity (w, half_tag)
long w;
{
	register short s;

	w ^= half_tag; /* Учет тега */
	/* compute parity of word w, return 1 if even */
	s = (short) (w >> 16);
	s ^= (short) w;
	s ^= s >> 8;
	s ^= s >> 4;
	s ^= s >> 2;
	s ^= s >> 1;
	return ((s ^ 1) & 1);
}

void pt_sendl (h, cmd)
register long h;
{
	register union {
		 unsigned long h;
		 unsigned char c[4];
	} w;

	register unsigned char *c;

	w.h = h; c = &w.c[3];

	/* send word h to Elbrus-B */
	pt_send (*c--, cmd);
	pt_send (*c--, cmd);
	pt_send (*c--, cmd);
	pt_send (*c  , cmd);
}
# else
void pt_sendl (h, cmd)
register long h;
{
	ioctl (f_pult, PTSENDL (cmd), (char far *) h);
}
# endif

# ifdef MSDOS
void pt_addr (a)
register unsigned long a;
{
	if (a == old_addr)
		return;

	old_addr = a;
	/* compute address word */
	a |= (long) pt_parity (a & 0x40003ffL, 0) << 27;
	a |= (long) pt_parity (a & 0x07ffc00L, 0) << 28;

	/* send address */
	pt_sendl (a, BADDR);
}
# endif

# ifdef MSDOS
void pt_store (l, r, a, t, nt)
unsigned long l, r;
register unsigned long a;
{
	/* send word (l:r) to elbrus-b to address a */

	/* compute tag byte if needet */
	if (nt)
		t = pt_parity (l, 0) << 6 | pt_parity (r, 0) << 5;

	/* set address word */
	pt_addr (a & ADDRMASK | 0x4000000L);

	/* send tag byte */
	pt_send (t, BWRWORD);

	/* send word */
	pt_sendl (l, BWRWORD);
	pt_sendl (r, BWRWORD);

	/* start memory transaction (УГАПМ) */
	pt_data (1);
	pt_putc (BBUTTON3);
}
# else
void pt_store (l, r, a, t, nt)
unsigned long l, r;
register unsigned long a;
{
	struct ptword w;

	w.left = l;
	w.right = r;
	lseek (f_pult, a*8L, 0);
	write (f_pult, (char *) &w, WORDSZ);
}
# endif

unsigned long fgeth (f)
register FILE *f;
{
	union {
		 unsigned long h;
		 unsigned char c[4];
	} w;

	register unsigned char *c = &w.c[3];

	*c-- = (unsigned char) getc (f);
	*c-- = (unsigned char) getc (f);
	*c-- = (unsigned char) getc (f);
	*c   = (unsigned char) getc (f);
	return w.h;
}

unsigned long fgeth16 (f)
register FILE *f;
{
	union {
		 unsigned long h;
		 unsigned char c[4];
	} w;

	register unsigned char *c = &w.c[3];

	*c-- = (unsigned char) getc (f);
	*c-- = (unsigned char) getc (f);
	*c-- = (unsigned char) 0;
	*c   = (unsigned char) 0;
	return w.h;
}

# define fgetword(f, h) { \
	(h) -> tag = getc ( (f) ); \
	(h) -> left = fgeth ( (f) ); \
	(h) -> right = fgeth ( (f) ); \
}

int put_tst_file (name, flag_name)
char *name;
{
	register long addr, len;
	long	 st_addr, st_len;
	time_t	 b_time, e_time;

	struct wordb word;

	register struct wordb *w = &word;
	char *buf, *base_name;
	size_t bufsize;
	register FILE *f;

	if ((f = fopen (name, "r")) == NULL) {
		error ("%s : %s", name, sys_errlist[errno]);
		return 0;
	}

	time (&b_time);
	for (bufsize = 0x7FFF; ; bufsize -= 16)
		if ((buf = malloc(bufsize)) != NULL) {
			bufsize--;
			break;
		}

	setvbuf(f, buf, _IOFBF, bufsize );

	fgetword (f, w);
	st_addr = addr = w->left;
	st_len	= len  = w->right;

	if (flag_name) /* Если загрузка САС, то не переспрашивать адрес загрузки файла */
	if ((st_addr = addr = editval (10, "Загрузка TST-файла",
				      "Введите адрес загрузки TST-файла",
				      "%lX", addr)) == -1l) {
		fclose (f);
		free (buf);
		return 0;
	}
	base_name = strdup (make_base_name (name));

	bmessage (" Загрузка ", "Файл '%s' адрес %7.7lXH длина %5.5lXH загружено %3dК",
		 base_name, addr, len, 0);
	for (; --len>=0; ++addr) {
		fgetword (f, w);
		pt_store (w->left, w->right, addr, w->tag, 0);
		if (! (addr % 512))
			bmessage (" Загрузка ", "Файл '%s' адрес %7.7lXH длина %5.5lXH загружено %3dК",
				 base_name, addr, len, (int) ((addr - st_addr + 127) / 128));
	}

	if (fclose (f) == EOF) {
		error ("%s : %s", name, sys_errlist[errno]);
		free (buf); free (base_name);
		return 0;
	}
	free (buf);
	time (&e_time);
{
	char msg1 [100], msg2[100];

	endmesg ();
	sprintf (msg1, "Файл '%s' загружен, %5.5lX слов (%dК)",
		 base_name, st_len, (int) ((addr - st_addr + 127) / 128));
	sprintf (msg2, " с адреса %7.7lX, за время %d", st_addr, (int) (e_time - b_time));
	getchoice (0, " Загрузка ", msg1, msg2, NULL, NULL, NULL);
}
	free (base_name);
	return 1;
}

int put_any_file (name)
char *name;
{
	register long addr, len;
	long     st_addr, st_len;
	time_t   b_time, e_time;
	unsigned long left, right;

	char *buf, *base_name;
	register i;
	size_t bufsize;
	struct stat stat_buf;
	register FILE *f;

	if ((f = fopen (name, "r")) == NULL) {
		error ("%s : %s", name, sys_errlist[errno]);
		return 0;
	}

	time (&b_time);
	for (bufsize = 0x7FFF; ; bufsize -= 16)
		if ((buf = malloc(bufsize)) != NULL) {
			bufsize--;
			break;
		}

	setvbuf(f, buf, _IOFBF, bufsize );

	addr    = 0x100000l;
	if ((st_addr = addr = editval (10, "Загрузка файла",
				      "Введите адрес загрузки файла",
				      "%lX", addr)) == -1l) {
		fclose (f);
		free (buf);
		return 0;
	}
	base_name = strdup (make_base_name (name));

	st_len	= len  = ((fstat(fileno(f), &stat_buf) < 0 ? 0 :
					    stat_buf.st_size) + 7) / 8;

	bmessage (" Загрузка ", "Файл '%s' адрес %7.7lXH длина %5.5lXH загружено %3dК",
		 base_name, addr, len, 0);
	for (; --len>=0; ++addr) {
		left = fgeth (f);
		right = fgeth (f);
		pt_store (left, right, addr, 0, 1);
		if (! (addr % 512))
			bmessage (" Загрузка ", "Файл '%s' адрес %7.7lXH длина %5.5lXH загружено %3dК",
				 base_name, addr, len, (int) ((addr - st_addr + 127) / 128));
	}

	if (fclose (f) == EOF) {
		error ("%s : %s", name, sys_errlist[errno]);
		free (buf); free (base_name);
		return 0;
	}
	free (buf);
	time (&e_time);
{
	char msg1 [100], msg2[100];

	endmesg ();
	i = (int) ((addr - st_addr + 127) / 128);
	sprintf (msg1, "Файл '%s' загружен, %5.5lX слов (%d,%XH К)",
		 base_name, st_len, i, i);
	sprintf (msg2, " с адреса %7.7lX. За время %d", st_addr, (int) (e_time - b_time));
	getchoice (0, " Загрузка ", msg1, msg2, NULL, NULL, NULL);
}
	free (base_name);
	return 1;
}

# ifdef MSDOS
int pt_load (pl, pr, a, pt, nt)
unsigned long *pl, *pr;
unsigned char *pt;
register unsigned long a;
{
	unsigned char t, tt, t1;
	unsigned long l, r;

	/* get word (l:r) from elbrus-b from address a */

	/* set address word */
	pt_addr (a & ADDRMASK);

	/* start memory transaction (УГАПМ) */
	pt_data (1);
	pt_putc (BBUTTON3);

	/* get word */
	l = pt_getl (0, 0214);
	r = pt_getl (0, 0210);

	/* get tag byte */
	t = pt_getb (0, 0240, 7);

	*pl = l;
	*pr = r;
	*pt = t;

	if (nt) {
		t1 = (unsigned char) (((unsigned) t >> 5) & 03);
		/* compute tag byte if needet */
		tt = (unsigned char) (pt_parity (l, t & 030) << 1 |
				      pt_parity (r, t & 007)	   );

		return (t1 != tt);
	} else
		return 0;
}
# else
int pt_load (pl, pr, a, pt, nt)
unsigned long *pl, *pr;
unsigned char *pt;
register unsigned long a;
{
	struct ptword w;

	lseek (f_pult, a*8L, 0);
	read (f_pult, (char *) &w, WORDSZ);
	*pl = w.left;
	*pr = w.right;
	*pt = 0;

	return 0; /* всегда все ок */
}
# endif

# ifdef MSDOS
unsigned long pt_getl (ia, ib)
int ia, ib;
{
	union {
		unsigned long l;
		unsigned int  i[2];
		unsigned char c[4];
	} regl;

	unsigned register char c, *pc;
	register i, j;

	pc = &regl.c[3];

	pt_data(ia)
	pt_putc(BINDEX);

	for (i = 3; i >= 0; i--)
		pt_get(ib + i, 8, *pc--);

	return regl.l;
}
# else
unsigned long pt_getl (ia, ib)
int ia, ib;
{
	long h;

	h = ia;
	ioctl (f_pult, PTRECVL (ib), (char far *) &h);
	return (h);
}
# endif

# ifdef MSDOS
unsigned char pt_getb (ia, ib, bits)
int ia, ib, bits;
{

	register unsigned char c, c1;
	register j;

	pt_data(ia)
	pt_putc(BINDEX);

	pt_get(ib, bits, c1);

	return c1;
}
# else
unsigned char pt_getb (ia, ib, bits)
int ia, ib, bits;
{
	unsigned char c;

	c = ib;
	ioctl (f_pult, PTRECV (ia), (char far *) &c);
	return (c & (unsigned char) (~(0xff << bits)));
}
# endif

void fputh (value, f)
register FILE *f;
unsigned long value;
{
	union {
		 unsigned long h;
		 unsigned char c[4];
	} w;

	register unsigned char *c = &w.c[3];

	w.h = value;

	putc ((int) *c--, f);
	putc ((int) *c--, f);
	putc ((int) *c--, f);
	putc ((int) *c	, f);
}

void fputh16 (value, f)
register FILE *f;
unsigned long value;
{
	union {
		 unsigned long h;
		 unsigned char c[4];
	} w;

	register unsigned char *c = &w.c[3];

	w.h = value;

	putc ((int) *c--, f);
	putc ((int) *c	, f);
}

# define fputword(h, f) { \
	putc  ( (int) (h) -> tag, (f)); \
	fgeth ( (h) -> left, (f)); \
	fgeth ( (h) -> right, (f)); \
}

int get_any_file (name)
char *name;
{
	register long addr, len;
	long     st_addr, st_len;
	time_t   b_time, e_time;
	unsigned long left, right; unsigned char c;

	char *buf, *base_name;
	register i;
	size_t bufsize;
	register FILE *f;

	if ((f = fopen (name, "w")) == NULL) {
		error ("%s : %s", name, sys_errlist[errno]);
		return 0;
	}

	time (&b_time);
	for (bufsize = 0x7FFF; ; bufsize -= 16)
		if ((buf = malloc(bufsize)) != NULL) {
			bufsize--;
			break;
		}

	setvbuf(f, buf, _IOFBF, bufsize );

	addr	= 0x100000l; len = 0l;
	if ((st_addr = addr = editval (10, "Создание файла",
				      "Введите начальный адрес памяти",
				      "%lX", addr)) == -1l) {
		fclose (f);
		free (buf);
		return 0;
	}

	if ((st_len = len  = editval (10, "Создание файла",
				      "Введите количество слов",
				      "%lX", len)) <= 0l) {
		fclose (f);
		free (buf);
		return 0;
	}
	base_name = strdup (make_base_name (name));

	bmessage (" Создание ", "Файл '%s' адрес %7.7lXH длина %5.5lXH создается %3dК",
		 base_name, addr, len, 0);
	for (; --len>=0; ++addr) {
		pt_load (&left, &right, addr, &c, 0);
		fputh (left, f);
		fputh (right, f);
		if (! (addr % 512))
			bmessage (" Создание ", "Файл '%s' адрес %7.7lXH длина %5.5lXH создано %3dК",
				 base_name, addr, len, (int) ((addr - st_addr + 127) / 128));
	}

	if (fclose (f) == EOF) {
		error ("%s : %s", name, sys_errlist[errno]);
		free (buf); free (base_name);
		return 0;
	}
	free (buf);
	time (&e_time);
{
	char msg1 [100], msg2[100];

	endmesg ();
	i = (int) ((addr - st_addr + 127) / 128);
	sprintf (msg1, "Файл '%s' создан, %5.5lX слов (%d,%XH К)",
		 base_name, st_len, i, i);
	sprintf (msg2, " с адреса %7.7lX. За время %d", st_addr, (int) (e_time - b_time));
	getchoice (0, " Создание ", msg1, msg2, NULL, NULL, NULL);
}
	free (base_name);
	return 1;
}

void stop ()
{
	/* enable step-by step mode (ОР) */
	pt_send (1, BSTEP);
}

void zero ()
{
	if (stopped == 0) { /* Процессор работает */
		if (! ask_are_you_shure (" Процессор работает !") )
			return ;
	}
	/* reset processor (У0 | У0С) */
	pt_send (3, BBUTTON1);
# ifdef MSDOS
	old_addr = -1;
# endif
}

void pt_enable ()
{
	/* enable reading registers (ВКИ) */
	pt_send (3, BSTEP);
}

void run ()
{
	/* disable step-by step mode (ОР) */
	pt_send (0, BSTEP);
	/* start execution (ПУСК) */
	pt_send (010, BBUTTON1);
	time (& start_time);
# ifdef MSDOS
	_disable ();
	int_tab [0].ticks = TICK_CHK;
	_enable ();
# endif
}
extern int  loaded_sas;

void rrun ()
{
	struct wordb word;

	if (! loaded_sas)
		if (! pt_load (&word.left, &word.right, 0x3E010l, &word.tag, 1))
			if ((word.left & 0xFFFFF000l) == 0x03B3E000l)
				loaded_sas = 1;

	run ();
}

int open_part (part_name, begin, len)
char * part_name;
unsigned * begin, * len;
{
	char c, *p, t_name[13];
	unsigned ver = 0, t_ver;
	int i;
	union word w;
	unsigned long index, dummy, n_index,
	a1, /* 8 символов имени */
	a2, /* 4 символа имени, версия раздела, начальный блок */
	a3, /* длина, ссылка на следующий */
	a4; /* признаки */

	if ((p = strchr (part_name, '/')) != NULL) {
		sscanf(&p[1], "%3d", &ver);
		if (ver < 0 || ver > 255) {
			error ("Недопустимый номер поколения : %d", ver);
			return -1;
		}
		*p = '\0';
	}
	pt_load (&dummy, &index,0x38l, &c, 0);
	pt_load (&w.l[1], &w.l[0], 0x39l, &c, 0);
	a1 = w.l[0];
	pt_load (&w.l[1], &w.l[0], 0x3Al, &c, 0);
	a2 = w.l[0];
	pt_load (&w.l[1], &w.l[0], 0x3Bl, &c, 0);
	a3 = w.l[0];
	pt_load (&w.l[1], &w.l[0], 0x3Cl, &c, 0);
	a4 = w.l[0];
NEXT :
	/* Поиск раздела по таблице */
	pt_load (&w.l[1], &w.l[0], a1 + index, &c, 0);
	for (i = 0; i < 8; i++)
		t_name[i] = toalt (w.c[7 - i]);
	t_name [8] = '\0';
	pt_load (&w.l[1], &w.l[0], a2 + index, &c, 0);
	for (i = 0; i < 4; i++)
		t_name[i + 8] = toalt (w.c[7 - i]);
	t_name [12] = '\0';
	t_ver = (unsigned) w.c[3];
	* begin = (unsigned) w.l[0] & 0x1FFFFFl;
	pt_load (&w.l[1], &w.l[0], a3 + index, &c, 0);
	* len	= (unsigned) w.l[0] & 0x1FFFFFl;
	n_index = ((w.l[0] >> 24) & 0xFF) | ((w.l[1] << 8) & 0xFFF00);
	if (strcmp (part_name, t_name) == 0) {
		if (ver == 0) { /* раздел должен быть рабочим */
			pt_load (&w.l[1], &w.l[0], a4 + index, &c, 0);
			if ((w.l[1] >> 31) != 0)
				return 0 ;
		} else if (ver == t_ver)
				return 0 ;
	}
	/* pmessage (" Table ", "%s/%d %lX", t_name, t_ver, n_index); */
	if (n_index == 0)
		return 1;
	index = n_index;
	goto NEXT;
}

int get_part_to_file (name, begin, lenb)
char * name;
unsigned begin, lenb;
{
	register long addr, len, bytes;
	long     st_addr, st_len;
	time_t   b_time, e_time;
	unsigned long left, right; unsigned char c;

	char *buf, *base_name;
	register i, mode1, mode2;
	size_t bufsize;
	register FILE *f;

	mode1 = getchoice (1, " Создание файла ", " Выберете формат слова ",
			   NULL, " 64 ", " 48 ", NULL);
	if (mode1 == -1)
		return 0;

	if ((f = fopen (name, "w")) == NULL) {
		error ("%s : %s", name, sys_errlist[errno]);
		return 0;
	}

	time (&b_time);
	for (bufsize = 0x7FFF; ; bufsize -= 16)
		if ((buf = malloc(bufsize)) != NULL) {
			bufsize--;
			break;
		}

	setvbuf(f, buf, _IOFBF, bufsize );

	addr	= 0x0l; len = lenb;
	addr = mode1 ? editval (5, "Чтение из раздела ДВС", "Введите начальную зону",
			   "%lo", addr) * 8
		     : editval (5, "Чтение из раздела ДВС", "Введите начальный блок",
			   "%lX", addr)
		     ;
	if (addr >= lenb || addr < 0l) {
		if (addr >= lenb)
			error ( mode1 ? "Начальная зона больше допустимой (%o)"
				      : "Начальный блок больше допустимого (%X)",
				mode1 ? (lenb + 7) / 8 - 1
				      :	lenb - 1);
		fclose (f);
		free (buf);
		return 0;
	}

	mode2 = getchoice (1, " Создание файла ", " Выберете еденицу измерения ",
			   NULL, mode1 ? " зоны " : " блоки ", " байты ", NULL);
	if (mode2 == -1) {
		fclose (f);
		free (buf);
		return 0;
	}

	if (mode2) {
	bytes = editval (5, "Чтение из раздела ДВС", "Введите количество байтов",
			   "%ld", len * 1024);
	len = (bytes + 1023) / 1024;
	}
	else
	len = mode1 ? editval (5, "Чтение из раздела ДВС", "Введите количество зон",
			   "%lo", (len + 7) / 8) * 8
		    : editval (5, "Чтение из раздела ДВС", "Введите количество блоков",
			   "%lX", len)
		    ;
	if (len + addr > lenb || len < 0l) {
		if (len + addr > lenb)
			error ( mode1 ? "Количество зон больше допустимого (%lo)"
				      : "Количество блоков больше допустимого (%lX)",
				mode1 ? (lenb - addr + 7) / 8
				      :	lenb - addr);
		fclose (f);
		free (buf);
		return 0;
	}
	addr = (addr + begin) << 7, st_addr = addr;
	len = mode2 ? (bytes + 7) / 8 : len << 7, st_len = len;

	base_name = strdup (make_base_name (name));

	bmessage (" Создание ", "Файл '%s' создано %3dК", base_name, 0);
	for (; --len>=0; ++addr) {
		pt_load (&left, &right, addr, &c, 0);
		fputh (left, f);
		if (mode1) fputh16 (right, f);
		else	   fputh   (right, f);
		if (! (addr % 512))
			bmessage (" Создание ", "Файл '%s' создано %3dК", base_name,
				  mode1 ? (int) (((addr - st_addr) * 6 + 1023) / 1024) :
					  (int) ((addr - st_addr + 127) / 128));
	}

	if (mode2)
		chsize (fileno (f), bytes);
	if (fclose (f) == EOF) {
		error ("%s : %s", name, sys_errlist[errno]);
		free (buf); free (base_name);
		return 0;
	}
	free (buf);
	time (&e_time);
{
	char msg1 [100], msg2[100];

	endmesg ();
	i = mode1 ? (int) (((addr - st_addr) * 6 + 1023) / 1024) :
		    (int) ((addr - st_addr + 127) / 128);
	sprintf (msg1, "Файл '%s' создан, %d,%XH Килобайт",
		 base_name, i, i);
	sprintf (msg2, " За время %d", (int) (e_time - b_time));
	getchoice (0, " Создание ", msg1, msg2, NULL, NULL, NULL);
}
	free (base_name);
	return 1;
}

int put_file_to_part (name, begin, lenb)
char * name;
unsigned begin, lenb;
{
	register long addr, len;
	long     st_addr, st_len;
	time_t   b_time, e_time;
	unsigned long left, right;

	char *buf, *base_name;
	register i, mode1;
	size_t bufsize;
	struct stat stat_buf;
	register FILE *f;

	mode1 = getchoice (1, " Загрузка файла ", " Выберете формат слова ",
			   NULL, " 64 ", " 48 ", NULL);
	if (mode1 == -1)
		return 0;

	if ((f = fopen (name, "r")) == NULL) {
		error ("%s : %s", name, sys_errlist[errno]);
		return 0;
	}

	time (&b_time);
	for (bufsize = 0x7FFF; ; bufsize -= 16)
		if ((buf = malloc(bufsize)) != NULL) {
			bufsize--;
			break;
		}

	setvbuf(f, buf, _IOFBF, bufsize );

	addr	= 0x0l;
	addr = mode1 ? editval (5, "Запись в раздел ДВС", "Введите начальную зону",
			   "%lo", addr) * 8
		     : editval (5, "Запись в раздел ДВС", "Введите начальный блок",
			   "%lX", addr)
		     ;
	if (addr >= lenb || addr < 0l) {
		if (addr >= lenb)
			error ( mode1 ? "Начальная зона больше допустимой (%o)"
				      : "Начальный блок больше допустимого (%X)",
				mode1 ? (lenb + 7) / 8 - 1
				      :	lenb - 1);
		fclose (f);
		free (buf);
		return 0;
	}


	len = ((fstat(fileno(f), &stat_buf) < 0 ? 0 : stat_buf.st_size) + 1023) / 1024;

	if (len + addr > lenb || len < 0l) {
		if (len + addr > lenb)
			error ("Длина файла больше допустимой (%ld Килобайт)", lenb-addr);
		fclose (f);
		free (buf);
		return 0;
	}
	addr = (addr + begin) << 7, st_addr = addr;
	len <<= 7, st_len = len;

	base_name = strdup (make_base_name (name));

	bmessage (" Загрузка ", "Файл '%s' загружено %3dК", base_name, 0);
	for (; --len>=0; ++addr) {
		left = fgeth (f);
		if (mode1) right = fgeth16 (f);
		else	   right = fgeth (f);
		pt_store (left, right, addr, 0, 1);
		if (! (addr % 512))
			bmessage (" Загрузка ", "Файл '%s' загружено %3dК", base_name,
				  mode1 ? (int) (((addr - st_addr) * 6 + 1023) / 1024) :
					  (int) ((addr - st_addr + 127) / 128));
	}

	if (fclose (f) == EOF) {
		error ("%s : %s", name, sys_errlist[errno]);
		free (buf); free (base_name);
		return 0;
	}
	free (buf);
	time (&e_time);
{
	char msg1 [100], msg2[100];

	endmesg ();
	i = mode1 ? (int) (((addr - st_addr) * 6 + 1023) / 1024) :
		    (int) ((addr - st_addr + 127) / 128);
	sprintf (msg1, "Файл '%s' записан в раздел, %d,%XH Килобайт",
		 base_name, i, i);
	sprintf (msg2, " За время %d", (int) (e_time - b_time));
	getchoice (0, " Загрузка ", msg1, msg2, NULL, NULL, NULL);
}
	free (base_name);
	return 1;
}

# ifdef MSDOS
#pragma check_pointer (off)
void ptinitialize ()
{
# ifdef VAR_PORT
	int far *p;

	/* compute address of printer ports */
	p = (int far *) MK_FP (PSEG, POFF);
	pt_port = *p;
	pt_port1 = pt_port + 1;
	pt_port2 = pt_port + 2;
# endif

	/* disable init and interrupts */
	outp (RCNTRL, CINIT);
}
#pragma check_pointer ()
# else

extern void doquit ();

# define DEVPULT        "/dev/null"

void ptinitialize ()
{
	if ((f_pult = open (DEVPULT, 2)) < 0) {
		error ("%s : %s", DEVPULT, sys_errlist[errno]);
		doquit ();
	}
}
# endif

