
# include <stdio.h>
# include <ctype.h>

# ifdef CROSS
#    include "../h/a.out.h"
# else
#    include <a.out.h>
# endif

# define W 8                    /* длина слова в байтах */

struct tsthdr {                 /* заголовок */
	int magic;              /* 1 byte */
	long addr;              /* 4 bytes */
	long len;               /* 4 bytes */
} hdr;

FILE *text;

char *lcmd2 [] = {
	"зп",   "зпм",  "счн",  "счм",  "сл",   "вч",   "вчоб", "вчаб",
	"сч",   "и",    "нтж",  "слк",  "из",   "или",  "дел",  "умн",
	"сбр",  "рзб",  "чед",  "нед",  "слпп", "вчпп", "сдпп", "уржп",
	"пб",   "пв",   "циклу","цикл", "@1c",  "@1d",  "сдлп", "@1f",
	"пирв", "пинр", "пибр", "пимн", "пимр", "пибл", "@26",  "@27",
	"прв",  "пнр",  "пбр",  "пмн",  "пмр",  "пбл",  "пос",  "пно",
	"счт",  "сем",  "счх",  "счк",  "зпн",  "зпк",  "зпх",  "зпт",
	"где",  "пфс",  "пфа",  "пф",   "уиа",  "слиа", "груп", "@3f",
	"счц",  "счцм", "счнц", "счлм", "слц",  "вчц",  "вчобц","вчабц",
	"счл",  "ил",   "нтжл", "слкл", "изц",  "илил", "@4e",  "умнц",
	"сбрл", "рзбл", "чедл", "недл", "слф",  "вчф",  "вчобф","вчабф",
	"@58",  "@59",  "@5a",  "@5b",  "@5c",  "@5d",  "@5e",  "@5f",
	"зп.",  "зпм.", "счн.", "счм.", "сл.",  "вч.",  "вчоб.","вчаб.",
	"сч.",  "и.",   "нтж.", "слк.", "из.",  "или.", "дел.", "умн.",
	"сбр.", "рзб.", "чед.", "нед.", "слпп.","вчпп.","сдпп.","@77",
	"счт.", "сем.", "счх.", "счк.", "зпн.", "зпк.", "сдлп.","зпт.",
};

char *lcmd1 [] = {
	"atx",  "stx",  "xtra", "xts",  "aadx", "asux", "xsua", "amsx",
	"xta",  "aax",  "aex",  "arx",  "avx",  "aox",  "adx",  "amux",
	"apkx", "aux",  "acx",  "anx",  "eax",  "esx",  "asrx", "xtr",
	"uj",   "vjm",  "vgm",  "vlm",  "@1c",  "@1d",  "alx",  "@1f",
	"vzm",  "vim",  "vpzm", "vnm",  "vnzm", "vpm",  "@26",  "@27",
	"uz",   "ui",   "upz",  "un",   "unz",  "up",   "uiv",  "uzv",
	"xtga", "xtqa", "xtha", "xtta", "ztx",  "atcx", "ath",  "atgx",
	"pctm", "atc",  "utcs", "wtc",  "vtm",  "utm",  "do",   "@3f",
	"xtal", "xtsl", "utra", "uts",  "aadu", "asuu", "usua", "amu",
	"uta",  "aau",  "aeu",  "aru",  "avu",  "aou",  "@4e",  "amuu",
	"apu",  "auu",  "acu",  "anu",  "@54",  "@55",  "@56",  "@57",
	"@58",  "@59",  "@5a",  "@5b",  "@5c",  "@5d",  "@5e",  "@5f",
	"atk",  "stk",  "ktra", "kts",  "aadk", "asuk", "ksua", "amk",
	"kta",  "aak",  "aek",  "ark",  "avk",  "aok",  "adk",  "amuk",
	"apk",  "auk",  "ack",  "ank",  "eak",  "esk",  "ask",  "@77",
	"ktga", "ktsa", "ktha", "ktta", "ztk",  "atck", "alk",  "atgk",
};

char *scmd2 [] = {
	"эк",   "выт",  "врг",  "вых",  "$04",  "$05",  "ург",  "стоп",
	"$08",  "$09",  "$0a",  "$0b",  "$0c",  "$0d",  "$0e",  "$0f",
	"$10",  "вдп",  "$12",  "$13",  "кор",  "$15",  "сдп",  "$17",
	"врж",  "вд",   "нтжп", "упр",  "слп",  "вчп",  "сдл",  "урж",
	"уи",   "уим",  "ви",   "виц",  "уии",  "сли",  "вчиоб","вчи",
	"ур",   "урм",  "вр",   "$2b",  "ури",  "$2d",  "$2e",  "$2f",
	"$30",  "$31",  "$32",  "$33",  "цела", "целф", "сдпд", "$37",
	"уисч", "$39",  "$3a",  "$3b",  "инв",  "$3d",  "сдлд", "лог",
	"$40",  "$41",  "$42",  "$43",  "$44",  "$45",  "$46",  "$47",
	"$48",  "$49",  "$4a",  "$4b",  "$4c",  "$4d",  "$4e",  "$4f",
	"$50",  "вдпм", "$52",  "$53",  "корм", "$55",  "сдпм", "$57",
	"вржм", "вдм",  "нтжпм","$5b",  "слпм", "вчпм", "сдлм", "уржм",
	"$60",  "$61",  "вим",  "вицм", "$64",  "$65",  "$66",  "$67",
	"$68",  "$69",  "врм",  "$6b",  "$6c",  "$6d",  "$6e",  "$6f",
	"$70",  "$71",  "$72",  "$73",  "целам","целфм","сдпдм","$77",
	"уисчм","$79",  "$7a",  "$7b",  "инвм", "$7d",  "сдлдм","логм",
};

char *scmd1 [] = {
	"ex",   "pop",  "rmod", "ij",   "$04",  "$05",  "wmod", "halt",
	"$08",  "$09",  "$0a",  "$0b",  "$0c",  "$0d",  "$0e",  "$0f",
	"$10",  "yma",  "$12",  "$13",  "ecn",  "$15",  "asn",  "$17",
	"rta",  "yta",  "een",  "set",  "ean",  "esn",  "aln",  "ntr",
	"ati",  "sti",  "ita",  "iita", "mtj",  "jam",  "jsm",  "msj",
	"ato",  "sto",  "ota",  "$2b",  "mto",  "$2d",  "$2e",  "$2f",
	"$30",  "$31",  "$32",  "$33",  "ent",  "int",  "asy",  "$37",
	"atia", "$39",  "$3a",  "$3b",  "aca",  "$3d",  "aly",  "tst",
	"$40",  "$41",  "$42",  "$43",  "$44",  "$45",  "$46",  "$47",
	"$48",  "$49",  "$4a",  "$4b",  "$4c",  "$4d",  "$4e",  "$4f",
	"$50",  "yms",  "$52",  "$53",  "ecns", "$55",  "asns", "$57",
	"rts",  "yts",  "eens", "$5b",  "eans", "esns", "alns", "ntrs",
	"$60",  "$61",  "its",  "iits", "$64",  "$65",  "$66",  "$67",
	"$68",  "$69",  "ots",  "$6b",  "$6c",  "$6d",  "$6e",  "$6f",
	"$70",  "$71",  "$72",  "$73",  "ents", "ints", "asys", "$77",
	"atis", "$79",  "$7a",  "$7b",  "acs",  "$7d",  "alys", "tsts",
};

char **lcmd = lcmd1, **scmd = scmd1;

# define MSG(l,r) (msg ? (r) : (l))

char msg;

initmsg ()
{
	register char *p;
	extern char *getenv ();

	msg = (p = getenv ("MSG")) && *p == 'r';
}

main (argc, argv)
register char **argv;
{
	int yesarg;     /* были ли параметры - имена файлов */

	initmsg ();
	while(--argc) {
		++argv;
		if (**argv == '-') {
			register char *cp;

			for (cp = *argv+1; *cp; cp++) switch (*cp) {
			case 'b':
				lcmd = lcmd2;
				scmd = scmd2;
				break;
			}
		} else {
			show (*argv);
			yesarg = 1;
		}
	}
	if (! yesarg) show ((char *) 0);
	return (0);
}

show (fname)
register char *fname;
{
	if (! fname)
		text = stdin;
	else if (! (text = fopen (fname, "r"))) {
		printf (MSG ("show: %s not found\n",
				"show: %s не найден\n"),
			fname);
		return;
	}
	fgethdr (text, &hdr);
	if (feof (text)) {
		printf (MSG ("show: %s not a test file\n",
				"show: %s не тестовый файл\n"),
			fname);
		return;
	}
	if (! fname)
		printf ("; File \"%s\"\n", fname);
	printf (";\n");
	printf ("; Magic %xh Address %lxh Length %lxh End %lxh\n",
		hdr.magic, hdr.addr, hdr.len,  hdr.addr+hdr.len);
	printf (";\n");
	showfile (hdr.addr, hdr.len);
}

long fgeth (f)
register FILE *f;
{
	register long h;

	h = (long) getc (f) << 24;
	h |= (long) getc (f) << 16;
	h |= getc (f) << 8;
	h |= getc (f);
	return (h);
}

fgethdr (text, h)
register FILE *text;
register struct tsthdr *h;
{
	h->magic = getc (text);
	h->addr = fgeth (text);
	h->len = fgeth (text);
}

showfile (addr, len)
long addr, len;
{
	register t;
	register long l, r;

	while (len--) {
		t = getc (text);
		l = fgeth (text);
		r = fgeth (text);
		printf ("x%lx:\t", addr++);
		prcmd (l);
		printf ("\t; .word 0%08lx%08lxh\n", l, r);
		printf ("\t");
		prcmd (r);
		printf ("\n");
	}
}

char *prhex (x)
register long x;
{
	register i, c;
	static char buf [10];
	register char *p = buf;

	for (i=16; i>=0; i-=4)
		if (c = (x >> i & 0xf))
			break;
	if (c > 9)
		*p++ = '0';
	for (; i>=0; i-=4)
		*p++ = "0123456789abcdef" [x >> i & 0xf];
	*p++ = 'h';
	*p = 0;
	return (buf);
}

prcmd (c)
register long c;
{
	register r;
	register long a;

	if (r = (c >> 28) & 017)
		printf ("%d\t", r);
	else
		printf ("\t");
	if ((c & 0xff00000L) == 0x3f00000L) {
		if (c & 0x80000L)
			printf ("$%02x", c>>12 & 0377);
		else
			printf (scmd [c>>12 & 0177]);
		a = c & 07777;
		if (a & 04000)
			a -= 010000;
	} else {
		if (c & 0x8000000L)
			printf ("@%02x", c>>20 & 0377);
		else
			printf (lcmd [c>>20 & 0177]);
		a = c & 0xfffff;
		if (a & 0x80000)
			a -= 0x100000;
	}
	printf ("\t");
	if (! a)
		;
	else if (a < 16 && a > -16)
		printf ("%d", a);
	else if (a < 0)
		printf ("-%s", prhex (-a));
	else
		printf (prhex (a));
	printf ("\t");
}
