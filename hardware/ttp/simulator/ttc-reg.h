/*
 * Описание регистров контроллера TTP.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
#ifndef uint32_t
#define uint32_t unsigned int
#endif
#ifndef uint16_t
#define uint16_t unsigned short
#endif

/*
 * Ревизия прошивки.
 */
#define TTC_REVISION_NUMBER	0x00		/* A0 */
#define TTC_REVISION_DATE	0x080725	/* yymmdd */

/*
 * Адреса внутренней памяти.
 */
#define TTC_WORM_BASE		0x0000	/* Таблица расписания MEDL */
#define TTC_RAM_BASE		0x4000	/* Принимаемые и передаваемые пакеты */
#define TTC_REG_BASE		0xf000	/* Управляющие регистры */

#define TTC_WORM_BYTES		(TTC_RAM_BASE - TTC_WORM_BASE)
#define TTC_RAM_BYTES		(TTC_REG_BASE - TTC_RAM_BASE)

/*----------------------------------------------
 * Общесистемные регистры
 */
/*
 * FRR – firmware revision register, 16 R
 *
 * Формат в шестнадцатеричном виде РНГГ, где:
 * Р - буква ревизии прошивки (0=A, 1=B и т.д.)
 * Н - цифра ревизии прошивки
 * ГГ - последние две цифры номера года
 */
#define TTC_FRR			0xfe00

/*
 * FDR – firmware date register, 16 R
 *
 * Формат в шестнадцатеричном виде ММДД, где:
 * ММ - месяц
 * ДД - день месяца
 */
#define TTC_FDR			0xfe02

/*
 * SID – schedule identification register, 32 R/WO
 */
#define TTC_SID			0xfe04

/*
 * NID – node identifier, 16 R/WO
 */
#define TTC_NID			0xfe08

/*
 * GCR – global configuration register, 16 R/W, default 0x0000
 */
#define TTC_GCR			0xfe0a

#define TTC_GCR_GRST		0x0001		/* общий сброс */
#define TTC_GCR_GRUN		0x0002		/* общий пуск */

#define TTC_GCR_ISEL_MASK	0x000с		/* выбор интерфейса */
#define TTC_GCR_ISEL_MANCHESTER	0x0000		/* RS-485 кодирование Manchester */
#define TTC_GCR_ISEL_SNI	0x0004		/* 10Base2 */
#define TTC_GCR_ISEL_MII	0x0008		/* 100Base-TX или 100Base-SX*/
#define TTC_GCR_ISEL_CAN	0x000с		/* шина CAN */

#define TTC_GCR_IEN		0x0010		/* разрешение прерывания */
#define TTC_GCR_N32		0x0020		/* 32 узла в кластере */
#define TTC_GCR_N64		0x0040		/* 32 узла в кластере */
#define TTC_GCR_STRT		0x0080		/* режим старта узла */

/*
 * GSR – global status register, 16 R
 * IER – interrupt mask register, 16 R/W
 */
#define TTC_GSR			0xfe0c
#define TTC_IER			0xfe0e

#define TTC_GSR_SF		0x0001		/* закончился слот */
#define TTC_GSR_TDN		0x0002		/* передан пакет */
#define TTC_GSR_TABT		0x0004		/* передача оборвана на границе слота */
#define TTC_GSR_SINT		0x4000		/* закончился слот с флагом INTR */
#define TTC_GSR_CCL		0x8000		/* закончился цикл кластера */

/*
 * RSRi – receive status registers, 16 R/W
 * RIER – receive interrupt enable register, 16 R/W
 */
#define TTC_RSR(n)		(0xfe10 | (n)<<1)
#define TTC_RIER		0xfe16

#define TTC_RSR_SF		0x0001		/* закончился слот */
#define TTC_RSR_RDN		0x0002		/* успешно принят пакет */
#define TTC_RSR_STRT		0x0004		/* принят стартовый пакет */
#define TTC_RSR_REV		0x0008		/* ошибка кодирования приёмника */
#define TTC_RSR_RABT		0x0010		/* приём оборван на границе слота */
#define TTC_RSR_RLE		0x0020		/* неверная длина пакета */
#define TTC_RSR_CSE		0x0040		/* ошибка CRC приёмника */
#define TTC_RSR_RCME		0x0080		/* неверный режим кластера */

/*
 * CMR – current mode register, 16 R
 * NMR – next mode register, 16 R/W
 */
#define TTC_CMR			0xfe18
#define TTC_NMR			0xfe1a

#define TTC_MR_CMODE_MASK	0x00ff		/* режим кластера */
#define TTC_MR_RXEN0		0x0100		/* пуск приёмника шины 0 */
#define TTC_MR_RXEN1		0x0200		/* пуск приёмника шины 1 */
#define TTC_MR_RXEN2		0x0400		/* пуск приёмника шины 2 */
#define TTC_MR_TXEN0		0x0800		/* пуск передатчика шины 0 */
#define TTC_MR_TXEN1		0x1000		/* пуск передатчика шины 1 */
#define TTC_MR_TXEN2		0x2000		/* пуск передатчика шины 2 */
#define TTC_MR_STRT		0x4000		/* режим старта */

/*
 * SSR – schedule start register, 16 R/W
 * SCP – schedule current pointer, 16 R
 * SSRP – schedule start receive pointer, 16 R/W
 */
#define TTC_SSR			0xfe1c
#define TTC_SCP			0xfe20
#define TTC_SSRP		0xfe22

/*
 * TPLR – transmit preamble length register, 16 R/W
 */
#define TTC_TPLR		0xfe24


/*----------------------------------------------
 * Вектора
 */
/*
 * RVEC – receive vector, 64 R
 */
#define TTC_RVEC		0xfe30

/*
 * AVEC – acknowledge vector, 64 R
 */
#define TTC_AVEC		0xfe38


/*----------------------------------------------
 * Время
 */
/*
 * CTR – cycle time register, 32 R
 */
#define TTC_CTR			0xfe40

/*
 * CNR – cycle number register, 32 R
 */
#define TTC_CNR			0xfe44

/*
 * TDR – time delta register, 32 R
 */
#define TTC_TDR			0xfe48

/*
 * MTR – time delta register, 32 R/W
 * NMTR – next time delta register, 32 R/W
 */
#define TTC_MTR			0xfe4c
#define TTC_NMTR		0xfe5c

/*
 * MINLi – slowest node time, 32 R
 * MINUi – next to slowest node time, 32 R
 * MAXLi – next to fastest node time, 32 R
 * MAXUi – fastest node time, 32 R
 */
#define TTC_MINL(n)		(0xfe50 | (n)<<2)
#define TTC_MINU(n)		(0xfe60 | (n)<<2)
#define TTC_MAXL(n)		(0xfe70 | (n)<<2)
#define TTC_MAXU(n)		(0xfe80 | (n)<<2)

/*
 * RPCi – receive packets counter, 32 R
 * TPC – transmit packets counter, 32 R
 * RSPCi – receive start packets counter, 32 R
 * EVCi – encoding violations counter, 32 R
 * CECi – checksum errors counter, 32 R
 * CMECi – cluster mode errors counter, 32 R
 */
#define TTC_RPC(n)		(0xfe90 | (n)<<2)
#define TTC_TPC			0xfe9c
#define TTC_RSPC(n)		(0xfea0 | (n)<<2)
#define TTC_EVC(n)		(0xfeb0 | (n)<<2)
#define TTC_CEC(n)		(0xfec0 | (n)<<2)
#define TTC_CMEC(n)		(0xfed0 | (n)<<2)


/*----------------------------------------------
 * Структура таблицы расписания узла (MEDL)
 */
struct _medl_t {
	uint32_t	ticks;			/* длительность слота */
	uint16_t	node;			/* номер узла */
	uint16_t	bytes;			/* длина пакета в байтах */
	uint16_t	data_addr0;		/* буфер данных передачи или приема 0 */
	uint16_t	data_addr1;		/* буфер данных приема шины 1 */
	uint16_t	data_addr2;		/* буфер данных приема шины 1 */
	uint16_t 	flags;
#define TTC_MEDL_START		0x0001		/* стартовый пакет */
#define TTC_MEDL_TSYN		0x0002		/* запомнить время приёма */
#define TTC_MEDL_CLKSYN		0x0004		/* синхронизация в конце слота */
#define TTC_MEDL_ETS		0x0008		/* установка внешнего сигнала индикации */
#define TTC_MEDL_INTR		0x0010		/* прерывание в начале слота */
};
typedef struct _medl_t medl_t;


/* Конец описания регистров контроллера TTP.
 *----------------------------------------------*/
