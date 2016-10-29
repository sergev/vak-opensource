/*
 * Unix-style assembler for PIC17C4x processors.
 * The mnemonics are taken from the old Soviet mainframe BESM-6.
 *
 * Copyright (C) 1997-2002 Serge Vakulenko <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#define STSIZE          2000    /* symbol table size */
#define TXTSIZE         8196    /* command segment size */
#define DATSIZE         256     /* data segment size */
#define DATSTART        0x20    /* data segment size */
#define OUTSIZE         20      /* hex output line size */
#define MAXCONS         4096    /* max constants */
#define MAXREL          10240   /* max relocated references */
#define MAXLIBS         10      /* max libraries */
#define MAXLABELS       1000    /* max relative (digit) labels */

/*
 * Lexical items.
 */
#define LEOF    0       /* end of file */
#define LEOL    1       /* end of line */
#define LNUM    2       /* number */
#define LNAME   3       /* name */
#define LCMD    4       /* machine command */
#define LEQU    5       /* .equ */
#define LDATA   6       /* .data */
#define LCONST  7       /* .const */
#define LORG    8       /* .org */
#define LCONFIG 9       /* .config */
#define LLSHIFT 10      /* << */
#define LRSHIFT 11      /* >> */

/*
 * Symbol/expression types.
 */
#define TUNDF   0       /* unknown symbol */
#define TABS    1       /* constant */
#define TDATA   2       /* variable or array */
#define TTEXT   3       /* label or function */
#define TCONST  4       /* far readonly data in text segment */

/*
 * Command types.
 */
#define FNARG   1       /* no argument */
#define FLONG   2       /* long argument */
#define FBIT    3       /* additional bit argument */
#define FREG    4       /* additional register argument */
#define FLREG   5       /* additional register argument to left */
#define FSL4    6       /* shift argument left by 4 */
#define FCOND   0x40    /* conditional command */
#define FNOTR   0x80    /* next command not reached */

/*
 * Relocation flags.
 */
#define RLONG   1       /* 13-bit address */
#define RHIGH   2       /* upper 8 bits of address */
#define RLAB    4       /* relative label */

/*
 * Processor option flags.
 */
#define C_EXTMEM        1
#define C_EXT           2
#define C_PROT          3
#define C_CTLR          4
#define O_RC            1
#define O_CLK           2
#define O_CRYST         3
#define O_LOW           4

/*
 * Processor configuration.
 */
#define CFGADDR         0xfe00

#define CFG_PMC_MODE    0x00AF
#define CFG_XMC_MODE    0xFFBF
#define CFG_MC_MODE     0xFFEF
#define CFG_MP_MODE     0xFFFF
#define CFG_WDT_OFF     0xFFF3
#define CFG_WDT_64      0xFFF7
#define CFG_WDT_256     0xFFFB
#define CFG_WDT_1       0xFFFF
#define CFG_LF_OSC      0xFFFC
#define CFG_RC_OSC      0xFFFD
#define CFG_XT_OSC      0xFFFE
#define CFG_EC_OSC      0xFFFF

struct stab {
	char *name;
	int len;
	int type;
	int value;
} stab [STSIZE];

struct labeltab {
	int num;
	int value;
} labeltab [MAXLABELS];

struct constab {
	char *val;
	int len;
	int sym;
} constab [MAXCONS];

struct reltab {
	int addr;
	int sym;
	int flags;
} reltab [MAXREL];

struct libtab {
	char *name;
} libtab [MAXLIBS];

char *infile, *infile1, *outfile;
int debug;
int line;
int filenum;
int count;
int dcount = DATSTART;
int reached;
int optim;
int opt_atx_xta, opt_not_reached;
int lastcmd;
int blexflag, backlex, blextype;
char name [256];
int intval;
int extref;
int extflag;
int stabfree;
int config = C_CTLR;
int wdog = 0;
int osc = O_CLK;
int nconst;
int nrel;
int nlib;
int nlabels;
int outaddr;
unsigned char outbuf [OUTSIZE], *outptr = outbuf;

unsigned char data [2*DATSIZE];
unsigned short text [2*TXTSIZE];
unsigned char tbusy [2*TXTSIZE];

#define ISHEX(c)        (ctype[(c)&0377] & 1)
#define ISOCTAL(c)      (ctype[(c)&0377] & 2)
#define ISDIGIT(c)      (ctype[(c)&0377] & 4)
#define ISLETTER(c)     (ctype[(c)&0377] & 8)

short ctype [256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,7,7,7,7,7,7,7,7,5,5,0,0,0,0,0,0,
	0,9,9,9,9,9,9,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,0,0,0,8,
	0,9,9,9,9,9,9,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,0,0,0,0,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
};

void parse (void);
void relocate (void);
void libraries (void);
void listing (void);
void output (void);
void makecmd (int code, int type);
void makeconst (int sym);
void makecfg (void);
int getexpr (int *s);

struct table {
	unsigned val;
	char *name;
	short type;
} table [] = {
/*
 * PIC 17c4x instruction set.
 *
- Opcode - Mnemonic - Flags --- Description -------------------- Old mnemonic */
#define ATX 0x0100
#define XTA 0x6a00
{ 0x0000,  "nop",     FNARG }, /* no operation                   nop */
{ 0x0002,  "ret",     FNARG | FNOTR }, /* return                 return */
{ 0x0003,  "sleep",   FNARG }, /* enter sleep mode               sleep */
{ 0x0004,  "awake",   FNARG }, /* clear watchdog timer           clrwdt */
{ 0x0005,  "reti",    FNARG | FNOTR }, /* return; enable interrupts retfie */
{ 0x0006,  "dump",    FNARG }, /* pseudo-command: dump register contents */
{ 0x0007,  "halt",    FNARG }, /* pseudo-command: halt simulation */
{ ATX,     "atx",     0 },     /* f = w                          movwf f */
{ 0x0200,  "a-bx",    0 },     /* w = f - (w + ~carry)           subwfb f,w */
{ 0x0300,  "x-ba",    0 },     /* f -= w + ~carry                subwfb f */
{ 0x0400,  "a-x",     0 },     /* w = f - w                      subwf f,w */
{ 0x0500,  "x-a",     0 },     /* f -= w                         subwf f */
{ 0x0600,  "x--a",    0 },     /* w = f-1                        decf f,w */
{ 0x0700,  "x--",     0 },     /* f = f-1                        decf f */
{ 0x0800,  "a|x",     0 },     /* w |= f                         iorwf f,w */
{ 0x0900,  "x|a",     0 },     /* f |= w                         iorwf f */
{ 0x0a00,  "a&x",     0 },     /* w &= f                         andwf f,w */
{ 0x0b00,  "x&a",     0 },     /* f &= w                         andwf f */
{ 0x0c00,  "a^x",     0 },     /* w ^= f                         xorwf f,w */
{ 0x0d00,  "x^a",     0 },     /* f ^= w                         xorwf f */
{ 0x0e00,  "a+x",     0 },     /* w += f                         addwf f,w */
{ 0x0f00,  "x+a",     0 },     /* f += w                         addwf f */
{ 0x1000,  "a+cx",    0 },     /* w += f + carry                 addwfc f,w */
{ 0x1100,  "x+ca",    0 },     /* f += w + carry                 addwfc f */
{ 0x1200,  "xca",     0 },     /* w = ~f                         comf f,w */
{ 0x1300,  "xc",      0 },     /* f = ~f                         comf f */
{ 0x130a,  "ac",      FNARG }, /* w = ~w                         comf w */
{ 0x1400,  "x++a",    0 },     /* w = f+1                        incf f,w */
{ 0x1500,  "x++",     0 },     /* f = f+1                        incf f */
{ 0x150a,  "a++",     FNARG }, /* w = w+1                        incf wreg */
{ 0x1600,  "x--a?",   FCOND }, /* w = f-1; if (w != 0)           decfsz f,w */
{ 0x1700,  "x--?",    FCOND }, /* f = f-1; if (f != 0)           decfsz f */
{ 0x170a,  "a--?",    FNARG | FCOND }, /* w = w-1; if (w != 0)  decfsz wreg */
{ 0x1800,  "xc>>a",   0 },     /* w = carry:f >> 1               rrcf f,w */
{ 0x1900,  "xc>>x",   0 },     /* f = carry:f >> 1               rrcf f */
{ 0x1a00,  "xc<<a",   0 },     /* w = carry:f << 1               rlcf f,w */
{ 0x1b00,  "xc<<x",   0 },     /* f = carry:f << 1               rlcf f */
{ 0x1c00,  "xwa",     0 },     /* w = swap(f)                    swapf f,w */
{ 0x1d00,  "xw",      0 },     /* f = swap(f)                    swapf f */
{ 0x1d0a,  "aw",      FNARG }, /* w = swap(w)                    swapf wreg */
{ 0x1e00,  "x++a?",   FCOND }, /* w = f+1; if (w != 0)           incfsz f,w */
{ 0x1f00,  "x++?",    FCOND }, /* f = f+1; if (f != 0)           incfsz f */
{ 0x1f0a,  "a++?",    FNARG | FCOND }, /* w = w+1; if (w != 0)   incfsz wreg */
{ 0x2000,  "x>>a",    0 },     /* w = f >> 1                     rrncf f,w */
{ 0x2100,  "x>>x",    0 },     /* f = f >> 1                     rrncf f */
{ 0x2200,  "x<<a",    0 },     /* w = f << 1                     rlncf f,w */
{ 0x2300,  "x<<x",    0 },     /* f = f << 1                     rlncf f */
{ 0x2400,  "x++az?",  FCOND }, /* w = f+1; if (w == 0)           infsnz f,w */
{ 0x2500,  "x++z?",   FCOND }, /* f = f+1; if (f == 0)           infsnz f */
{ 0x250a,  "a++z?",   FNARG | FCOND }, /* w = w+1; if (w == 0)   infsnz wreg */
{ 0x2600,  "x--az?",  FCOND }, /* w = f-1; if (w == 0)           dcfsnz f,w */
{ 0x2700,  "x--z?",   FCOND }, /* f = f-1; if (f == 0)           dcfsnz f */
{ 0x270a,  "a--z?",   FNARG | FCOND }, /* w = w-1; if (w == 0)   dcfsnz wreg */
{ 0x2800,  "xza",     0 },     /* f = w = 0                      clrf f,w */
{ 0x2900,  "xz",      0 },     /* f = 0                          clrf f */
{ 0x290a,  "az",      FNARG }, /* w = 0                          clrf wreg */
{ 0x2a00,  "xsa",     0 },     /* f = w = 0xff                   setf f,w */
{ 0x2b00,  "xs",      0 },     /* f = 0xff                       setf f */
{ 0x2b0a,  "as",      FNARG }, /* w = 0xff                       setf wreg */
{ 0x2c00,  "anax",    0 },     /* f = w = ~w+1                   negw f,w */
{ 0x2d00,  "anx",     0 },     /* f = ~w+1                       negw f */
{ 0x2e00,  "adax",    0 },     /* f = w = adjust(w)              daw f,w */
{ 0x2f00,  "adx",     0 },     /* f = adjust(w)                  daw f */
{ 0x3000,  "x>=a?",   FCOND }, /* if (x >= a)                    cpfslt f */
{ 0x3100,  "x!=a?",   FCOND }, /* if (x != a)                    cpfseq f */
{ 0x3200,  "x<=a?",   FCOND }, /* if (x <= a)                    cpfsgt f */
{ 0x3300,  "x?",      FCOND }, /* if (f != 0)                    tstfsz f */
{ 0x330a,  "a?",      FNARG | FCOND }, /* if (w != 0)            tstfsz wreg */
{ 0x3400,  "a*x",     0 },     /* prod = w*f                     mulwf f */
{ 0x3800,  "bt",      FBIT },  /* f.b ^= 1                       btg f,b */
{ 0x4000,  "rtx",     FLREG }, /* f = p                          movpf p,f */
{ 0x6000,  "xtr",     FREG },  /* p = f                          movfp f,p */
{ XTA,     "xta",     0 },     /* w = f                          movfp f,wreg */
{ 0x8000,  "bs",      FBIT },  /* f.b = 1                        bsf f,b */
{ 0x8800,  "bz",      FBIT },  /* f.b = 0                        bcf f,b */
{ 0x9000,  "bz?",     FBIT | FCOND },  /* if (f.b == 0)          btfss f,b */
{ 0x9800,  "bs?",     FBIT | FCOND },  /* if (f.b != 0)          btfsc f,b */
{ 0x9a04,  "z?",      FNARG | FCOND }, /* if (ALUSTA.Z)          btfsc ALUSTA,Z */
{ 0x9204,  "nz?",     FNARG | FCOND }, /* if (! ALUSTA.Z)        btfss ALUSTA,Z */
{ 0x9804,  "c?",      FNARG | FCOND }, /* if (ALUSTA.C)          btfsc ALUSTA,C */
{ 0x9804,  "nb?",     FNARG | FCOND }, /* if (ALUSTA.C)          btfsc ALUSTA,C */
{ 0x9004,  "b?",      FNARG | FCOND }, /* if (! ALUSTA.C)        btfss ALUSTA,C */
{ 0x9004,  "nc?",     FNARG | FCOND }, /* if (! ALUSTA.C)        btfss ALUSTA,C */
{ 0xa000,  "llx",     0 },     /* f = low(latch)                 tlrd 0,f */
{ 0xa200,  "lhx",     0 },     /* f = high(latch)                tlrd 1,f */
{ 0xa400,  "xll",     0 },     /* low(latch)  = f                tlwt 0,f */
{ 0xa600,  "xhl",     0 },     /* high(latch) = f                tlwt 1,f */
{ 0xa800,  "plx",     0 },     /* f = low(latch); latch = *tptr  tablrd 0,0,f */
{ 0xa900,  "pl++x",   0 },     /* f = low(latch); latch = *tptr++ tablrd 0,1,f */
{ 0xae00,  "xhp",     0 },     /* high(latch) = f; *tptr = latch tablwt 1,0,f */
{ 0xaf00,  "xhp++",   0 },     /* high(latch) = f; *tptr++ = latch tablwt 1,1,f */
{ 0xb000,  "cta",     0 },     /* w = c                          movlw k */
{ 0xb100,  "a+c",     0 },     /* w += c                         addlw k */
{ 0xb200,  "c-a",     0 },     /* w = c - w                      sublw k */
{ 0xb300,  "a|c",     0 },     /* w |= c                         iorlw k */
{ 0xb400,  "a^c",     0 },     /* w ^= c                         xorlw k */
{ 0xb500,  "a&c",     0 },     /* w &= c                         andlw k */
{ 0xb600,  "retc",    FNOTR }, /* w = c; return                  retlw k */
{ 0xb700,  "lcall",   0 },     /* lcall c()                      lcall k */
{ 0xb800,  "reg",     0 },     /* low(bsr) = c                   movlb k */
{ 0xba00,  "dat",     FSL4 },  /* high(bsr) = c                  movlr k */
{ 0xbc00,  "a*c",     0 },     /* prod = w * c                   mullw k */
{ 0xc000,  "goto",    FLONG | FNOTR }, /* goto                   goto */
{ 0xe000,  "call",    FLONG }, /* call                           call */
{ 0 }};

void uerror (char *s, ...)
{
	va_list ap;

	va_start (ap, s);
	fprintf (stderr, "as: ");
	if (infile)
		fprintf (stderr, "%s, ", infile);
	fprintf (stderr, "%d: ", line);
	vfprintf (stderr, s, ap);
	va_end (ap);
	fprintf (stderr, "\n");
	if (outfile)
		unlink (outfile);
	exit (1);
}

/*
 * Look up the symbol.
 */
int lookname ()
{
	int i, len;
	struct stab *s;

	len = strlen (name);
	s = 0;
	if (name[0] == 'L')
		name[0] = '.';
	for (i=0; i<stabfree; ++i) {
		if (! stab[i].len && ! s) {
			s = stab+i;
			continue;
		}
		if (name[0] == '.' && stab[i].len == len+1 &&
		    stab[i].name[0] == 'A'+filenum &&
		    ! strcmp (stab[i].name+1, name))
			return i;
		if (stab[i].len == len && ! strcmp (stab[i].name, name))
			return i;
	}
	if (! s)
		s = stab + stabfree++;

	/* Add the new symbol. */
	if (s >= stab + STSIZE)
		uerror ("symbol table overflow");
	s->name = malloc (2 + len);
	if (! s->name)
		uerror ("out of memory");
	s->len = len;
	if (name[0] == '.') {
		s->name[0] = 'A' + filenum;
		strcpy (s->name+1, name);
		++s->len;
	} else
		strcpy (s->name, name);
	s->value = 0;
	s->type = 0;
	return s - stab;
}

int main (int argc, char **argv)
{
	int i;
	char *cp;

	for (i=1; i<argc; i++)
		switch (argv[i][0]) {
		case '-':
			for (cp=argv[i]; *cp; cp++) switch (*cp) {
			case 'd':
				debug++;
				break;
			case 'O':
				optim++;
				break;
			case 'o':
				if (cp [1]) {
					/* -ofile */
					outfile = cp+1;
					while (*++cp);
					--cp;
				} else if (i+1 < argc)
					/* -o file */
					outfile = argv[++i];
				break;
			case 'l':
				if (nlib >= MAXLIBS)
					uerror ("too many libraries");
				if (cp [1]) {
					/* -lname */
					libtab[nlib++].name = cp+1;
					while (*++cp);
					--cp;
				} else if (i+1 < argc)
					/* -l name */
					libtab[nlib++].name = argv[++i];
				break;
			}
			break;
		default:
			infile = argv[i];
			if (! infile1)
				infile1 = infile;
			if (! freopen (infile, "r", stdin))
				uerror ("cannot open");
			line = 1;
			parse ();
			infile = 0;
			++filenum;
			break;
		}

	if (! outfile) {
		if (! infile1) {
			printf ("PIC 17c4x Assembler, by Serge V.Vakulenko\n");
			printf ("Copyright (C) 1997 Cronyx Engineering Ltd.\n\n");
			printf ("Usage:\n\tas17 [-O] [-o outfile.hex] infile.s ...\n\n");
			return -1;
		}
		outfile = malloc (4 + strlen (infile1));
		if (! outfile)
			uerror ("out of memory");
		strcpy (outfile, infile1);
		cp = strrchr (outfile, '.');
		if (! cp)
			cp = outfile + strlen (outfile);
		strcpy (cp, ".hex");
	}

	if (! freopen (outfile, "w", stdout))
		uerror ("cannot open %s", outfile);

	if (! nlib)
		libtab[nlib++].name = "/usr/local/lib/pic17";
	libraries ();
	relocate ();
	listing ();
	output ();
	return 0;
}

int lookcmd ()
{
	int i;

	for (i=0; table[i].name; ++i)
		if (! strcmp (table[i].name, name))
			return i;
	return -1;
}

int hexdig (int c)
{
	if (c <= '9')      return c - '0';
	else if (c <= 'F') return c - 'A' + 10;
	else               return c - 'a' + 10;
}

/*
 * Read the integer value.
 * 1234   - decimal
 * 01234  - octal
 * 0x1234 - hexadecimal
 * 0b1101 - binary
 */
void getnum (int c)
{
	intval = 0;
	if (c == '0') {
		c = getchar ();
		if (c == 'x' || c == 'X') {
			while (ISHEX (c = getchar()))
				intval = intval*16 + hexdig (c);
			if (c >= 0)
				ungetc (c, stdin);
			return;
		}
		if (c == 'b' || c == 'B') {
			while ((c = getchar()) == '0' || c == '1')
				intval = intval*2 + c - '0';
			if (c >= 0)
				ungetc (c, stdin);
			return;
		}
		if (c >= 0)
			ungetc (c, stdin);
		while (ISOCTAL (c = getchar()))
			intval = intval*8 + hexdig (c);
		if (c >= 0)
			ungetc (c, stdin);
		return;
	}
	if (c >= 0)
		ungetc (c, stdin);
	while (ISDIGIT (c = getchar()))
		intval = intval*10 + hexdig (c);
	if (c >= 0)
		ungetc (c, stdin);
}

void getname (int c, int extname)
{
	char *cp;

	for (cp=name; c>' ' && c!=':'; c=getchar()) {
		if (! extname && ! ISLETTER (c) && ! ISDIGIT (c))
			break;
		*cp++ = c;
	}
	*cp = 0;
	ungetc (c, stdin);
}

/*
 * Read the next lexical item from the input stream.
 */
int getlex (int *pval, int extname)
{
	int c;

	if (blexflag) {
		blexflag = 0;
		*pval = blextype;
		return backlex;
	}
	for (;;) switch (c = getchar()) {
	case ';':
	case '#':
skiptoeol:      while ((c = getchar()) != '\n')
			if (c == EOF)
				return LEOF;
	case '\n':
		++line;
		c = getchar ();
		if (c == '#')
			goto skiptoeol;
		ungetc (c, stdin);
		*pval = line;
		return LEOL;
	case ' ':
	case '\t':
		continue;
	case EOF:
		return LEOF;
	case '<':
		if ((c = getchar()) == '<')
			return LLSHIFT;
		ungetc (c, stdin);
		return '<';
	case '>':
		if ((c = getchar()) == '>')
			return LRSHIFT;
		ungetc (c, stdin);
		return '>';
	case '\'':
		c = getchar ();
		if (c == '\'')
			uerror ("bad char constant");
		if (c == '\\')
			switch (c = getchar()) {
			case 'a':  c = 0x07; break;
			case 'b':  c = '\b'; break;
			case 'f':  c = '\f'; break;
			case 'n':  c = '\n'; break;
			case 'r':  c = '\r'; break;
			case 't':  c = '\t'; break;
			case 'v':  c = '\v'; break;
			case '\'': break;
			case '\\': break;
			default: uerror ("bad char constant");
			}
		if (getchar() != '\'')
			uerror ("bad char constant");
		intval = c;
		return LNUM;
	case '*':       case '/':       case '%':       case '\\':
	case '^':       case '&':       case '|':       case '~':
	case '"':       case ',':       case '[':       case ']':
	case '(':       case ')':       case '{':       case '}':
	case '=':       case ':':       case '+':       case '-':
	case '@':
		return c;
	case '0':       case '1':       case '2':       case '3':
	case '4':       case '5':       case '6':       case '7':
	case '8':       case '9':
		getnum (c);
		return LNUM;
	default:
		if (! ISLETTER (c))
			uerror ("bad character: \\%o", c & 0377);
		getname (c, extname);
		if (name[0] == '.') {
			if (! name[1]) return '.';
			if (! strcmp (name, ".equ"))    return LEQU;
			if (! strcmp (name, ".data"))   return LDATA;
			if (! strcmp (name, ".org"))    return LORG;
			if (! strcmp (name, ".const"))  return LCONST;
			if (! strcmp (name, ".config")) return LCONFIG;
		}
		if ((*pval = lookcmd()) != -1)
			return LCMD;
		*pval = lookname ();
		return LNAME;
	}
}

#if 0
int getlex (int *pval, int extname)
{
	int clex;

	clex = getlex1 (pval, extname);
	switch (clex) {
	case LEOF:    fprintf (stderr, "EOF\n"); break;
	case LEOL:    fprintf (stderr, "EOL\n"); break;
	case LNUM:    fprintf (stderr, "NUM %d\n", intval); break;
	case LNAME:   fprintf (stderr, "NAME %s\n", stab[*pval].name); break;
	case LCMD:    fprintf (stderr, "CMD %s\n", table[*pval].name); break;
	case LEQU:    fprintf (stderr, ".EQU\n"); break;
	case LDATA:   fprintf (stderr, ".DATA\n"); break;
	case LCONST:  fprintf (stderr, ".CONST\n"); break;
	case LORG:    fprintf (stderr, ".ORG\n"); break;
	case LCONFIG: fprintf (stderr, ".CONFIG\n"); break;
	default:      fprintf (stderr, "LEX `%c'\n", clex); break;
	}
	return clex;
}
#endif

void ungetlex (int val, int type)
{
	blexflag = 1;
	backlex = val;
	blextype = type;
}

/*
 * Get the expression term.
 */
int getterm ()
{
	int cval, s;

	switch (getlex (&cval, 0)) {
	default:
		uerror ("operand missing");
	case LNUM:
		cval = getchar ();
		if (cval == 'f' || cval == 'F' || cval == 'b' || cval == 'B') {
			if (cval == 'b' || cval == 'B')
				extref = -intval;
			else
				extref = intval;
			extflag |= RLAB;
			intval = 0;
			return TUNDF;
		}
		ungetc (cval, stdin);
		return TABS;
	case '@':
		extflag |= RHIGH;
		if (getlex (&cval, 0) != LNAME)
			uerror ("bad @ syntax");
	case LNAME:
		if (stab[cval].type == TUNDF || stab[cval].type == TCONST) {
			intval = 0;
			extref = cval;
		} else
			intval = stab[cval].value;
		return stab[cval].type;
	case '.':
		intval = count;
		return TTEXT;
	case '(':
		getexpr (&s);
		if (getlex (&cval, 0) != ')')
			uerror ("bad () syntax");
		return s;
	}
}

/*
 * Read the expression.
 * Put the type of expression into *s.
 *
 * expr = [term] {op term}...
 * term = LNAME | LNUM | "." | "(" expr ")"
 * op   = "+" | "-" | "&" | "|" | "^" | "~" | "<<" | ">>" | "/" | "*" | "%"
 */
int getexpr (int *s)
{
	int clex, cval, s2, rez;

	/* Get the first item. */
	switch (clex = getlex (&cval, 0)) {
	default:
		ungetlex (clex, cval);
		rez = 0;
		*s = TABS;
		break;
	case LNUM:
	case LNAME:
	case '.':
	case '(':
	case '@':
		ungetlex (clex, cval);
		*s = getterm ();
		rez = intval;
		break;
	}
	for (;;) {
		switch (clex = getlex (&cval, 0)) {
		case '+':
			s2 = getterm ();
			if (*s == TABS) *s = s2;
			else if (s2 != TABS)
				uerror ("too complex expression");
			rez += intval;
			break;
		case '-':
			s2 = getterm ();
			if (s2 != TABS)
				uerror ("too complex expression");
			rez -= intval;
			break;
		case '&':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez &= intval;
			break;
		case '|':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez |= intval;
			break;
		case '^':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez ^= intval;
			break;
		case '~':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez ^= ~intval;
			break;
		case LLSHIFT:
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez <<= intval;
			break;
		case LRSHIFT:
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez >>= intval;
			break;
		case '*':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez *= intval;
			break;
		case '/':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			if (! intval)
				uerror ("division by zero");
			rez /= intval;
			break;
		case '%':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			if (! intval)
				uerror ("division (%%) by zero");
			rez %= intval;
			break;
		default:
			ungetlex (clex, cval);
			intval = rez;
			return rez;
		}
	}
}

void parse ()
{
	int clex, cval, tval;

	for (;;) {
		clex = getlex (&cval, 1);
		switch (clex) {
		case LEOF:
			return;
		case LEOL:
			continue;
		case LCMD:
			makecmd (table[cval].val, table[cval].type);
			break;
		case LNAME:
			clex = getlex (&tval, 0);
			switch (clex) {
			case ':':               /* name: */
				if (stab[cval].type != TUNDF) {
					uerror ("name redefined");
					break;
				}
				stab[cval].value = count;
				stab[cval].type = TTEXT;
				if (! reached)
					reached = 1;
				lastcmd = 0;
				continue;
			case LEQU:              /* name .equ val */
				getexpr (&tval);
				if (tval == TUNDF)
					uerror ("bad value .equ");
				if (stab[cval].type != TUNDF) {
					if (stab[cval].type != tval ||
					    stab[cval].value != intval)
						uerror ("name redefined");
					break;
				}
				stab[cval].type = tval;
				stab[cval].value = intval;
				break;
			case LDATA:             /* name .data size */
				getexpr (&tval);
				if (tval != TABS || intval < 0)
					uerror ("bad length .data");
				if (stab[cval].type != TUNDF) {
					if (stab[cval].type != TDATA)
						uerror ("name already defined");
					break;
				}
				stab[cval].type = TDATA;
				stab[cval].value = dcount;
				dcount += intval;
				break;
			case LCONST:            /* name .const val,val... */
				if (stab[cval].type != TUNDF)
					uerror ("name already defined");
				stab[cval].type = TCONST;
				stab[cval].value = 0;
				makeconst (cval);
				break;
			default:
				uerror ("bad command");
			}
			break;
		case LNUM:
			if (nlabels >= MAXLABELS)
				uerror ("too many digital labels");
			labeltab[nlabels].num = intval;
			labeltab[nlabels].value = count;
			++nlabels;
			clex = getlex (&tval, 0);
			if (clex != ':')
				uerror ("bad digital label");
			if (! reached)
				reached = 1;
			lastcmd = 0;
			continue;
		case LORG:
			getexpr (&tval);
			if (tval != TABS)
				uerror ("bad value .org");
			count = intval;
			break;
		case LCONFIG:
			makecfg ();
			break;
		default:
			uerror ("syntax error");
		}
		clex = getlex (&cval, 0);
		if (clex != LEOL) {
			if (clex == LEOF)
				return;
			uerror ("bad command argument");
		}
	}
}

/*
 * Flush the output buffer.
 */
void outflush (void)
{
	unsigned char *p, sum = 0;

	/* set byte count */
	outbuf[0] = outptr - outbuf - 4;

	putchar (':');
	for (p=outbuf; p<outptr; ++p) {
		printf ("%02X", *p);
		sum += *p;
	}
	printf ("%02X\n", (unsigned char) -sum);
	outptr = outbuf;
}

/*
 * Put the word to the output buffer.
 */
void outhex (long addr, int val)
{
	if (outptr >= outbuf + OUTSIZE ||
	    (outptr > outbuf && addr != outaddr+1))
		outflush ();
	if (outptr == outbuf) {
		*outptr++ = 0;
		*outptr++ = addr >> 7;
		*outptr++ = addr << 1;
		*outptr++ = 0;
	}
	*outptr++ = val;
	*outptr++ = val >> 8;
	outaddr = addr;
}

/*
 * Write the resulting hex image.
 */
void output ()
{
	int i, option;

	printf (":020000040000FA\n");
	for (i=0; i<TXTSIZE; ++i)
		if (tbusy [i])
			outhex (i, text[i]);
	if (outptr > outbuf)
		outflush ();

	option = 0xffff;
	switch (config) {
	case C_PROT:   option &= CFG_PMC_MODE; break;
	case C_EXT:    option &= CFG_XMC_MODE; break;
	case C_CTLR:   option &= CFG_MC_MODE;  break;
	case C_EXTMEM: option &= CFG_MP_MODE;  break;
	}
	switch (wdog) {
	case 0:   option &= CFG_WDT_OFF; break;
	case 64:  option &= CFG_WDT_64;  break;
	case 256: option &= CFG_WDT_256; break;
	case 1:   option &= CFG_WDT_1;   break;
	}
	switch (osc) {
	case O_LOW:   option &= CFG_LF_OSC; break;
	case O_RC:    option &= CFG_RC_OSC; break;
	case O_CRYST: option &= CFG_XT_OSC; break;
	case O_CLK:   option &= CFG_EC_OSC; break;
	}
	printf (":020000040001F9\n");
	outhex (CFGADDR, option);

	outflush ();
	printf (":00000001FF\n");
}

/*
 * Compile the constant.
 */
void makeconst (int sym)
{
	char buf [1024], *p = buf;
	int len, tval, clex, cval;

	if (nconst >= MAXCONS)
		uerror ("too many constants");
	for (;;) {
		getexpr (&tval);
		if (tval != TABS)
			uerror ("bad value .const");

		if (p >= buf + sizeof (buf))
			uerror ("too long .const");
		*p++ = intval;

		clex = getlex (&cval, 0);
		if (clex != ',') {
			ungetlex (clex, cval);
			break;
		}
	}
	len = p - buf;

	constab[nconst].val = malloc (len + 1);
	if (! constab[nconst].val)
		uerror ("out of memory");
	memcpy (constab[nconst].val, buf, len);

	constab[nconst].len = len;
	constab[nconst].sym = sym;
	++nconst;
}

/*
 * Set the processor options.
 */
void makecfg ()
{
	int c, clex, cval;

	for (;;) {
		while ((c = getchar()) == ' ' || c == '\t')
			continue;
		if (! ISLETTER (c))
			uerror ("bad option .config");
		getname (c, 0);

		if      (! strcasecmp (name, "microprocessor")) config = C_EXTMEM;
		else if (! strcasecmp (name, "extended"))    config = C_EXT;
		else if (! strcasecmp (name, "protected"))   config = C_PROT;
		else if (! strcasecmp (name, "watchdog"))    wdog = 1;
		else if (! strcasecmp (name, "watchdog64"))  wdog = 64;
		else if (! strcasecmp (name, "watchdog256")) wdog = 256;
		else if (! strcasecmp (name, "nowatchdog"))  wdog = 0;
		else if (! strcasecmp (name, "rc"))          osc = O_RC;
		else if (! strcasecmp (name, "extclock"))    osc = O_CLK;
		else if (! strcasecmp (name, "crystal"))     osc = O_CRYST;
		else if (! strcasecmp (name, "lowcrystal"))  osc = O_LOW;
		else
			uerror ("bad option .config");

		clex = getlex (&cval, 0);
		if (clex != ',') {
			ungetlex (clex, cval);
			break;
		}
	}
}

/*
 * Set text segment value.
 */
void settext (int addr, int val)
{
	text [addr] = val;
	tbusy [addr] = 1;
	if (debug)
		fprintf (stderr, "code %3d: %04x\n", addr, val);
	lastcmd = val;
}

/*
 * Resolve pending references, adding
 * modules from libraries.
 */
void libraries ()
{
	struct stab *s;
	int n, undefined;
	char name [256];

	/* For every undefined reference,
	 * add the module from the library. */
	undefined = 0;
	for (s=stab; s<stab+stabfree; ++s) {
		if (s->type != TUNDF)
			continue;

		for (n=0; n<nlib; ++n) {
			sprintf (name, "%s/%s.lib", libtab[n].name, s->name);
			if (freopen (name, "r", stdin)) {
				infile = name;
				line = 1;
				parse ();
				infile = 0;
				++filenum;
				break;
			}
		}
		if (n >= nlib) {
			fprintf (stderr, "as: undefined: %s\n", s->name);
			++undefined;
		}
	}
	if (undefined > 0) {
		unlink (outfile);
		exit (1);
	}
}

/*
 * Find the relative label address,
 * by the reference address and the label number.
 * Backward references have negative label numbers.
 */
int findlabel (int addr, int sym)
{
	struct labeltab *p;

	if (sym < 0) {
		/* Backward reference. */
		for (p=labeltab+nlabels-1; p>=labeltab; --p)
			if (p->value <= addr && p->num == -sym)
				return p->value;
		uerror ("undefined label %db at address %d", -sym, addr);
	} else {
		/* Forward reference. */
		for (p=labeltab; p<labeltab+nlabels; ++p)
			if (p->value > addr && p->num == sym)
				return p->value;
		uerror ("undefined label %df at address %d", sym, addr);
	}
	return 0;
}

int compare_constab_len (const void *pa, const void *pb)
{
	const struct constab *a = pa, *b = pb;

	if (a->len > b->len)
		return -1;
	return (a->len < b->len);
}

/*
 * Allocate constants and relocate references.
 */
void relocate ()
{
	int n, v;
	struct constab *c, *p;
	struct reltab *r;
	int tsize, csize, dsize;

	tsize = csize = 0;
	dsize = dcount - DATSTART;

	for (n=0; n<TXTSIZE; ++n)
		if (tbusy [n])
			++tsize;

	/* Place the constants at the end of text segment. */
	qsort (constab, nconst, sizeof (constab[0]), compare_constab_len);
	for (c=constab; c<constab+nconst; ++c) {
		/* Try to find and reuse the constant. */
		for (p=constab; p<c; ++p)
			if (p->len >= c->len &&
			    memcmp (p->val + p->len - c->len, c->val, c->len) == 0) {
				stab[c->sym].value = stab[p->sym].value + p->len - c->len;
				stab[c->sym].type = TTEXT;
				break;
			}
		if (p < c)
			continue;

		stab[c->sym].value = count;
		stab[c->sym].type = TTEXT;
		for (n=0; n<c->len; ++n) {
			settext (count++, c->val[n]);
			++csize;
		}
	}

	/* Relocate pending references. */
	for (r=reltab; r<reltab+nrel; ++r) {
		if (r->flags & RLAB)
			v = findlabel (r->addr, r->sym);
		else
			v = stab[r->sym].value;

		if (r->flags & RHIGH)
			v >>= 8;

		if (r->flags & RLONG) {
			v += text [r->addr] & 0x1fff;
			text [r->addr] &= ~0x1fff;
			text [r->addr] |= v & 0x1fff;
		} else {
			v += text [r->addr] & 0xff;
			text [r->addr] &= ~0xff;
			text [r->addr] |= v & 0xff;
		}
	}
	if (opt_atx_xta || opt_not_reached)
		fprintf (stderr, "Optimization: atx-xta: %d words, not-reached: %d words\n",
			opt_atx_xta, opt_not_reached);
	fprintf (stderr, "Total text %d words, const %d words, data %d bytes\n",
		tsize, csize, dsize);
	if (count > TXTSIZE)
		uerror ("text segment overflow: %d words", count);
	if (dcount > DATSIZE)
		uerror ("data segment overflow: %d bytes", dsize);
	fprintf (stderr, "Free text %d words, data %d bytes\n",
		TXTSIZE - count, DATSIZE - dcount);
}

int compare_stab (const void *pa, const void *pb)
{
	const struct stab *a = pa, *b = pb;

	if (a->type == TDATA && b->type != TDATA)
		return -1;
	if (a->type != TDATA && b->type == TDATA)
		return 1;
	if (a->value < b->value)
		return -1;
	return (a->value > b->value);
}

int compare_constab (const void *pa, const void *pb)
{
	const struct constab *a = pa, *b = pb;

	if (a->sym < b->sym)
		return -1;
	return (a->sym > b->sym);
}

/*
 * Print the table of symbols and text constants.
 */
void listing ()
{
	struct stab *s;
	struct constab *c;
	char *p, *lstname;
	FILE *lstfile;
	int t, n;

	lstname = malloc (4 + strlen (outfile));
	if (! lstname)
		uerror ("out of memory");
	strcpy (lstname, outfile);
	p = strrchr (lstname, '.');
	if (! p)
		p = lstname + strlen (lstname);
	strcpy (p, ".lst");

	lstfile = fopen (lstname, "w");
	if (! lstfile)
		uerror ("cannot write to %s", lstname);

	/* Remember the addresses of constants. */
	for (c=constab; c<constab+nconst; ++c)
		c->sym = stab[c->sym].value;

	/* Sort the symbol table. */
	qsort (stab, stabfree, sizeof (stab[0]), compare_stab);

	fprintf (lstfile, "Data symbols:\n");
#if 0
	datamode = 1;
	for (s=stab; s<stab+stabfree; ++s) {
		if (s->name[1] == '.')
			continue;
		if (s->type != TDATA && datamode) {
			fprintf (lstfile, "\nCode symbols:\n");
			datamode = 0;
		}
		switch (s->type) {
		case TUNDF:  t = 'U'; break;
		case TABS:   t = 'A'; break;
		case TDATA:  t = 'D'; break;
		case TTEXT:  t = 'T'; break;
		case TCONST: t = 'C'; break;
		default:     t = '?'; break;
		}
		fprintf (lstfile, "\t%04x  %c  %.*s\n",
			s->value, t, s->len, s->name);
	}
#else
	for (s=stab; s<stab+stabfree; ++s) {
		if (s->name[1] == '.')
			continue;
		switch (s->type) {
		default:     continue;
		case TDATA:  t = 'D'; break;
		case TABS:   t = 'A'; break;
		}
		fprintf (lstfile, "\t%04x  %c  %.*s\n",
			s->value, t, s->len, s->name);
	}
	fprintf (lstfile, "\nCode symbols:\n");
	for (s=stab; s<stab+stabfree; ++s) {
		if (s->name[1] == '.')
			continue;
		switch (s->type) {
		default:     continue;
		case TUNDF:  t = 'U'; break;
		case TTEXT:  t = 'T'; break;
		case TCONST: t = 'C'; break;
		}
		fprintf (lstfile, "\t%04x  %c  %.*s\n",
			s->value, t, s->len, s->name);
	}
	t = 0;
	for (n=TXTSIZE-1; n>=0; --n)
		if (tbusy [n]) {
			t = n;
			break;
		}
	fprintf (lstfile, "\t%04x  T  <end>\n", t);
#endif
	/* Sort the table of constants. */
	qsort (constab, nconst, sizeof (constab[0]), compare_constab);
	fprintf (lstfile, "\nText constants:\n");
	for (c=constab; c<constab+nconst; ++c) {
		/* Skip repeated constants. */
		if (c > constab && c->sym < c[-1].sym + c[-1].len)
			continue;

		fprintf (lstfile, "\t%04x  C  \"", c->sym);
		for (p=c->val; p<c->val+c->len; ++p) {
			switch (*p) {
			case '\n': fprintf (lstfile, "\\n");  break;
			case '\r': fprintf (lstfile, "\\r");  break;
			case '"':  fprintf (lstfile, "\\\""); break;
			case '\\': fprintf (lstfile, "\\\\"); break;
			case '\0':
				if (p < c->val+c->len-1)
					fprintf (lstfile, "\\000");
				break;
			default:
				if (*p < ' ' || *p > '~')
					fprintf (lstfile, "\\%03o", *p);
				else
					fprintf (lstfile, "%c", *p);
			}
		}
		fprintf (lstfile, "\"\n");
	}
	fprintf (lstfile, "\nDone.\n");
}

void addreloc (int addr, int sym, int flags)
{
	if (nrel >= MAXREL)
		uerror ("too many relocations");
	reltab[nrel].addr = addr;
	reltab[nrel].sym = sym;
	reltab[nrel].flags = flags;
	++nrel;
	if (debug) {
		fprintf (stderr, "reloc %d", addr);
		if (sym)
			fprintf (stderr, " %d", sym);
		if (flags & RLAB)
			fprintf (stderr, " RLAB");
		if (flags & RHIGH)
			fprintf (stderr, " RHIGH");
		if (flags & RLONG)
			fprintf (stderr, " RLONG");
		fprintf (stderr, "\n");
	}
}

/*
 * Compile the command.
 */
void makecmd (int code, int type)
{
	int tval, tval2, cval;

	extflag = 0;
	if (! reached && (count == 0 || count == 8 || count == 0x10 ||
	    count == 0x18 || count == 0x20))
		reached = 1;
	switch (type & ~(FCOND | FNOTR)) {
	case FNARG:             /* no argument */
		if (! reached) {
			++opt_not_reached;
			break;
		}
		settext (count++, code);
		break;
	default:
		getexpr (&tval);
		if (! reached) {
			++opt_not_reached;
			break;
		}
		if (tval == TUNDF || tval == TCONST)
			addreloc (count, extref, extflag);
		intval &= 0xff;
		if (optim && code == ATX && lastcmd == (XTA | intval)) {
			/* fprintf (stderr, "atx-xta at 0x%x\n", count); */
			++opt_atx_xta;
			break;
		}
		settext (count++, code | intval);
		break;
	case FSL4:
		getexpr (&tval);
		if (tval != TABS)
			uerror ("bad data page number");
		if (! reached) {
			++opt_not_reached;
			break;
		}
		code |= intval << 4 & 0xf0;
		settext (count++, code);
		break;
	case FLONG:             /* long argument */
		getexpr (&tval);
		if (! reached) {
			++opt_not_reached;
			break;
		}
		if (tval == TUNDF || tval == TCONST)
			addreloc (count, extref, extflag | RLONG);
		code |= intval & 0x1fff;
		settext (count++, code);
		break;
	case FBIT:              /* additional bit argument */
		getexpr (&tval);
		code |= intval & 0xff;
		if (getlex (&cval, 0) != ',')
			uerror ("need bit number");
		getexpr (&tval2);
		if (tval2 != TABS)
			uerror ("bad bit number");
		if (! reached) {
			++opt_not_reached;
			break;
		}
		if (tval == TUNDF || tval == TCONST)
			addreloc (count, extref, extflag);
		code |= intval << 8 & 0x700;
		settext (count++, code);
		break;
	case FREG:              /* additional register argument */
		getexpr (&tval);
		code |= intval & 0xff;
		if (getlex (&cval, 0) != ',')
			uerror ("need register number");
		getexpr (&tval2);
		if (tval2 != TABS)
			uerror ("bad register number");
		if (! reached) {
			++opt_not_reached;
			break;
		}
		if (tval == TUNDF || tval == TCONST)
			addreloc (count, extref, extflag);
		code |= intval << 8 & 0x1f00;
		settext (count++, code);
		break;
	case FLREG:             /* additional register argument to left */
		getexpr (&tval);
		if (tval != TABS)
			uerror ("bad register number");
		code |= intval << 8 & 0x1f00;
		if (getlex (&cval, 0) != ',')
			uerror ("need register number");
		getexpr (&tval);
		if (! reached) {
			++opt_not_reached;
			break;
		}
		if (tval == TUNDF || tval == TCONST)
			addreloc (count, extref, extflag);
		code |= intval & 0xff;
		settext (count++, code);
		break;
	}
	if (optim && reached) {
		if (type & FNOTR)
			--reached;
		else {
			reached = 1;
			if (type & FCOND)
				reached = 2;
		}
	}
}
