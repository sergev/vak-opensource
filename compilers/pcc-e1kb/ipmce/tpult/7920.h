# define NUM_ONLY 04    /* цифровое поле */
# define PROTECT  010   /* защищенное поле */
# define MODIF    0100  /* признак модификации поля */
# define FIELD    0200  /* признак атрибута */

#define  ATTR_LAB ('+') /* символ, которым отбражается атрибут на экране */

typedef enum {NORMAL, BOLD, INVISIBLE} v7920_attr;

typedef struct {
		unsigned char c;        /* символ, '\377' - в этой позиции атрибут */
# define                                ATTR       '\377'
		unsigned char a;        /* атрибут */
		unsigned int  n_attr;    /* ссылка на атрибут */
	} scr_7920;


