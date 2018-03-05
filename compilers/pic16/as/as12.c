/*
 * Unix-style assembler for PIC12 processors.
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
#define TXTSIZE         256     /* command segment size */
#define DATSIZE         32      /* data segment size */
#define DATSTART        10      /* data segment size */
#define OUTSIZE         20      /* hex output line size */
#define MAXCONS         256     /* max constants */
#define MAXREL          10240   /* max relocated references */
#define MAXLIBS         10      /* max libraries */
#define MAXLABELS       1000    /* max relative (digit) labels */

/*
 * Lexical items.
 */
#define LEOF            0       /* end of file */
#define LEOL            1       /* end of line */
#define LNUM            2       /* number */
#define LNAME           3       /* name */
#define LCMD            4       /* machine command */
#define LEQU            5       /* .equ */
#define LDATA           6       /* .data */
#define LCONST          7       /* .const */
#define LORG            8       /* .org */
#define LCONFIG         9       /* .config */
#define LLSHIFT         10      /* << */
#define LRSHIFT         11      /* >> */

/*
 * Symbol/expression types.
 */
#define TUNDF           0       /* unknown symbol */
#define TABS            1       /* constant */
#define TDATA           2       /* variable or array */
#define TTEXT           3       /* label or function */
#define TCONST          4       /* readonly data in text segment */

/*
 * Command types.
 */
#define FNARG           1       /* no argument */
#define FLONG           2       /* 9-bit argument */
#define FBYTE           3       /* 8-bit argument */
#define FBIT            4       /* additional bit argument */
#define FCOND           0x40    /* conditional command */
#define FNOTR           0x80    /* next command not reached */

/*
 * Relocation flags.
 */
#define RLONG           1       /* 9-bit address */
#define RLAB            2       /* relative label */

/*
 * Processor option flags.
 */
#define O_LOW           1   /* LP */
#define O_CRYST         2   /* XT */
#define O_IRC           3
#define O_XRC           4

/*
 * Processor configuration.
 */
#define CFGADDR         0xfff

#define CFG_LP_OSC      0x00    /* low power crystal */
#define CFG_XT_OSC      0x01    /* crystal/resonator */
#define CFG_IRC_OSC     0x02    /* internal 4 MHz RC oscillator */
#define CFG_XRC_OSC     0x03    /* external resistor/capacitor */
#define CFG_WDTE        0x04    /* watchdog enable */
#define CFG_UNPROTECT   0x08    /* code protection disable */
#define CFG_MCLRE       0x10    /* /MCLR pin enable */

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
int unprotect = 0;
int wdog = 0;
int mclre = 0;
int osc = O_IRC;
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

void parse(void);
void relocate(void);
void libraries(void);
void listing(void);
void output(void);
void makecmd(int code, int type);
void makeconst(int sym);
void makecfg(void);
int getexpr(int *s);

struct table {
    unsigned val;
    char *name;
    short type;
} table [] = {
/*
 * PIC 16Cxx instruction set.
 *
- Opcode - Mnemonic - Flags --- Description -------------------- Old mnemonic */
#define ATX 0x020
#define XTA 0x200
{ 0x000,  "nop",     FNARG },           /* no operation */
{ 0x002,  "option",  FNARG },           /* option := w */
{ 0x003,  "sleep",   FNARG },           /* enter sleep mode */
{ 0x004,  "awake",   FNARG },           /* clear watchdog timer */
{ 0x006,  "tris",    FNARG },           /* tris := w */
{ 0x007,  "dump",    FNARG },           /* pseudo-command: dump registers */
{ 0x008,  "halt",    FNARG },           /* pseudo-command: halt simulation */
{ ATX,    "atx",     0 },               /* f = w */
{ 0x040,  "az",      FNARG },           /* w = 0 */
{ 0x060,  "xz",      0 },               /* f = 0 */
{ 0x080,  "a-x",     0 },               /* w = f - w */
{ 0x0a0,  "x-a",     0 },               /* f -= w */
{ 0x0c0,  "x--a",    0 },               /* w = f-1 */
{ 0x0e0,  "x--",     0 },               /* f = f-1 */
{ 0x100,  "a|x",     0 },               /* w |= f */
{ 0x120,  "x|a",     0 },               /* f |= w */
{ 0x140,  "a&x",     0 },               /* w &= f */
{ 0x160,  "x&a",     0 },               /* f &= w */
{ 0x180,  "a^x",     0 },               /* w ^= f */
{ 0x1a0,  "x^a",     0 },               /* f ^= w */
{ 0x1c0,  "a+x",     0 },               /* w += f */
{ 0x1e0,  "x+a",     0 },               /* f += w */
{ XTA,    "xta",     0 },               /* w = f */
{ 0x220,  "tst",     0 },               /* f = f, set Z */
{ 0x240,  "xca",     0 },               /* w = ~f */
{ 0x260,  "xc",      0 },               /* f = ~f */
{ 0x280,  "x++a",    0 },               /* w = f+1 */
{ 0x2a0,  "x++",     0 },               /* f = f+1 */
{ 0x2c0,  "x--a?",   FCOND },           /* w = f-1; if (w != 0) */
{ 0x2e0,  "x--?",    FCOND },           /* f = f-1; if (f != 0) */
{ 0x300,  "xc>>a",   0 },               /* w = carry:f >> 1 */
{ 0x320,  "xc>>x",   0 },               /* f = carry:f >> 1 */
{ 0x340,  "xc<<a",   0 },               /* w = carry:f << 1 */
{ 0x360,  "xc<<x",   0 },               /* f = carry:f << 1 */
{ 0x380,  "xwa",     0 },               /* w = swap(f) */
{ 0x3a0,  "xw",      0 },               /* f = swap(f) */
{ 0x3c0,  "x++a?",   FCOND },           /* w = f+1; if (w != 0) */
{ 0x3e0,  "x++?",    FCOND },           /* f = f+1; if (f != 0) */
{ 0x400,  "bz",      FBIT },            /* f.b = 0 */
{ 0x500,  "bs",      FBIT },            /* f.b = 1 */
{ 0x600,  "bs?",     FBIT | FCOND },    /* if (f.b != 0) */
{ 0x700,  "bz?",     FBIT | FCOND },    /* if (f.b == 0) */
{ 0x800,  "ret",     FBYTE | FNOTR },   /* w = c; return */
{ 0x900,  "call",    FBYTE },           /* call c() */
{ 0xa00,  "goto",    FLONG | FNOTR },   /* goto c() */
{ 0xc00,  "cta",     FBYTE },           /* w = c */
{ 0xd00,  "a|c",     FBYTE },           /* w |= c */
{ 0xe00,  "a&c",     FBYTE },           /* w &= c */
{ 0xf00,  "a^c",     FBYTE },           /* w ^= c */
{ 0x643,  "z?",      FNARG | FCOND },   /* if (Z) */
{ 0x743,  "nz?",     FNARG | FCOND },   /* if (! Z) */
{ 0x603,  "c?",      FNARG | FCOND },   /* if (C) */
{ 0x703,  "nc?",     FNARG | FCOND },   /* if (! C) */
{ 0x603,  "nb?",     FNARG | FCOND },   /* if (! borrow) */
{ 0x703,  "b?",      FNARG | FCOND },   /* if (borrow) */
{ 0 }};

void uerror(char *s, ...)
{
    va_list ap;

    va_start(ap, s);
    fprintf(stderr, "as: ");
    if (infile)
        fprintf(stderr, "%s, ", infile);
    fprintf(stderr, "%d: ", line);
    vfprintf(stderr, s, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    if (outfile)
        unlink(outfile);
    exit(1);
}

/*
 * Look up the symbol.
 */
int lookname()
{
    int i, len;
    struct stab *s;

    len = strlen(name);
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
            ! strcmp(stab[i].name+1, name))
            return i;
        if (stab[i].len == len && ! strcmp(stab[i].name, name))
            return i;
    }
    if (! s)
        s = stab + stabfree++;

    /* Add the new symbol. */
    if (s >= stab + STSIZE)
        uerror("symbol table overflow");
    s->name = malloc(2 + len);
    if (! s->name)
        uerror("out of memory");
    s->len = len;
    if (name[0] == '.') {
        s->name[0] = 'A' + filenum;
        strcpy(s->name+1, name);
        ++s->len;
    } else
        strcpy(s->name, name);
    s->value = 0;
    s->type = 0;
    return s - stab;
}

int main(int argc, char **argv)
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
                    uerror("too many libraries");
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
            if (! freopen(infile, "r", stdin))
                uerror("cannot open");
            line = 1;
            parse();
            infile = 0;
            ++filenum;
            break;
        }

    if (! outfile) {
        if (! infile1) {
            printf("PIC12 Assembler, by Serge V.Vakulenko\n\n");
            printf("Usage:\n\tas12 [-O] [-o outfile.hex] infile.s ...\n\n");
            return -1;
        }
        outfile = malloc(4 + strlen(infile1));
        if (! outfile)
            uerror("out of memory");
        strcpy(outfile, infile1);
        cp = strrchr(outfile, '.');
        if (! cp)
            cp = outfile + strlen(outfile);
        strcpy(cp, ".hex");
    }

    if (! freopen(outfile, "w", stdout))
        uerror("cannot open %s", outfile);

    if (! nlib)
        libtab[nlib++].name = "/usr/local/lib/pic17";
    libraries();
    relocate();
    listing();
    output();
    return 0;
}

int lookcmd()
{
    int i;

    for (i=0; table[i].name; ++i)
        if (! strcmp(table[i].name, name))
            return i;
    return -1;
}

int hexdig(int c)
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
void getnum(int c)
{
    intval = 0;
    if (c == '0') {
        c = getchar();
        if (c == 'x' || c == 'X') {
            while (ISHEX(c = getchar()))
                intval = intval*16 + hexdig(c);
            if (c >= 0)
                ungetc(c, stdin);
            return;
        }
        if (c == 'b' || c == 'B') {
            while ((c = getchar()) == '0' || c == '1')
                intval = intval*2 + c - '0';
            if (c >= 0)
                ungetc(c, stdin);
            return;
        }
        if (c >= 0)
            ungetc(c, stdin);
        while (ISOCTAL(c = getchar()))
            intval = intval*8 + hexdig(c);
        if (c >= 0)
            ungetc(c, stdin);
        return;
    }
    if (c >= 0)
        ungetc(c, stdin);
    while (ISDIGIT(c = getchar()))
        intval = intval*10 + hexdig(c);
    if (c >= 0)
        ungetc(c, stdin);
}

void getname(int c, int extname)
{
    char *cp;

    for (cp=name; c>' ' && c!=':'; c=getchar()) {
        if (! extname && ! ISLETTER(c) && ! ISDIGIT(c))
            break;
        *cp++ = c;
    }
    *cp = 0;
    ungetc(c, stdin);
}

/*
 * Read the next lexical item from the input stream.
 */
int getlex(int *pval, int extname)
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
skiptoeol:
        while ((c = getchar()) != '\n')
            if (c == EOF)
                return LEOF;
    case '\n':
        ++line;
        c = getchar();
        if (c == '#')
            goto skiptoeol;
        ungetc(c, stdin);
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
        ungetc(c, stdin);
        return '<';
    case '>':
        if ((c = getchar()) == '>')
            return LRSHIFT;
        ungetc(c, stdin);
        return '>';
    case '\'':
        c = getchar();
        if (c == '\'')
            uerror("bad char constant");
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
            default: uerror("bad char constant");
            }
        if (getchar() != '\'')
            uerror("bad char constant");
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
        getnum(c);
        return LNUM;
    default:
        if (! ISLETTER(c))
            uerror("bad character: \\%o", c & 0377);
        getname(c, extname);
        if (name[0] == '.') {
            if (! name[1]) return '.';
            if (! strcmp(name, ".equ"))    return LEQU;
            if (! strcmp(name, ".data"))   return LDATA;
            if (! strcmp(name, ".org"))    return LORG;
            if (! strcmp(name, ".const"))  return LCONST;
            if (! strcmp(name, ".config")) return LCONFIG;
        }
        if ((*pval = lookcmd()) != -1)
            return LCMD;
        *pval = lookname();
        return LNAME;
    }
}

void ungetlex(int val, int type)
{
    blexflag = 1;
    backlex = val;
    blextype = type;
}

/*
 * Get the expression term.
 */
int getterm()
{
    int cval, s;

    switch (getlex(&cval, 0)) {
    default:
        uerror("operand missing");
    case LNUM:
        cval = getchar();
        if (cval == 'f' || cval == 'F' || cval == 'b' || cval == 'B') {
            if (cval == 'b' || cval == 'B')
                extref = -intval;
            else
                extref = intval;
            extflag |= RLAB;
            intval = 0;
            return TUNDF;
        }
        ungetc(cval, stdin);
        return TABS;
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
        getexpr(&s);
        if (getlex(&cval, 0) != ')')
            uerror("bad () syntax");
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
int getexpr(int *s)
{
    int clex, cval, s2, rez;

    /* Get the first item. */
    switch (clex = getlex(&cval, 0)) {
    default:
        ungetlex(clex, cval);
        rez = 0;
        *s = TABS;
        break;
    case LNUM:
    case LNAME:
    case '.':
    case '(':
    case '@':
        ungetlex(clex, cval);
        *s = getterm();
        rez = intval;
        break;
    }
    for (;;) {
        switch (clex = getlex(&cval, 0)) {
        case '+':
            s2 = getterm();
            if (*s == TABS) *s = s2;
            else if (s2 != TABS)
                uerror("too complex expression");
            rez += intval;
            break;
        case '-':
            s2 = getterm();
            if (s2 != TABS)
                uerror("too complex expression");
            rez -= intval;
            break;
        case '&':
            s2 = getterm();
            if (*s != TABS || s2 != TABS)
                uerror("too complex expression");
            rez &= intval;
            break;
        case '|':
            s2 = getterm();
            if (*s != TABS || s2 != TABS)
                uerror("too complex expression");
            rez |= intval;
            break;
        case '^':
            s2 = getterm();
            if (*s != TABS || s2 != TABS)
                uerror("too complex expression");
            rez ^= intval;
            break;
        case '~':
            s2 = getterm();
            if (*s != TABS || s2 != TABS)
                uerror("too complex expression");
            rez ^= ~intval;
            break;
        case LLSHIFT:
            s2 = getterm();
            if (*s != TABS || s2 != TABS)
                uerror("too complex expression");
            rez <<= intval;
            break;
        case LRSHIFT:
            s2 = getterm();
            if (*s != TABS || s2 != TABS)
                uerror("too complex expression");
            rez >>= intval;
            break;
        case '*':
            s2 = getterm();
            if (*s != TABS || s2 != TABS)
                uerror("too complex expression");
            rez *= intval;
            break;
        case '/':
            s2 = getterm();
            if (*s != TABS || s2 != TABS)
                uerror("too complex expression");
            if (! intval)
                uerror("division by zero");
            rez /= intval;
            break;
        case '%':
            s2 = getterm();
            if (*s != TABS || s2 != TABS)
                uerror("too complex expression");
            if (! intval)
                uerror("division (%%) by zero");
            rez %= intval;
            break;
        default:
            ungetlex(clex, cval);
            intval = rez;
            return rez;
        }
    }
}

void parse()
{
    int clex, cval, tval;

    for (;;) {
        clex = getlex(&cval, 1);
        switch (clex) {
        case LEOF:
            return;
        case LEOL:
            continue;
        case LCMD:
            makecmd(table[cval].val, table[cval].type);
            break;
        case LNAME:
            clex = getlex(&tval, 0);
            switch (clex) {
            case ':':               /* name: */
                if (stab[cval].type != TUNDF) {
                    uerror("name redefined");
                    break;
                }
                stab[cval].value = count;
                stab[cval].type = TTEXT;
                if (! reached)
                    reached = 1;
                lastcmd = 0;
                continue;
            case LEQU:              /* name .equ val */
                getexpr(&tval);
                if (tval == TUNDF)
                    uerror("bad value .equ");
                if (stab[cval].type != TUNDF) {
                    if (stab[cval].type != tval ||
                        stab[cval].value != intval)
                        uerror("name redefined");
                    break;
                }
                stab[cval].type = tval;
                stab[cval].value = intval;
                break;
            case LDATA:             /* name .data size */
                getexpr(&tval);
                if (tval != TABS || intval < 0)
                    uerror("bad length .data");
                if (stab[cval].type != TUNDF) {
                    if (stab[cval].type != TDATA)
                        uerror("name already defined");
                    break;
                }
                stab[cval].type = TDATA;
                stab[cval].value = dcount;
                dcount += intval;
                break;
            case LCONST:            /* name .const val,val... */
                if (stab[cval].type != TUNDF)
                    uerror("name already defined");
                stab[cval].type = TCONST;
                stab[cval].value = 0;
                makeconst(cval);
                break;
            default:
                uerror("bad command");
            }
            break;
        case LNUM:
            if (nlabels >= MAXLABELS)
                uerror("too many digital labels");
            labeltab[nlabels].num = intval;
            labeltab[nlabels].value = count;
            ++nlabels;
            clex = getlex(&tval, 0);
            if (clex != ':')
                uerror("bad digital label");
            if (! reached)
                reached = 1;
            lastcmd = 0;
            continue;
        case LORG:
            getexpr(&tval);
            if (tval != TABS)
                uerror("bad value .org");
            count = intval;
            break;
        case LCONFIG:
            makecfg();
            break;
        default:
            uerror("syntax error");
        }
        clex = getlex(&cval, 0);
        if (clex != LEOL) {
            if (clex == LEOF)
                return;
            uerror("bad command argument");
        }
    }
}

/*
 * Flush the output buffer.
 */
void outflush(void)
{
    unsigned char *p, sum = 0;

    /* set byte count */
    outbuf[0] = outptr - outbuf - 4;

    putchar(':');
    for (p=outbuf; p<outptr; ++p) {
        printf("%02X", *p);
        sum += *p;
    }
    printf("%02X\n", (unsigned char) -sum);
    outptr = outbuf;
}

/*
 * Put the word to the output buffer.
 */
void outhex(long addr, int val)
{
    if (outptr >= outbuf + OUTSIZE ||
        (outptr > outbuf && addr != outaddr+1))
        outflush();
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
void output()
{
    int i, option;

    for (i=0; i<TXTSIZE; ++i)
        if (tbusy [i])
            outhex(i, text[i]);
    if (outptr > outbuf)
        outflush();

    option = 0xfe0;
    if (unprotect)
        option |= CFG_UNPROTECT;
    if (wdog)
        option |= CFG_WDTE;
    if (mclre)
        option |= CFG_MCLRE;
    switch (osc) {
    case O_LOW:   option |= CFG_LP_OSC;  break;
    case O_CRYST: option |= CFG_XT_OSC;  break;
    case O_IRC:   option |= CFG_IRC_OSC; break;
    case O_XRC:   option |= CFG_XRC_OSC; break;
    }
    outhex(CFGADDR, option);

    outflush();
    printf(":00000001FF\n");
}

/*
 * Compile the constant.
 */
void makeconst(int sym)
{
    char buf [1024], *p = buf;
    int len, tval, clex, cval;

    if (nconst >= MAXCONS)
        uerror("too many constants");
    for (;;) {
        getexpr(&tval);
        if (tval != TABS)
            uerror("bad value .const");

        if (p >= buf + sizeof(buf))
            uerror("too long .const");
        *p++ = intval;

        clex = getlex(&cval, 0);
        if (clex != ',') {
            ungetlex(clex, cval);
            break;
        }
    }
    len = p - buf;

    constab[nconst].val = malloc(len + 1);
    if (! constab[nconst].val)
        uerror("out of memory");
    memcpy(constab[nconst].val, buf, len);

    constab[nconst].len = len;
    constab[nconst].sym = sym;
    ++nconst;
}

/*
 * Set the processor options.
 */
void makecfg()
{
    int c, clex, cval;

    for (;;) {
        while ((c = getchar()) == ' ' || c == '\t')
            continue;
        if (! ISLETTER(c))
            uerror("bad option .config");
        getname(c, 0);

        if      (! strcasecmp(name, "unprotected")) unprotect = 1;
        else if (! strcasecmp(name, "watchdog"))    wdog = 1;
        else if (! strcasecmp(name, "mclr"))        mclre = 1;
        else if (! strcasecmp(name, "rc"))          osc = O_XRC;
        else if (! strcasecmp(name, "intclock"))    osc = O_IRC;
        else if (! strcasecmp(name, "crystal"))     osc = O_CRYST;
        else if (! strcasecmp(name, "lowcrystal"))  osc = O_LOW;
        else
            uerror("bad option .config");

        clex = getlex(&cval, 0);
        if (clex != ',') {
            ungetlex(clex, cval);
            break;
        }
    }
}

/*
 * Set text segment value.
 */
void settext(int addr, int val)
{
    text [addr] = val;
    tbusy [addr] = 1;
    if (debug)
        fprintf(stderr, "code %3d: %04x\n", addr, val);
    lastcmd = val;
}

/*
 * Resolve pending references, adding
 * modules from libraries.
 */
void libraries()
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
            sprintf(name, "%s/%s.lib", libtab[n].name, s->name);
            if (freopen(name, "r", stdin)) {
                infile = name;
                line = 1;
                parse();
                infile = 0;
                ++filenum;
                break;
            }
        }
        if (n >= nlib) {
            fprintf(stderr, "as: undefined: %s\n", s->name);
            ++undefined;
        }
    }
    if (undefined > 0) {
        unlink(outfile);
        exit(1);
    }
}

/*
 * Find the relative label address,
 * by the reference address and the label number.
 * Backward references have negative label numbers.
 */
int findlabel(int addr, int sym)
{
    struct labeltab *p;

    if (sym < 0) {
        /* Backward reference. */
        for (p=labeltab+nlabels-1; p>=labeltab; --p)
            if (p->value <= addr && p->num == -sym)
                return p->value;
        uerror("undefined label %db at address %d", -sym, addr);
    } else {
        /* Forward reference. */
        for (p=labeltab; p<labeltab+nlabels; ++p)
            if (p->value > addr && p->num == sym)
                return p->value;
        uerror("undefined label %df at address %d", sym, addr);
    }
    return 0;
}

int compare_constab_len(const void *pa, const void *pb)
{
    const struct constab *a = pa, *b = pb;

    if (a->len > b->len)
        return -1;
    return (a->len < b->len);
}

/*
 * Allocate constants and relocate references.
 */
void relocate()
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
    qsort(constab, nconst, sizeof(constab[0]), compare_constab_len);
    for (c=constab; c<constab+nconst; ++c) {
        /* Try to find and reuse the constant. */
        for (p=constab; p<c; ++p)
            if (p->len >= c->len &&
                memcmp(p->val + p->len - c->len, c->val, c->len) == 0) {
                stab[c->sym].value = stab[p->sym].value + p->len - c->len;
                stab[c->sym].type = TTEXT;
                break;
            }
        if (p < c)
            continue;

        stab[c->sym].value = count;
        stab[c->sym].type = TTEXT;
        for (n=0; n<c->len; ++n) {
            settext(count++, c->val[n]);
            ++csize;
        }
    }

    /* Relocate pending references. */
    for (r=reltab; r<reltab+nrel; ++r) {
        if (r->flags & RLAB)
            v = findlabel(r->addr, r->sym);
        else
            v = stab[r->sym].value;

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
        fprintf(stderr, "Optimization: atx-xta: %d words, not-reached: %d words\n",
            opt_atx_xta, opt_not_reached);
    fprintf(stderr, "Total text %d words, const %d words, data %d bytes\n",
        tsize, csize, dsize);
    if (count > TXTSIZE)
        uerror("text segment overflow: %d words", count);
    if (dcount > DATSIZE)
        uerror("data segment overflow: %d bytes", dsize);
    fprintf(stderr, "Free text %d words, data %d bytes\n",
        TXTSIZE - count, DATSIZE - dcount);
}

int compare_stab(const void *pa, const void *pb)
{
    const struct stab *a = pa, *b = pb;

#define ISDATA(a) (((a)->type == TDATA || (a)->type == TABS))
    if (ISDATA(a) && !ISDATA(b))
        return -1;
    if (!ISDATA(a) && ISDATA(b))
        return 1;
    if (a->value < b->value)
        return -1;
    return (a->value > b->value);
#undef ISDATA
}

int compare_constab(const void *pa, const void *pb)
{
    const struct constab *a = pa, *b = pb;

    if (a->sym < b->sym)
        return -1;
    return (a->sym > b->sym);
}

/*
 * Print the table of symbols and text constants.
 */
void listing()
{
    struct stab *s;
    struct constab *c;
    char *p, *lstname;
    FILE *lstfile;
    int t, n;

    lstname = malloc(4 + strlen(outfile));
    if (! lstname)
        uerror("out of memory");
    strcpy(lstname, outfile);
    p = strrchr(lstname, '.');
    if (! p)
        p = lstname + strlen(lstname);
    strcpy(p, ".lst");

    lstfile = fopen(lstname, "w");
    if (! lstfile)
        uerror("cannot write to %s", lstname);

    /* Remember the addresses of constants. */
    for (c=constab; c<constab+nconst; ++c)
        c->sym = stab[c->sym].value;

    /* Sort the symbol table. */
    qsort(stab, stabfree, sizeof(stab[0]), compare_stab);

    fprintf(lstfile, "Data symbols:\n");
    for (s=stab; s<stab+stabfree; ++s) {
        if (s->name[1] == '.')
            continue;
        switch (s->type) {
        default:     continue;
        case TDATA:  t = 'D'; break;
        case TABS:   t = 'A'; break;
        }
        fprintf(lstfile, "\t%04x  %c  %.*s\n",
            s->value, t, s->len, s->name);
    }
    fprintf(lstfile, "\nCode symbols:\n");
    for (s=stab; s<stab+stabfree; ++s) {
        if (s->name[1] == '.')
            continue;
        switch (s->type) {
        default:     continue;
        case TUNDF:  t = 'U'; break;
        case TTEXT:  t = 'T'; break;
        case TCONST: t = 'C'; break;
        }
        fprintf(lstfile, "\t%04x  %c  %.*s\n",
            s->value, t, s->len, s->name);
    }
    t = 0;
    for (n=TXTSIZE-1; n>=0; --n)
        if (tbusy [n]) {
            t = n;
            break;
        }
    fprintf(lstfile, "\t%04x  T  <end>\n", t);

    /* Sort the table of constants. */
    qsort(constab, nconst, sizeof(constab[0]), compare_constab);
    fprintf(lstfile, "\nText constants:\n");
    for (c=constab; c<constab+nconst; ++c) {
        /* Skip repeated constants. */
        if (c > constab && c->sym < c[-1].sym + c[-1].len)
            continue;

        fprintf(lstfile, "\t%04x  C  \"", c->sym);
        for (p=c->val; p<c->val+c->len; ++p) {
            switch (*p) {
            case '\n': fprintf(lstfile, "\\n");  break;
            case '\r': fprintf(lstfile, "\\r");  break;
            case '"':  fprintf(lstfile, "\\\""); break;
            case '\\': fprintf(lstfile, "\\\\"); break;
            case '\0':
                if (p < c->val+c->len-1)
                    fprintf(lstfile, "\\000");
                break;
            default:
                if (*p < ' ' || *p > '~')
                    fprintf(lstfile, "\\%03o", *p);
                else
                    fprintf(lstfile, "%c", *p);
            }
        }
        fprintf(lstfile, "\"\n");
    }
    fprintf(lstfile, "\nDone.\n");
}

void addreloc(int addr, int sym, int flags)
{
    if (nrel >= MAXREL)
        uerror("too many relocations");
    reltab[nrel].addr = addr;
    reltab[nrel].sym = sym;
    reltab[nrel].flags = flags;
    ++nrel;
    if (debug) {
        fprintf(stderr, "reloc %d", addr);
        if (sym)
            fprintf(stderr, " %d", sym);
        if (flags & RLAB)
            fprintf(stderr, " RLAB");
        if (flags & RLONG)
            fprintf(stderr, " RLONG");
        fprintf(stderr, "\n");
    }
}

/*
 * Compile the command.
 */
void makecmd(int code, int type)
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
        settext(count++, code);
        break;
    default:
        getexpr(&tval);
        if (! reached) {
            ++opt_not_reached;
            break;
        }
        if (tval == TUNDF || tval == TCONST)
            addreloc(count, extref, extflag);
        intval &= 0xff;
        if (optim && code == ATX && lastcmd == (XTA | intval)) {
            /* fprintf(stderr, "atx-xta at 0x%x\n", count); */
            ++opt_atx_xta;
            break;
        }
        settext(count++, code | intval);
        break;
    case FBYTE:     /* byte argument */
        getexpr(&tval);
        if (! reached) {
            ++opt_not_reached;
            break;
        }
        if (tval == TUNDF || tval == TCONST)
            addreloc(count, extref, extflag | RLONG);
        code |= intval & 0x1ff;
        settext(count++, code);
        break;
    case FLONG:             /* 9-bit argument */
        getexpr(&tval);
        if (! reached) {
            ++opt_not_reached;
            break;
        }
        if (tval == TUNDF || tval == TCONST)
            addreloc(count, extref, extflag | RLONG);
        code |= intval & 0x1ff;
        settext(count++, code);
        break;
    case FBIT:              /* additional bit argument */
        getexpr(&tval);
        code |= intval & 0xff;
        if (getlex(&cval, 0) != ',')
            uerror("need bit number");
        getexpr(&tval2);
        if (tval2 != TABS)
            uerror("bad bit number");
        if (! reached) {
            ++opt_not_reached;
            break;
        }
        if (tval == TUNDF || tval == TCONST)
            addreloc(count, extref, extflag);
        code |= intval << 5 & 0xe0;
        settext(count++, code);
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
