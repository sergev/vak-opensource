#include "getpar.h"

#include <stdio.h>
#include <stdlib.h>

#include "trek.h"

/**
 **     get integer parameter
 **/

int getintpar(char *s)
{
    register int i;
    int n;

    while (1) {
        if (testnl() && s)
            printf("%s: ", s);
        i = scanf("%d", &n);
        if (i < 0)
            exit(1);
        if (i > 0 && testterm())
            return (n);
        printf("Плохое значение, здесь нужно целое\n");
        skiptonl(0);
    }
}

/**
 **     get floating parameter
 **/

FLOAT getfltpar(char *s)
{
    register int i;
    FLOAT d;

    while (1) {
        if (testnl() && s)
            printf("%s: ", s);
        i = scanf("%lf", &d);
        if (i < 0)
            exit(1);
        if (i > 0 && testterm())
            return (d);
        printf("Плохое значение, здесь нужно число\n");
        skiptonl(0);
    }
}

/**
 **     get yes/no parameter
 **/

struct cvntab Yntab[] = { { "y", "es", 1, 0, 0 },
                          { "д", "а", 1, 0, 0 },
                          { "n", "o", 0, 0, 0 },
                          { "н", "ет", 0, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

int getynpar(char *s)
{
    struct cvntab *r;

    r = getcodpar(s, Yntab);
    return ((int)r->value);
}

/**
 **     get coded parameter
 **/

struct cvntab *getcodpar(char *s, struct cvntab tab[])
{
    char input[60];
    register struct cvntab *r;
    int flag;
    register char *p, *q;
    int c;
    int f;

    flag = 0;
    while (1) {
        flag |= (f = testnl());
        if (flag)
            printf("%s: ", s);
        if (f)
            cgetc(); /* throw out the newline */
        if (scanf("%*[ \t;]") < 0)
            exit(1);
        if ((c = scanf("%[^ \t;\n]", input)) < 0)
            exit(1);
        if (c == 0)
            continue;
        flag = 1;

        /* if command list, print four per line */
        if (input[0] == '?' && input[1] == 0) {
            c = 4;
            for (r = tab; r->abrev; r++) {
                concat(r->abrev, r->full, input);
                printf("%-15.15s", input);
                if (--c > 0)
                    continue;
                c = 4;
                printf("\n");
            }
            if (c != 4)
                printf("\n");
            continue;
        }

        /* search for in table */
        for (r = tab; r->abrev; r++) {
            p = input;
            for (q = r->abrev; *q; q++)
                if (*p++ != *q)
                    break;
            if (!*q) {
                for (q = r->full; *p && *q; q++, p++)
                    if (*p != *q)
                        break;
                if (!*p || !*q)
                    break;
            }
        }

        /* check for not found */
        if (!r->abrev) {
            printf("Плохой ввод; напечатайте ?\n");
            skiptonl(0);
        } else
            return (r);
    }
}

/**
 **     get string parameter
 **/

void getstrpar(char *s, char *r, int l, char *t)
{
    register int i;
    char format[20];
    register int f;

    if (t == 0)
        t = " \t\n;";
    sprintf(format, "%%%d[^%s]", l, t);
    while (1) {
        if ((f = testnl()) != 0 && s != NULL)
            printf("%s: ", s);
        if (f)
            cgetc();
        if (scanf("%*[\t ;]") < 0)
            exit(1);
        i = scanf(format, r);
        if (i < 0)
            exit(1);
        if (i != 0)
            return;
    }
}

/**
 **     test if newline is next valid character
 **/

int testnl()
{
    register char c;

    while ((c = cgetc()) != '\n')
        if ((c >= '0' && c <= '9') || c == '.' || c == '!' || (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') || c == '-' || ((c & 0300) == 0300)) {
            ungetc(c, stdin);
            return (0);
        }
    ungetc(c, stdin);
    return (1);
}

/**
 **     scan for newline
 **/

void skiptonl(char c)
{
    while (c != '\n')
        if (!(c = cgetc()))
            return;
    ungetc('\n', stdin);
    return;
}

/**
 **     test for valid terminator
 **/

int testterm()
{
    register char c;

    if (!(c = cgetc()))
        return (1);
    if (c == '.')
        return (0);
    if (c == '\n' || c == ';')
        ungetc(c, stdin);
    return (1);
}

/*
**  TEST FOR SPECIFIED DELIMETER
**
**      The standard input is scanned for the parameter.  If found,
**      it is thrown away and non-zero is returned.  If not found,
**      zero is returned.
*/

int readdelim(char d)
{
    register char c;

    while ((c = cgetc()) != 0) {
        if (c == d)
            return (1);
        if (c == ' ')
            continue;
        ungetc(c, stdin);
        break;
    }
    return (0);
}
