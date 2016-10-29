/* регистр прерываний от селекторных каналов */

# define SC_CTL         0x24

/* биты селекторных каналов в регистре прерываний */

# define SCC_HRESET(n)  (1 << (43-(n))) /* обнуление */
# define SCC_IOEND(n)   (1 << (39-(n))) /* конец обмена */
# define SCC_CSTATUS(n) (1 << (35-(n))) /* изменение состояния */

/* местная память каналов */

# define SC_LMEM(n)     ((n)<<3|0x80)   /* база МПК */
# define SC_CCW(n)      SC_LMEM(n)      /* управляющее слово канала */
# define SC_CAW(n)      (SC_LMEM(n)|4)  /* адрес следующего УСК */
# define SC_CSW(n)      (SC_LMEM(n)|6)  /* слово состояния канала */

/* управляющие биты адреса МПК */

# define SCA_NOHALT     0x200           /* блок. авостов из-за плохих КР */
# define SCA_RESET      0x400           /* обнуление канала */
# define SCA_SBLOCK     0x001           /* сохранение БЛК-К */
# define SCA_START      0x400           /* пуск */

# define XFER           0x8             /* команда перехода */

struct sccmd {
	int addr        :24;
	int count       :16;
	int flg         :8;
	int cmd         :8;
	int unit        :8;
};
