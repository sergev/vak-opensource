# include <stdio.h>
# include <string.h>
# include "wkb.h"
# include "choice.h"
# include "ut.h"
# include "intr.h"

# define NAME_LENGTH 40
# define PART_NAME_LENGTH 16

static char tst_file[NAME_LENGTH] = "САС.TST", any_file[NAME_LENGTH] = "";
static char part_name[PART_NAME_LENGTH] = "";

void store_tst() {

	register char *s;

	if ((s = getstring(NAME_LENGTH, tst_file, "Загрузка TST-файла",
			 "Введите имя TST-файла")) != NULL) {
		strncpy (tst_file, s, NAME_LENGTH);
		if (extension (tst_file, "tst") != NULL) {
			/* stop (); По просьбе трудящихся убран */
# ifdef UNDEFINED
			if ((s = select_file (tst_file)) == NULL)
				return ;
# endif
			(void) put_tst_file (s, 1);
		}
	}
}

void store_any() {

	register char *s;

	if ((s = getstring(NAME_LENGTH, any_file, "Загрузка файла",
			 "Введите имя файла")) != NULL) {
		strncpy (any_file, s, NAME_LENGTH);
		if (extension (any_file, "") != NULL) {
			/* stop (); */
			(void) put_any_file (any_file);
		}
	}
}

void load_any() {

	register char *s;

	if ((s = getstring(NAME_LENGTH, any_file, "Создание файла",
			 "Введите имя файла")) != NULL) {
		strncpy (any_file, s, NAME_LENGTH);
		if (extension (any_file, "") != NULL) {
			/* stop (); */
			(void) get_any_file (any_file);
		}
	}
}

void load_part() {

	register char *s;
	unsigned begin, len;
	int i;

	if ((s = getstring(PART_NAME_LENGTH, part_name, "Чтение из раздела ДВС",
			 "Введите имя раздела")) != NULL) {
		strncpy (part_name, s, PART_NAME_LENGTH);
		if ((i = open_part (part_name, &begin, &len)) != 0) {
			if (i > 0)
				error ("Нет раздела : %s", part_name);
			return ;
		}
	}
	if ((s = getstring(NAME_LENGTH, any_file, "Чтение из раздела ДВС",
			 "Введите имя файла")) != NULL) {
		strncpy (any_file, s, NAME_LENGTH);
		if (extension (any_file, "") != NULL)
			(void) get_part_to_file (any_file, begin, len);
	}
}

void store_part() {

	register char *s;
	unsigned begin, len;
	int i;

	if ((s = getstring(PART_NAME_LENGTH, part_name, "Запись в раздел ДВС",
			 "Введите имя раздела")) != NULL) {
		strncpy (part_name, s, PART_NAME_LENGTH);
		if ((i = open_part (part_name, &begin, &len)) != 0) {
			if (i > 0)
				error ("Нет раздела : %s", part_name);
			return ;
		}
	}
	if ((s = getstring(NAME_LENGTH, any_file, "Запись в раздел ДВС",
			 "Введите имя файла")) != NULL) {
		strncpy (any_file, s, NAME_LENGTH);
		if (extension (any_file, "") != NULL)
			(void) put_file_to_part (any_file, begin, len);
	}
}

void reset_U_Chan () {

	/* 24 ячейка := 1 - сброс универсального канала */
	pt_store (0l, 1l, 0x24l, 0, 1);
# ifdef MSDOS
	_disable ();
	int_tab [1].ticks = TICKPERMIN / 60; /* раз в секунду */
	_enable ();
# endif

}

