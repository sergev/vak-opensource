/*
 *      Редактор RED. ИАЭ им. И.В. Курчатова, ОС ДЕМОС
 *      Мини / вариант termcap.
 *      $Header: /home/sergev/Project/vak-opensource/trunk/relcom/nred/RCS/r.termc.c, 3.1 1986/04/20 23:42:58 alex Exp $
 *      $Log: r.termc.c, $
 *      Revision 3.1  1986/04/20 23:42:58  alex
 *      *** empty log message ***
 *
 */
#define MAXHOP  32  /* max number of tc= indirections */
#define BUFSIZ  1024

#include <string.h>
#include <sgtty.h>
#include <ctype.h>
#define E_TERMCAP "/etc/termcap"
/*
 * Программы для взаимодействия с описанием  терминала "/etc/termcap"
 *
 */

static char *tbuf;
static int hopcount;   /* detect infinite loops in termcap, init 0 */
static char    *tskip();
static char    *tdecode();
char    *tgetstr();
char    *getenv();
/* В случае ОС ДЕМОС используется мини-вариант tgetent,так как
 * в  этом случае описание находится в переменной TERMCAP
 * ko= 1 - нормально, 0  - TERMCAP не найден
*/
#ifdef FROMTCAP
tgettc()
{
    tbuf=getenv("TERMCAP");
    return((tbuf?1:0));
}
#else FROMTCAP
/*
 * Получает описание терминала и помещает его в буфер bp
 * name - тип терминала.
 */
tgetent(bp, name)
char *bp, *name;
{
    register char *cp;
    register int c;
    register int i = 0, cnt = 0;
    char ibuf[BUFSIZ];
    char *cp2;
    int tf;
    tbuf = bp;
    tf = 0;
#ifndef V6
    cp = getenv("TERMCAP");
    /*
         * TERMCAP can have one of two things in it. It can be the
         * name of a file to use instead of /etc/termcap. In this
         * case it better start with a "/". Or it can be an entry to
         * use so we don't have to read the file. In this case it
         * has to already have the newlines crunched out.
         */
    if (cp && *cp) {
        if (*cp!='/') {
            cp2 = getenv("TERM");
            if (cp2==(char *) 0 || strcmp(name,cp2)==0) {
                strcpy(bp,cp);
                return(tnchktc());
            }
            else {
                tf = open(E_TERMCAP, 0);
            }
        }
        else
            tf = open(cp, 0);
    }
    if (tf==0)
        tf = open(E_TERMCAP, 0);
#else
    tf = open(E_TERMCAP, 0);
#endif
    if (tf < 0)
        return (-1);
    for (;;) {
        cp = bp;
        for (;;) {
            if (i == cnt) {
                cnt = read(tf, ibuf, BUFSIZ);
                if (cnt <= 0) {
                    close(tf);
                    return (0);
                }
                i = 0;
            }
            c = ibuf[i++];
            if (c == '\n') {
                if (cp > bp && cp[-1] == '\\'){
                    cp--;
                    continue;
                }
                break;
            }
            if (cp >= bp+BUFSIZ) {
                write(2,"Termcap entry too long\n", 23);
                break;
            }
            else
                *cp++ = c;
        }
        *cp = 0;
        /*
                 * The real work for the match.
                 */
        if (tnamatch(name)) {
            close(tf);
            return(tnchktc());
        }
    }
}

/*
 * tnchktc: check the last entry, see if it's tc=xxx. If so,
 * recursively find xxx and append that entry (minus the names)
 * to take the place of the tc=xxx entry. This allows termcap
 * entries to say "like an HP2621 but doesn't turn on the labels".
 * Note that this works because of the left to right scan.
 */
tnchktc()
{
    register char *p, *q;
    char tcname[16];    /* name of similar terminal */
    char tcbuf[BUFSIZ];
    char *holdtbuf = tbuf;
    int l;
    p = tbuf + strlen(tbuf) - 2;    /* before the last colon */
    while (*--p != ':')
        if (p<tbuf) {
            write(2, "Bad termcap entry\n", 18);
            return (0);
        }
    p++;
    /* p now points to beginning of last field */
    if (p[0] != 't' || p[1] != 'c')
        return(1);
    strcpy(tcname,p+3);
    q = tcname;
    while (q && *q != ':')
        q++;
    *q = 0;
    if (++hopcount > MAXHOP) {
        write(2, "Infinite tc= loop\n", 18);
        return (0);
    }
    if (tgetent(tcbuf, tcname) != 1)
        return(0);
    for (q=tcbuf; *q != ':'; q++)
        ;
    l = p - holdtbuf + strlen(q);
    if (l > BUFSIZ) {
        write(2, "Termcap entry too long\n", 23);
        q[BUFSIZ - (p-tbuf)] = 0;
    }
    strcpy(p, q+1);
    tbuf = holdtbuf;
    return(1);
}

/*
 * Tnamatch deals with name matching.  The first field of the termcap
 * entry is a sequence of names separated by |'s, so we compare
 * against each such name.  The normal : terminator after the last
 * name (before the first field) stops us.
 */
tnamatch(np)
char *np;
{
    register char *Np, *Bp;
    Bp = tbuf;
    if (*Bp == '#')
        return(0);
    for (;;) {
        for (Np = np; *Np && *Bp == *Np; Bp++, Np++)
            continue;
        if (*Np == 0 && (*Bp == '|' || *Bp == ':' || *Bp == 0))
            return (1);
        while (*Bp && *Bp != ':' && *Bp != '|')
            Bp++;
        if (*Bp == 0 || *Bp == ':')
            return (0);
        Bp++;
    }
}
#endif FROMTCAP

/*
 * Skip to the next field.  Notice that this is very dumb, not
 * knowing about \: escapes or any such.  If necessary, :'s can be put
 * into the termcap file in octal.
 */
static char *
tskip(bp)
register char *bp;
{
    while (*bp && *bp != ':')
        bp++;
    if (*bp == ':')
        bp++;
    return (bp);
}

/*
 * Return the (numeric) option id.
 * Numeric options look like
 *  li#80
 * i.e. the option string is separated from the numeric value by
 * a # character.  If the option is not found we return -1.
 * Note that we handle octal numbers beginning with 0.
 */
tgetnum(id)
char *id;
{
    register int i, base;
    register char *bp = tbuf;
    for (;;) {
        bp = tskip(bp);
        if (*bp == 0)
            return (-1);
        if (*bp++ != id[0] || *bp == 0 || *bp++ != id[1])
            continue;
        if (*bp == '@')
            return(-1);
        if (*bp != '#')
            continue;
        bp++;
        base = 10;
        if (*bp == '0')
            base = 8;
        i = 0;
        while (isdigit(*bp))
            i *= base, i += *bp++ - '0';
        return (i);
    }
}

/*
 * Handle a flag option.
 * Flag options are given "naked", i.e. followed by a : or the end
 * of the buffer.  Return 1 if we find the option, or 0 if it is
 * not given.
 */
tgetflag(id)
char *id;
{
    register char *bp = tbuf;
    for (;;) {
        bp = tskip(bp);
        if (!*bp)
            return (0);
        if (*bp++ == id[0] && *bp != 0 && *bp++ == id[1]) {
            if (!*bp || *bp == ':')
                return (1);
            else if (*bp == '@')
                return(0);
        }
    }
}

/*
 * Get a string valued option.
 * These are given as
 *  cl=^Z
 * Much decoding is done on the strings, and the strings are
 * placed in area, which is a ref parameter which is updated.
 * No checking on area overflow.
 */
char *
tgetstr(id, area)
char *id, **area;
{
    register char *bp = tbuf;
    for (;;) {
        bp = tskip(bp);
        if (!*bp)
            return (0);
        if (*bp++ != id[0] || *bp == 0 || *bp++ != id[1])
            continue;
        if (*bp == '@')
            return(0);
        if (*bp != '=')
            continue;
        bp++;
        return (tdecode(bp, area));
    }
}

/*
 * Tdecode does the grung work to decode the
 * string capability escapes.
 */
static char *
tdecode(str, area)
register char *str;
char **area;
{
    register char *cp;
    register int c;
    register char *dp;
    int i,jdelay=0;
    while(*str>='0' && *str<='9') {
        jdelay=jdelay*10+(*str++ - '0');
    }
    cp = *area;
    while ((c = *str++) && c != ':') {
        switch (c) {
        case '^':
            c = *str++ & 037;
            break;
        case '\\':
            dp = "E\033^^\\\\::n\nr\rt\tb\bf\f";
            c = *str++;
nextc:
            if (*dp++ == c) {
                c = *dp++;
                break;
            }
            dp++;
            if (*dp)
                goto nextc;
            if (isdigit(c)) {
                c -= '0', i = 2;
                do
                    c <<= 3, c |= *str++ - '0';
                while (--i && isdigit(*str));
            }
            break;
        }
        *cp++ = c;
    }
#if 0
    if ( jdelay )
    { extern ospeed;
      register int i = ospeed;
      while ( i < B9600 )
        jdelay /= 2, i++;
        jdelay++;
    }
#endif
    if(jdelay>100) jdelay=100;
    while(jdelay--) *cp++='\200';
    *cp++ = 0;
    str = *area;
    *area = cp;
    return (str);
}
#define CTL(c) ('c' & 037)

char    *UP;
char    *BC;
#define UPTC UP
#define BCTC BC

/*
 * Routine to perform cursor addressing.
 * CM is a string containing printf type escapes to allow
 * cursor addressing.  We start out ready to print the destination
 * line, and switch each time we print row or column.
 * The following escapes are defined for substituting row/column:
 *
 *  %d  as in printf
 *  %2  like %2d
 *  %3  like %3d
 *  %.  gives %c hacking special case characters
 *  %+x like %c but adding x first
 *
 *  The codes below affect the state but don't use up a value.
 *
 *  %>xy    if value > x add y
 *  %r  reverses row/column
 *  %i  increments row/column (for one origin indexing)
 *  %%  gives %
 *  %B  BCTCD (2 decimal digits encoded in one byte)
 *  %D  Delta Data (backwards bcd)
 *
 * all other characters are ``self-inserting''.
 */
char *
tgoto(CM, destcol, destline)
char *CM;
int destcol, destline;
{
    static char result[16];
    static char added[10];
    char *cp = CM;
    register char *dp = result;
    register int c;
    int oncol = 0;
    register int which = destline;
    if (cp == 0) {
toohard:
        /*
                 * ``We don't do that under BOZO's big top''
                 */
        return ("OOPS");
    }
    added[0] = 0;
    while (c = *cp++) {
        if (c != '%') {
            *dp++ = c;
            continue;
        }
        switch (c = *cp++) {
#ifdef CM_N
        case 'n':
            destcol ^= 0140;
            destline ^= 0140;
            goto setwhich;
#endif
        case 'd':
            if (which < 10)
                goto one;
            if (which < 100)
                goto two;
            /* fall into... */
        case '3':
            *dp++ = (which / 100) | '0';
            which %= 100;
            /* fall into... */
        case '2':
two:
            *dp++ = which / 10 | '0';
one:
            *dp++ = which % 10 | '0';
swap:
            oncol = 1 - oncol;
setwhich:
            which = oncol ? destcol : destline;
            continue;
#ifdef CM_GT
        case '>':
            if (which > *cp++)
                which += *cp++;
            else
                cp++;
            continue;
#endif
        case '+':
            which += *cp++;
            /* fall into... */

        case '.':
#ifndef lint
casedot:
#endif
            /*
             * This code is worth scratching your head at for a
             * while.  The idea is that various weird things can
             * happen to nulls, EOT's, tabs, and newlines by the
             * tty driver, arpanet, and so on, so we don't send
             * them if we can help it.
             *
 * Tab is taken out to get Ann Arbors to work, otherwise
             * when they go to column 9 we increment which is wrong
             * because bcd isn't continuous.  We should take out
             * the rest too, or run the thing through more than
             * once until it doesn't make any of these, but that
             * would make termlib (and hence pdp-11 ex) bigger,
             * and also somewhat slower.  This requires all
             * programs which use termlib to stty tabs so they
             * don't get expanded.  They should do this anyway
             * because some terminals use ^I for other things,
             * like nondestructive space.
             */
            if (UPTC && (which == 0 || which == CTL(d) || which==CTL(n) || which == '\t' || which == '\n')) {
                if (oncol || UPTC) /* Assumption: backspace works */
                    /*
                                         * Loop needed because newline happens
                                         * to be the successor of tab.
                                         */
                do {
                    strcat(added, oncol ? (BCTC ? BCTC : "\b") :
                    UPTC);
                    which++;
                }
                while (which == '\n');
            }
            *dp++ = which;
            goto swap;
        case 'r':
            oncol = 1;
            goto setwhich;
        case 'i':
            destcol++;
            destline++;
            which++;
            continue;
        case '%':
            *dp++ = c;
            continue;
#ifdef CM_B
        case 'B':
            which = (which/10 << 4) + which%10;
            continue;
#endif
#ifdef CM_D
        case 'D':
            which = which - 2 * (which%16);
            continue;
#endif

        default:
            goto toohard;
        }
    }
    strcpy(dp, added);
    return (result);
}
