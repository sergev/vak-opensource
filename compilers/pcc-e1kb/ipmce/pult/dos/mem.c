# include <stdio.h>

# ifndef NULL
#    define NULL ((char *) 0)
# endif

# define MCMD		1
# define MREV		2
# define MMADL		0x10
# define MRACH		0x20
# define MTYAP		0x40
# define MBIN		0x100

# define ADDRMASK	0x7fffff

static char *mnemLrach [128] = {
	"зп",   "зпм",  "счн",  "счм",  "сл",   "вч",   "вчоб", "вчаб",
	"сч",   "и",    "нтж",  "слк",  "из",   "или",  "дел",  "умн",
	"сбр",  "рзб",  "чед",  "нед",  "слпп", "вчпп", "сдпп", "уржп",
	"пб",   "пв",   "циклу","цикл", 0,	0,	"сдлп", 0,
	"пирв", "пинр", "пибр", "пимн", "пимр", "пибл", 0,	0,
	"прв",  "пнр",  "пбр",  "пмн",  "пмр",  "пбл",  "пос",  "пно",
	"счт",  "сем",  "счх",  "счк",  "зпн",  "зпк",  "зпх",  "зпт",
	"где",  "пфс",  "пфа",  "пф",   "уиа",  "слиа", "груп", 0,
	"счц",  "счцм", "счнц", "счлм", "слц",  "вчц",  "вчобц","вчабц",
	"счл",  "ил",   "нтжл", "слкл", "изц",  "илил", 0,	"умнц",
	"сбрл", "рзбл", "чедл", "недл", "слф",  "вчф",  "вчобф","вчабф",
	0,	0,	0,	0,	0,	0,	0,	0,
	"зп+",  "зпм+", "счн+", "счм+", "сл+",  "вч+",  "вчоб+","вчаб+",
	"сч+",  "и+",   "нтж+", "слк+", "из+",  "или+", "дел+", "умн+",
	"сбр+", "рзб+", "чед+", "нед+", "слпп+","вчпп+","сдпп+",0,
	"счт+", "сем+", "счх+", "счк+", "зпн+", "зпк+", "сдлп+","зпт+",
};

static char *mnemLmadl [128] = {
	"atx",  "stx",  "xtra", "xts",  "aadx", "asux", "xsua", "amsx",
	"xta",  "aax",  "aex",  "arx",  "avx",  "aox",  "adx",  "amux",
	"apkx", "aux",  "acx",  "anx",  "eax",  "esx",  "asrx", "xtr",
	"uj",   "vjm",  "vgm",  "vlm",  0,	0,	"alx",  0,
	"vzm",  "vim",  "vpzm", "vnm",  "vnzm", "vpm",  0,	0,
	"uz",   "ui",   "upz",  "un",   "unz",  "up",   "uiv",  "uzv",
	"xtga", "xtqa", "xtha", "xtta", "ztx",  "atcx", "ath",  "atgx",
	"pctm", "atc",  "utcs", "wtc",  "vtm",  "utm",  "do",   0,
	"xtal", "xtsl", "utra", "uts",  "aadu", "asuu", "usua", "amu",
	"uta",  "aau",  "aeu",  "aru",  "avu",  "aou",  0,	"amuu",
	"apu",  "auu",  "acu",  "anu",  0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	"atk",  "stk",  "ktra", "kts",  "aadk", "asuk", "ksua", "amk",
	"kta",  "aak",  "aek",  "ark",  "avk",  "aok",  "adk",  "amuk",
	"apk",  "auk",  "ack",  "ank",  "eak",  "esk",  "ask",  0,
	"ktga", "ktsa", "ktha", "ktta", "ztk",  "atck", "alk",  "atgk",
};

static char *mnemSrach [128] = {
	"эк",   "выт",  "врг",  "вых",  0,	0,	"ург",  "стоп",
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	"вдп",  0,	0,	"кор",  0,	"сдп",	0,
	"врж",  "вд",   "нтжп", "упр",  "слп",  "вчп",  "сдл",  "урж",
	"уи",   "уим",  "ви",   "виц",  "уии",  "сли",  "вчиоб","вчи",
	"ур",   "урм",  "вр",   0,	"ури",  0,	0,	0,
	0,	0,	0,	0,	"цела", "целф", "сдпд", 0,
	"уисч", 0,	0,	0,	"инв",  0,	"сдлд", "лог",
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	"вдпм", 0,	0,	"корм", 0,	"сдпм", 0,
	"вржм", "вдм",  "нтжпм",0,	"слпм", "вчпм", "сдлм", "уржм",
	0,	0,	"вим",  "вицм", 0,	0,	0,	0,
	0,	0,	"врм",  0,	0,	0,	0,	0,
	0,	0,	0,	0,	"целам","целфм","сдпдм",0,
	"уисчм",0,	0,	0,	"инвм", 0,	"сдлдм","логм",
};

static char *mnemSmadl [128] = {
	"ex",   "pop",  "rmod", "ij",   0,	0,	"wmod", "halt",
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	"yma",  0,	0,	"ecn",  0,	"asn",  0,
	"rta",  "yta",  "een",  "set",  "ean",  "esn",  "aln",  "ntr",
	"ati",  "sti",  "ita",  "iita", "mtj",  "jam",  "msj",  "jsm",
	"ato",  "sto",  "ota",  0,	"mto",  0,	0,	0,
	0,	0,	0,	0,	"ent",  "int",  "asy",  0,
	"atia", 0,	0,	0,	"aca",  0,	"aly",  "tst",
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	"yms",  0,	0,	"ecns", 0,	"asns", 0,
	"rts",  "yts",  "eens", 0,	"eans", "esns", "alns", "ntrs",
	0,	0,	"its",  "iits", 0,	0,	0,	0,
	0,	0,	"ots",  0,	0,	0,	0,	0,
	0,	0,	0,	0,	"ents", "ints", "asys", 0,
	"atis", 0,	0,	0,	"acs",  0,	"alys", "tsts",
};

static char *mnemStyap [128];
static char *mnemLtyap [128];

extern (*funchome) ();
extern (*funcend) ();
extern (*funcpgup) ();
extern (*funcpgdn) ();
extern (*funcup) ();
extern (*funcdown) ();
extern (*funcleft) ();
extern (*funcright) ();

extern halting;
extern curgroup, curwin, oldwin;

extern Jprintf ();
extern char *getstring ();

static long addr1, addr2;
static long *addr = &addr1;

static memreg;

static up ()
{
	long l, r;

	*addr = (*addr - 1) & ADDRMASK;
	ptload (&l, &r, *addr);
	Jscrool (4, 22, 1);
	Jmove (4, 5);
	prmem (l, r, *addr);
}

static down ()
{
	long l, r;

	*addr = (*addr + 1) & ADDRMASK;
	ptload (&l, &r, *addr + 18);
	Jscrool (4, 22, -1);
	Jmove (22, 5);
	prmem (l, r, *addr + 18);
}

static pgup ()
{
	*addr = (*addr - 16) & ADDRMASK;
	mem0 ();
}

static pgdn ()
{
	*addr = (*addr + 16) & ADDRMASK;
	mem0 ();
}

mem0 ()
{
	register i;
	register long a;
	long l, r;

	funcpgup = pgup;
	funcpgdn = pgdn;
	funcup = up;
	funcdown = down;
	Jmprintf (2, 4, "Память %c", addr==&addr1 ? '1' : '2');
	if ((memreg & (MCMD|MBIN)) == MBIN) {
		Jmove (1, 18);
		Jprintf ("6         5         4");
		Jprintf ("         3         2         1         ");
		Jmove (2, 14);
		Jprintf ("4321098765432109876543210");
		Jprintf ("987654321098765432109876543210987654321");
	}
	for (i=0, a= *addr; i<19; ++i, ++a) {
		Jmove (i+4, 5);
		ptload (&l, &r, a);
		prmem (l, r, a & ADDRMASK);
	}
}

mem1 ()
{
	register char *p;
	char a [40+1];

	addr = &addr1;
	sprintf (a, "%lx", *addr);
	p = getstring (6, a, " Адрес ", "Введите шестнадцатеричный адрес");
	if (p)
		sscanf (p, "%lx", addr);
	curwin = 0;
	prmenu ();
	mem0 ();
	oldwin = 0;
}

mem2 ()
{
	register char *p;
	char a [40+1];

	addr = &addr2;
	sprintf (a, "%lx", *addr);
	p = getstring (6, a, " Адрес ", "Введите шестнадцатеричный адрес");
	if (p)
		sscanf (p, "%lx", addr);
	curwin = 0;
	prmenu ();
	mem0 ();
	oldwin = 0;
}

mem3 ()
{
	addr = addr==&addr1 ? &addr2 : &addr1;
	curwin = 0;
	prmenu ();
	mem0 ();
	oldwin = 0;
}

mem5 ()
{
	memreg ^= MCMD;
	curwin = 0;
	prmenu ();
	mem0 ();
	oldwin = 0;
}

mem6 ()
{
	register r;

	switch (memreg & (MBIN)) {
	default:	r = MBIN;	break;
	case MBIN:	r = 0;		break;
	}
	memreg = (memreg & ~(MBIN)) | r;
	curwin = 0;
	prmenu ();
	mem0 ();
	oldwin = 0;
}

mem7 ()
{
	register r;

	switch (memreg & (MMADL|MRACH|MTYAP)) {
	default:	r = MMADL;	break;
	case MMADL:	r = MRACH;	break;
	case MRACH:	r = MTYAP;	break;
	case MTYAP:	r = 0;		break;
	}
	memreg = (memreg & ~(MMADL|MRACH|MTYAP)) | r;
	curwin = 0;
	prmenu ();
	mem0 ();
	oldwin = 0;
}

mem8 ()
{
	memreg ^= MREV;
	curwin = 0;
	prmenu ();
	mem0 ();
	oldwin = 0;
}

mem4 () { mem0 (); }

static prmem (l, r, a)
long l, r, a;
{
	Jprintf ("%06lx:  ", a);
	if (memreg & MCMD) {
		prcmd (l);
		Jprintf ("   ");
		prcmd (r);
		Jprintf ("    ");
	} else if (memreg & MBIN) {
		prbin (l);
		prbin (r);
		return;
	} else {
		prhalf (l);
		Jprintf ("  ");
		prhalf (r);
		Jprintf ("    ");
	}
	if (memreg & MREV) {
		prchar ((int) (l>>24));
		prchar ((int) (l>>16));
		prchar ((int) l>>8);
		prchar ((int) l);
		prchar ((int) (r>>24));
		prchar ((int) (r>>16));
		prchar ((int) r>>8);
		prchar ((int) r);
	} else {
		prchar ((int) r);
		prchar ((int) r>>8);
		prchar ((int) (r>>16));
		prchar ((int) (r>>24));
		prchar ((int) l);
		prchar ((int) l>>8);
		prchar ((int) (l>>16));
		prchar ((int) (l>>24));
	}
}

static prchar (c)
register c;
{
	c &= 0xff;
	if (c == 0177) {
		Jputch ('^');
		Jputch ('?');
	} else if (c == 0377) {
		Jputch ('~');
		Jputch ('?');
	} else if (c < ' ') {
		Jputch ('^');
		Jputch (c & 037 | '@');
	} else {
		Jputch (' ');
		Jputch (koitoecod (c));
	}
}

prmemhalf (h)
long h;
{
	if (memreg & MCMD)
		prcmd (h);
	else if (memreg & MBIN)
		prbin (h);
	else
		prhalf (h);
}

static prcmd (h)
register long h;
{
	register r, c, s;
	char creg [4], cmnem [8], caddr [16], *mnem;
	int shortcmd;

	r = h >> 28 & 0xf;
	c = h >> 20 & 0xff;
	shortcmd = (c == 0x3f);
	if (shortcmd) {
		s = h >> 12 & 0xff;
		h &= 0xfff;
	} else {
		s = c;
		h &= 0xfffff;
	}
	mnem = 0;
	switch (memreg & (MMADL|MRACH|MTYAP)) {
	default:
		Jputch ("0123456789abcdef" [r]);
		Jprintf (shortcmd ? " 3f %02x %03x" : " %02x  %05x", s, h);
		return;
	case MMADL:
		if (s < 128)
			mnem = (shortcmd ? mnemSmadl : mnemLmadl) [s];
		break;
	case MRACH:
		if (s < 128)
			mnem = (shortcmd ? mnemSrach : mnemLrach) [s];
		break;
	case MTYAP:
		if (s < 128)
			mnem = (shortcmd ? mnemStyap : mnemLtyap) [s];
		break;
	}
	creg [0] = 0;
	caddr [0] = 0;
	if (r)
		creg [0] = "0123456789abcdef" [r], creg [1] = 0;
	if (! mnem) {
		if (s < 0xa0)
			sprintf (cmnem, shortcmd ? "3f %02x" : "%02x", s);
		else
			sprintf (cmnem, shortcmd ? "3f 0%02x" : "0%02x", s);
		mnem = cmnem;
	}
	if (h)
		sprintf (caddr, "%lx", h);
	Jprintf ("%2s %-5s %-5s", creg, mnem, caddr);
}

static prbin (h)
register long h;
{
	register i;

	/* Jprintf ("21098765432109876543210987654321"); */
	for (i=31; i>=0; --i)
		Jputch (h >> i & 1 ? '1' : '0');
}
