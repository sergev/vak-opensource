// #define DEBUG

#include "extern.h"
#include "screen.h"
#include "termcap.h"

struct {
    const char n0, n1, *val;
} const static ly_linux_console[] = {
#include "captab.inc"
    { 0, 0, 0 }
};

int ScreenImp::InitCap(char *bp)
{
    return 1;
}

void ScreenImp::GetCap(struct Captab *t)
{
    struct Captab *p;
    int i;

    for (p = t; p->tname[0]; ++p) {
        for (i = 0; ly_linux_console[i].n0; i++) {
            if (p->tname[0] == ly_linux_console[i].n0 &&
                p->tname[1] == ly_linux_console[i].n1) {
                switch (p->ttype) {
                case CAPNUM:
                    *(p->ti) = (long)ly_linux_console[i].val;
                    break;
                case CAPFLG:
                    *(p->tc) = (long)ly_linux_console[i].val;
                    break;
                case CAPSTR:
                    *(p->ts) = ly_linux_console[i].val;
                    break;
                }
                break;
            }
        }
    }

#ifdef DEBUG
    for (p = t; p->tname[0]; ++p) {
        printf("%c%c", p->tname[0], p->tname[1]);
        switch (p->ttype) {
        case CAPNUM:
            printf("#%d\n", *(p->ti));
            break;
        case CAPFLG:
            printf(" %s\n", *(p->tc) ? "on" : "off");
            break;
        case CAPSTR:
            if (*(p->ts))
                printf("='%s'\n", *(p->ts));
            else
                printf("=NULL\n");
            break;
        }
    }
#endif
}
