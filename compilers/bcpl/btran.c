/*
 * BCPL compiler rewritten to C.
 * Based on work of Robert Nordier, www.nordier.com.
 *
 * Copyright (C) 2013 Serge Vakulenko
 *
 * It is free software; you can redistribute it and/or modify it
 * under the terms of the "Artistic License".
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

/*
 * Functions from standard B library.
 */
#define getbyte(s, i)   *(i + (unsigned char*)s)
#define newline()       putchar('\n')
#define longjump(s,l)   longjmp((void*) s, (int) l)
#define setjump(s,l)    if (setjmp((void*) s)) goto l;
#define JUMPSZ          14

static inline void putbyte(s, i, c)
    int *s;
{
    *(i + (char*)s) = c;
}

int packstring(src, dest)
    int *src, *dest;
{
    int i, c;

    for (i=0; (c = src[i]); i++) {
        putbyte(dest, i, c);
    }
    do {
        putbyte(dest, i++, 0);
    } while (i & 3);
    return (i - 1) >> 2;
}

void unpackstring(src, dest)
    int *src, *dest;
{
    int i, c;

    for (i = 0; ; i++) {
        c = getbyte(src, i);
        dest[i] = c;
        if (c == 0)
            break;
    }
}

void aptovec(func, n)
    void (*func)();
{
    int vec [n];

    func(vec, n);
}

/* Forward declarations. */
void load(int *x);
void loadlv(int *x);
void loadlist(int *x);
void assign(int *x, int *y);
void trans(int *x);
void nextsymb();
int *rexp(int n);
int *rcom(void);
int *rdblockbody(void);

/* Boolean values */
#define FALSE   0
#define TRUE    -1

#define INCDIR  "/usr/local/lib/bcpl/"

/*
 * AE operators and symbols
 */
#define S_NUMBER	1
#define S_NAME          2
#define S_STRING	3
#define S_TRUE	        4
#define S_FALSE	        5
#define S_VALOF	        6
#define S_LV	        7
#define S_RV	        8
#define S_VECAP	        9
#define S_FNAP	        10
#define S_MULT	        11
#define S_DIV	        12
#define S_REM	        13
#define S_PLUS	        14
#define S_MINUS	        15
#define S_QUERY	        16
#define S_NEG	        17
#define S_EQ	        20
#define S_NE	        21
#define S_LS	        22
#define S_GR	        23
#define S_LE	        24
#define S_GE	        25
#define S_NOT	        30
#define S_LSHIFT	31
#define S_RSHIFT	32
#define S_LOGAND	33
#define S_LOGOR	        34
#define S_EQV	        35
#define S_NEQV	        36
#define S_COND	        37
#define S_COMMA	        38
#define S_TABLE	        39

#define S_AND	        40
#define S_VALDEF	41
#define S_VECDEF	42
#define S_CONSTDEF	43
#define S_FNDEF	        44
#define S_RTDEF	        45

#define S_ASSIGN        50
#define S_RTAP	        51
#define S_GOTO	        52
#define S_RESULTIS	53
#define S_COLON	        54
#define S_TEST	        55
#define S_FOR	        56
#define S_IF	        57
#define S_UNLESS	58
#define S_WHILE	        59
#define S_UNTIL	        60
#define S_REPEAT	61
#define S_REPEATWHILE	62
#define S_REPEATUNTIL	63
#define S_LOOP	        65
#define S_BREAK	        66
#define S_RETURN	67
#define S_FINISH	68
#define S_ENDCASE	69
#define S_SWITCHON	70
#define S_CASE	        71
#define S_DEFAULT	72
#define S_SEQ	        73
#define S_LET	        74
#define S_MANIFEST	75
#define S_GLOBAL	76
#define S_LOCAL	        77
#define S_LABEL	        78
#define S_STATIC	79

/*
 * Other canonical symbols
 */
#define S_BE	        89
#define S_END	        90
#define S_LSECT	        91
#define S_RSECT	        92
#define S_GET	        93
#define S_SEMICOLON	97
#define S_INTO	        98
#define S_TO	        99
#define S_BY	        100
#define S_DO	        101
#define S_OR	        102
#define S_VEC	        103
#define S_LPAREN	105
#define S_RPAREN	106

/*
 * OCODE instructions
 */
#define S_LP	        40
#define S_LG	        41
#define S_LN	        42
#define S_LSTR	        43
#define S_LL	        44
#define S_LLP	        45
#define S_LLG	        46
#define S_LLL	        47

#define S_SP	        80
#define S_SG	        81
#define S_SL	        82
#define S_STIND	        83
#define S_JUMP	        85
#define S_JT	        86
#define S_JF	        87
#define S_LAB	        90
#define S_STACK	        91
#define S_STORE	        92
#define S_RSTACK	93
#define S_ENTRY	        94
#define S_SAVE	        95
#define S_FNRN	        96
#define S_RTRN	        97
#define S_RES	        98
#define S_RESLAB	99
#define S_DATALAB	100
#define S_ITEML	        101
#define S_ITEMN	        102
#define S_ENDPROC	103

/*
 * Selectors
 */
#define H1	0
#define H2	1
#define H3	2
#define H4	3
#define H5	4
#define H6	5

/*
 * Globals used in lex
 */
int *chbuf;
int decval;
int *getv;
int getp;
int *wordv;
int wordsize;
int *charv;
int charp;
int prsource;
int prline;
int symb;
int *wordnode;
int *nulltag;
int ch;
int nlpending;
int pptrace;
int *option;
int chcount;
int linecount;
int *rec_p;
int *rec_l;
int terminator;

/*
 * Globals used in CAE
 */
int *nametable;
int nametablesize;
int *treep;
int *treevec;
int reportcount;
int reportmax;
FILE *sourcestream;
FILE *ocode;

/*
 * Globals used in translator
 */
int paramnumber;
int ocount;
int *dvec;
int dvecs;
int dvece;
int dvecp;
int dvect;
int *casek;
int *casel;
int casep;
int caset;
int caseb;
int *currentbranch;
int breaklabel;
int resultlabel;
int defaultlabel;
int endcaselabel;
int looplabel;
int ssp;
int vecssp;
int savespacesize;
int *globdecl;
int globdecls;
int globdeclt;
int comcount;

int nextparam()
{
    paramnumber = paramnumber + 1;
    return paramnumber;
}

int cellwithname(n)
    int *n;
{
    int x = dvece;

    do {
        x = x - 3;
    } while (x != 0 && (int*) dvec[x] != n);
    return x;
}

void wrc(ch)
{
    ocount = ocount + 1;
    if (ocount > 62 && ch == ' ') {
        putc('\n', ocode);
        ocount = 0;
        return;
    }
    putc(ch, ocode);
}

void trnmessage(n)
{
    char *s;

    switch (n) {
    default:  printf("COMPILER ERROR #%u\n", n); return;

    case 141: s = "TOO MANY CASES"; break;
    case 104: s = "ILLEGAL USE OF BREAK, LOOP OR RESULTIS"; break;
    case 101:
    case 105: s = "ILLEGAL USE OF case OR DEFAULT"; break;
    case 106: s = "TWO caseS WITH SAME CONSTANT"; break;
    case 144: s = "TOO MANY GLOBALS"; break;
    case 142: s = "NAME DECLARED TWICE"; break;
    case 143: s = "TOO MANY NAMES DECLARED"; break;
    case 115: s = "NAME NOT DECLARED"; break;
    case 116: s = "DYNAMIC FREE VARIABLE USED"; break;
    case 117: case 118: case 119:
              s = "ERROR IN CONSTANT EXPRESSION"; break;
    case 110: case 112:
              s = "LHS AND RHS DO NOT MATCH"; break;
    case 109: case 113:
              s = "LTYPE EXPRESSION EXPECTED"; break;
    }
    printf("%s\n", s);
}

void plist(x, n, d)
    int *x;
{
    int size = 0;
    int i;

    if (x == 0) {
        printf("NIL");
        return;
    }

    switch (H1[x]) {
    case S_NUMBER:
        printf("%d", H2[x]);
        return;

    case S_NAME:
        printf("%s", (char*) (x+2));
        return;

    case S_STRING:
        printf("\"%s\"", (char*) (x+1));
        return;

    case S_FOR:
        size = size + 2;

    case S_COND: case S_FNDEF: case S_RTDEF:
    case S_TEST: case S_CONSTDEF:
        size = size + 1;

    case S_VECAP: case S_FNAP:
    case S_MULT: case S_DIV: case S_REM: case S_PLUS: case S_MINUS:
    case S_EQ: case S_NE: case S_LS: case S_GR: case S_LE: case S_GE:
    case S_LSHIFT: case S_RSHIFT: case S_LOGAND: case S_LOGOR:
    case S_EQV: case S_NEQV: case S_COMMA:
    case S_AND: case S_VALDEF: case S_VECDEF:
    case S_ASSIGN: case S_RTAP: case S_COLON: case S_IF: case S_UNLESS:
    case S_WHILE: case S_UNTIL: case S_REPEATWHILE:
    case S_REPEATUNTIL:
    case S_SWITCHON: case S_CASE: case S_SEQ: case S_LET:
    case S_MANIFEST: case S_STATIC: case S_GLOBAL:
        size = size + 1;

    case S_VALOF: case S_LV: case S_RV: case S_NEG: case S_NOT:
    case S_TABLE: case S_GOTO: case S_RESULTIS: case S_REPEAT:
    case S_DEFAULT:
        size = size + 1;

    case S_LOOP:
    case S_BREAK: case S_RETURN: case S_FINISH: case S_ENDCASE:
    case S_TRUE: case S_FALSE: case S_QUERY:
    default:
        size = size + 1;
        if (n == d) {
            printf("ETC");
            return;
        }
        printf("OP");
        printf("%u", H1[x]);
        for (i = 2; i <= size; i++) {
            newline();
            printf("%*s*-", n+n, "");
            plist((int*) H1[x+i-1], n+1, d);
        }
        return;
    }
}

void transreport(n, x)
    int *x;
{
    reportcount = reportcount + 1;
    if (reportcount >= reportmax) {
        printf("\nCOMPILATION ABORTED\n");
        exit(8);
    }
    printf("\nREPORT:   ");
    trnmessage(n);
    printf("\nCOMMANDS COMPILED %u\n", comcount);
    plist(x, 0, 4);
    newline();
}

void writeop(x)
{
    char *s;
    int i;

    switch (x) {
    default: transreport(199, currentbranch); s = "ERROR"; break;

    case S_MULT:     s = "MULT";     break;
    case S_DIV:      s = "DIV";      break;
    case S_REM:      s = "REM";      break;
    case S_PLUS:     s = "PLUS";     break;
    case S_MINUS:    s = "MINUS";    break;
    case S_EQ:       s = "EQ";       break;
    case S_NE:       s = "NE";       break;
    case S_LS:       s = "LS";       break;
    case S_GR:       s = "GR";       break;
    case S_LE:       s = "LE";       break;
    case S_GE:       s = "GE";       break;
    case S_LSHIFT:   s = "LSHIFT";   break;
    case S_RSHIFT:   s = "RSHIFT";   break;
    case S_LOGAND:   s = "LOGAND";   break;
    case S_LOGOR:    s = "LOGOR";    break;
    case S_EQV:      s = "EQV";      break;
    case S_NEQV:     s = "NEQV";     break;

    case S_NEG:      s = "NEG";      break;
    case S_NOT:      s = "NOT";      break;
    case S_RV:       s = "RV";       break;

    case S_TRUE:     s = "TRUE";     break;
    case S_FALSE:    s = "FALSE";    break;
    case S_QUERY:    s = "QUERY";    break;

    case S_LP:       s = "LP";       break;
    case S_LG:       s = "LG";       break;
    case S_LN:       s = "LN";       break;
    case S_LSTR:     s = "LSTR";     break;
    case S_LL:       s = "LL";       break;

    case S_LLP:      s = "LLP";      break;
    case S_LLG:      s = "LLG";      break;
    case S_LLL:      s = "LLL";      break;

    case S_SP:       s = "SP";       break;
    case S_SG:       s = "SG";       break;
    case S_SL:       s = "SL";       break;
    case S_STIND:    s = "STIND";    break;

    case S_JUMP:     s = "JUMP";     break;
    case S_JT:       s = "JT";       break;
    case S_JF:       s = "JF";       break;
    case S_GOTO:     s = "GOTO";     break;
    case S_LAB:      s = "LAB";      break;
    case S_STACK:    s = "STACK";    break;
    case S_STORE:    s = "STORE";    break;

    case S_ENTRY:    s = "ENTRY";    break;
    case S_SAVE:     s = "SAVE";     break;
    case S_FNAP:     s = "FNAP";     break;
    case S_FNRN:     s = "FNRN";     break;
    case S_RTAP:     s = "RTAP";     break;
    case S_RTRN:     s = "RTRN";     break;
    case S_ENDPROC:  s = "ENDPROC";  break;
    case S_RES:      s = "RES";      break;
    case S_RSTACK:   s = "RSTACK";   break;
    case S_FINISH:   s = "FINISH";   break;

    case S_SWITCHON: s = "SWITCHON"; break;
    case S_GLOBAL:   s = "GLOBAL";   break;
    case S_DATALAB:  s = "DATALAB";  break;
    case S_ITEML:    s = "ITEML";    break;
    case S_ITEMN:    s = "ITEMN";    break;
    }

    for (i = 0; ; i++) {
        int c = getbyte(s, i);
        if (c == 0)
            break;
        wrc(c);
    }
}

void wrpn(n)
{
    if (n > 9)
        wrpn(n / 10);
    wrc(n % 10 + '0');
}

void wrn(n)
{
    if (n < 0) {
        wrc('-');
        n = - n;
    }
    wrpn(n);
}

void endocode()
{
    putc('\n', ocode);
    ocount = 0;
}

void out1(x)
{
    writeop(x);
    wrc(' ');
}

void out2(x, y)
{
    writeop(x);
    wrc(' ');
    wrn(y);
    wrc(' ');
}

void out2p(x, y)
{
    writeop(x);
    wrc(' ');
    wrc('L');
    wrn(y);
    wrc(' ');
}

void out3p(x, y, z)
{
    writeop(x);
    wrc(' ');
    wrn(y);
    wrc(' ');
    wrc('L');
    wrn(z);
    wrc(' ');
}

void outn(n)
{
    wrn(n);
}

void outl(x)
{
    wrc(' ');
    wrc('L');
    wrn(x);
    wrc(' ');
}

void outc(x)
{
    wrn(x);
    wrc(' ');
}

void complab(l)
{
    out2p(S_LAB, l);
}

void compentry(n, l)
    int *n;
{
    int *s = &n[2];
    int i, len;

    len = strlen((char*) s);
    out3p(S_ENTRY, len, l);
    for (i = 0; i < len; i++)
        outc(getbyte(s, i));
    wrc(' ');
}

void compdatalab(l)
{
    out2p(S_DATALAB, l);
}

void compjump(l)
{
    out2p(S_JUMP, l);
}

void addname(n, p, a)
    int *n;
{
    if (dvecs >= dvect)
        transreport(143, currentbranch);
    dvec[dvecs]   = (int) n;
    dvec[dvecs+1] = p;
    dvec[dvecs+2] = a;
    dvecs = dvecs + 3;
}

void declstat(x, l)
    int *x;
{
    int t = cellwithname(x);
    int m;

    if (dvec[t+1] == S_GLOBAL) {
        int n = dvec[t+2];

        addname(x, S_GLOBAL, n);
        if (globdecls >= globdeclt)
            transreport(144, x);
        globdecl[globdecls] = n;
        globdecl[globdecls+1] = l;
        globdecls = globdecls + 2;
        return;
    }

    m = nextparam();
    addname(x, S_LABEL, m);
    compdatalab(m);
    out2p(S_ITEML, l);
}

void decldyn(x)
    int *x;
{
    if (x != 0) {
        if (H1[x] == S_NAME) {
            addname(x, S_LOCAL, ssp);
            ssp = ssp + 1;
            return;
        }
        if (H1[x] == S_COMMA) {
            addname((int*) H2[x], S_LOCAL, ssp);
            ssp = ssp + 1;
            decldyn((int*) H3[x]);
            return;
        }
        transreport(103, x);
    }
}

void declnames(x)
    int *x;
{
    if (x != 0) {
        switch (H1[x]) {
        default:
            transreport(102, currentbranch);
            return;

        case S_VECDEF: case S_VALDEF:
            decldyn((int*) H2[x]);
            return;

        case S_RTDEF: case S_FNDEF:
            H5[x] = nextparam();
            declstat((int*) H2[x], H5[x]);
            return;

        case S_AND:
            declnames((int*) H2[x]);
            declnames((int*) H3[x]);
            return;
        }
    }
}

void scanlabels(x)
    int *x;
{
    if (x != 0) {
        switch (H1[x]) {
        default:
            return;

        case S_COLON:
            H4[x] = nextparam();
            declstat((int*) H2[x], H4[x]);

        case S_IF: case S_UNLESS: case S_WHILE: case S_UNTIL:
        case S_SWITCHON: case S_CASE:
            scanlabels((int*) H3[x]);
            return;

        case S_SEQ:
            scanlabels((int*) H3[x]);

        case S_REPEAT:
        case S_REPEATWHILE: case S_REPEATUNTIL: case S_DEFAULT:
            scanlabels((int*) H2[x]);
            return;

        case S_TEST:
            scanlabels((int*) H3[x]);
            scanlabels((int*) H4[x]);
            return;
        }
    }
}

void checkdistinct(e, s)
{
    while (e != s) {
        int p = e + 3;
        int n = dvec[e];

        while (p < s) {
            if (dvec[p] == n)
                transreport(142, (int*) n);
            p = p + 3;
        }
        e = e + 3;
    }
}

void decllabels(x)
    int *x;
{
    int b = dvecs;

    scanlabels(x);
    checkdistinct(b, dvecs);
    dvece = dvecs;
}

void transname(x, p, g, l, n)
    int *x;
{
    int t = cellwithname(x);
    int k = dvec[t+1];
    int a = dvec[t+2];

    if (t == 0) {
        transreport(115, x);
        out2(g, 2);
        return;
    }

    switch (k) {
    case S_LOCAL:
        if (t < dvecp)
            transreport(116, x);
        out2(p, a);
        return;

    case S_GLOBAL:
        out2(g, a);
        return;

    case S_LABEL:
        out2p(l, a);
        return;

    case S_NUMBER:
        if (n == 0) {
            transreport(113, x);
            n = p;
        }
        out2(n, a);
    }
}

void jumpcond(x, b, l)
    int *x;
{
    int sw = b;

    switch (H1[x]) {
    case S_FALSE:
        b = ! b;
    case S_TRUE:
        if (b)
            compjump(l);
        return;

    case S_NOT:
        jumpcond((int*) H2[x], ! b, l);
        return;

    case S_LOGAND:
        sw = ! sw;
    case S_LOGOR:
        if (sw) {
            jumpcond((int*) H2[x], b, l);
            jumpcond((int*) H3[x], b, l);
        } else {
            int m = nextparam();
            jumpcond((int*) H2[x], ! b, m);
            jumpcond((int*) H3[x], b, l);
            complab(m);
        }
        return;

    default:
        load(x);
        out2p(b ? S_JT : S_JF, l);
        ssp = ssp - 1;
        return;
    }
}

int evalconst(x)
    int *x;
{
    if (x == 0) {
        transreport(117, currentbranch);
        return 0;
    }
    switch (H1[x]) {
    default:
        transreport(118, x);
        return 0;

    case S_NAME: {
        int t = cellwithname(x);
        if (dvec[t+1] == S_NUMBER)
            return dvec[t+2];
        transreport(119, x);
        return 0;
    }
    case S_NUMBER: return H2[x];
    case S_TRUE:   return TRUE;
    case S_FALSE:  return FALSE;
    case S_NEG:    return - evalconst((int*) H2[x]);
    case S_MULT:   return evalconst((int*) H2[x]) * evalconst((int*) H3[x]);
    case S_DIV:    return evalconst((int*) H2[x]) / evalconst((int*) H3[x]);
    case S_PLUS:   return evalconst((int*) H2[x]) + evalconst((int*) H3[x]);
    case S_MINUS:  return evalconst((int*) H2[x]) - evalconst((int*) H3[x]);
    }

}

void transdyndefs(x)
    int *x;
{
    switch (H1[x]) {
    case S_AND:
        transdyndefs((int*) H2[x]);
        transdyndefs((int*) H3[x]);
        return;

    case S_VECDEF:
        out2(S_LLP, vecssp);
        ssp = ssp + 1;
        vecssp = vecssp + 1 + evalconst((int*) H3[x]);
        return;

    case S_VALDEF:
        loadlist((int*) H3[x]);
        return;

    default:
        return;
    }
}

void transstatdefs(x)
    int *x;
{
    switch (H1[x]) {
    case S_AND:
        transstatdefs((int*) H2[x]);
        transstatdefs((int*) H3[x]);
        return;

    case S_FNDEF: case S_RTDEF: {
        int a = dvece;
        int b = dvecs;
        int c = dvecp;
        int bl = breaklabel;
        int ll = looplabel;
        int rl = resultlabel;
        int cb = caseb;

        breaklabel = -1;
        looplabel = -1;
        resultlabel = -1;
        caseb = -1;

        compentry((int*) H2[x], H5[x]);
        ssp = savespacesize;

        dvecp = dvecs;
        decldyn((int*) H3[x]);
        checkdistinct(b, dvecs);
        dvece = dvecs;
        decllabels((int*) H4[x]);

        out2(S_SAVE, ssp);

        if (H1[x] == S_FNDEF) {
            load((int*) H4[x]);
            out1(S_FNRN);
        } else {
            trans((int*) H4[x]);
            out1(S_RTRN);
        }
        out2(S_ENDPROC, 0);

        breaklabel = bl;
        looplabel = ll;
        resultlabel = rl;
        caseb = cb;
        dvece = a;
        dvecs = b;
        dvecp = c;
    }
    default:
        return;
    }
}

int statdefs(x)
    int *x;
{
    if (H1[x] == S_FNDEF || H1[x] == S_RTDEF)
        return TRUE;
    if (H1[x] != S_AND)
        return FALSE;
    if (statdefs((int*) H2[x]))
        return TRUE;
    return statdefs((int*) H3[x]);
}

void transdef(x)
    int *x;
{
    transdyndefs(x);
    if (statdefs(x)) {
        int l = nextparam();
        int s = ssp;

        compjump(l);
        transstatdefs(x);
        ssp = s;
        out2(S_STACK, ssp);
        complab(l);
    }
}

void transswitch(x)
    int *x;
{
    int p = casep;
    int b = caseb;
    int dl = defaultlabel;
    int ecl = endcaselabel;
    int l = nextparam();
    int i;

    endcaselabel = nextparam();
    caseb = casep;

    compjump(l);
    defaultlabel = 0;
    trans((int*) H3[x]);
    compjump(endcaselabel);

    complab(l);
    load((int*) H2[x]);
    if (defaultlabel == 0)
        defaultlabel = endcaselabel;
    out3p(S_SWITCHON, casep-p, defaultlabel);

    for (i = caseb; i<casep; i++) {
        outn(casek[i]);
        outl(casel[i]);
    }
    ssp = ssp - 1;
    complab(endcaselabel);
    endcaselabel = ecl;
    casep = p;
    caseb = b;
    defaultlabel = dl;
}

void transfor(x)
    int *x;
{
    int a = dvece;
    int b = dvecs;
    int l = nextparam();
    int m = nextparam();
    int bl = breaklabel;
    int ll = looplabel;
    int k = 0;
    int n = 0;
    int step = 1;
    int s = ssp;
    int *h4 = (int*) H4[x];

    breaklabel = 0;
    looplabel = 0;

    addname((int*) H2[x], S_LOCAL, s);
    dvece = dvecs;
    load((int*) H3[x]);

    if (H1[h4] == S_NUMBER) {
        k = S_LN;
        n = H2[h4];
    } else {
        k = S_LP;
        n = ssp;
        load(h4);
    }

    if (H5[x] != 0)
        step = evalconst((int*) H5[x]);

    out1(S_STORE);
    compjump(l);
    decllabels((int*) H6[x]);
    complab(m);
    trans((int*) H6[x]);
    if (looplabel != 0)
        complab(looplabel);
    out2(S_LP, s);
    out2(S_LN, step);
    out1(S_PLUS);
    out2(S_SP, s);
    complab(l);
    out2(S_LP, s);
    out2(k, n);
    out1(step<0 ? S_GE : S_LE);
    out2p(S_JT, m);

    if (breaklabel != 0)
        complab(breaklabel);
    breaklabel = bl;
    looplabel = ll;
    ssp = s;
    out2(S_STACK, ssp);
    dvece = a;
    dvecs = b;
}

void trans(x)
    int *x;
{
    int sw;
next:
    if (x == 0)
        return;
    currentbranch = x;
    sw = FALSE;

    switch (H1[x]) {
    default:
        transreport(100, x);
        return;

    case S_LET: {
        int a = dvece;
        int b = dvecs;
        int s = ssp;
        int s1 = 0;
        int v = vecssp;

        declnames((int*) H2[x]);
        checkdistinct(b, dvecs);
        dvece = dvecs;
        vecssp = ssp;
        s1 = ssp;
        ssp = s;
        transdef((int*) H2[x]);
        if (ssp != s1)
            transreport(110, x);
        if (ssp != vecssp) {
            ssp = vecssp;
            out2(S_STACK, ssp);
        }
        out1(S_STORE);
        decllabels((int*) H3[x]);
        trans((int*) H3[x]);
        vecssp = v;
        if (ssp != s)
            out2(S_STACK, s);
        dvece = a;
        dvecs = b;
        ssp = s;
        return;
    }
    case S_STATIC:
    case S_GLOBAL:
    case S_MANIFEST: {
        int a = dvece;
        int b = dvecs;
        int s = ssp;
        int op = H1[x];
        int *y = (int*) H2[x];

        if (op == S_MANIFEST)
            op = S_NUMBER;

        while (y != 0) {
            if (op == S_STATIC) {
                int m = nextparam();

                addname((int*) H3[y], S_LABEL, m);
                compdatalab(m);
                out2(S_ITEMN, evalconst((int*) H4[y]));
            } else
                addname((int*) H3[y], op, evalconst((int*) H4[y]));

            y = (int*) H2[y];
            dvece = dvecs;
        }
        decllabels((int*) H3[x]);
        trans((int*) H3[x]);
        dvece = a;
        dvecs = b;
        ssp = s;
        return;
    }
    case S_ASSIGN:
       assign((int*) H2[x], (int*) H3[x]);
       return;

    case S_RTAP: {
        int s = ssp;

        ssp = ssp + savespacesize;
        out2(S_STACK, ssp);
        loadlist((int*) H3[x]);
        load((int*) H2[x]);
        out2(S_RTAP, s);
        ssp = s;
        return;
    }
    case S_GOTO:
        load((int*) H2[x]);
        out1(S_GOTO);
        ssp = ssp-1;
        return;

    case S_COLON:
        complab(H4[x]);
        trans((int*) H3[x]);
        return;

    case S_UNLESS:
        sw = TRUE;
    case S_IF: {
        int l = nextparam();

        jumpcond((int*) H2[x], sw, l);
        trans((int*) H3[x]);
        complab(l);
        return;
    }
    case S_TEST: {
        int l = nextparam();
        int m = nextparam();

        jumpcond((int*) H2[x], FALSE, l);
        trans((int*) H3[x]);
        compjump(m);
        complab(l);
        trans((int*) H4[x]);
        complab(m);
        return;
    }
    case S_LOOP:
        if (looplabel < 0)
            transreport(104, x);
        if (looplabel == 0)
            looplabel = nextparam();
        compjump(looplabel);
        return;

    case S_BREAK:
        if (breaklabel < 0)
            transreport(104, x);
        if (breaklabel == 0)
            breaklabel = nextparam();
        compjump(breaklabel);
        return;

    case S_RETURN:
        out1(S_RTRN);
        return;

    case S_FINISH:
        out1(S_FINISH);
        return;

    case S_RESULTIS:
        if (resultlabel < 0)
            transreport(104, x);
        load((int*) H2[x]);
        out2p(S_RES, resultlabel);
        ssp = ssp - 1;
        return;

    case S_WHILE:
        sw = TRUE;
    case S_UNTIL: {
        int l = nextparam();
        int m = nextparam();
        int bl = breaklabel;
        int ll = looplabel;
        breaklabel = 0;
        looplabel = m;
        compjump(m);
        complab(l);
        trans((int*) H3[x]);
        complab(m);
        jumpcond((int*) H2[x], sw, l);
        if (breaklabel != 0)
            complab(breaklabel);
        breaklabel = bl;
        looplabel = ll;
        return;
    }
    case S_REPEATWHILE:
        sw = TRUE;
    case S_REPEATUNTIL:
    case S_REPEAT: {
        int l = nextparam();
        int bl = breaklabel;
        int ll = looplabel;
        breaklabel = 0;
        looplabel = 0;
        complab(l);
        if (H1[x] == S_REPEAT) {
            looplabel = l;
            trans((int*) H2[x]);
            compjump(l);
        } else {
            trans((int*) H2[x]);
            if (looplabel != 0)
                complab(looplabel);
            jumpcond((int*) H3[x], sw, l);
        }
        if (breaklabel != 0)
            complab(breaklabel);
        breaklabel = bl;
        looplabel = ll;
        return;
    }
    case S_CASE: {
        int l = nextparam();
        int k = evalconst((int*) H2[x]);
        int i;

        if (casep >= caset)
            transreport(141, x);
        if (caseb < 0)
            transreport(105, x);
        for (i = caseb; i < casep; i++)
            if (casek[i] == k)
                transreport(106, x);
        casek[casep] = k;
        casel[casep] = l;
        casep = casep + 1;
        complab(l);
        trans((int*) H3[x]);
        return;
    }
    case S_DEFAULT:
        if (caseb < 0)
            transreport(105, x);
        if (defaultlabel != 0)
            transreport(101, x);
        defaultlabel = nextparam();
        complab(defaultlabel);
        trans((int*) H2[x]);
        return;

    case S_ENDCASE:
        if (caseb < 0)
            transreport(105, x);
        compjump(endcaselabel);
        return;

    case S_SWITCHON:
        transswitch(x);
        return;

    case S_FOR:
        transfor(x);
        return;

    case S_SEQ:
        trans((int*) H2[x]);
        comcount = comcount + 1;
        x = (int*) H3[x];
        goto next;
    }
}

void loadzero()
{
    out2(S_LN, 0);
    ssp = ssp + 1;
}

void load(x)
    int *x;
{
    int op, i;

    if (x == 0) {
        transreport(148, currentbranch);
        loadzero();
        return;
    }

    op = H1[x];

    switch (op) {
    default:
        transreport(147, currentbranch);
        loadzero();
        return;

    case S_DIV:    case S_REM:    case S_MINUS:
    case S_LS:     case S_GR:     case S_LE:    case S_GE:
    case S_LSHIFT: case S_RSHIFT:
        load((int*) H2[x]);
        load((int*) H3[x]);
        out1(op);
        ssp = ssp - 1;
        return;

    case S_VECAP:  case S_MULT:  case S_PLUS: case S_EQ:  case S_NE:
    case S_LOGAND: case S_LOGOR: case S_EQV:  case S_NEQV: {
        int *a = (int*) H2[x];
        int *b = (int*) H3[x];
        if (H1[a] == S_NAME || H1[a] == S_NUMBER) {
            a = (int*) H3[x];
            b = (int*) H2[x];
        }
        load(a);
        load(b);
        if (op == S_VECAP) {
            out1(S_PLUS);
            op = S_RV;
        }
        out1(op);
        ssp = ssp - 1;
        return;
    }
    case S_NEG: case S_NOT: case S_RV:
        load((int*) H2[x]);
        out1(op);
        return;

    case S_TRUE: case S_FALSE: case S_QUERY:
        out1(op);
        ssp = ssp + 1;
        return;

    case S_LV:
        loadlv((int*) H2[x]);
        return;

    case S_NUMBER:
        out2(S_LN, H2[x]);
        ssp = ssp + 1;
        return;

    case S_STRING: {
        int *s = &H2[x];
        int len = strlen((char*) s);
        out2(S_LSTR, len);
        for (i = 0; i < len; i++)
            outc(getbyte(s, i));
        wrc(' ');
        ssp = ssp + 1;
        return;
    }
    case S_NAME:
         transname(x, S_LP, S_LG, S_LL, S_LN);
         ssp = ssp + 1;
         return;

    case S_VALOF: {
        int rl = resultlabel;
        int a = dvecs;
        int b = dvece;
        decllabels((int*) H2[x]);
        resultlabel = nextparam();
        trans((int*) H2[x]);
        complab(resultlabel);
        out2(S_RSTACK, ssp);
        ssp = ssp + 1;
        dvecs = a;
        dvece = b;
        resultlabel = rl;
        return;
    }
    case S_FNAP: {
        int s = ssp;
        ssp = ssp + savespacesize;
        out2(S_STACK, ssp);
        loadlist((int*) H3[x]);
        load((int*) H2[x]);
        out2(S_FNAP, s);
        ssp = s + 1;
        return;
    }
    case S_COND: {
        int l = nextparam();
        int m = nextparam();
        int s = ssp;
        jumpcond((int*) H2[x], FALSE, m);
        load((int*) H3[x]);
        compjump(l);
        ssp = s;
        out2(S_STACK, ssp);
        complab(m);
        load((int*) H4[x]);
        complab(l);
        return;
    }
    case S_TABLE: {
        int m = nextparam();
        compdatalab(m);
        x = (int*) H2[x];
        while (H1[x] == S_COMMA) {
            out2(S_ITEMN, evalconst((int*) H2[x]));
            x = (int*) H3[x];
        }
        out2(S_ITEMN, evalconst(x));
        out2p(S_LLL, m);
        ssp = ssp + 1;
        return;
    }
    }
}

void loadlv(x)
    int *x;
{
    if (x == 0)
        goto err;

    switch (H1[x]) {
    default:
err:    transreport(113, currentbranch);
        loadzero();
        return;

    case S_NAME:
        transname(x, S_LLP, S_LLG, S_LLL, 0);
        ssp = ssp + 1;
        return;

    case S_RV:
        load((int*) H2[x]);
        return;

    case S_VECAP: {
        int *a = (int*) H2[x];
        int *b = (int*) H3[x];
        if (H1[a] == S_NAME) {
            a = (int*) H3[x];
            b = (int*) H2[x];
        }
        load(a);
        load(b);
        out1(S_PLUS);
        ssp = ssp - 1;
        return;
    }
    }
}

void loadlist(x)
    int *x;
{
    if (x != 0) {
        if (H1[x] != S_COMMA) {
            load(x);
            return;
        }
        loadlist((int*) H2[x]);
        loadlist((int*) H3[x]);
    }
}

void assign(x, y)
    int *x, *y;
{
    if (x == 0 || y == 0) {
        transreport(110, currentbranch);
        return;
    }

    switch (H1[x]) {
    case S_COMMA:
        if (H1[y] != S_COMMA) {
            transreport(112, currentbranch);
            return;
        }
        assign((int*) H2[x], (int*) H2[y]);
        assign((int*) H3[x], (int*) H3[y]);
        return;

    case S_NAME:
        load(y);
        transname(x, S_SP, S_SG, S_SL, 0);
        ssp = ssp - 1;
        return;

    case S_RV: case S_VECAP: case S_COND:
        load(y);
        loadlv(x);
        out1(S_STIND);
        ssp = ssp - 2;
        return;

    default:
        transreport(109, currentbranch);
    }
}

void compileae(x)
    int *x;
{
    int a [1200];
    int d [100];
    int k [150];
    int l [150];
    int i;

    dvec = a;
    dvecs = 3;
    dvece = 3;
    dvecp = 3;
    dvect = 1200;
    dvec[0] = 0;
    dvec[1] = 0;
    dvec[2] = 0;

    globdecl = d;
    globdecls = 0;
    globdeclt = 100;

    casek = k;
    casel = l;
    casep = 0;
    caset = 150;
    caseb = -1;
    endcaselabel = 0;
    defaultlabel = 0;

    resultlabel = -1;
    breaklabel = -1;
    looplabel = -1;

    comcount = 0;
    currentbranch = x;

    ocount = 0;

    paramnumber = 0;
    ssp = savespacesize;
    out2(S_STACK, ssp);
    decllabels(x);
    trans(x);
    out2(S_GLOBAL, globdecls/2);

    for (i = 0; i < globdecls; i = i + 2) {
        outn(globdecl[i]);
        outl(globdecl[i+1]);
    }
    endocode();
}

void rch()
{
    ch = getc(sourcestream);

    if (prsource && getp == 0 && ch != EOF) {
        if (linecount != prline) {
            printf("%4u  ", linecount);
            prline = linecount;
        }
        putchar(ch);
    }
    chcount = chcount + 1;
    chbuf[chcount & 63] = ch;
}

void wrchbuf()
{
    int p;

    printf("\n...");
    for (p = chcount-63; p <= chcount; p++) {
        int k = chbuf[p & 63];
        if (k != 0)
            putchar(k);
    }
    newline();
}

void caemessage(n)
{
    char *s;

    switch (n) {
    default: printf("%u", n); return;

    case 91: s = "'8'  '(' OR ')' EXPECTED"; break;
    case 94: s = "ILLEGAL CHARACTER"; break;
    case 95: s = "STRING TOO LONG"; break;
    case 96: s = "NO INPUT %s"; break;
    case 97: s = "STRING OR NUMBER EXPECTED"; break;
    case 98: s = "PROGRAM TOO LARGE"; break;
    case 99: s = "INCORRECT TERMINATION"; break;

    case 8: case 40: case 43:
             s = "NAME EXPECTED"; break;
    case 6:  s = "'{' EXPECTED"; break;
    case 7:  s = "'}' EXPECTED"; break;
    case 9:  s = "UNTAGGED '}' MISMATCH"; break;
    case 32: s = "ERROR IN EXPRESSION"; break;
    case 33: s = "ERROR IN NUMBER"; break;
    case 34: s = "BAD STRING"; break;
    case 15: case 19: case 41:
             s = "')' MISSING"; break;
    case 30: s = "',' MISSING"; break;
    case 42: s = "'=' OR 'BE' EXPECTED"; break;
    case 44: s = "'=' OR '(' EXPECTED"; break;
    case 50: s = "ERROR IN LABEL"; break;
    case 51: s = "ERROR IN COMMAND"; break;
    case 54: s = "'OR' EXPECTED"; break;
    case 57: s = "'=' EXPECTED"; break;
    case 58: s = "'TO' EXPECTED"; break;
    case 60: s = "'INTO' EXPECTED"; break;
    case 61: case 62:
             s = "':' EXPECTED"; break;
    case 63: s = "'*/' MISSING"; break;
    }
    printf(s, wordv);
}

void caereport(n)
{
    reportcount = reportcount + 1;
    printf("\nSYNTAX ERROR NEAR LINE %u:  ", linecount);
    caemessage(n);
    wrchbuf();
    if (reportcount > reportmax) {
        printf("\nCOMPILATION ABORTED\n");;
        exit(8);
    }
    nlpending = FALSE;

    while (symb != S_LSECT && symb != S_RSECT &&
           symb != S_LET   && symb != S_AND &&
           symb != S_END   && ! nlpending) {
        nextsymb();
    }
    longjump(rec_p, rec_l);
}

int value(ch)
{
    return ('0' <= ch && ch <= '9') ? (ch - '0') :
           ('A' <= ch && ch <= 'F') ? (ch - 'A' + 10) :
           ('a' <= ch && ch <= 'f') ? (ch - 'a' + 10) : 100;
}

void readnumber(radix)
{
    int d = value(ch);

    decval = d;
    if (d >= radix)
        caereport(33);

    for (;;) {
        rch();
        d = value(ch);
        if (d >= radix)
            return;
        decval = radix*decval + d;
    }
}

void rdtag(x)
{
    charp = 0;
    charv[0] = x;
    for (;;) {
        rch();
        if (! (('A' <= ch && ch <= 'Z') ||
               ('a' <= ch && ch <= 'z') ||
               ('0' <= ch && ch <= '9') || ch == '.'))
            break;
        charp = charp+1;
        charv[charp] = ch;
    }
    charv[charp+1] = 0;
    wordsize = packstring(charv, wordv);
}

int *newvec(n)
{
    treep = treep - n - 1;
    if (treep <= treevec) {
        reportmax = 0;
        caereport(98);
    }
    return treep;
}

int lookupword()
{
    int hashval = (wordv[0] + (wordv[wordsize] >> 1)) % nametablesize;
    int *m = &nametable[hashval];
    int i;
next:
    wordnode = (int*) *m;
    if (wordnode != 0) {
        for (i = 0; i <= wordsize; i++) {
            if (wordnode[i+2] != wordv[i]) {
                m = wordnode+1;
                goto next;
            }
        }
        return wordnode[0];
    }
    wordnode = newvec(wordsize+2);
    wordnode[0] = S_NAME;
    wordnode[1] = nametable[hashval];
    for (i = 0; i <= wordsize; i++)
        wordnode[i+2] = wordv[i];
    nametable[hashval] = (int) wordnode;
    return S_NAME;
}

FILE *findlibinput(name)
    int *name;
{
    char path [256];

    if (sizeof(INCDIR) + strlen((char*) name) > 255)
        return 0;
    strcpy(path, INCDIR);
    strcat(path, (char*) name);
    return fopen(path, "r");
}

void performget()
{
    nextsymb();
    if (symb != S_STRING)
        caereport(97);

    if (option[5])
        return;

    getv[getp] = (int) sourcestream;
    getv[getp+1] = linecount;
    getv[getp+2] = ch;
    getp = getp + 3;
    linecount = 1;
    sourcestream = fopen((char*) wordv, "r");
    if (sourcestream == 0)
        sourcestream = findlibinput(wordv);
    if (sourcestream == 0)
        caereport(96);
    rch();
}

void nextsymb()
{
    nlpending = FALSE;
next:
    if (pptrace) {
        putchar(ch);
    }

    switch (ch) {
    case '\n':
        linecount = linecount + 1;
        nlpending = TRUE;           // ignorable characters
    case '\v':
    case '\t':
    case ' ':
        do {
            rch();
        } while (ch == ' ');
        goto next;

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        symb = S_NUMBER;
        readnumber(10);
        return;

    case 'A': case 'B': case 'C': case 'D': case 'E':
    case 'F': case 'G': case 'H': case 'I': case 'J':
    case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y':
    case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e':
    case 'f': case 'g': case 'h': case 'i': case 'j':
    case 'k': case 'l': case 'm': case 'n': case 'o':
    case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y':
    case 'z':
        rdtag(ch);
        symb = lookupword();
        if (symb == S_GET) {
            performget();
            goto next;
        }
        return;

    case '$':
        rch();
        if (ch != '(' && ch != ')')
            caereport(91);
        symb = (ch == '(') ? S_LSECT : S_RSECT;
        rdtag('$');
        lookupword();
        return;

    case '[':
    case '(': symb = S_LPAREN; goto L;
    case ']':
    case ')': symb = S_RPAREN; goto L;

    case '#':
        symb = S_NUMBER;
        rch();
        if ('0' <= ch && ch <= '7') {
            readnumber(8);
            return;
        }
        if (ch == 'B') {
            rch();
            readnumber(2);
            return;
        }
        if (ch == 'O') {
            rch();
            readnumber(8);
            return;
        }
        if (ch == 'X') {
            rch();
            readnumber(16);
            return;
        }
        caereport(33);

    case '?': symb = S_QUERY;     goto L;
    case '+': symb = S_PLUS;      goto L;
    case ',': symb = S_COMMA;     goto L;
    case ';': symb = S_SEMICOLON; goto L;
    case '@': symb = S_LV;        goto L;
    case '&': symb = S_LOGAND;    goto L;
    case '=': symb = S_EQ;        goto L;
    case '!': symb = S_VECAP;     goto L;
    case '_': symb = S_ASSIGN;    goto L;
    case '*': symb = S_MULT;      goto L;

    case '/':
        rch();
        if (ch == '\\') {
            symb = S_LOGAND;
            goto L;
        }
        if (ch == '/')
            goto comment;
        if (ch != '*') {
            symb = S_DIV;
            return;
        }
        rch();
        while (ch != EOF) {
            if (ch == '*') {
                rch();
                if (ch != '/')
                    continue;
                rch();
                goto next;
            } else {
                if (ch == '\n')
                    linecount = linecount+1;
                rch();
            }
        }
        caereport(63);
comment:
        do {
            rch();
        } while (ch != '\n' && ch != EOF);
        goto next;

    case '|':
        rch();
        if (ch == '|')
             goto comment;
        symb = S_LOGOR;
        return;

    case '\\':
        rch();
        if (ch == '/') {
            symb = S_LOGOR;
            goto L;
        }
        if (ch == '=') {
            symb = S_NE;
            goto L;
        }
        symb = S_NOT;
        return;

    case '<':
        rch();
        if (ch == '=') {
            symb = S_LE;
            goto L;
        }
        if (ch == '<') {
            symb = S_LSHIFT;
            goto L;
        }
        symb = S_LS;
        return;

    case '>':
        rch();
        if (ch == '=') {
            symb = S_GE;
            goto L;
        }
        if (ch == '>') {
            symb = S_RSHIFT;
            goto L;
        }
        symb = S_GR;
        return;

    case '-':
        rch();
        if (ch == '>') {
            symb = S_COND;
            goto L;
        }
        symb = S_MINUS;
        return;

    case ':':
        rch();
        if (ch == '=') {
            symb = S_ASSIGN;
            goto L;
        }
        symb = S_COLON;
        return;

     case '\'': case '\"': {
        int quote = ch;
        charp = 0;

        for (;;) {
            rch();
            if (ch == quote || charp == 255) {
                if (ch != quote)
                    caereport(95);
                if (charp == 1 && ch == '\'') {
                    symb = S_NUMBER;
                    goto L;
                }
                charv[charp] = 0;
                wordsize = packstring(charv, wordv);
                symb = S_STRING;
                goto L;
            }

            if (ch == '\n')
                linecount = linecount + 1;

            if (ch == '*') {
                rch();
                if (ch == '\n') {
                    linecount = linecount+1;
                    do {
                        rch();
                    } while (ch == ' ' || ch == '\t');
                    if (ch != '*')
                        caereport(34);
                    continue;
                }
                if (ch == 'T') ch = '\t';
                if (ch == 'S') ch = ' ';
                if (ch == 'N') ch = '\n';
                if (ch == 'B') ch = '\b';
                if (ch == 'P') ch = '\f';
            }

            decval = ch;
            charv[charp] = ch;
            charp = charp+1;
        }
    }
    case '.':
    case EOF:
        if (getp == 0) {
            symb = S_END;
            return;
        }
        fclose(sourcestream);
        getp = getp - 3;
        sourcestream = (FILE*) getv[getp];
        linecount = getv[getp+1];
        ch = getv[getp+2];
        goto next;

    default:
        ch = ' ';
        caereport(94);
        rch();
        goto next;
    }
L:
    rch();
}

int readn()
{
    int sum = 0;
    int neg = FALSE;
L:
    terminator = getc(sourcestream);
    switch (terminator) {
    case ' ':
    case '\t':
    case '\n': goto L;

    case '-':  neg = TRUE;
    case '+':  terminator = getc(sourcestream);
    }
    while ('0' <= terminator && terminator <= '9') {
        sum = 10*sum + terminator - '0';
        terminator = getc(sourcestream);
    }
    if (neg)
        sum = -sum;
    return sum;
}

void ignore(item)
{
    if (symb == item)
        nextsymb();
}

void checkfor(item, n)
{
    if (symb != item)
        caereport(n);
    nextsymb();
}

int *rdsect(func)
    int *(*func)();
{
    int *tag = wordnode;
    int *a = 0;

    checkfor(S_LSECT, 6);
    a = func();
    if (symb != S_RSECT)
        caereport(7);

    if (tag == wordnode)
        nextsymb();
    else if (wordnode == nulltag) {
        symb = 0;
        caereport(9);
    }
    return a;
}

int *rname()
{
    int *a = wordnode;

    checkfor(S_NAME, 8);
    return a;
}

int *list1(x)
{
    int *p = newvec(0);
    p[0] = x;
    return p;
}

int *list2(x, y)
    int *y;
{
    int *p = newvec(1);
    p[0] = x;
    p[1] = (int) y;
    return p;
}

int *list3(x, y, z)
    int *y, *z;
{
    int *p = newvec(2);
    p[0] = x;
    p[1] = (int) y;
    p[2] = (int) z;
    return p;
}

int *list4(x, y, z, t)
    int *y, *z, *t;
{
    int *p = newvec(3);
    p[0] = x;
    p[1] = (int) y;
    p[2] = (int) z;
    p[3] = (int) t;
    return p;
}

int *list5(x, y, z, t, u)
    int *y, *z, *t, *u;
{
    int *p = newvec(4);
    p[0] = x;
    p[1] = (int) y;
    p[2] = (int) z;
    p[3] = (int) t;
    p[4] = (int) u;
    return p;
}

int *list6(x, y, z, t, u, v)
    int *y, *z, *t, *u, *v;
{
    int *p = newvec(5);
    p[0] = x;
    p[1] = (int) y;
    p[2] = (int) z;
    p[3] = (int) t;
    p[4] = (int) u;
    p[5] = (int) v;
    return p;
}

int *rexplist()
{
    int *a = 0;
    int *ptr = (int*) &a;

    for (;;) {
        int *b = rexp(0);
        if (symb != S_COMMA) {
            *ptr = (int) b;
            return a;
        }
        nextsymb();
        *ptr = (int) list3(S_COMMA, b, 0);
        ptr = &H3[(int*) *ptr];
    }
}

int *rdseq()
{
    int *a = 0;

    ignore(S_SEMICOLON);
    a = rcom();
    if (symb == S_RSECT || symb == S_END)
        return a;
    return list3(S_SEQ, a, rdseq());
}

int *rbcom()
{
    int *a = 0;
    int *b = 0;
    int op = symb;

    switch (symb) {
    default:
        return 0;

    case S_NAME: case S_NUMBER: case S_STRING:
    case S_TRUE: case S_FALSE:  case S_LV:    case S_RV: case S_VECAP:
    case S_LPAREN:
        a = rexplist();
        if (symb == S_ASSIGN) {
            op = symb;
            nextsymb();
            return list3(op, a, rexplist());
        }
        if (symb == S_COLON) {
            if (H1[a] != S_NAME)
                caereport(50);
            nextsymb();
            return list4(S_COLON, a, rbcom(), 0);
        }
        if (H1[a] == S_FNAP) {
            H1[a] = S_RTAP;
            return a;
        }
        caereport(51);
        return a;

    case S_GOTO: case S_RESULTIS:
        nextsymb();
        return list2(op, rexp(0));

    case S_IF:    case S_UNLESS:
    case S_WHILE: case S_UNTIL:
        nextsymb();
        a = rexp(0);
        ignore(S_DO);
        return list3(op, a, rcom());

    case S_TEST:
        nextsymb();
        a = rexp(0);
        ignore(S_DO);
        b = rcom();
        checkfor(S_OR, 54);
        return list4(S_TEST, a, b, rcom());

    case S_FOR: {
        int *i = 0;
        int *j = 0;
        int *k = 0;
        nextsymb();
        a = rname();
        checkfor(S_EQ, 57);
        i = rexp(0);
        checkfor(S_TO, 58);
        j = rexp(0);
        if (symb == S_BY) {
            nextsymb();
            k = rexp(0);
        }
        ignore(S_DO);
        return list6(S_FOR, a, i, j, k, rcom());
    }
    case S_LOOP:
    case S_BREAK: case S_RETURN: case S_FINISH: case S_ENDCASE:
        a = wordnode;
        nextsymb();
        return a;

    case S_SWITCHON:
        nextsymb();
        a = rexp(0);
        checkfor(S_INTO, 60);
        return list3(S_SWITCHON, a, rdsect(rdseq));

    case S_CASE:
        nextsymb();
        a = rexp(0);
        checkfor(S_COLON, 61);
        return list3(S_CASE, a, rbcom());

    case S_DEFAULT:
        nextsymb();
        checkfor(S_COLON, 62);
        return list2(S_DEFAULT, rbcom());

    case S_LSECT:
        return rdsect(rdblockbody);
    }
}

int *rcom()
{
    int *a = rbcom();

    if (a == 0)
        caereport(51);

    while (symb == S_REPEAT || symb == S_REPEATWHILE || symb == S_REPEATUNTIL)
    {
        int op = symb;
        nextsymb();
        if (op == S_REPEAT)
            a = list2(op, a);
        else
            a = list3(op, a, rexp(0));
    }
    return a;
}

int *rbexp()
{
    int *a = 0;
    int op = symb;
    int i;

    switch (symb) {
    default:
        caereport(32);

    case S_QUERY:
        nextsymb();
        return list1(S_QUERY);

    case S_TRUE:
    case S_FALSE:
    case S_NAME:
        a = wordnode;
        nextsymb();
        return a;

    case S_STRING:
        a = newvec(wordsize+1);
        a[0] = S_STRING;
        for (i = 0; i <= wordsize; i++)
            a[i+1] = wordv[i];
        nextsymb();
        return a;

    case S_NUMBER:
        a = list2(S_NUMBER, (int*) decval);
        nextsymb();
        return a;

    case S_LPAREN:
        nextsymb();
        a = rexp(0);
        checkfor(S_RPAREN, 15);
        return a;

    case S_VALOF:
        nextsymb();
        return list2(S_VALOF, rcom());

    case S_VECAP:
        op = S_RV;
    case S_LV:
    case S_RV:
        nextsymb();
        return list2(op, rexp(35));

    case S_PLUS:
        nextsymb();
        return rexp(34);

    case S_MINUS:
        nextsymb();
        a = rexp(34);
        if (H1[a] == S_NUMBER)
            H2[a] = - H2[a];
        else
            a = list2(S_NEG, a);
        return a;

    case S_NOT:
        nextsymb();
        return list2(S_NOT, rexp(24));

    case S_TABLE:
        nextsymb();
        return list2(S_TABLE, rexplist());
    }
}

int *rexp(n)
{
    int *a = rbexp();
    int *b = 0;
    int *c = 0;
    int p = 0;
    int q = 0;
    int op;
L:
    op = symb;
    if (nlpending)
        return a;

    switch (op) {
    default:
        return a;

    case S_LPAREN:
        nextsymb();
        b = 0;
        if (symb != S_RPAREN)
            b = rexplist();
        checkfor(S_RPAREN, 19);
        a = list3(S_FNAP, a, b);
        goto L;

    case S_VECAP:
        p = 40;
        goto LASSOC;

    case S_REM: case S_MULT: case S_DIV:
        p = 35;
        goto LASSOC;

    case S_PLUS: case S_MINUS:
        p = 34;
        goto LASSOC;

    case S_EQ: case S_NE:
    case S_LE: case S_GE:
    case S_LS: case S_GR:
        if (n >= 30)
            return a;

        do {
            nextsymb();
            b = rexp(30);
            a = list3(op, a, b);
            if (c == 0)
                c = a;
            else
                c = list3(S_LOGAND, c, a);
            a = b;
            op = symb;
        } while (S_EQ <= op && op <= S_GE);

        a = c;
        goto L;

    case S_LSHIFT: case S_RSHIFT:
        p = 25;
        q = 30;
        goto DIADIC;

    case S_LOGAND:
        p = 23;
        goto LASSOC;

    case S_LOGOR:
        p = 22;
        goto LASSOC;

    case S_EQV: case S_NEQV:
        p = 21;
        goto LASSOC;

    case S_COND:
        if (n >= 13)
            return a;
        nextsymb();
        b = rexp(0);
        checkfor(S_COMMA, 30);
        a = list4(S_COND, a, b, rexp(0));
        goto L;
LASSOC:
        q = p;
DIADIC:
        if (n >= p)
            return a;
        nextsymb();
        a = list3(op, a, rexp(q));
        goto L;
    }
}

int *rdcdefs()
{
    int *a = 0;
    int *b = 0;
    int *ptr = (int*) &a;
    int *p = rec_p;
    int *l = rec_l;
    int jumpbuf [JUMPSZ];

    rec_p = jumpbuf;
    rec_l = &&recover;
    setjump(rec_p, recover);

    do {
        b = rname();
        if (symb == S_EQ || symb == S_COLON)
            nextsymb();
        else
            caereport(45);

        *ptr = (int) list4(S_CONSTDEF, 0, b, rexp(0));
        ptr = &H2[(int*) *ptr];
recover:
        ignore(S_SEMICOLON);
    } while (symb == S_NAME);

    rec_p = p;
    rec_l = l;
    return a;
}

int *rnamelist()
{
    int *a = rname();

    if (symb != S_COMMA)
        return a;
    nextsymb();
    return list3(S_COMMA, a, rnamelist());
}

int *rdef()
{
    int *n = rnamelist();

    switch (symb) {
    case S_LPAREN: {
        int *a = 0;
        nextsymb();
        if (H1[n] != S_NAME)
            caereport(40);
        if (symb == S_NAME)
            a = rnamelist();
        checkfor(S_RPAREN, 41);

        if (symb == S_BE) {
            nextsymb();
            return list5(S_RTDEF, n, a, rcom(), 0);
        }
        if (symb == S_EQ) {
            nextsymb();
            return list5(S_FNDEF, n, a, rexp(0), 0);
        }
        caereport(42);
    }
    default:
        caereport(44);
    case S_EQ:
        nextsymb();
        if (symb == S_VEC) {
            nextsymb();
            if (H1[n] != S_NAME)
                caereport(43);
            return list3(S_VECDEF, n, rexp(0));
        }
        return list3(S_VALDEF, n, rexplist());
    }
}

int *rdblockbody()
{
    int *p = rec_p;
    int *l = rec_l;
    int *a = 0;
    int jumpbuf [JUMPSZ];

    rec_p = jumpbuf;
    rec_l = &&recover;
    setjump(rec_p, recover);

    ignore(S_SEMICOLON);

    switch (symb) {
    case S_MANIFEST:
    case S_STATIC:
    case S_GLOBAL: {
        int op = symb;
        nextsymb();
        a = rdsect(rdcdefs);
        a = list3(op, a, rdblockbody());
        break;
    }
    case S_LET:
        nextsymb();
        a = rdef();
recover:
        while (symb == S_AND) {
            nextsymb();
            a = list3(S_AND, a, rdef());
        }
        a = list3(S_LET, a, rdblockbody());
        break;

    default:
        a = rdseq();
        if (symb != S_RSECT && symb != S_END)
            caereport(51);
        break;

    case S_RSECT: case S_END:
        break;
    }
    rec_p = p;
    rec_l = l;
    return a;
}

void d(s, item)
    void *s;
{
    unpackstring(s, charv);
    wordsize = packstring(charv, wordv);
    lookupword();
    wordnode[0] = item;
}

void declsyswords()
{
    d("AND", S_AND);

    d("BE", S_BE);
    d("BREAK", S_BREAK);
    d("BY", S_BY);

    d("CASE", S_CASE);

    d("DO", S_DO);
    d("DEFAULT", S_DEFAULT);

    d("EQ", S_EQ);
    d("EQV", S_EQV);
    d("ELSE", S_OR);
    d("ENDCASE", S_ENDCASE);

    d("FALSE", S_FALSE);
    d("FOR", S_FOR);
    d("FINISH", S_FINISH);

    d("GOTO", S_GOTO);
    d("GE", S_GE);
    d("GR", S_GR);
    d("GLOBAL", S_GLOBAL);
    d("GET", S_GET);

    d("IF", S_IF);
    d("INTO", S_INTO);

    d("LET", S_LET);
    d("LV", S_LV);
    d("LE", S_LE);
    d("LS", S_LS);
    d("LOGOR", S_LOGOR);
    d("LOGAND", S_LOGAND);
    d("LOOP", S_LOOP);
    d("LSHIFT", S_LSHIFT);

    d("MANIFEST", S_MANIFEST);

    d("NE", S_NE);
    d("NOT", S_NOT);
    d("NEQV", S_NEQV);

    d("OR", S_OR);

    d("RESULTIS", S_RESULTIS);
    d("RETURN", S_RETURN);
    d("REM", S_REM);
    d("RSHIFT", S_RSHIFT);
    d("RV", S_RV);
    d("REPEAT", S_REPEAT);
    d("REPEATWHILE", S_REPEATWHILE);
    d("REPEATUNTIL", S_REPEATUNTIL);

    d("SWITCHON", S_SWITCHON);
    d("STATIC", S_STATIC);

    d("TO", S_TO);
    d("TEST", S_TEST);
    d("TRUE", S_TRUE);
    d("THEN", S_DO);
    d("TABLE", S_TABLE);

    d("UNTIL", S_UNTIL);
    d("UNLESS", S_UNLESS);

    d("VEC", S_VEC);
    d("VALOF", S_VALOF);

    d("WHILE", S_WHILE);

    d("$", 0);
    nulltag = wordnode;
}

int *formtree()
{
    int getbuf [10];             // FOR 'GET' STREAMS
    int wordbuf [100];
    int charbuf [256];
    int namebuf [100];
    int jumpbuf [JUMPSZ];
    int i;
    int *a;

    chcount = 0;
    for (i = 0; i<63; i++)
        chbuf[i] = 0;

    getv = getbuf;
    getp = 0;

    wordv = wordbuf;

    charv = charbuf;
    charp = 0;

    nametable = namebuf;
    nametablesize = 100;
    for (i = 0; i<100; i++)
        nametable[i] = 0;

    rec_p = jumpbuf;
    rec_l = &&L;
    setjump(rec_p, L);

    linecount = 1;
    prline = 0;
    rch();

    if (ch == EOF)
        return 0;
    declsyswords();

L:  nextsymb();

    if (option[1]) {            // PP DEBUGGING OPTION
        printf("%u %s\n", symb, (char*) wordv);
        if (symb == S_END)
            return 0;
        goto L;
    }

    a = rdblockbody();
    if (symb != S_END) {
        caereport(99);
        goto L;
    }
    return a;
}

void comp(v, treemax)
    int *v;
{
    int b [64];
    int *a;

    chbuf = b;
    for (;;) {
        treep = v+treemax;
        treevec = v;

        a = formtree();
        if (a == 0)
            break;

        printf("\nTREE SIZE %u\n", treemax+treevec-treep);

        if (option[2]) {
            printf("AE TREE\n");
            plist(a, 0, 20);
            newline();
        }

        if (reportcount != 0)
            exit(8);

        if (! option[3])
            compileae(a);
    }
}

int main()
{
    int opt[20];
    int treesize = 5500;
    int i;

    printf("\nBCPL Translator\n");

    option = opt;
    savespacesize = 2;
    pptrace = FALSE;
    prsource = FALSE;
    for (i = 0; i < 20; i++)
        opt[i] = FALSE;

    sourcestream = fopen("OPTIONS", "r");
    if (sourcestream != 0) {
        int ch = 0;
        int n = 0;
        printf("OPTIONS  ");

        for (;;) {
            ch = getc(sourcestream);
L:          if (ch == '\n' || ch == EOF)
                break;
            putchar(ch);
            if (ch == 'P') n = 1;
            if (ch == 'T') n = 2;
            if (ch == 'C') n = 3;
            if (ch == 'M') n = 4;
            if (ch == 'N') n = 5;
            if (ch == 'S') prsource = TRUE;
            if (ch == 'E') pptrace = TRUE;
            if (ch == 'L') {
                treesize = readn();
                printf("%u", treesize);
                ch = terminator;
                goto L;
            }
            if (ch == '3')
                savespacesize = 3;
            option[n] = TRUE;
        }
        newline();
        fclose(sourcestream);
    }

    reportmax = 20;
    reportcount = 0;
    sourcestream = stdin;

    ocode = fopen("OCODE", "w");
    if (ocode == 0) {
        perror("OCODE");
        exit(8);
    }

    aptovec(comp, treesize);

    //if (option[4]) mapstore();

    printf("\nPHASE 1 COMPLETE\n");
    if (reportcount != 0) {
        exit(8);
    }
    return 0;
}
