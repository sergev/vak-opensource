# ifdef MSDOS
# include <stdlib.h>
# endif
# include "scr.h"
# include "menu.h"

/* menu handler */

extern void load_sas(), load_batch(), save_batch(), change_batch(), init_batch();
extern void run_sas(), figwam(), doquit(), rrun(), zero(), stop(), State_B();
extern void run_pult (), load_part (), store_part (),
# ifdef MSDOS
run_dos (),
# else
run_unix (),
# endif
get_batch_wg (), store_tst (), store_any (), load_any ();
extern void reset_U_Chan (), Debug_Toggle ();
extern inited_batch, loaded_sas, loaded_batch, saved_batch, debug_mode;
static tag();

static struct submenu sas [] = {
	{       "Загрузить САС",        'l',    load_sas,       },
	{       "Загрузить задание",    'f',    load_batch,     },
	{       "Сохранить задание",    's',    save_batch,     },
	{       "Загрузить задание из ОП",'g',  get_batch_wg,   },
	{       "Выбор типа задания",   'i',    init_batch,     },
	{       "Изменить задание",     'b',    change_batch,   },
	{       "Выполнить САС",        'r',    run_sas,        },
	{       "Просмотр регистров",   'v',    State_B,        },
	{       "Общий нуль (У0)",      'u',    zero,           },
	{       "Одиночный режим (ОР)", 'o',    stop,           },
	{       "Пуск процессора (АР)", 'a',    rrun,           },
	/*{       "Возврат",              'e',    0,              },*/
	{       0,                      0,      0,              },
};

static struct submenu term [] = {
	{	"Эмулятор VDT",	      '7',    figwam,	       },
	/*{       "Возврат",              'e',    0,              },*/
	{       0,                      0,      0,              },
};

static struct submenu utils [] = {
	{	"Загрузить TST-файл",	't',	store_tst,	},
	{	"Загрузить файл",	'a',	store_any,	},
	{	"Создать файл", 	'c',	load_any,	},
	{	"Чтение из раздела",	'r',	load_part,	},
	{	"Запись в раздел",	'w',	store_part,	},
	{	"Сброс унив. канала",	'u',	reset_U_Chan,	},
	/*{       "Возврат",              'e',    0,              },*/
	{       0,                      0,      0,              },
};

static struct submenu debug [] = {
# ifdef MSDOS
	{       "Программа Пульт",      'p',    run_pult,       },
# endif
	{	"Отладочный режим",	'm',	Debug_Toggle,	},
	/*{       "Возврат",              'e',    0,              },*/
	{       0,                       0,     0,              },
};

static struct submenu quit [] = {
	{       "Конец сеанса",         'n',    doquit,         },
# ifdef MSDOS
	{       "Выполнить команду DOS",'d',    run_dos,        },
# else
	{       "Выполнить команду UNIX",'d',    run_unix,        },
# endif
	/*{       "Возврат",              'e',    0,              },*/
	{       0,                       0,     0,              },
};

struct menu menu [] = {
	{       "Сервис",       's',            sas,    0,0,0,0,6},
	{       "Утилиты",      'u',            utils,  },
	{       "Терминал",     't',            term,   },
	{       "Отладчик",     'd',            debug,  },
	{       "Выход",        'q',            quit,   },
	{       0,              0,              0,      },
};

int nmenu = 0;

runmenu (mkey)
{
	register struct menu *m;
	register struct submenu *sm;
	int      mnkey;

	nmenu = 0;

	if ( mkey == 0)
		mnkey = menu->key;
	else
		mnkey = abs(mkey);

	for (m=menu; m->name; ++m) {
		if (m->key == mnkey)
			nmenu = m-menu;
		for (sm=m->submenu; sm->name; ++sm) {
			sm->active = sm->exec != 0;
			sm->tag = tag (m->key, sm->key);
		}
		if (mnkey != m->key) {
			m->nsm = -1;
			for (;;) {
				if (! m->submenu[++m->nsm].name)
					m->nsm = 0;
				if (m->submenu[m->nsm].active)
					break;
			}
		}
	}
	if (! getmenu (mkey))
		return 0;
	m = &menu[nmenu];
	sm = &m->submenu[m->nsm];
	if (sm->exec) {
		(*sm->exec) (m->key, sm->key);
		return m->key;
	}
	else
		return -m->key;
}

static tag (k, sk)
{
	switch (k) {
	case 's':
		switch (sk) {
		case 'i':
			return (inited_batch);
		case 'l':
			return (loaded_sas);
		case 'f':
			return (loaded_batch);
		case 's':
			return (saved_batch);
		}
		break;
	case 'd':
		switch (sk) {
		case 'm':
			return (debug_mode);
		}
		break;
	}
	return (0);
}

