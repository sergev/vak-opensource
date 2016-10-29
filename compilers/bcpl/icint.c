/* Copyright (c) 2004 Robert Nordier.  All rights reserved. */

#include <stdio.h>
#include "blib.h"

#define VSIZE       32000
#define MGLOB       1
#define MPROG       402

#define FALSE       0
#define TRUE        1

#define FSHIFT      13
#define IBIT        010000
#define PBIT        04000
#define GBIT        02000
#define DBIT        01000
#define ABITS       0777
#define WORDSIZE    32
#define BYTESIZE    8

#define LIG1        0012001
#define K2          0140002
#define X22         0160026

int *M;
FILE *fp;

static int G;
static int P;
static int Ch;
static int Cyclecount;
static int *Labv;
static int Cp;
static int A;
static int B;
static int C;
static int D;
static int W;

static void
rch()
{
    for (;;) {
        Ch = fgetc(fp);
        if (Ch != '/')
            return;
        do {
            Ch = fgetc(fp);
        } while (Ch != '\n');
    }
}

static void
setlab(n)
{
    int k = Labv[n];
    if (k < 0)
        printf("L%d ALREADY SET TO %d AT P = %d\n", n, -k, P);
    while (k > 0) {
        int n = M[k];
        M[k] = P;
        k = n;
    }
    Labv[n] = -P;
}

static void
labref(n, a)
{
    int k = Labv[n];
    if (k < 0)
        k = -k;
    else
        Labv[n] = a;
    M[a] += k;
}

static int
rdn()
{
    int a = 0, b = FALSE;
    if (Ch == '-') { b = TRUE; rch(); }
    while ('0' <= Ch && Ch <= '9') { a = 10 * a + Ch - '0'; rch(); }
    if (b) a = -a;
    return a;
}

static void
stw(w)
{
    M[P++] = w;
    Cp = 0;
}

static void
stc(c)
{
    if (Cp == 0) {
        stw(0);
        Cp = WORDSIZE;
    }
    Cp -= BYTESIZE;
    M[P - 1] += c << Cp;
}

static void
assemble()
{
    int v[501];
    int f = 0;
    int i;

    Labv = v;
clear:
    for (i = 0; i <= 500; i++) Labv[i] = 0;
    Cp = 0;
next:
    rch();
sw:
    switch (Ch) {

    default: if (Ch == EOF) return;
        printf("\nBAD CH %c AT P = %d\n", Ch, P);
        goto next;

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        setlab(rdn());
        Cp = 0;
        goto sw;

    case '$': case ' ': case '\n': goto next;

    case 'L': f = 0; break;
    case 'S': f = 1; break;
    case 'A': f = 2; break;
    case 'J': f = 3; break;
    case 'T': f = 4; break;
    case 'F': f = 5; break;
    case 'K': f = 6; break;
    case 'X': f = 7; break;

    case 'C': rch(); stc(rdn()); goto sw;

    case 'D': rch();
        if (Ch == 'L') {
            rch();
            stw(0);
            labref(rdn(), P - 1);
        } else
            stw(rdn());
        goto sw;

    case 'G': rch();
        A = rdn() + G;
        if (Ch == 'L') rch();
        else printf("\nBAD CODE AT P = %d\n", P);
        M[A] = 0;
        labref(rdn(), A);
        goto sw;
    case 'Z': for (i = 0; i <= 500; i++)
        if (Labv[i] > 0) printf("L%d UNSET\n", i);
        goto clear;
    }
    W = f << FSHIFT;
    rch();
    if (Ch == 'I') { W = W + IBIT; rch(); }
    if (Ch == 'P') { W = W + PBIT; rch(); }
    if (Ch == 'G') { W = W + GBIT; rch(); }

    if (Ch == 'L') {
        rch();
        stw(W + DBIT);
        stw(0);
        labref(rdn(), P - 1);
    } else {
        int a = rdn();
        if ((a & ABITS) == a)
            stw(W + a);
        else { stw(W + DBIT); stw(a); }
    }
    goto sw;
}

static int
interpret()
{
fetch:
    Cyclecount++;
    W = M[C++];
    if ((W & DBIT) == 0)
        D = W & ABITS;
    else
        D = M[C++];

    if ((W & PBIT) != 0) D += P;
    if ((W & GBIT) != 0) D += G;
    if ((W & IBIT) != 0) D = M[D];

    switch (W >> FSHIFT) {
    error: default: printf("\nINTCODE ERROR AT C = %d\n", C - 1);
        return -1;
    case 0: B = A; A = D; goto fetch;
    case 1: M[D] = A; goto fetch;
    case 2: A = A + D; goto fetch;
    case 3: C = D; goto fetch;
    case 4: A = !A;
    case 5: if (!A) C = D; goto fetch;
    case 6: D += P;
        M[D] = P; M[D + 1] = C;
        P = D; C = A;
        goto fetch;
    case 7: switch (D) {
        default: goto error;
        case 1: A = M[A]; goto fetch;
        case 2: A = -A; goto fetch;
        case 3: A = ~A; goto fetch;
        case 4: C = M[P + 1];
            P = M[P];
            goto fetch;
        case 5: A = B * A; goto fetch;
        case 6: A = B / A; goto fetch;
        case 7: A = B % A; goto fetch;
        case 8: A = B + A; goto fetch;
        case 9: A = B - A; goto fetch;
        case 10: A = B == A ? ~0 : 0; goto fetch;
        case 11: A = B != A ? ~0 : 0; goto fetch;
        case 12: A = B < A  ? ~0 : 0; goto fetch;
        case 13: A = B >= A ? ~0 : 0; goto fetch;
        case 14: A = B > A ? ~0 : 0; goto fetch;
        case 15: A = B <= A ? ~0 : 0; goto fetch;
        case 16: A = B << A; goto fetch;
        case 17: A = B >> A; goto fetch;
        case 18: A = B & A; goto fetch;
        case 19: A = B | A; goto fetch;
        case 20: A = B ^ A; goto fetch;
        case 21: A = B ^ ~A; goto fetch;
        case 22: return 0;
        case 23: B = M[C]; D = M[C + 1];
            while (B != 0) {
                B--; C += 2;
                if (A == M[C]) { D = M[C + 1]; break; }
            }
            C = D;
            goto fetch;

        case 24: selectinput(A); goto fetch;
        case 25: selectoutput(A); goto fetch;
        case 26: A = rdch(); goto fetch;
        case 27: wrch(A); goto fetch;
        case 28: A = findinput(A); goto fetch;
        case 29: A = findoutput(A); goto fetch;
        case 30: return A;
        case 31: A = M[P]; goto fetch;
        case 32: P = A; C = B; goto fetch;
        case 33: endread(); goto fetch;
        case 34: endwrite(); goto fetch;
        case 35: D = P + B + 1;
                 M[D] = M[P];
                 M[D + 1] = M[P + 1];
                 M[D + 2] = P;
                 M[D + 3] = B;
                 P = D;
                 C = A;
                 goto fetch;
        case 36: A = getbyte(A, B); goto fetch;
        case 37: putbyte(A, B, M[P + 4]); goto fetch;
        case 38: A = input(); goto fetch;
        case 39: A = output(); goto fetch;
        }
    }
}

int
main(argc, argv)
    char **argv;
{
    int pgvec[VSIZE];

    if (argc != 2) {
        fprintf(stderr, "usage: icint file\n");
        return 1;
    }
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "%s: Can't open\n", argv[1]);
        return 0;
    }
    M = pgvec;
    G = MGLOB;
    P = MPROG;
    M[P++] = LIG1;
    M[P++] = K2;
    M[P++] = X22;
    initio();
    //printf("INTCODE SYSTEM ENTERED\n");
    assemble();
    fclose(fp);
    printf("INTCODE SIZE = %d\n", P - MPROG);
    C = MPROG;
    Cyclecount = 0;
    A = interpret();
    printf("EXECUTION CYCLES = %d, STATUS = %d\n", Cyclecount, A);
    return A;
}
