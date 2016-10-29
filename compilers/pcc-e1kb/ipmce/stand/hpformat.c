/*
 * Разметка ЕС-диска для Эльбрус-Б
 * ===============================
 * в формате 808 x 19 x 3 x 4k без тегов
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
 * В данном случае N=3, L=4096 байт,
 * объем диска 808*19*12k = 184,224k (179 Mb).
 *
 * (L + 133) * N + 179 = 12866 < 13440
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
 * (7) - (9) 3 команды записи блока в режиме без тегов
 *
 *              YY 1d 04 0200 AAAAAA
 *
 *      AAAAAA - адрес массива длиной 512 слов.
 *      Во всех 3-х командах адреса массивов разные.
 *
 * (10) чтение собств. адреса
 *
 *              YY 1a 54 0000 AAAAAA
 *
 *      AAAAAA - адрес буфера в 1 слово
 *
 * (11) - (13) 3 команды чтения блока в режиме без тегов
 *
 *              YY 1e 04 0200 AAAAAA
 *
 *      AAAAAA - адрес массива длиной 512 слов (4096 байт).
 *      Во всех 3-х командах адреса массивов разные.
 *
 *
 * Структура массива разметки
 * --------------------------
 *
 * (1) 8 байт - маркер сектора                          0
 *
 *              0ЦЦЦ 00ГГ 0Б00 1000
 *                              ||
 *                             4096
 *
 * (2) роспись 512 слов кодом разметки                  1-512
 *
 *              5555 5555 5555 5555
 *              aaaa aaaa aaaa aaaa
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
 * 3) нет команд (7) - (13)
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

# include "svsb.h"
# include "scio.h"

# define K              1024
# define W              ((int) sizeof (int))
# define HPBSIZE        4096            /* block size */

# define HPC            19              /* головок на цилиндре */
# define BPT            3               /* блоков на дорожке */
# define NCYL           808             /* используемых цилиндров */
# define NPHYSCYL       814             /* всего цилиндров, реально 815 */

# define BPC            (HPC*BPT)       /* блоков на на цилиндре */
# define NBLOCKS        (NCYL*HPC*BPT)  /* всего блоков */
# define NTRACKS        (NCYL*HPC)      /* всего дорожек */
# define NPHYSTRACKS    (NPHYSCYL*HPC)  /* физически дорожек */
# define NREPTRACKS     ((NPHYSCYL-NCYL)*HPC)   /* дорожек замены */

# define MAXRETRY       10              /* max number of retry */

# define DEBUG

/* состояние дорожек замены */

# define TFREE  0                       /* свободно */
# define TUSED  1                       /* занято */
# define TBAD   -1                      /* плохая дорожка */

int fmtcode = 0x5555555555555555;

struct fmtblock {
	int marker;
	int block [HPBSIZE/W];
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
/* &fmtwr[0] */         0,      0x200,  0x04,   0x1d,   0,
/* &fmtwr[1] */         0,      0x200,  0x04,   0x1d,   0,
/* &fmtwr[2] */         0,      0x200,  0x04,   0x1d,   0,
/* &fmtbuf */           0,      0,      0x54,   0x1a,   0,
/* &fmtrd[0] */         0,      0x200,  0x04,   0x1e,   0,
/* &fmtrd[1] */         0,      0x200,  0x04,   0x1e,   0,
/* &fmtrd[2] */         0,      0x200,  0x00,   0x1e,   0,
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
	printf (MSG ("\nFORMAT 4K %s    %s\n", "\nРАЗМЕТКА 4K %s    %s\n"),
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
	printf (MSG ("\nFormat unit %d", "\nФорматировать устройство %d"),
		unit);
	if (yes () != 1)
		return;
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
	register *c;
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
	fmttrack[9].addr = (int) &fmtbuf;
	fmttrack[10].addr = (int) &fmtrd[0];
	fmttrack[11].addr = (int) &fmtrd[1];
	fmttrack[12].addr = (int) &fmtrd[2];

	for (b=fmtwr; b<fmtwr+BPT; ++b) {
		b->marker = ((b-fmtwr) + 1) << 24 | HPBSIZE;
		for (c=b->block; c<b->block+HPBSIZE/W; ) {
			*c++ = fmtcode;
			*c++ = ~fmtcode;
		}
	}
	for (i=0; i<NREPTRACKS; ++i);
		reptrack [i] = TFREE;
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
	for (i=0; i<BPT; ++i)
		fmtwr[i].marker = fmtaddr | (i + 1) << 24 | HPBSIZE;

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
