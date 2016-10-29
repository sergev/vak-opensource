# include <stdio.h>
# include <string.h>
# ifdef MSDOS
# include <stdlib.h>
# include <process.h>
# include <conio.h>
# include <dos.h>
# else
# include <errno.h>
extern char *sys_errlist [];
# include <sys/pult.h>
# include <memory.h>
# endif
# include "intr.h"
# include "scr.h"
# include "wkb.h"
# include "choice.h"
# include "menu.h"
# include "batch.h"
# include "ut.h"

# define NAME_LENGTH 40
# define COMMAND_LENGTH 60

extern char *batch_name[];
extern int batch_n, max_batch_n, is_name;
       char batch_file[NAME_LENGTH] = "std.sas";
static char sas_file[NAME_LENGTH] = "САС.tst";
unsigned char CAC_Name[8] = " tst.CAC";
# ifdef MSDOS
static char pult_file[] = "PULT.EXE";
extern unsigned long old_addr;
# endif
extern union  word batch_w[MAX_BATCH];
int inited_batch = 0, loaded_batch = 0, saved_batch = 0, loaded_sas = 0;

void save_batch(), change_batch (), figwam();

# ifdef MSDOS
extern void (interrupt far *old_timer)( void );
extern void interrupt far timer_int ();
# endif
extern void Upd_NameB ();

# ifdef MSDOS
void run_pult () {

	register ret, o_errno;
	struct wordb word;

	_dos_setvect (0x1C, old_timer);
	VIClear ();
	ret = spawnlp (P_WAIT, pult_file, pult_file, NULL);
	o_errno = errno; /* VRedraw () портит errno */
	VRedraw ();
	_dos_setvect (0x1C, timer_int);
	if (ret == -1)
		error ("%s : %s", pult_file, sys_errlist[o_errno]);
	else {
		ptinitialize();
		old_addr = -1;
		_disable ();
		int_tab [0].ticks = TICK_CHK;
		_enable ();
	}
}

void run_dos () {

	static char command[COMMAND_LENGTH] = "";
	register char *s;
	register ret, o_errno;
	CURSOR	 Old;

	if ((s = getstring(COMMAND_LENGTH, command, "Выполнение команды DOS",
			 "Введите команду DOS")) != NULL) {
		strncpy (command, s, COMMAND_LENGTH);
		_dos_setvect (0x1C, old_timer);
		Old = VGetCursor ();
		VMove (0, 0);
		VSync ();
		VIClear ();
		s = strcmp (command, "") == 0 ? "COMMAND.COM" : command;
		ret = system (s);
		o_errno = errno; /* VRedraw () портит errno */
		fprintf (stderr, "Нажмите 'Enter' для продолжения.");
		(void) getchar ();
		VSetCursor( Old );
		VRedraw ();
		_dos_setvect (0x1C, timer_int);
		old_addr = -1;
		if (ret == -1)
			error ("DOS : %s", sys_errlist[o_errno]);
	}
}
# else
void run_unix () {

	static char command[COMMAND_LENGTH] = "";
	register char *s;
	register ret, o_errno;
	CURSOR	 Old;

	if ((s = getstring(COMMAND_LENGTH, command, "Выполнение команды SHELL",
			 "Введите команду SHELL")) != NULL) {
		strncpy (command, s, COMMAND_LENGTH);
		Old = VGetCursor ();
		VMove (0, 0);
		VSync ();
		VIClear ();
		VFlush ();
		s = strcmp (command, "") == 0 ? "/bin/sh" : command;
		TtyReset ();
		ret = system (s);
		o_errno = errno; /* VRedraw () портит errno */
		fprintf (stderr, "Нажмите 'Enter' для продолжения.");
		(void) getchar ();
		TtySet ();
		VSetCursor( Old );
		VRedraw ();
		if (ret == -1)
			error ("SHELL : %s", sys_errlist[o_errno]);
	}
}
# endif


void load_sas() {

	loaded_sas = put_tst_file (sas_file, 0);

}

void load_batch() {

	register char *s;
	FILE *fi;

	if (is_name > 1)
		goto do_load;
# ifdef VERBOSE
	if (! saved_batch && loaded_batch) {

		int  i;

		i = getchoice (1, " Предупреждение ", "Задание не сохранено",
				  "Сохранять задание ?", " Нет ", " Да ", " Возврат ");
		switch (i) {
		case 1:
			save_batch ();
			break;
		case -1:
		case 2:
			return;
		}
	}
# endif

	if ((s = getstring(NAME_LENGTH, batch_file, "Загрузка задания",
			 "Введите имя файла с заданием для САС")) != NULL) {
		strncpy (batch_file, s, NAME_LENGTH);
		if (extension (batch_file, "sas") != NULL) {
do_load :
			if ((fi = fopen (batch_file, "r")) == NULL) {
				error ("%s : %s", batch_file, sys_errlist[errno]);
				return;
			}
			if ((batch_n = getw(fi)) == EOF ||
			     fread (batch_w[0].c, sizeof (batch_w[0].c), MAX_BATCH, fi) != MAX_BATCH) {
				error ("%s : %s", batch_file, sys_errlist[errno]);
			}
			if (fclose (fi) == EOF) {
				error ("%s : %s", batch_file, sys_errlist[errno]);
				return;
			}
			inited_batch = 0, loaded_batch = saved_batch = 1;
			big_menu (0);
		}
	}
}

static get_batch (n) /* from Elbrus-B memory */
{
	register i;
	char     c;
	union  word batch_wb[MAX_BATCH];
	struct wordb word;
        if (! pt_load (&word.left, &word.right, 0x3E010l, &word.tag, 1)) {
		if ((word.left & 0xFFFFF000l) != 0x03B3E000l) {
			if (n)
				error ("Не загружен САС.");
			return 0;
		}
	} else {
		if (n)
			error ("Ошибка при чтении адреса: %5lX", 0x3E010l);
		return 0;
	}
	for (i = 0; i < MAX_BATCH; i++)
		if (pt_load (&batch_wb[i].l[1], &batch_wb[i].l[0], 0x3E000l + i, &c, 1)) {
			if (n)
				error ("Ошибка при чтении адреса: %5lX", 0x3E000l + i);
			return 0;
		}
	if ((unsigned) batch_wb[0].i[0] >= (unsigned) max_batch_n) {
		if (n)
			error ("Неверный номер задания.");
		return 0;
	}
	batch_n = batch_wb[0].i[0];
	for (i = 0; i < MAX_BATCH; i++)
		batch_w[i] = batch_wb[i];
	inited_batch = saved_batch = 0, loaded_batch = 1;
	big_menu (0);
	return 1;
}

void get_batch_wg () {
	(void) get_batch (1);
}

void get_batch_nwg () {
	if (get_batch (0))
		saved_batch = 1;
}

void save_batch() {

	register char *s;
	FILE *fo;

	if (! loaded_batch) {
		error ("%s", "Нет задания");
		return;
	}

	if ((s = getstring(NAME_LENGTH, batch_file, "Сохранение задания",
			 "Введите имя файла с заданием для САС")) != NULL) {
		strncpy (batch_file, s, NAME_LENGTH);
		if (extension (batch_file, "sas") != NULL) {
			if ((fo = fopen (batch_file, "w")) == NULL) {
				error ("%s : %s", batch_file, sys_errlist[errno]);
				return;
			}
			if (putw(batch_n, fo) == EOF ||
			    fwrite (batch_w[0].c, sizeof (batch_w[0].c),
				    MAX_BATCH, fo) != MAX_BATCH) {
				error ("%s : %s", batch_file, sys_errlist[errno]);
			}
			if (fclose (fo) == EOF) {
				error ("%s : %s", batch_file, sys_errlist[errno]);
				return;
			}
			inited_batch = 0, saved_batch = 1;
		}
	}
}

void init_batch() {

	register i, ret;
	char *header = "Выбор  задания";

# ifdef VERBOSE
	if (! saved_batch && loaded_batch) {
		i = getchoice (1, " Предупреждение ", "Задание не сохранено",
				  "Сохранять задание ?", " Нет ", " Да ", " Возврат ");
		switch (i) {
		case 1:
			save_batch ();
			break;
		case -1:
		case 2:
			return;
		}
	}
# endif

	ret = small_menu (header, batch_name, batch_n);

	if (ret >= 0) {
		for (i = 0; i < MAX_BATCH; i++)
		       batch_w[i].l[0] = batch_w[i].l[1] = 0;
		batch_n = batch_w[0].i[0] = ret;
		inited_batch = 1, loaded_batch = 0, saved_batch = 1;
		change_batch ();
	}

}

void change_batch() {

	if (! loaded_batch && ! inited_batch)
		load_batch ();

	if (! loaded_batch && ! inited_batch ) {
		init_batch ();
		return;
	}

	big_menu (0);
	big_menu (1);
	inited_batch = saved_batch = 0, loaded_batch = 1;
	big_menu (0);

}

void run_sas() {

	register i;
	struct wordb word;
	union  word W_0;


	if (! loaded_sas)
		if (! pt_load (&word.left, &word.right, 0x3E010l, &word.tag, 1)) {
			if ((word.left & 0xFFFFF000l) != 0x03B3E000l)
				load_sas ();
			else
				loaded_sas = 1;
		} else
			load_sas ();

	if (! loaded_batch) {
		if (is_name)
			load_batch ();
		else
			change_batch ();
	}

	if (! loaded_batch)
		return ;

	if (stopped == 0) /* Процессор работает */
		if (! ask_are_you_shure (" Процессор работает !") )
			return ;
	stop ();
	stopped = 4;
	/* Запись в память параметров для САС */
	for (i = 0; i < 10; i++)
		pt_store (batch_w[i].l[1], batch_w[i].l[0], 0x3E000l + i, 0, 1);
	zero ();
	/* Установка регистра РКП */
	pt_sendl (0x0183E010l, BCMD);
	/* Установка регистра РК (РК := РКП) */
	pt_send (04, BBUTTON1);
	memcpy (W_0.c, CAC_Name, 8);
	/* Имя САС.tst ==> в 0 слово */
	pt_store (W_0.l[1], W_0.l[0], 0x0l, 0, 1);
	pt_store (W_0.l[1], W_0.l[0], 0x36l, 0, 1);
	run ();

}

void doquit() {

	if (! saved_batch && loaded_batch)
		save_batch ();

	VMove (LINES - 1, 0);
	VClearLine ();
	VMove (LINES - 2, 0);
	VSetPalette (2, 0, 7, 1, 15, 0, 15, 7, 14, 0, 15, 1);
	VSetBold ();
	VSync ();
	VClose ();
# ifdef MSDOS
	_dos_setvect (0x1C, old_timer);
# endif
	exit (0);

}

void doabort(msg)
char * msg;
{

	VMove (LINES - 1, 0);
	VClearLine ();
	VMove (LINES - 2, 0);
	VSetPalette (2, 0, 7, 1, 15, 0, 15, 7, 14, 0, 15, 1);
	VSetBold ();
	VSync ();
	VClose ();
# ifdef MSDOS
	_dos_setvect (0x1C, old_timer);
# endif
	outerr (msg);
	exit (1);

}

void figwam() {

	error ("%s", "Пока нельзя.");

}

