/*
 *      Редактор RED.
 *      r.gettc.c - вытаскивание описания терминала из termcap
 *               и работа с таблицей команд.
 *      $Header: S.gettc.c,v 4.1 88/03/31 22:07:35 alex Exp $
 *      $Log:   S.gettc.c,v $
 * Revision 4.1  88/03/31  22:07:35  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 * 
 * Revision 3.1.2.5  87/07/09  20:45:01  alex
 * REVISION
 * 
 * Revision 3.1.2.4  87/07/03  22:04:33  alex
 * Graph_characters used in margin
 * 
 * Revision 3.1.2.3  87/06/29  17:58:35  alex
 * Commans set to 0200-0277
 * 
 * Revision 3.1.2.2  87/06/24  22:37:51  alex
 * New readch + gettc + tc table Tested
 * 
 * Revision 3.1.2.1  87/06/19  16:56:03  alex
 * Start revision for red/4
 * 
 * Revision 3.12  87/06/12  18:06:28  alex
 * Исправлены ошибки в режиме vilcase и мелочи для vt-200
 * 
 * Revision 3.11  87/06/09  19:45:10  alex
 * Исправлена работа с tty ~ VT200 и 15-ИЭ-0013
 * 
 * Revision 3.10  87/06/06  16:47:39  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.9  87/06/05  23:48:02  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.8  87/06/04  23:41:36  alex
 * Scroll on -cs- or -al/dl- added
 * 
 * Revision 3.7  87/04/02  18:31:14  alex
 * Версия для Utec слита с версией с выделением прописных букв
 * 
 * Revision 3.6  87/04/01  19:14:49  alex
 * Revision for Utec: Cs, Ce, Ct, hR
 * 
 * Revision 3.5  86/10/29  02:45:41  root
 * Выход 4 * ESC вместо 2 * ESC из за пропадающих символов.
 * 
 * Revision 3.4  86/09/19  20:01:20  alex
 * Версия для СМ-1700
 * 
 * Revision 3.3  86/08/04  20:55:51  alex
 * Версия Демос/Мнос-2
 * 
 * Revision 3.2  86/07/24  19:08:52  alex
 * Об'единены версии текстов для ЕС и СМ
 * 
 * Revision 3.1  86/04/20  23:41:41  alex
 * Базовая версия для ЕС.
 * 
 * Revision 3.1  86/04/20  23:41:41  alex
 * *** empty log message ***
 * 
 * Revision 1.4  86/04/13  21:56:14  alex
 */

extern char *UP, *BC;
char **toup= &UP;        /* Так сложно из-за совпадения имен */

#include "r.defs.h"
#define TELE_INIT
#include "S.tele.h"

# define INCTABSZ 100
# define ZZ(a,b) ('a'&0377) << 8 | 'b'&0377

/*
 * Кодирование кодов в строке определений:
 * ^C - Контрол-символ C&037
 * =C - Символ С
 * xx - функциональная возможность из termcap с кодом xx
 * ?xx - то же, но если xx нет, то и не надо;
 * :yyyyy:zzzzz:... - одна из строк yyyyy, zzzzzz
 * (выбирается та, которая определена в termcap)
 */

/*
 * Выходные коды
 */
char *cvtout[] = {
/* COSTART: */ "cl?is?ti?ks?ho",          /* COUP:       */"up",
/* CODN   : */ "do",              /* CORN:       */"\015",
/* COHO   : */ "ho",              /* CORT:       */"nd",
/* COLT   : */ "bc",              /* COCURS:     */"cu",
/* COBELL : */ "\007",           /* COFIN:      */"?Ge?me?Ce?cl?fs?te?ke",
/* COERASE: */ "cl"  ,           /* COOPEN: */    "?so",
/* COCLOSE  */ "?se",
/* COVIOPE  */ "vs",   /* COVICLO  */ "ve",
/* COCYON   */ "Cs",   /* COCYOFF  */ "Ce",
/* COILINE  */ "al",   /* CODELIN  */ "dl",
/* COSRFWD  */ "sf",   /* COSRBAK  */ "sr",
/* COCLLIN  */ "ce",   /* COCLSCR  */ "cd",
/* COANORMAL  */ "?me?se",
/* COAINFO    */ ":m1:so",
/* COAERROR   */ ":m2:?mbso",
/* COAMARG    */ ":m3:md:so",
/* COAOUT     */ ":m4:mr:so",
/* COGSTART   */ "gs",
/* COGEND     */ "ge",
   (char *)0
};

char *curspos;
short slowsw=1;

/*   Входные коды
 *
 */

/*  Таблица кодов клавиш терминала и их комбинаций */
/*  Сюда же записываются коды при переопределении  */
/* Состоит из трех частей:
 * 1. Движение курсора. Всегда можно ввести с клавиатуры.
 * 2. Функциональная клавитура и ее замена;
 * 3. Ввод через контрол-символы.
 */
char escch1 = '\012';
struct ex_int inctab[INCTABSZ] = {
/*
 * Движение курсора. Кодируем так, чтобы было всегда
 */
CC+RN,    "^M",
CC+RN,    "^J",
CC+LT,    ":kl:^L",
CC+RT,    ":kr:^R",
CC+UP,    ":ku:^U",
CC+DN,    ":kd:^D",
CC+HO,    ":kh:^H",
CC+BT,    "^B",
CC+TB,    "^I",
/*
 * Это нужно всегда
 */
 CCBACKSPACE    ,"^?",
 CCCTRLQUOTE    ,"^A",
 CCQUIT         ,"^[^?",
 CCENTER        ,"^[= ",
/*
 * Ввод команд через цифровую клавиатуру
 * а заодно и замена если ее нет
 */
CCCHPORT         ,":f0:^[=0",  CCDOCMD         ,":f2f0:^[^[=0",
CCENTER          ,":f1:^[=1",  CCENTER         ,":f2f1:^[^[=1",
CCPLSRCH         ,":f3:^[=3",  CCMISRCH        ,":f2f3:^[^[=3",
CCGOTO           ,":f4:^[=4",  CCMAKEPORT      ,":f2f4:^[^[=4",
CCINSMODE        ,":f5:^[=5",  CCTABS          ,":f2f5:^[^[=5",
CCDELCH          ,":f6:^[=6",  CCDELCH         ,":f2f6:^[^[=6",
CCPLPAGE         ,":f7:^[=7",  CCMIPAGE        ,":f2f7:^[^[=7",
CCOPEN           ,":f8:^[=8",  CCCLOSE         ,":f2f8:^[^[=8",
CCPICK           ,":f9:^[=9",  CCPUT           ,":f2f9:^[^[=9",
CC+BT            ,":f.:^[=.",  CCCTRLQUOTE      ,":f2f.:^[^[=.",
CCSETFILE        ,":f-:^[=-",  CCSAVEFILE      ,":f2f-:^[^[=-",
CCLPORT          ,":f2kl:^[kl",CCRPORT         ,":f2kr:^[kr",
/*
 * Ввод отдельных команд через упр. клавиши
 */
CC+BT           ,":kB:.f.",
CCPLPAGE        ,":kN:^Z",
CCMIPAGE        ,":kP:^X",
CCOPEN          ,":kA:k2",
CCCLOSE         ,":kL:k3",
CCENTER         ,":kE:k1",
CCDELCH         ,":kD:k5",
CCINSMODE       ,":kI:k4",
CCPLLINE        ,":kF:k6",
CCMILINE        ,":kR:k7",
/*
 * Ввод команд через  ^С:
 */
 CCPLLINE       ,"^T", /* Для совместимости */
 CCMILINE       ,"^W", /* Для совместимости */
 CCDELCH        ,"^U", /* Для совместимости */
 CCSETFILE      ,"^D", /* Для совместимости */
 CCPICK         ,"^F", /* Для совместимости */
 CCPUT          ,"^G", /* Для совместимости */
 CCDOCMD        ,"^P", /* Для совместимости */
 CCRESCREEN     ,"^L",
 CCPLSRCH     ,"^R",
 CCMISRCH     ,"^E",
 CCSAVEFILE   ,"^V",
 CCBACKSPACE  ,"^H",
 0,0,   0,0,   0,0,   0,0,   0,0,
 0,0,   0,0,   0,0,   0,0,   0,0,
 0,0,   0,0,   0,0,   0,0,   0,0,
 0, 0 };

mktab ()
{
        register char *p;
        register f;
        extern char *getenv (), *zAlloc ();
        register struct ex_int *tabptr;
        char buf [2048];
        int len;

        if (! (p = getenv ("REDKEYTAB"))) return;
        if ((f = open (p, 0)) == -1) return;
        if ((len = read (f, buf, 2048)) == -1) {
                close (f);
                return;
        }
        close (f);
        if (len<2048) buf[len] = 0; else buf[2047] = 0;
        p = buf;
        tabptr = inctab;
        for (;;) {
                register char *s, *q;
                register c;

                if (tabptr >= &inctab[INCTABSZ]) {
                        tabptr--;
                        break;
                }
                for (; *p; p++)
                        if (*p!=' ' && *p!='\t' && *p!='\n') break;
                if (! *p) break;
                for (s=p; *s && *s!='"'; s++);
                if (! *s) break;
                s++;
                for (q=s; *q && *q!='"'; q++);
                if (! *q) break;
                *q = 0;

                c = (p[0] & 0377) << 8;
                if (p[1] && p[1]!=' ' && p[1]!='\t' && p[1]!='"')
                        c |= p[1] & 0377;

                if ((tabptr->incc = makecc (c)) != -1 &&
                        (tabptr->excc = zAlloc (strlen (s) + 1)))
                {
                        strcpy (tabptr->excc, s);
                        tabptr++;
                }
                p = q+1;
        }
        tabptr->incc = 0;
        tabptr->excc = 0;
}

makecc (c)
register c;
{
        switch (c) {
/* ВВод / Return */
        case ZZ(R,t) :   return (CC+RN);     /* ReTurn       */
/* Команды движения курсора */
        case ZZ(C,d) :   return (CC+DN);     /* CursorDown   */
        case ZZ(C,h) :   return (CC+HO);     /* CursorHome   */
        case ZZ(C,l) :   return (CC+LT);     /* CursorLeft   */
        case ZZ(C,r) :   return (CC+RT);     /* CursorRight  */
        case ZZ(C,u) :   return (CC+UP);     /* CursorUp     */
                                             /* CursorBegin  */
                                             /* CursorEnd    */
/* Работа с табуляциями */
        case ZZ(T,r) :   return (CC+BT);     /* TabRight   */
        case ZZ(T,l) :   return (CC+TB);     /* TabLeft    */
        case ZZ(T,s) :   return (CCTABS);    /* TabSet     */
                                             /* TabClean   */
                                             /* TabInit    */
/* Работа по словам     */
                                             /* WordLeft   */
                                             /* WordRight  */
                                             /* WordDelete */
                                             /* WordGet    */
/* Команды движения по файлу */
        case ZZ(p,+) :   return (CCPLPAGE);  /* Page+    */
        case ZZ(l,+) :   return (CCPLLINE);  /* Line+    */
        case ZZ(p,r) :   return (CCRPORT);   /* PageRight*/
        case ZZ(p,-) :   return (CCMIPAGE);  /* Page-    */
        case ZZ(l,-) :   return (CCMILINE);  /* Line-    */
        case ZZ(p,l) :   return (CCLPORT);   /* PageLeft */
        case ZZ(g,o) :   return (CCGOTO);    /* GOto     */
/* Команды поиска по шаблону */
        case ZZ(s,+):   return (CCPLSRCH);   /* Search+  */
        case ZZ(s,-):   return (CCMISRCH);   /* Search-  */
/* Команды редактирования строки */
        case ZZ(D,c) :   return (CCDELCH);    /* DelChar */
        case ZZ(B,s) :   return (CCBACKSPACE);/* BackSpace */
        case ZZ(I,m) :   return (CCINSMODE);  /* InsertMode*/
        case ZZ(Q,c) :   return (CCCTRLQUOTE);/* QuoteChar */
/* Команды работы с окнами */
        case ZZ(W,c):   return (CCCHPORT);    /* WindChange */
        case ZZ(W,m):   return (CCMAKEPORT);  /* WindMake   */
/* Команды работы со строками и группами строк */
        case ZZ(L,i):   return (CCOPEN);      /* LineInsert */
        case ZZ(L,d):   return (CCCLOSE);     /* LineDelete */
        case ZZ(L,g):   return (CCPICK);      /* LineGet    */
        case ZZ(L,p):   return (CCPUT);       /* LinePut    */
                                              /* LineTruncate */
                                              /* LineSplit  */
                                              /* LineJoin   */
/* Команды работы с файлами */
        case ZZ(F,o):   return (CCSETFILE);   /* FileOpen   */
        case ZZ(F,s):   return (CCSAVEFILE);  /* FileSave   */
                                              /* FileSelect */
/* Разные команды */
        case ZZ(E,c):   return (CCDOCMD);     /* ExecCmd */
        case ZZ(E,n):   return (CCENTER);     /* ENter   */
        case ZZ(Q,u):   return (CCQUIT);      /* QUit    */
        case ZZ(R,s):   return (CCRESCREEN);  /* ReScreen */
                                              /* HElp     */
        }
        return (-1);
}

/* Декодирование termcap */
int nfinc=8;
char *tgetstr(), *tgoto();
char *salloc();
static char *stc;
static int ltc;
#define LTCM 400
int tcread();
int (*atcread)()= tcread;

/*
 * char *gettcs(tc) -
 * дай описание возможности "tc"
 * tc="XXYY..ZZ", (см. комментарий перед таблицей)
 */
char *gettcs(tc)
char *tc;
{
        char buftc0[LTCM], *buftc=buftc0;
        register char c1, *c;
        int i,fcase, casec=0;
        if ( (*tc&0340) == 0 || *tc==0177) return(tc);
        c=tc;
        while(*c)
        {
                if(*c=='?'){
                        fcase=1;
                        c++;
                }
                else fcase=0;
                if( *c == '^') {
                        *buftc++ = (*++c=='?' ? 0177 : (*c & 037));
                        c++;
                        continue;
                        }
                if( *c == '=') {
                        *buftc++ = *++c;
                        c++;
                        continue;
                        }
                if( *c == ':' ) {
                        if(casec) break;
                        casec = 1;
                        c++;
                        }
                c1=c[2];
                c[2]=0;
                i= (!tgetstr(c,&buftc));
                c += 2;
                *c=c1;
                if(i){
                        if(fcase) continue;
                        if (!casec) return(0);
                        while (*c && *c != ':') if( *c++ == '=') c++;
                        if( *c != ':') return((char *)0);
                        c++; buftc = buftc0;
                        continue;
                }
                buftc--;
        }
        *buftc++=0;
        if((i=buftc-buftc0)>ltc)
        {
                if (i>LTCM) return(0);
                stc=salloc(LTCM);
                ltc=LTCM;
        }
        c=buftc0;
        buftc=stc;
        do
            {
                *stc++ = *c;
                ltc--;
        }
        while (*c++);
#ifdef TEST
         printf("%s=",tc); ptss(buftc);
#endif
        return(buftc);
}

/*
 * tcread()
 * Чтение описаний. Код ответа:
 * 0 - term cap reading O'KEY
 * 1 - no termcap
 * 2 - not enoughf termcap
 */
tcread()
{
    char *fname;
    register int i;
    register struct ex_int *ir, *iw;
#ifndef FROMTCAP
    char buftcs[2048];
    extern char *getenv();
    if ( tgetent(buftcs,getenv("TERM"))!=1) return(1);
#else
    if ( !tgettc()) return(1);
#endif
    mktab();
    LINEL=tgetnum("co");
    NLINES=tgetnum("li");
    for (i=0; i<COMCOD ; i++)
    {
        cvtout[i]=gettcs(cvtout[i]);
    }
    if (tgetflag("nb")) cvtout[COBELL]="\0";
    if (tgetflag("bs")) cvtout[COLT]="\010";
    if(curspos=gettcs("cm")) agoto= tgoto;
    if ( !cvtout[COCURS] && cvtout[COOPEN] )
        cvtout[COCURS] = append(cvtout[COOPEN], append("*", cvtout[COCLOSE]));
    eolflag= !tgetflag("am") || tgetflag("xn");
    if( !cvtout[COCURS]) cvtout[COCURS]="@";
    if (LINEL<=60 || NLINES<8) return(2);
    if(LINEL>LINELM) LINEL=LINELM;
    if(NLINES>NLINESM) NLINES=NLINESM;
    if(!curspos) for(i=0;i<COERASE; i++) if(!cvtout[i]) return(2);
#ifdef RED_CYRILL
    /* Red can in/out cyryll without the system
     * latf, but tty is cyrillic
     */
    if ( tgetflag("CY") && !cvtout[COCYON])
                {cvtout[COCYON] = DFL_CYON; cvtout[COCYOFF] = DFL_CYOFF; }
    if ( !lcasef && latf && cvtout[COCYON] && cvtout[COCYOFF] )
                {Rcyflag = 1; latf=0; }
    if ( (fname=gettcs(TABLENAME)) && (i=open(fname,0))) {
#ifdef NO_AOUT
        lseek(i,(long)(sizeof (struct exec)),0);
#endif
        if(read(i, &tablecod, (sizeof tablecod)) != sizeof tablecod)Rcyflag=0;
        close(i);
        }
#endif
    if(!cvtout[COSRFWD] && !(cvtout[COSRFWD]=gettcs("nl")) )
        cvtout[COSRFWD] = "\n";
    ch_scroll = gettcs("cs");
    if( ch_scroll && cvtout[COSRBAK]) can_scroll = ROLL_CS;
    else if(cvtout[COILINE] && cvtout[CODELIN]) can_scroll = ROLL_IL;
    else if(cvtout[COSRBAK] && cvtout[COCLLIN] && cvtout[COCLSCR])
         can_scroll = ROLL_SF;
/* If scroll - define default/+line for RETURN */
    if(can_scroll) {
        on_scroll = 1;
        if(can_scroll != ROLL_SF) defreturn = PL1MOVEW;
        }
    if (fname=gettcs(HELP_NAME)) deffile = fname;
/*
 * Разбираемся с псевдо-графикой
 */
    g_table = gettcs("gt");
    if ( g_table && cvtout[COGSTART] && cvtout[COGEND] )
                { register char *c;
                  for(c=g_table; *c; c++);
                  gchars = c - g_table;
                  if ( gchars > (G_END-G_START+1)) gchars = G_END-G_START+1;
                }
    /*
     input codes definition
     */
    ir=iw=inctab;
    while(ir->excc)
    {
        if ((iw->excc=gettcs(ir->excc)))
        {
            iw->incc=ir->incc;
            iw++;
        }
        else nfinc++;
        ir++;
    }
    iw->excc=NULL;
    iw->incc = 0;
    itsort(inctab,iw-1,0);
    *toup=cvtout[UP];
    BC=cvtout[LT];
    return(0);
}

/*
 * itsort(fb,fe,ns) -
 * сортировка inctab  для работы
 * функции findt
 */
itsort(fb,fe,ns)
struct ex_int *fb,*fe;
int ns;
{
    register struct ex_int *fr, *fw;
    char c;
    struct ex_int temp;
    fw= fb-1;
    while(fw != fe)
    {
        fr=fb= ++fw;
        c=fw->excc[ns];
        while(fr++ != fe)
        {
            if(fr->excc[ns]==c)
            {
                if(fr!= ++fw){
                    temp= *fr;
                    *fr= *fw;
                    *fw=temp;
                }
            }
        }
        if(c!=0 && ((fw-fb)>1)) itsort(fb,fw,ns+1);
    }
}

#ifdef TEST
#include "t.tc.c"
#endif
