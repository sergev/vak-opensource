/*
 *      Редактор RED.
 *      r.gettc.c - вытаскивание описания терминала из termcap
 *               и работа с таблицей команд.
 *      $Header: S.gettc.c,v 4.20 90/05/22 23:22:37 alex Exp $
 *      $Log:   S.gettc.c,v $
 * Revision 4.20  90/05/22  23:22:37  alex
 * First rev. red 4.2
 * 
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

#ifndef TEST1
# define PIT /* */
# else
# define PIT pttt();
# endif

extern char *UP, *BC;
char **toup= &UP;        /* Так сложно из-за совпадения имен */

#include "r.defs.h"
#define TELE_INIT
#include "S.tele.h"

# define INCTABSZ 150
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
/* COSTART: */  "cl?is?ti?ks?ho",  /* COUP:     */  "up",
/* CODN   : */  "do",              /* CORN:     */  "\015",
/* COHO   : */  "ho",              /* CORT:     */  "nd",
/* COLT   : */  "bc",              /* COCURS:   */  "cu",
/* COBELL : */  "\007",            /* COFIN:    */  "?Ge?me?Ce?cl?fs?te?ke",
/* COERASE: */  "cl"  ,            /* COOPEN:   */  "?so",
/* COCLOSE  */  "?se",
/* COVIOPE  */  "vs",              /* COVICLO   */ "ve",
/* COCYON   */  "Cs",              /* COCYOFF   */ "Ce",
/* COILINE  */  "al",              /* CODELIN   */ "dl",
/* COSRFWD  */  "sf",              /* COSRBAK   */ "sr",
/* COCLLIN  */  "ce",              /* COCLSCR   */ "cd",
/* COANORMAL*/  "?me?se",
/* COAINFO  */  ":m1:mr:so",
/* COAERROR */  ":m2:?mbmr:?mbso",
/* COAMARG  */  ":m3:md",
/* COAOUT   */  ":m4:mr?md:so",
/* COGSTART */  "gs",
/* COGEND   */  "ge",
                (char *)0
};

char *curspos;
int slowsw=1;

/*   Входные коды
 *   DEFTABL - число позиций в начале таблицы, которые не сбрасываются
 *
 */

/*  Таблица кодов клавиш терминала и их комбинаций */
/*  Сюда же записываются коды при переопределении  */
/* Состоит из трех частей:
 * 1. Движение курсора. Всегда можно ввести с клавиатуры.
 * 2. Функциональная клавитура и ее замена;
 * 3. Ввод через контрол-символы.
 * Внимание. При сортировке используется предположение о том,
 * что если p2 = zAlloc(N) выполнено позже p1 = zAlloc(N),
 * то будет p2  > p1 . Если это не так (хотя в это трудно поверить),
 * то более поздние определения клавиш будут действовать не всегда!
 */
char escch1 = '\012';
struct ex_int inctab[INCTABSZ] = {
/*
 * Движение курсора. Кодируем так, чтобы было всегда
 */
CC+RN,    "^M",
CC+RN,    "^J",
CC+LT,    ":kl:^[^L",
CC+RT,    ":kr:^[^R",
CC+UP,    ":ku:^[^U",
CC+DN,    ":kd:^[^D",
CC+HO,    ":kh:^[^H",
CC+BT,    "^B",
CC+TB,    "^I",
/*
 * Это нужно всегда
 */
 CCBACKSPACE    ,"^?",
 CCCTRLQUOTE    ,"^A",
 CCQUIT         ,"^[^?",
 CCENTER        ,"^[= ",
 CCHELP         ,"^\\",
# define DEFTABL        13
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
 0,0,   0,0,   0,0,   0,0,   0,0,
 0,0,   0,0,   0,0,   0,0,   0,0,
 0,0,   0,0,   0,0,   0,0,   0,0,
 0,0,   0,0,   0,0,   0,0,   0,0,
 0,0,   0,0,   0,0,   0,0,   0,0,
 0,0,   0,0,   0,0,   0,0,   0,0,
 0,0,   0,0,   0,0,   0,0,   0,0,
 0, 0 };

/*
 * Заполнение таблицы клавиатуры
 * (из инит-файла)
 */
int LkeySpace = 2048; /* Величина временного буфера для кодов клавиш */
static char *PkeySpace, *PSpace; /* Ук на буфер и положение в нем */
static LSpace = 0;              /* Длина буфера (текущего), 0 - признак НЕТ */
static struct ex_int *PTab = (struct ex_int *)0;     /* УК на свободное место в таблице */
/*
 * ClearKeys(i) - обнуление таблицы клавиатуры
 * i = 0 - обнулить и таблицу, и буфер
 * i = 1 - обнулить только буфер (после чтения termcap)
 */
ClearKeys(m)
int m;
{
    register struct ex_int *iw;
    if ( m == 0 ) {
        iw = &inctab[DEFTABL];
        PTab = iw+1;
        while ( ++iw < &inctab[INCTABSZ] ) iw->excc = NULL;
    }
    if ( LSpace ) {
        zFree(PkeySpace);
        LSpace = 0;
    }
}

/*
 * char *AddKey(ac, av) - добавление клавиши в таблицу
 * av[0] - код команды, av[1] - код клавиши
 * ответ: 0 - O'KEY, иначе текст диагностики ошибки
 * char *AddCmd(int cmd, char *key) - то же самое, но только
 *     cmd и key даны явно
 */
char *AddKey(ac, av)
int ac; /* <0 - ac = 2, а это cmd */
char **av;
{
        register struct ex_int *iw;
        int cmd,i;
        if ( ac < 0 )
        {
                cmd = -ac;
                ac = 2;
        }
        else
        {
                if ( ac > 2 && *(av[2]) == '#' ) ac = 2;
                if ( ac != 2 ) return("Must be: CC \"KeyValue\" ");
                cmd = makecc(av[0]);
                if ( cmd < 0 ) return("Illegal cmd");
        }
        if ( PTab == 0 )
        {
                for(iw = inctab;(iw->excc!= NULL)  && iw < &inctab[INCTABSZ];iw++);
                PTab = iw;
        }
        if ( PTab > &inctab[INCTABSZ] )
                return("Key table overflow");
        if ( !LSpace )
                PSpace = PkeySpace = salloc ( LSpace = LkeySpace);
        i = strlen(av[1])+1;
        if ( PSpace + i > &PkeySpace[LSpace] )
                return("Key space overflow");
        strcopy(PSpace, av[1]);
        PTab->excc = PSpace;
        PTab->incc = cmd;
        PTab++;
        PSpace += i;
        return((char *)0);
}

char *AddCmd(cmd,key)
char *key;
{
char *av[2];
        av[1] = key;
        if ( cmd <= 0 ) return("Internal error");
        return(AddKey(-cmd, av));
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
        c=tc;
        while(*c)
        {
                if(*c=='?'){
                        fcase=1;
                        c++;
                }
                else fcase=0;
                if( *c == '\'' ) {
                        while ( *buftc++ = *++c );
                        buftc--;
                        break;
                        }
                if ( (*tc&0340) == 0 || *tc==0177) {
                        while ( *buftc++ = *c++ );
                        buftc--;
                        break;
                        }
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
 * Буфер для termcap.
 * Все равно fsd-s займут эти 2 килобайта, так что пожалеем стек
 */
static char *buftc;
GetTC(ifd,ailine)
FILE *ifd;
char *ailine;
{
    register int c,co;
    register char *p;
    /* Если в буфере ничего не было - создадим его */
    if ( !buftc ) buftc = salloc(TBUFSIZE);
    if ( !buftc ) return(-1);
    /* Сделаем нач. установки */
    p = buftc;
    c = '\n';
    /* Крутим цикл и читаем */
    while ( (c = ((co = c), getc(ifd)) )>= 0 )
        {
        if ( c == '\n' ) *ailine += 1;
        if ( !c ) continue;
            if ( c == '\n' && co == '\\' ) continue;
            if ( co == '\n' && c == '.' ) break;
            if ( co == '\n' ) {
            if (c == ' ' || c == '\t' )
                c = co;
                continue;
        }
        if ( (p - buftc ) > 2047 ) return(-1);
        if ( p != buftc && p[-1] == ':' && co == ':' ) continue;
        *p++ = co;
    }
    *p++ = 0;
        return(0);
}

/*
 * tcread()
 * Чтение описаний. Код ответа:
 * 0 - term cap reading O'KEY
 * 1 - no termcap
 * 2 - not enoughf termcap
 */
static it_compare();
tcread()
{
    char *fname;
    struct ex_int *iw, *ir;
    register int i;
    extern char *getenv();
    if ( !buftc ) {
        if ( !(buftc = salloc(2048)) || tgetent(buftc,getenv("TERM"))!=1)
                return(1);
    }
    else if ( tgetent(buftc,NULL) != 1 ) return(1);
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
    {
        cvtout[COCYON] = DFL_CYON; 
        cvtout[COCYOFF] = DFL_CYOFF; 
    }
    if ( !lcasef && latf && cvtout[COCYON] && cvtout[COCYOFF] )
    {
        Rcyflag = 1; 
        latf=0; 
    }
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
    /*
 * Разбираемся с псевдо-графикой
 */
    get_gch();
    /*
     * Вычисляем таблицу клавиатуры
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
    PIT;
    /*
     * Теперь ее сортируем
     */
    iw->excc=NULL;
    iw->incc = 0;
    qsort(inctab, iw - inctab, (sizeof (struct ex_int)), it_compare);
    PIT;
    /*
     * А теперь убираем повторяющиеся значения, оставляя последнее
     */
    ir = iw = inctab;
    while ( ir->excc )
    {
        while ( (ir+1)->excc && strcmp((ir+1)->excc,ir->excc) == 0 ) ir++;
        if ( ir != iw ) *iw = *ir ;
        ir++ ; iw++;
    }
    /* И чистим хвост */
    while ( iw < &inctab[INCTABSZ] ) iw++->excc = 0;
    PIT;
    *toup=cvtout[UP];
    BC=cvtout[LT];
    zFree(buftc);
    ClearKeys(1);
    return(0);
}

/*
 * it_compare(tp1,tp2) -
 * сравнение двух элементов таблицы
 * tp1->excc == tp2->excc == NULL - 0
 * tp1->excc == NULL - -1
 * tp2->excc == NULL - +1
 * scompare(tp1->excc,tp2->excc) -
 * != 0   - его ответ
 * иначе если tp2->excc позже создано, чем tp1->excc , то -1
 * иначе 1
 */
static it_compare(tp1, tp2)
register struct ex_int *tp1, *tp2;
{
        register int i;
        if ( tp1->excc == NULL && tp2->excc == NULL ) return(0);
        /* Нулевое значение всегда самое большое */
        if ( tp1->excc == NULL ) return(1);
        if ( tp2->excc == NULL ) return(-1);
        i = strcmp(tp1->excc, tp2->excc);
        if ( i ) return(i);
        /* Если равны, то с меньшим адресом меньше */
        return(( tp2->excc > tp1->excc ) ? -1 : 1 );
}

/*
 * grapf_gg[i]="GG"
 * GG - описатель из TERMCAP, затем сюда вешается ссылка на описатель
 * ins_how[i] = "nnnnnnnnnnn"
 * nnnn - куда подставить данное поле, +1. 0 - не подставлять
 * \77 - признак - не заносить
 * Есть только одна тонкость: если g2 есть и g1 есть, то
 * в поле стрелки вниз заносится одинарная верт. черта
 * g_table= 1 - гориз. черта
 *          2 - отметка на экране
 *          3 - верт. черта
 *          4 - BL 5 BM 6 BR 7 ML 10 CC 11 MR 12 TL 13 TM 14 TR
 *         15 - стрелка вниз/признак пустого поля
 *         16 - стрелка вверх/указатель табуляции
 *         20 - стрелка влево/указатель сдвига вправо
 *         21 - стрелка вправо/указатель текста справа
 */
static char *ins_how[] =
{
 "\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20\21",
 "\1\3\4\5\6\7\10\11\12\13\14",
 "\1\3\4\5\6\7\10\11\12\13\14",
 "\21\20\16\15",
 "\77\77\77\77\77\2" ,
 (char *)0
};
/*
 * Имена termcap, сюда же вешаем ссылки
 */
#define G_g1 1
#define G_g2 2
static char *graf_gg[] =
{
 "gt",
 "g1",
 "g2",
 "g5",
 "g6",
 (char *)0
 };
/*
 * Буфер под псевдографику
 */
static char buf_grc[G_END - G_START + 1];
get_gch()
{
    char **pcc,**gg;
    register char *gi,*p;
    register int i;
    if ( !cvtout[COGSTART] || !cvtout[COGEND] )
        return;
    g_table = buf_grc;
    for(pcc = ins_how,gg=graf_gg;*pcc && *gg; pcc++, gg++)
    {
        *gg = gi = gettcs( *gg );
        if ( !gi ) continue;
        for( p = *pcc;*p && *gi; p++ , gi++ )
        {
            i = *p;
            if ( i != 077 && i > 0 && i < (G_END - G_START + 1) )
                g_table[i-1] = *gi;
        }
    }
    /*
         * А теперь, если есть оба вида рамок, вместо стрелки вниз ставим
         * тонкую рамку
         */
    if ( graf_gg[G_g1] && (gi = graf_gg[G_g2] ) )
        g_table[014] = gi[1];
    return;
}

#ifdef TEST1
# include <stdio.h>
pttt()
{
        register struct ex_int *fr;
        register char *c;
        char buf[128];
        fr = inctab;
        while ( fr < &inctab[INCTABSZ] )
        {
                if ( fr->excc )
                {
                        fprintf(stderr,"%d: %d = ",fr-inctab, fr->incc);
                        for(c= fr->excc ; *c ;c++)
                                if ( *c&0340 ) putchar(*c);
                                else fprintf(stderr,"\033[7m%c\033[m",(*c)+0100);
                        putchar(012);
                }
        fr++;
        }
        gets(buf);
}
#endif
