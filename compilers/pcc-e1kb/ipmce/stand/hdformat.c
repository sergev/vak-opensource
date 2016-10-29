/*
 * Разметка ЕС-диска для Эльбрус-Б
 * ===============================
 * в формате 808 x 19 x 10 x 1k с тегами
 *
 * Всего на диске 815 цилиндров и 19 головок.
 * Объем дорожки - 13440 байт.
 * Полный объем диска - 208,118,400 байт (198 Mb).
 * В начале дорожки находится служебный блок
 * размером 92+13+7+49+11+7 = 179 байт.
 * Затем идут N информационных блоков длиной L байт.
 * На каждый блок дополнительно расходуется
 * 59+11+7+49+7 = 133 байта.
 *
 * В данном случае N=10, L=131*9=1179 байт,
 * объем диска 808*19*10k = 153,520k (149 Mb).
 *
 * (L + 133) * N + 179 = 13299 < 13440
 *
 * Наиболее рациональный вариант использования диска -
 * 1) не писать теги
 * 2) на дорожке 6 секторов по 2k или 3 сектора по 4k.
 * Получится 808*19*12k = 184,224k (179 Mb), выигрыш 30 Mb.
 *
 * Структура канальной программы разметки дорожки
 * ----------------------------------------------
 *
 * (1) задание маски
 *
 *              YY 1f 14 0000 AAAAAA
 *
 *      AAAAAA - адрес маски
 *
 *              c000 0000 0000 0000
 *
 * (2) позиционирование головок
 *
 *              YY 07 64 0000 AAAAAA
 *
 *      AAAAAA - адрес параметра
 *
 *              0000 0ЦЦЦ 00ГГ XXXX
 *
 *      ЦЦЦ - номер цилиндра, 0-813
 *      ГГ - номер головки, 0-18
 *
 * (3) ожидание собств. адреса
 *
 *              YY 39 44 0000 AAAAAA
 *
 *      AAAAAA - адрес параметра
 *
 *              0ЦЦЦ 00ГГ 0000 0000
 *
 * (4) переход на предыдущую команду
 *
 *              YY 08 04 0000 AAAAAA
 *
 *      AAAAAA - адрес предыдущей команды
 *
 * (5) запись собств. адреса
 *
 *              YY 19 34 0001 AAAAAA
 *
 *      AAAAAA - адрес параметра
 *
 *              000Ц ЦЦ00 ГГ80 0080
 *              0080 0000 0000 0000
 *
 * (6) запись метки
 *
 *              YY 15 04 0001 AAAAAA
 *
 *      AAAAAA - то же, что и в (3)
 *
 * (7) - (16) 10 команд записи блока в режиме без тегов
 *
 *              YY 1d 34 0094 AAAAAA
 *
 *      AAAAAA - адрес массива длиной 149 слов.
 *      Используются 1187 байт (из посл. слова - 3 байта).
 *      Во всех 10-ти командах адреса массивов разные.
 *
 * (17) чтение собств. адреса
 *
 *              YY 1a 54 0000 AAAAAA
 *
 *      AAAAAA - адрес буфера в 1 слово
 *
 * (18) - (27) 10 команд чтения блока в режиме без тегов
 *
 *              YY 1e 34 0094 AAAAAA
 *
 *      AAAAAA - адрес массива длиной 149 слов (1187 байт).
 *      Во всех 10-ти командах адреса массивов разные.
 *
 *
 * Структура массива разметки
 * --------------------------
 *
 * (1) 8 байт - маркер сектора                          0
 *
 *              0ЦЦЦ 00ГГ 0Б00 049b
 *                              ||
 *                             131*9
 *
 * (2) роспись 128*9/8 == 144 слов кодом разметки       1-144
 *
 *              5555 5555 5555 5555
 *              d8aa aaaa aaaa aaaa
 *              aad8 5555 5555 5555
 *              5555 d8aa aaaa aaaa
 *              aaaa aad8 5555 5555
 *              5555 5555 d8aa aaaa
 *              aaaa aaaa aad8 5555
 *              5555 5555 5555 d8aa
 *              aaaa aaaa aaaa aad8
 *
 * (3) 8 байт - контр. сумма                            145
 *
 *              ffff ffff ffff ffff
 *
 * (4) 8x3 байт                                         146-148
 *
 *              d8TT TTNN NNNN BBBB
 *              BBXX 0000 0000 0000
 *              0000 d800 0000 0000
 *
 *      TTTT    - сборка имени тома по маске 0303 0303 0303 0303
 *      NNNNNN  - номер блока 0..808*19*10-1
 *      BBBBBB  - разряды 44-21 таймера
 *      XX      - 0xd8 ^ биты четности предыдущего слова
 *
 *
 * Формат таблицы дорожек замены
 * -----------------------------
 *
 *      0ЦЦЦ 00ГГ 0ЦЦЦ 00ГГ
 *          |         |
 *       дорожка   плохая
 *       замены    дорожка
 *
 *
 * Дефектация дорожки
 * ------------------
 *
 * 1) в параметре команд (3) и (6) (addr) указывается адрес дорожки замены
 *
 * 2) в параметр команды (5) (waddr) добавляется признак
 *
 *      020Ц ЦЦ00 ...
 *      ~~
 * 3) нет команд (7) - (27)
 *
 *
 * Разметка дорожки замены
 * -------------------------
 *
 * 1) параметр команды (5):
 *
 *      0100 0000 0000 0000
 *      ...
 *
 * 2) параметр команды (2) - адрес дорожки замены
 *
 * 3) параметр команд (3) и (6) - адрес дорожки замены
 */

# include "vollab.h"
# include "svsb.h"
# include "scio.h"

# define K 1024
# define W ((int) sizeof (int))

# define HPC            19              /* головок на цилиндре */
# define BPT            10              /* блоков на дорожке */
# define NCYL           808             /* используемых цилиндров */
# define NPHYSCYL       814             /* всего цилиндров, реально 815 */

# define BPC            (HPC*BPT)       /* блоков на на цилиндре */
# define NBLOCKS        (NCYL*HPC*BPT)  /* всего блоков */
# define NTRACKS        (NCYL*HPC)      /* всего дорожек */
# define NPHYSTRACKS    (NPHYSCYL*HPC)  /* физически дорожек */
# define NREPTRACKS     ((NPHYSCYL-NCYL)*HPC)   /* дорожек замены */

# define VLABELSZ       9               /* размер метки тома в блоках */

# define DFLTVOLNAME    "unknown"       /* имя тома по умолчанию */
# define EXTVOL         ".vol"          /* расширение раздела-тома */
# define EXTLBL         ".lbl"          /* расширение раздела-метки */

# define MAXRETRY       10              /* max number of retry */

# define DEBUG

/* состояние дорожек замены */

# define TFREE  0                       /* свободно */
# define TUSED  1                       /* занято */
# define TBAD   -1                      /* плохая дорожка */

struct word {
	int right:32;
	int left:32;
};

struct fmtcode {
	struct word word [9];
};

struct fmtblock {
	int marker;
	struct fmtcode code [16];
	unsigned checksum;
	int nblock;
	int time;
	int reserved;
};

struct fmtcode fmtcode = {
	{ 0x55555555, 0x55555555, },
	{ 0xaaaaaaaa, 0xd8aaaaaa, },
	{ 0x55555555, 0xaad85555, },
	{ 0xaaaaaaaa, 0x5555d8aa, },
	{ 0x55555555, 0xaaaaaad8, },
	{ 0xd8aaaaaa, 0x55555555, },
	{ 0xaad85555, 0xaaaaaaaa, },
	{ 0x5555d8aa, 0x55555555, },
	{ 0xaaaaaad8, 0xaaaaaaaa, },
};

static struct sccmd fmtbadtrack [] = {
/* дефектация дорожки   addr    count   flg     cmd     unit    */
/* &fmtmask */          0,      0,      0x14,   0x1f,   0,
/* &fmtcyl */           0,      0,      0x64,   0x07,   0,
/* &fmtaddr */          0,      0,      0x44,   0x39,   0,
/* &fmtbadtrack[2] */   0,      0,      0x04,   0x08,   0,
/* &fmtwaddr */         0,      1,      0x34,   0x19,   0,
/* &fmtaddr */          0,      1,      0,      0x15,   0,
			0,      0,      0,      0,      0,
};

static struct sccmd fmttrack [] = {
/* разметка дорожки     addr    count   flg     cmd     unit    */
/* &fmtmask */          0,      0,      0x14,   0x1f,   0,
/* &fmtcyl */           0,      0,      0x64,   0x07,   0,
/* &fmtaddr */          0,      0,      0x44,   0x39,   0,
/* &fmttrack[2] */      0,      0,      0x04,   0x08,   0,
/* &fmtwaddr */         0,      1,      0x34,   0x19,   0,
/* &fmtaddr */          0,      1,      0x04,   0x15,   0,
/* &fmtwr[0] */         0,      0x94,   0x34,   0x1d,   0,
/* &fmtwr[1] */         0,      0x94,   0x34,   0x1d,   0,
/* &fmtwr[2] */         0,      0x94,   0x34,   0x1d,   0,
/* &fmtwr[3] */         0,      0x94,   0x34,   0x1d,   0,
/* &fmtwr[4] */         0,      0x94,   0x34,   0x1d,   0,
/* &fmtwr[5] */         0,      0x94,   0x34,   0x1d,   0,
/* &fmtwr[6] */         0,      0x94,   0x34,   0x1d,   0,
/* &fmtwr[7] */         0,      0x94,   0x34,   0x1d,   0,
/* &fmtwr[8] */         0,      0x94,   0x34,   0x1d,   0,
/* &fmtwr[9] */         0,      0x94,   0x34,   0x1d,   0,
/* &fmtbuf */           0,      0,      0x54,   0x1a,   0,
/* &fmtrd[0] */         0,      0x94,   0x34,   0x1e,   0,
/* &fmtrd[1] */         0,      0x94,   0x34,   0x1e,   0,
/* &fmtrd[2] */         0,      0x94,   0x34,   0x1e,   0,
/* &fmtrd[3] */         0,      0x94,   0x34,   0x1e,   0,
/* &fmtrd[4] */         0,      0x94,   0x34,   0x1e,   0,
/* &fmtrd[5] */         0,      0x94,   0x34,   0x1e,   0,
/* &fmtrd[6] */         0,      0x94,   0x34,   0x1e,   0,
/* &fmtrd[7] */         0,      0x94,   0x34,   0x1e,   0,
/* &fmtrd[8] */         0,      0x94,   0x34,   0x1e,   0,
/* &fmtrd[9] */         0,      0x94,   0x30,   0x1e,   0,
			0,      0,      0,      0,      0,
};

int fmtmask;            /* c000 0000 0000 0000 */
int fmtcyl;             /* 0000 0ЦЦЦ 00ГГ XXXX */
int fmtaddr;            /* 0ЦЦЦ 00ГГ 0000 0000 */
int fmtwaddr [2];       /* 000Ц ЦЦ00 ГГ80 0080  0080 0000 0000 0000 */

struct fmtblock fmtwr [BPT];
struct fmtblock fmtrd [BPT];

int fmtbuf;

int badtrack [NREPTRACKS];      /* номера плохих дорожек из используемых */
int nbad;                       /* количество плохих дорожек */

int reptrack [NREPTRACKS];      /* состояние дорожек замены */
struct btt_entry reptab [NREPTRACKS];   /* таблица замены */

int buf [VLABELSZ*K/W];
int unit;                       /* номер форматируемого устройства */
int devunit;                    /* номер форматируемого устройства */

extern hdchan;                  /* номер канала дисков */
extern hdsbuf [];
extern chantab [];
extern struct sccmd hdsense;

yes ()
{
	char rep [80];
	int r;

	printf ("? (y, n, д, н) [н] ");
	gets (rep);
	r = rep [0];
	if (r=='y' || r=='Y' || r=='д' || r=='Д')
		return (1);
	if (r=='n' || r=='N' || r=='н' || r=='Н')
		return (-1);
	return (0);
}

main ()
{
	char volname [80];

	printf (MSG ("\nFORMAT %s    %s\n", "\nРАЗМЕТКА %s    %s\n"),
		BOOTVERSION, MSG (LCOPYRIGHT, RCOPYRIGHT));
	printf (MSG ("\nUsing this program you may OVERWRITE information on your hard disks.\n",
		"\nПри работе с этой программой Вы можете ЗАТЕРЕТЬ информацию на дисках.\n"));
	printf (MSG ("\nIt is strongly recommended to TURN unnecessary disks 'READ-ONLY'.\n",
		"\nНастоятельно рекомендуется ЗАКРЫТЬ ненужные дискм на ЗАПИСЬ.\n"));
	printf (MSG ("Do you want to continue",
		"Продолжать"));
	if (yes () != 1)
		return;
	unit = getunit ();
	nbad = 0;
	getname (volname);
	printf (MSG ("\nFormat unit %d, volume name '%s'",
		"\nФорматировать устройство %d, том '%s'"),
		unit, volname);
	switch (yes ()) {
	default:
		return;
	case 0:
		printf (MSG ("\nMake volume label on unit %d, volume name '%s'",
			"\nСоздать метку тома на устройстве %d, том '%s'"),
			unit, volname);
		if (yes () != 1)
			return;
		goto labelonly;
	case 1:
		break;
	}
	/* инициализация переменных и массивов разметки */
	init (unit);

	printf (MSG ("\nINITIAL FORMATTING\n", "\nПЕРВИЧНАЯ РАЗМЕТКА\n"));
	if (! format1 ())
		return;

	if (nbad) {
		printf (MSG ("\nBAD TRACKS REMAPPING\n", "\nДЕФЕКТАЦИЯ\n"));
		if (! format2 ())
			return;
	}
labelonly:
	printf (MSG ("\nWRITING VOLUME LABEL\n", "\nЗАПИСЬ МЕТКИ ТОМА\n"));
	wrlabel (volname, unit);
	printf (MSG ("\nFormat completed.\n", "\nФорматирование закончено.\n"));
}

format1 ()
{
	int track;

	/* первичная разметка и заполнение массива плохих дорожек */
	for (track=0; track<NPHYSTRACKS; ++track) {
		idle (1);
		if (! (track % HPC))
			printf ("\r%d", track/HPC);
		if (! formattrack (track, 0)) {
			printf ("\r%d.%d\t", track/HPC, track%HPC);
			printf (MSG ("\t***** Bad track\n",
				"\t***** Плохая дорожка\n"));
			if (track >= NTRACKS)
				/* плохая дорожка замены */
				reptrack [track-NTRACKS] = TBAD;
			else {
				if (nbad >= NREPTRACKS) {
					printf (MSG ("\nTOO MANY BAD TRACKS -- FORMAT ABORTED\n",
						"\nСЛИШКОМ МНОГО ПЛОХИХ ДОРОЖЕК -- ФОРМАТИРОВАНИЕ НЕВОЗМОЖНО\n"));
					return (0);
				}
				badtrack [nbad++] = track;
			}
			printf ("%d", track/HPC);
		}
	}
	return (1);
}

format2 ()
{
	int track, rep, i;

	/* дефектация плохих дорожек */
	for (i=0; i<nbad; ++i) {
		track = badtrack [i];
		do
			if (! (rep = findrep ())) {
				printf (MSG ("\nBAD TRACK TABLE OVERFLOW -- FORMAT ABORTED\n",
					"\nНЕТ МЕСТА В ТАБЛИЦЕ ЗАМЕНЫ -- ФОРМАТИРОВАНИЕ НЕВОЗМОЖНО\n"));
				return (0);
			}
		while (! formattrack (rep, 1));
		printf ("%d.%d -> %d.%d\n", track/HPC, track%HPC,
			rep/HPC, rep%HPC);
		if (! formatbad (track, rep)) {
			printf (MSG ("\nREASSIGNMENT ERROR -- FORMAT ABORTED\n",
				"\nОШИБКА ДЕФЕКТАЦИИ -- ФОРМАТИРОВАНИЕ ПРЕРВАНО\n"));
			return (0);
		}
		reptab[i].btt_badcyl = track / HPC;
		reptab[i].btt_badhead = track % HPC;
		reptab[i].btt_newcyl = rep / HPC;
		reptab[i].btt_newhead = rep % HPC;
	}
	return (1);
}

getunit ()
{
	char line [80];

	printf (MSG ("\nEnter unit number: (0-7) [0] ",
		"\nВведите номер устройства: (0-7) [0] "));
	gets (line);
	return (atoi (line));
}

init (u)
{
	register struct sccmd *p;
	register struct fmtcode *c;
	register struct fmtblock *b;
	int i;

	fmtmask = 0xc0;            /* c000 0000 0000 0000 */
	fmtmask <<= 56;

	fmtwaddr [1] = 0x80;
	fmtwaddr [1] <<= 48;

	devunit = chantab [hdchan] + u;
	for (p=fmttrack; p->cmd; ++p)
		p->unit = devunit;
	for (p=fmtbadtrack; p->cmd; ++p)
		p->unit = devunit;

	hdsense.unit = devunit;
	hdsense.addr = (int) hdsbuf;

	fmtbadtrack[0].addr = (int) &fmtmask;
	fmtbadtrack[1].addr = (int) &fmtcyl;
	fmtbadtrack[2].addr = (int) &fmtaddr;
	fmtbadtrack[3].addr = (int) &fmtbadtrack[2];
	fmtbadtrack[4].addr = (int) fmtwaddr;
	fmtbadtrack[5].addr = (int) &fmtaddr;

	fmttrack[0].addr = (int) &fmtmask;
	fmttrack[1].addr = (int) &fmtcyl;
	fmttrack[2].addr = (int) &fmtaddr;
	fmttrack[3].addr = (int) &fmttrack[2];
	fmttrack[4].addr = (int) fmtwaddr;
	fmttrack[5].addr = (int) &fmtaddr;
	fmttrack[6].addr = (int) &fmtwr[0];
	fmttrack[7].addr = (int) &fmtwr[1];
	fmttrack[8].addr = (int) &fmtwr[2];
	fmttrack[9].addr = (int) &fmtwr[3];
	fmttrack[10].addr = (int) &fmtwr[4];
	fmttrack[11].addr = (int) &fmtwr[5];
	fmttrack[12].addr = (int) &fmtwr[6];
	fmttrack[13].addr = (int) &fmtwr[7];
	fmttrack[14].addr = (int) &fmtwr[8];
	fmttrack[15].addr = (int) &fmtwr[9];
	fmttrack[16].addr = (int) &fmtbuf;
	fmttrack[17].addr = (int) &fmtrd[0];
	fmttrack[18].addr = (int) &fmtrd[1];
	fmttrack[19].addr = (int) &fmtrd[2];
	fmttrack[20].addr = (int) &fmtrd[3];
	fmttrack[21].addr = (int) &fmtrd[4];
	fmttrack[22].addr = (int) &fmtrd[5];
	fmttrack[23].addr = (int) &fmtrd[6];
	fmttrack[24].addr = (int) &fmtrd[7];
	fmttrack[25].addr = (int) &fmtrd[8];
	fmttrack[26].addr = (int) &fmtrd[9];

	for (b=fmtwr; b<fmtwr+BPT; ++b) {
		b->marker = ((b-fmtwr) + 1) << 24 | (131 * 9);
		for (c=b->code; c<b->code+16; ++c)
			*c = fmtcode;
		b->checksum = ~(unsigned)0;
		b->nblock = makenblock (0);
		b->time = 0xd8;
		b->time <<= 48;
		b->reserved = 0xd8;
		b->reserved <<= 40;
	}
	for (i=0; i<NREPTRACKS; ++i);
		reptrack [i] = TFREE;
}

makenblock (n)
{
	int b;

	b = 0xd8;
	b <<= 56;
	return (b | n << 16);
}

getname (s)
char *s;
{
	/* ввести имя тома */
	printf (MSG ("\nVolume name: [%s] ",
		"\nИмя тома: [%s] "), DFLTVOLNAME);
	gets (s);
	if (! s [0]) {
		strcpy (s, DFLTVOLNAME);
		return (0);
	}
}

findrep ()
{
	/* поиск свободной дорожки замены */
	/* 0 если не найдена */
	int i;

	for (i=0; i<NREPTRACKS; ++i);
		if (reptrack [i] == TFREE) {
			reptrack [i] = TUSED;
			return (i+NTRACKS);
		}
	return (0);
}

wrlabel (volname, u)
char *volname;
{
	/* запись метки тома */
	static char unitname [] = "hd(0)";
	register struct vl_fixed *lbl;
	register struct pt_entry *pt;
	register struct btt_entry *btt;
	char name [80];
	int i, n;

	unitname [3] = '0' + u;
	if ((i = open (unitname, 1)) < 0)
		return;
	for (n=0; n<VLABELSZ*K/W; ++n)
		buf [n] = 0;
	lbl = (struct vl_fixed *) buf;
	store8name (volname, &lbl->vl_name); /* volume name */
	lbl->vl_magic = VL_MAGIC;        /* magic word */
	lbl->vl_fmttime = 0;             /* formatting time */
	lbl->vl_systemid = 1;            /* system where format was done */
	lbl->vl_revision = 0;            /* volume label revision */
	lbl->vl_version = 1;             /* volume label version */
	lbl->vl_size = NBLOCKS-2*VLABELSZ; /* volume size */
	lbl->vl_vlsize = VLABELSZ;       /* volume label size in blocks */
	lbl->vl_blksize = 7;             /* log2 (block size) */
	lbl->vl_model = 2;               /* storage media model */
	lbl->vl_vlcopy = NBLOCKS-VLABELSZ; /* block address of label copy */
	lbl->vl_restrack = NTRACKS;      /* 1st reserved track # */
	lbl->vl_nreserved = NREPTRACKS;  /* number of reserved tracks */
	lbl->vl_ptntab = 6+NREPTRACKS;   /* partition table address */
	lbl->vl_maxptn = 256;            /* maximum number of partitions */

	name [0] = name [1] = 0;
	strcpy (name+2, volname);
	/* strcat (name, EXTVOL); */

	pt = (struct pt_entry *) (buf + lbl->vl_ptntab);
	storename (name, pt);           /* partition name */
	pt->pt_offset = 0;              /* partition offset */
	pt->pt_version = 1;             /* partition version */
	pt->pt_size = NBLOCKS;          /* blocks in partition */

	strcpy (name, volname);
	strcat (name, EXTLBL);

	++pt;
	storename (name, pt);           /* partition name */
	pt->pt_offset = 0;              /* partition offset */
	pt->pt_version = 1;             /* partition version */
	pt->pt_size = VLABELSZ;         /* blocks in partition */

	/* заполнение таблицы замены */
	btt = (struct btt_entry *) (buf + 6);
	for (n=0; n<nbad; ++n)
		*btt++ = reptab [n];

	write (i, (char *) buf, VLABELSZ*K);
	close (i);
}

storename (s, p)
char *s;
int *p;
{
	int n, b;

	p[0] = 0;
	p[1] = p[1] << 32 >> 32;
	b = 1;
	for (n=0; n<8 && (b || *s); ++s, ++n) {
		if (*s)
			b = 0;
		p[0] |= *s << (56-8*n);
	}
	for (n=0; n<4 && *s; ++s, ++n)
		p[1] |= *s << (56-8*n);
}

store8name (s, p)
char *s;
int *p;
{
	int n;

	*p = 0;
	for (n=0; n<8 && *s; ++s, ++n)
		*p |= *s << (56-8*n);
}

fmtio (prog)
struct sccmd *prog;
{
	int retry, csw, csb;
# ifdef DEBUG
	int caw, ccw;
# endif

	retry = 0;
repeat:
	csw = scwait (hdchan, prog);
	switch (csw>>1 & 7) {
	case 2:         /* отказ канала */
		break;
	case 3:         /* ош. адреса ВУ */
		printf (MSG ("format: bad unit address\n", "format: ош. адреса ВУ\n"));
		exit (1);
	case 4:         /* ВУ не найдено */
		printf (MSG ("format: unit not found\n", "format: устройство не найдено\n"));
		exit (1);
	case 6:         /* ош. чтения ОЗУ */
		printf (MSG ("format: RAM error\n", "format: ошибка ОЗУ\n"));
		exit (1);
	case 7:         /* переход в канале */
		printf (MSG ("format: bad transfer\n", "format: плохой переход\n"));
		exit (1);
	case 0:
		if ((csw >> 32 & 0xff) != devunit)
			break;
		if (csw & 0x1f80)
			break;
		csb = csw >> 16 & 0xffff;
		if (csb == 0x0804) {
done:                   /* обнуление канала */
			_in_ (SC_CSW (hdchan) | SCA_RESET | SCA_NOHALT);
			return (0);
		}
		if (csb == 0x4a04) {
unitbusy:               /* обнуление канала */
			_in_ (SC_CCW (hdchan) | SCA_RESET|SCA_NOHALT|SCA_SBLOCK);
			goto repeat;
		}
		break;
	case 1:
		if ((csw >> 24 & 0xff) != devunit)
			break;
		if (csw & 0x1f80)
			break;
		csb = csw >> 16 & 0xff;
		if (csb == 0x0c || csb == 0x08)
			goto done;
		if (csb == 0x4e)
			goto unitbusy;
		break;
	}
# ifdef DEBUG
	ccw = _in_ (SC_CCW (hdchan) | SCA_NOHALT);      /* УСК */
	caw = _in_ (SC_CAW (hdchan) | SCA_NOHALT);      /* АСК */
# endif
	_in_ (SC_CCW (hdchan) | SCA_RESET | SCA_NOHALT); /* упр. слово канала */

	/* уточнение состояния */
	scwait (hdchan, &hdsense);

	_in_ (SC_CSW (hdchan) | SCA_RESET | SCA_NOHALT); /* обнуление канала */
# ifdef DEBUG
	printf ("\nУСК=%w  ССК=%w  АСК=%w\n", ccw, csw, caw);
	printf ("БУС0=%w  БУС1=%w  БУС2=%w\n", hdsbuf[0], hdsbuf[1], hdsbuf[2]);
# endif
	/* if ((hdsbuf [0] & 0xff) == 0x53) */
	if (retry++ <= MAXRETRY)
		goto repeat;
	return (-1);
}

formattrack (track, repflag)
{
	/* форматирование дорожки */
	/* возврат признака хорошей дорожки */
	int c, h, i;


	/* 1) заполнить fmtaddr, fmtwaddr, fmtcyl */
	c = track / HPC;        /* номер цилиндра */
	h = track - c * HPC;    /* номер головки */
	fmtcyl = c<<32 | h<<16;
	fmtaddr = fmtcyl << 16;
	if (repflag) {
		/* форматирование дорожки замены */
		fmtwaddr [0] = 0x01;
		fmtwaddr [0] <<= 56;
	} else
		fmtwaddr [0] = fmtcyl << 8 | 0x800080;

	/* 2) заполнить поля marker и nblock в fmtwr [i] */
	for (i=0; i<BPT; ++i) {
		fmtwr[i].marker = fmtaddr | (i + 1) << 24 | (131 * 9);
		fmtwr[i].nblock = makenblock (track*BPT + i + 1);
		c = (_nbits_ (fmtwr[i].nblock << 8 >> 32) & 1) << 7;
		c |= (_nbits_ (fmtwr[i].nblock << 40) & 1) << 6;
		fmtwr[i].time = (c ^ 0xd8) << 48;
	}
	/* 3) запустить fmtio (fmttrack) */
	if (fmtio (fmttrack) < 0)
		return (0);

	/* 4) сравнить fmtrd и fmtwr */
	for (i=0; i<BPT; ++i)
		if (bcmp ((char *) &fmtwr[i], (char *) &fmtrd[i],
		    sizeof (struct fmtblock))) {
# ifdef DEBUG
			printf (MSG ("\tchsum\n", "\tкс\n"));
# endif
			return (0);
		}

	/* 5) вернуть код ответа */
	return (1);
}

formatbad (track, rep)
{
	/* дефектация дорожки */
	/* возврат признака хорошей дорожки */
	int c, h;

	/* 1) заполнить fmtaddr, fmtwaddr, fmtcyl */
	c = track / HPC;        /* номер цилиндра */
	h = track - c * HPC;    /* номер головки */
	fmtcyl = c<<32 | h<<16;
	fmtwaddr [0] = 0x02;
	fmtwaddr [0] <<= 56;
	fmtwaddr [0] |= fmtcyl << 8 | 0x800080;

	c = rep / HPC;          /* номер цилиндра дорожки замены */
	h = rep - c * HPC;      /* номер головки дорожки замены */
	fmtaddr = (c<<32 | h<<16) << 16;

	/* 2) запустить fmtio (fmtbadtrack) */
	if (fmtio (fmtbadtrack) < 0)
		return (0);

	/* 3) вернуть код ответа */
	return (1);
}
