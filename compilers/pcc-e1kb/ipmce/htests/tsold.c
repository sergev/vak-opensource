
# include <stdio.h>
# include <ctype.h>

# ifdef CROSS
#    include "../h/a.out.h"
# else
#    include <a.out.h>
# endif

# define W 8            /* длина слова в байтах */

struct tsthdr {                /* заголовок */
	int magic;              /* 1 byte */
	long addr;              /* 4 bytes */
	long len;               /* 4 bytes */
} hdr;

FILE *text;

char *lcmd [] = {
	"atx",  "stx",  "xtra", "xts",  "a+x",  "a-x",  "x-a",  "amsx",
	"xta",  "aax",  "aex",  "arx",  "avx",  "aox",  "a/x",  "a*x",
	"apkx", "aux",  "acx",  "anx",  "e+x",  "e-x",  "asrx", "xtr",
	"uj",   "vjm",  "vgm",  "vlm",  "34",   "35",   "alx",  "37",
	"vzm",  "vim",  "vpzm", "vnm",  "vnzm", "vpm",  "46",   "47",
	"uz",   "ui",   "upz",  "un",   "unz",  "up",   "uiv",  "uzv",
	"xtga", "xtsa", "xtha", "xtta", "ztx",  "atcx", "ath",  "atgx",
	"pctm", "atc",  "utcs", "wtc",  "vtm",  "utm",  "do",   "77",

	"100",  "101",  "utra", "uts",  "a+u",  "a-u",  "u-a",  "amu",
	"uta",  "aau",  "aeu",  "aru",  "avu",  "aou",  "116",  "a*u",
	"apu",  "auu",  "acu",  "anu",  "124",  "125",  "126",  "127",
	"130",  "131",  "132",  "133",  "134",  "135",  "136",  "137",
	"atk",  "stk",  "ktra", "kts",  "a+k",  "a-k",  "k-a",  "amk",
	"kta",  "aak",  "aek",  "ark",  "avk",  "aok",  "a/k",  "a*k",
	"apk",  "auk",  "ack",  "ank",  "e+k",  "e-k",  "ask",  "167",
	"ktga", "ktsa", "ktha", "ktta", "ztk",  "atck", "alk",  "atgk",

	"200",  "201",  "202",  "203",  "204",  "205",  "206",  "207",
	"210",  "211",  "212",  "213",  "214",  "215",  "216",  "217",
	"220",  "221",  "222",  "223",  "224",  "225",  "226",  "227",
	"230",  "231",  "232",  "233",  "234",  "235",  "236",  "237",
	"240",  "241",  "242",  "243",  "244",  "245",  "246",  "247",
	"250",  "251",  "252",  "253",  "254",  "255",  "256",  "257",
	"260",  "261",  "262",  "263",  "264",  "265",  "266",  "267",
	"270",  "271",  "272",  "273",  "274",  "275",  "276",  "277",

	"300",  "301",  "302",  "303",  "304",  "305",  "306",  "307",
	"310",  "311",  "312",  "313",  "314",  "315",  "316",  "317",
	"320",  "321",  "322",  "323",  "324",  "325",  "326",  "327",
	"330",  "331",  "332",  "333",  "334",  "335",  "336",  "337",
	"340",  "341",  "342",  "343",  "344",  "345",  "346",  "347",
	"350",  "351",  "352",  "353",  "354",  "355",  "356",  "357",
	"360",  "361",  "362",  "363",  "364",  "365",  "366",  "367",
	"370",  "371",  "372",  "373",  "374",  "375",  "376",  "377",
};

char *scmd [] = {
	"ex",   "pop",  "rmod", "ij",   "04",   "05",   "wmod", "halt",
	"010",  "011",  "012",  "013",  "014",  "015",  "016",  "017",
	"020",  "yma",  "022",  "023",  "ecn",  "025",  "asn",  "027",
	"rta",  "yta",  "een",  "set",  "e+n",  "e-n",  "aln",  "ntr",
	"ati",  "sti",  "ita",  "iita", "mtj",  "j+m",  "j-m",  "m-j",
	"ato",  "sto",  "ota",  "053",  "mto",  "055",  "056",  "057",
	"060",  "061",  "062",  "063",  "ent",  "int",  "asy",  "067",
	"atia", "071",  "072",  "073",  "aca",  "075",  "aly",  "tst",

	"0100", "0101", "0102", "0103", "0104", "0105", "0106", "0107",
	"0110", "0111", "0112", "0113", "0114", "0115", "0116", "0117",
	"0120", "yms",  "0122", "0123", "ecns", "025",  "asns", "027",
	"rts",  "yts",  "eens", "0133", "e+ns", "e-ns", "alns", "ntrs",
	"0140", "0141", "its",  "iits", "0144", "0145", "0146", "0147",
	"0150", "0151", "ots",  "0153", "0154", "0155", "0156", "0157",
	"0160", "0161", "0162", "0163", "ents", "ints", "asys", "0167",
	"atis", "0171", "0172", "0173", "acs",  "0175", "alys", "tsts",

	"0200", "0201", "0202", "0203", "0204", "0205", "0206", "0207",
	"0210", "0211", "0212", "0213", "0214", "0215", "0216", "0217",
	"0220", "0221", "0222", "0223", "0224", "0225", "0226", "0227",
	"0230", "0231", "0232", "0233", "0234", "0235", "0236", "0237",
	"0240", "0241", "0242", "0243", "0244", "0245", "0246", "0247",
	"0250", "0251", "0252", "0253", "0254", "0255", "0256", "0257",
	"0260", "0261", "0262", "0263", "0264", "0265", "0266", "0267",
	"0270", "0271", "0272", "0273", "0274", "0275", "0276", "0277",

	"0300", "0301", "0302", "0303", "0304", "0305", "0306", "0307",
	"0310", "0311", "0312", "0313", "0314", "0315", "0316", "0317",
	"0320", "0321", "0322", "0323", "0324", "0325", "0326", "0327",
	"0330", "0331", "0332", "0333", "0334", "0335", "0336", "0337",
	"0340", "0341", "0342", "0343", "0344", "0345", "0346", "0347",
	"0350", "0351", "0352", "0353", "0354", "0355", "0356", "0357",
	"0360", "0361", "0362", "0363", "0364", "0365", "0366", "0367",
	"0370", "0371", "0372", "0373", "0374", "0375", "0376", "0377",
};

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
	printf ("Magic %xh Address %lxh Length %lxh End %lxh\n\n",
		hdr.magic, hdr.addr, hdr.len,  hdr.addr+hdr.len);
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
		printf ("*%lxh:\t%08lx%08lx", addr++, l, r);
		prcmd (l);
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
		printf ("  %2d ", r);
	else
		printf ("     ");
	if ((c & 0xff00000L) == 0x3f00000L) {
		printf ("%-5s", scmd [c>>12 & 0377]);
		a = c & 07777;
		if (a & 04000)
			a -= 010000;
	} else {
		printf ("%-5s", lcmd [c>>20 & 0377]);
		a = c & 0xfffff;
		if (a & 0x80000)
			a -= 0x100000;
	}
	if (! a)
		printf ("        ");
	else if (a < 16 && a > -16)
		printf ("%-3d     ", a);
	else if (a < 0)
		printf ("-%-7s", prhex (-a));
	else
		printf ("%-7s ", prhex (a));
}
