# define BC_CTL         0x23            /* регистр прерываний БК */

/* биты каналов в регистре прерываний */

# define BCC_IN(n)      (1 << (46-2*(n)))       /* приемник */
# define BCC_OUT(n)     (2 << (46-2*(n)))       /* источник */

/* местная память каналов */

# define BC_LMEM(u)     ((u)<<2|0xa0)   /* база МПК */
# define BC_OCW(n)      BC_LMEM(n)      /* управляющее слово источника */
# define BC_OSW(n)      (BC_LMEM(n)|1)  /* слово состояния источника */
# define BC_ICW(n)      (BC_LMEM(n)|2)  /* управляющее слово приемника */
# define BC_ISW(n)      (BC_LMEM(n)|3)  /* слово состояния приемника */

/* управляющие биты адреса МПК */

# define BCA_NOHALT     0x200           /* блок. авостов из-за плохих КР */
# define BCA_RESET      0x400           /* обнуление канала */
# define BCA_START      0x400           /* пуск */

struct bccmd {
	int addr        :24;
	int count       :16;
	int flg         :20;
	int bcount      :4;
};

extern bcrdflags;
extern bcwrflags;
