/*
 *      Структура файла a.out:
 *
 *      Заголовок из 9 слов     магическое число 407, 410
 *                              длина сегмента const    )
 *                              длина сегмента text     )
 *                              длина сегмента data     ) в байтах, кратна 8
 *                              длина сегмента bss      )
 *                              длина сегмента abss     )
 *                              длина таблицы символов  )
 *                              адрес входа
 *                              признак неперемещаемости
 *
 *      заголовок               0
 *      const:                  72
 *      text:                   72 + constsize
 *      data:                   72 + constsize + textsize
 *      настройка const:        72 + constsize + textsize + datasize
 *      настройка text:         72 + 2*constsize + textsize + datasize
 *      настройка data:         72 + 2*constsize + 2*textsize + datasize
 *      таблица символов:       72 + 2*constsize + 2*textsize + 2*datasize
 *
 */

struct exec {           /* заголовок файла a.out */
	int     a_magic;
	long    a_const;
	long    a_text;
	long    a_data;
	long    a_bss;
	long    a_abss;
	long    a_syms;
	long    a_entry;
	short   a_flag;
};

			/* элемент таблицы символов */
struct nlist {
	short           n_len;          /* длина имени в байтах */
	short           n_type;         /* тип */
	long            n_value;        /* значение */
	char *          n_name;         /* указатель на имя */
};

			/* флаги */
# define RELFLG 1       /* бит неперемещаемости */
# define TCDFLG 2       /* сегмент констант входит в сегмет данных */

# define HDRSZ  72      /* длина заголовка в байтах */

			/* типы файлов */
# define FMAGIC 0407
# define NMAGIC 0410
# define AMAGIC 0411

			/* типы символов */
# define N_EXT  040
# define N_TYPE 037     /* маска */
# define N_FN   037
# define N_UNDF 00
# define N_ABS  01
# define N_CONST 02
# define N_TEXT 03
# define N_DATA 04
# define N_BSS  05
# define N_ABSS 06
# define N_STRNG 07     /* для as */
# define N_COMM 010
# define N_ACOMM 011

			/* типы настройки */
# define RABS   0
# define RCONST 010
# define RTEXT  020
# define RDATA  030
# define RBSS   040
# define RABSS  050
# define RSTRNG 060     /* для as */
# define REXT   070     /* одновременно является битовой маской */

# define RSHIFT 04
# define RTRUNC 05
# define RLONG  06
# define RSHORT 07      /* одновременно является битовой маской */

# define RGETIX(h) ((h)>>6)
# define RPUTIX(h) ((long)(h)<<6)

/* макросы, определяющие различные позиции в файле, используя заголовок */

# define N_TXTOFF(x)    HDRSZ
# define N_SYMOFF(x)    (N_TXTOFF(x) + (x).a_const + (x).a_text + (x).a_data)
# define N_STROFF(x)    (N_SYMOFF(x) + (x).a_syms)

# define BADMAG(x)      ((x).a_magic != FMAGIC && (x).a_magic != NMAGIC)
# define N_BADMAG       BADMAG
# define FORMAT         "%07lx" /* для печати значения символа */
# define N_FORMAT       FORMAT
