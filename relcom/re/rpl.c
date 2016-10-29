/*
 *      Редактор RED.
 *
 *      $Header: rpl.c,v 4.20 90/05/22 23:22:41 alex Exp $
 *      $Log:   rpl.c,v $
 * Revision 4.20  90/05/22  23:22:41  alex
 * First rev. red 4.2
 * 
 * Revision 4.10  90/02/05  19:52:48  alex
 * Base revision 4.1
 * 
 * Revision 4.1  88/03/31  22:08:03  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 * 
 * Revision 3.1.2.1  87/06/22  16:20:14  alex
 * *** empty log message ***
 * 
 * Revision 3.4  86/09/19  20:01:37  alex
 * Версия для СМ-1700
 * 
 * Revision 3.3  86/08/04  20:56:06  alex
 * Bepqh dk LMNQ/DELNQ 2
 * 
 * Revision 3.2  86/07/24  19:13:11  alex
 * Об'единены версии текстов для ЕС и СМ
 * 
*/
#define printf2 printf
#
/*
 * rpl - copy standard input to standard output, replacing all occurrences
 *       of first argument by second argument.
 *
 */

#define CCHR    2
#define CDOT    4
#define CCL     6
#define NCCL    8
#define CDOL    10
#define CEOF    11

#define STAR    01

#define LBSIZE  256
#define ESIZE   256

char    ibuf[512];
char    expbuf[ESIZE];
int     lnum[2];
char    linebuf[LBSIZE+1];
char    obuf[LBSIZE+2],*obufp,*obufe;
char   *lastlp;
int     lrepl;
int     nfile;
int     circf;
int     blkno;
int     tln[2];

main(argc, argv)
char **argv;
{
        register char *c;

        obufp = obuf;
        obufe = obuf + LBSIZE + 2;

        if (argc != 3) printf2("arg\n",0),exit(-2);
        compile(argv[1]);

        c = argv[2];
        while (*c) c++;
        lrepl = c - argv[2];

        nfile = --argc;
        execute(argv[2]);
        exit(0);
}

compile(astr)
char *astr;
{
        register c;
        register char *ep, *sp;
        char *lastep;
        int cclcnt;

        ep = expbuf;
        sp = astr;
        if (*sp == '^') {
                circf++;
                sp++;
        }
        for (;;) {
                if (ep >= &expbuf[ESIZE])
                        goto cerror;
                if ((c = *sp++) != '*')
                        lastep = ep;
                switch (c) {

                case '\0':
                        *ep++ = CEOF;
                        return;

                case '@':
                        *ep++ = CDOT;
                        continue;

                case '*':
                        if (lastep==0)
                                goto defchar;
                        *lastep |= STAR;
                        continue;

                case '$':
                        if (*sp != '\0')
                                goto defchar;
                        *ep++ = CDOL;
                        continue;

                case '[':
                        *ep++ = CCL;
                        *ep++ = 0;
                        cclcnt = 1;
                        if ((c = *sp++) == '^') {
                                c = *sp++;
                                ep[-2] = NCCL;
                        }
                        do {
                                *ep++ = c;
                                cclcnt++;
                                if (c=='\0' || ep >= &expbuf[ESIZE])
                                        goto cerror;
                        } while ((c = *sp++) != ']');
                        lastep[1] = cclcnt;
                        continue;

                case '\\':
                        if ((c = *sp++) == '\0')
                                goto cerror;
                defchar:
                default:
                        *ep++ = CCHR;
                        *ep++ = c;
                }
        }
    cerror:
        printf2("RE error\n");
}

execute(repl)
char *repl;
{
        register char *p1, *p2;
        register c;
        char *ebp, *cbp, *op1;
        int nchars,f;

        f = 0;

        ebp = ibuf;
        cbp = ibuf;
        lnum[0] = 0;
        lnum[1] = 0;
        tln[0] = 0;
        tln[1] = 0;
        blkno = -1;
        for (;;) {
            /*  if ((++lnum[1])==0)
                        lnum[1]++;
                if((lnum[1]&0377) == 0)   */
                p1 = linebuf;
                p2 = cbp;
                for (;;) {
                        if (p2 >= ebp) {
                                if ((c = read(f, ibuf, 512)) <= 0) {
                                        close(f);
                                        return;
                                }
                                blkno++;
                                p2 = ibuf;
                                ebp = ibuf+c;
                        }
                        if ((c = *p2++) == '\n')
                                break;
                        if(c) {
                        if (p1 < &linebuf[LBSIZE-1])
                                *p1++ = c;
                          else printf2("line %s\n",linebuf),exit(-2);  /*   walt - avoidlosing text */
                                }
                }
                *p1++ = 0;
                nchars = p1 - linebuf;
                cbp = p2;
                op1 = p1 = linebuf;
                p2 = expbuf;
                if (circf) {
                        if (advance(p1, p2))
                                goto found;
                        goto nfound;
                }
              lookagain:
                /* fast check for first character */
                if (*p2==CCHR) {
                        c = p2[1];
                        do {
                                if (*p1!=c)
                                        continue;
                                if (advance(p1, p2))
                                        goto found;
                        } while (*p1++);
                        goto nfound;
                }
                /* regular algorithm */
                do {
                        if (advance(p1, p2))
                                goto found;
                } while (*p1++);
        nfound:
                puto(op1,nchars-1);
                dumpo();
                continue;
        found:
                /* match at p1 - write replacement out */
                if (op1 != p1) puto(op1,p1-op1);
                puto(repl,lrepl);
                nchars -= lastlp - op1;
                /* make sure not matching null string - this can't win. */
                if (circf == 0 && lastlp == op1) printf2("null\n",0),exit(-2);
                p1 = op1 = lastlp;
                if (circf || nchars <= 1) goto nfound;
                goto lookagain;
        }
}

puto(p,n)
char *p;
{       register char *pp;
        register int nn;

        nn = n;
        pp = p;

        while (--nn >= 0) {
                *obufp++ = *pp++;
                if (obufp >= obufe) printf2("obuf\n",0),exit(-2);
                }
        }

dumpo()
{       register char *p,*q;
        register int i;
char *r;
        p = obufp;
        obufp = obuf;

        while (--p >= obuf && *p == ' '); p++;   /* strip trailing blanks */
        *p++ = '\n';
        q = obuf;                               /* convert spaces to tabs */
        while (*q == ' ') q++;
        i = q - obuf;
        while (i & 7) {i--; q--; }
        while (i) {i -= 8; *--q = '\t'; }

        write(1,q,p-q);
        }

advance(alp, aep)
char *alp, *aep;
{
        register char *lp, *ep, *curlp;
        char *nextep;

        lp = alp;
        ep = aep;
        for (;;) switch (*ep++) {

        case CCHR:
                if (*ep++ == *lp++)
                        continue;
                return(0);

        case CDOT:
                if (*lp++)
                        continue;
                return(0);

        case CDOL:
                if (*lp==0)
                        continue;
                return(0);

        case CEOF:
                lastlp = lp;
                return(1);

        case CCL:
                if (cclass(ep, *lp++, 1)) {
                        ep += *ep;
                        continue;
                }
                return(0);

        case NCCL:
                if (cclass(ep, *lp++, 0)) {
                        ep += *ep;
                        continue;
                }
                return(0);

        case CDOT|STAR:
                curlp = lp;
                while (*lp++);
                goto star;

        case CCHR|STAR:
                curlp = lp;
                while (*lp++ == *ep);
                ep++;
                goto star;

        case CCL|STAR:
        case NCCL|STAR:
                curlp = lp;
                while (cclass(ep, *lp++, ep[-1]==(CCL|STAR)));
                ep += *ep;
                goto star;

        star:
                do {
                        lp--;
                        if (advance(lp, ep))
                                return(1);
                } while (lp > curlp);
                return(0);

        default:
                printf2("RE botch\n");
        }
}

cclass(aset, ac, af)
{
        register char *set, c;
        register n;

        set = aset;
        if ((c = ac) == 0)
                return(0);
        n = *set++;
        while (--n)
                if (*set++ == c)
                        return(af);
        return(!af);
}
/*
printf2(s, a)
{
        exit(-2);
}

*/
