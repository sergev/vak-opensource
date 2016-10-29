# define MT_WRITE       0x01    /* запись блока */
# define MT_READ        0x02    /* чтение блока */
# define MT_NOP         0x03    /* пустая операция */
# define MT_SENSE       0x04    /* уточнение состояния */
# define MT_REW         0x07    /* перемотка */
# define MT_RUN         0x0F    /* перемотка и разгрузка */
# define MT_ERG         0x17    /* стирание участка */
# define MT_WTM         0x1F    /* запись маркера */
# define MT_BSB         0x27    /* поиск блока назад */
# define MT_BSF         0x2F    /* поиск блока вперед */
# define MT_FSB         0x37    /* поиск маркера назад */
# define MT_FSF         0x3F    /* поиск маркера вперед */
# define MT_D32         0xCB    /* плотность 800 bpi */
# define MT_D63         0xC3    /* плотность 1600 bpi */
