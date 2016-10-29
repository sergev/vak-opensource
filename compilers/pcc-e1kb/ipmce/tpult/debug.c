# include "choice.h"
# include "wkb.h"

int debug_mode;

void Debug_Toggle () {

	register int  i;

	i = getchoice (1, " Отладчик ", "Установка отладочного режима",
			  "Отладочный режим ?" , " Да ", " Нет ", " Возврат ");

	switch (i) {
	case 0:
		debug_mode = 1;
		break;
	case 1:
		debug_mode = 0;
		break;
	case 2:
	case -1:
		return;
	}
	pt_store (0l, (long) debug_mode, 0x28l, 0, 1);
}
