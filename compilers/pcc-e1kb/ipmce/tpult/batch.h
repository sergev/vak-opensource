extern int select();
extern void big_menu();

struct f_menu {
	char    *name;  /* имя параметра */
	int     value;  /* значение флага */
};

struct b_field {
	char    *header;/* имя параметра */
	int     width;  /* ширина поля ввода */
	int     n_word; /* порядковый номер слова */
	int     b_byte; /* номер начального байта в слове */
	int     e_byte; /* номер конечного байта в слове */
	int     cn_word;
	int     cb_byte;
	int     ce_byte;
	enum    {
			STR,
			MENU,
			DEC,
			HEX,
			MASKA
		}
		type;   /* тип значения параметра */
	char    **menu; /* меню, если есть */
	long    max_val;/* максимальное значение параметра */

};


