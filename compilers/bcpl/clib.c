#include <stdio.h>
#include <stdlib.h>

extern void *G[];

#define STKSZ   (16*1024)       // Number of words reserved for BCPL stack.
#define FTSZ    20              // Max number of opened files.

/* Assign x = AX */
#define GET_A(x)   asm volatile("mov %%eax,%0" : "=r" (x))
#define SET_A(x)   asm volatile("" : : "a" (x))

static void *stack [STKSZ];     // BCPL stack area.
static FILE *ft [FTSZ];
static int fi, fo;

void finish()
{
    exit(0);
}

int main(argc, argv, envv)
    int argc;
    char **argv;
    char **envv;
{
    stack[0] = stack;
    stack[1] = finish;
    stack[2] = (void*) argc;
    stack[3] = argv;
    stack[4] = envv;

    ft[0] = stdin;
    ft[1] = stdout;
    ft[2] = stderr;
    fi = 0;
    fo = 1;

    asm volatile (
    "   movl    %0, %%ebp \n"
    "   movl    %1, %%edi \n"
    "	movl	4(%%edi), %%eax \n"
    "	jmp	*%%eax \n"
    : : "r" (stack), "r" (G) : "ax");

    return 0;
}

void stop()
{
    int status;

    GET_A(status);
    exit(status);
}

static unsigned char
getbyte(s, i)
    unsigned *s;
    int i;
{
    unsigned shift = ((i & 3) ^ 3) << 3;
    s += i >> 2;
    return *s >> shift;
}

#if 0
static void
putbyte(s, i, ch)
    unsigned *s;
    unsigned char ch;
{
    unsigned shift = ((i & 3) ^ 3) << 3;
    s += i >> 2;
    unsigned w = *s & ~(0xff << shift);
    *s = w | (ch << shift);
}
#endif

static void
get_cstr(s, buf)
    unsigned *s;
    char *buf;
{
    int n, i;

    n = getbyte(s, 0);
    for (i = 1; i <= n; i++)
        *buf++ = getbyte(s, i);
    *buf = 0;
}

static int
ftslot()
{
    int i;

    for (i = 3; i < FTSZ; i++)
        if (ft[i] == NULL)
            return i;
    return -1;
}

void
findinput(s)
{
    char filename[256];
    int x;

//  GET_A(s);
    get_cstr((unsigned*) s, filename);
    x = ftslot();
    if (x != -1) {
        ft[x] = fopen(filename, "r");
        if (! ft[x]) {
            SET_A(0);
            return;
        }
    }
    SET_A(x + 1);
}

void
findoutput()
{
    char filename[256];
    int s, x;

    GET_A(s);
    get_cstr((unsigned*) s, filename);
    x = ftslot();
    if (x != -1) {
        ft[x] = fopen(filename, "w");
        if (! ft[x]) {
            SET_A(0);
            return;
        }
    }
    SET_A(x + 1);
}

void
selectinput()
{
    int x;

    GET_A(x);
    fi = x - 1;
}

void
selectoutput()
{
    int x;

    GET_A(x);
    fo = x - 1;
}

void
input()
{
    SET_A(fi + 1);
}

void
output()
{
    SET_A(fo + 1);
}

void
rdch()
{
    SET_A(fgetc(ft[fi]));
}

void
wrch()
{
    int c;

    GET_A(c);
    fputc(c, ft[fo]);
}

void
unrdch()
{
    int c;

    GET_A(c);
    ungetc(c, ft[fo]);
}

void
endread()
{
    if (fi > 2) {
        fclose(ft[fi]);
        ft[fi] = NULL;
    }
    fi = 0;
}

void
endwrite()
{
    if (fo > 2) {
        fclose(ft[fo]);
        ft[fo] = NULL;
    } else
        fflush(ft[fo]);
    fo = 1;
}
