/*
 *      Редактор RED.
 *      файл r.pars.c - разбор мнемоники команд RED.
 *      $Header: r.pars.c,v 4.20 90/05/22 23:21:36 alex Exp $
 *      $Log:   r.pars.c,v $
 * Revision 4.20  90/05/22  23:21:36  alex
 * First rev. red 4.2
 * 
 */
#include "r.defs.h"

# define ZZ(a,b)  ('a'&0377) << 8 | 'b'&0377 | (040<<8) | 040
# define ZZS(a,b) (( a &0377) << 8 |  b &0377)  | (040<<8) | 040
# define C1(zz) (((zz>>8)&0377)&0137)
# define C2(zz) ((zz)&0377)

/*
 * Таблица имен регулярных команд
 * макро сюда не входят
 */
struct cmd_names {
int cmd_zz;
int cmd_cod;
} cmd_names[] =
{
/* ВВод / Return */
ZZ(R,t),    CC+RN,              /* ReTurn       */
/* Команды движения курсора */
ZZ(C,d),    CC+DN,              /* CursorDown   */
ZZ(C,h),    CC+HO,              /* CursorHome   */
ZZ(C,l),    CC+LT,              /* CursorLeft   */
ZZ(C,-),    CC+LT,              /* CursorLeft   */
ZZ(C,r),    CC+RT,              /* CursorRight  */
ZZ(C,+),    CC+RT,              /* CursorRight  */
ZZ(C,u),    CC+UP,              /* CursorUp     */
ZZ(C,b),    CCBEGIN,            /* CursorBegin  */
ZZ(C,e),    CCEND,              /* CursorEnd    */

/* Работа с табуляциями */
ZZ(T,-),    CC+BT,              /* TabLeft    */
ZZ(T,l),    CC+BT,              /* TabLeft    */
ZZ(T,r),    CC+TB,              /* TabRight   */
ZZ(T,+),    CC+TB,              /* TabRight   */
ZZ(T,s),    CCTABS,             /* TabSet     */
/* Работа по словам     */
ZZ(W,l),    CCWORDLEFT,         /* WordLeft   */
ZZ(W,r),    CCWORDRIGHT,        /* WordRight  */
ZZ(W,s),    CCWORDSELECT,       /* WordSelect */
ZZ(W,g),    CCWORDGET,          /* WordGet    */
/* Команды движения по файлу */
ZZ(P,+),    CCPLPAGE,           /* PageNext */
ZZ(L,+),    CCPLLINE,           /* LineNext */
ZZ(P,r),    CCRPORT,            /* PageRight*/
ZZ(P,-),    CCMIPAGE,           /* PagePrev */
ZZ(L,-),    CCMILINE,           /* Line-    */
ZZ(P,l),    CCLPORT,            /* PageLeft */
ZZ(G,o),    CCGOTO,             /* GOto     */
/* Команды поиска по шаблону */
ZZ(S,+),    CCPLSRCH,           /* Search+ */
ZZ(S,-),    CCMISRCH,           /* Search- */
ZZ(S,p),    CCPSEARCH,          /* SearchP */
/* Команды редактирования строки */
ZZ(D,c),    CCDELCH,                    /* DelChar */
ZZ(B,s),    CCBACKSPACE,                /* BackSpace */
ZZ(I,m),    CCINSMODE,                  /* InsertMode*/
ZZ(Q,c),    CCCTRLQUOTE,                /* QuoteChar */
ZZ(U,d),    CCUNDO,                     /* UnDo edition */
/* Команды работы с окнами */
ZZ(W,c),    CCCHPORT,                   /* WindChange */
ZZ(W,m),    CCMAKEPORT,                 /* WindMake   */
ZZ(W,z),    CCZOOMPORT,                 /* Window Zoom */
/* Команды работы со строками и группами строк */
ZZ(L,i),    CCOPEN,                     /* LineInsert */
ZZ(L,d),    CCCLOSE,                    /* LineDelete */
                                        /* LineErase  */
ZZ(L,t),    CCTRUNCATE,                 /* LineTruncate */
ZZ(L,s),    CCLINESPLIT,                /* LineSplit  */
ZZ(L,j),    CCLINEJOIN,                 /* LineJoin   */
ZZ(B,g),    CCPICK,                     /* BufGet     */
ZZ(B,p),    CCPUT,                      /* BufPut     */
                                        /* BufReplace  */
/* Команды работы с файлами */
ZZ(F,o),    CCSETFILE,                  /* FileOpen   */
ZZ(F,s),    CCSAVEFILE,                 /* FileSave   */
                                        /* FileChange */
/* Разные команды */
ZZ(E,c),    CCDOCMD,                    /* ExecCmd */
ZZ(E,n),    CCENTER,                    /* ENter   */
ZZ(A,s),    CCSELECT,                   /* ArgSelect */
ZZ(Q,u),    CCQUIT,                     /* QUit    */
ZZ(R,s),    CCRESCREEN,                 /* ReScreen */
ZZ(H,e),    CCHELP,                     /* HElp     */
/* Работа с макро */
ZZ(M,c),    CCDEFMACRO,                 /* MacroCreate */
ZZ(M,s),    CCSAVEMACRO,                /* MacroSave   */
ZZ(M,r),    CCREADMACRO,                /* MacroRead   */

/* Конец таблицы */
0, 0 };

/*
 * makecc(s) - дай команду по ее названию
 */
makecc (s)
char *s;
{
        int c;
        register struct cmd_names *pcm;
        if ( s[0] == '?' && s[1] == 0) return(CCHELP);
        if ( *s == '$' )
        {
                c = s[1]|040;
                if ( c == '$' ) return(CCMAC);
                if ( c >= '0' & c <= '9') c = 'z'+1+(c-'0');
                c = c - ('a'|040) + CCMAC + 1;
                if ( c <= CCMAC || c > CCEMAC )
                        return(-1);
                return(c);
        }
        c = ZZS(s[0],s[1]);
        for(pcm = cmd_names;pcm->cmd_zz;pcm++)
        {
                if ( pcm->cmd_zz == c ) return(pcm->cmd_cod);
        }
        return(-1);
}

/*
 * char *makecs(c) - дай имя команды по ее коду
 * ответ ссылается на статическую область памяти
 * NULL - несуществующая команда
 */
char *makecs (c)
int c;
{
        static char ss[3]; /* 2 символа и 0 */
        register struct cmd_names *pcm;
        /* Макро - отдельная обработка */
        if ( c > CCMAC && c <= CCEMAC )
        {
                ss[0] = '$';
                ss[1] = (c-CCMAC+1) + 'a';
                return(ss);
        }
        /* Не команда - возвращаем пустую строку */
        if ( !ISACMD(c)) return(NULL);
        for(pcm = cmd_names;pcm->cmd_zz;pcm++)
        {
                if ( pcm->cmd_cod == c ) goto retn;
        }
        return((char *)0);
retn:
        ss[0] = C1(pcm->cmd_zz);
        ss[1] = C2(pcm->cmd_zz);
        return(ss);
}

/*
 * SelectArgs(line, av, MAX) = ac
 * - разбирает строку аргументов и формирует
 *   вектор аргументов av, возвращая число
 *   аргументов. ac < 0 - ошибка
 * Аргумент - либо 'текст'
 *            либо "текст"
 *            либо текст
 *            либо текст;
 *      В последнем случае ; - конец оператора, и дает
 *      ссылку на переменную "semiColon"
 * Используется в разборе команды
 */
static char semiColon[2];
SelectArgs(cp, execargs, maxargs)
register char *cp;
char *execargs[];
int maxargs;
{
    register char **e = execargs;
    register char *wp;
    semiColon[0] = ';';
    semiColon[1] = 0;
    while (*cp == ' ' ) cp++;
    wp = cp;
    while (*cp != 0) {
        *e++ = wp;              /* адрес аргумента */
        if ((e-execargs) >= maxargs) goto noargerr; /* Слишком много */
        /* Начало или продолжение аргумента */
        while ( *cp && *cp != ';' && *cp != ' ' && *cp != '\n' )
        {
            if (*cp == DQUOTA) {
                cp++;
                while (*cp !=  DQUOTA)
                    if (*cp == 0) goto noargerr;
                    else *wp++ = *cp++;
                cp++;
                continue;
            };
            if (*cp == QUOTA) {
                cp++;
                while (*cp !=  QUOTA)
                    if (*cp == 0) goto noargerr;
                    else *wp++ = *cp++;
                cp++;
                continue;
            };
            while (*cp != ';' && *cp != DQUOTA && *cp != QUOTA &&
                   *cp != ' ' && *cp != '\n' && *cp ) *wp++ = *cp++;
        }
        if ( *cp == ';' ) {
                *e++ = semiColon;       /* ; - дает отдельный аргумент */
                if ((e-execargs) >= maxargs) goto noargerr; /* Слишком много */
        }
        while ( *cp == ';' || *cp == ' ' || *cp == '\n') *cp++ = 0;
        *wp++ = 0;
    }
    *e = 0;
    return(e - execargs);
noargerr:
    return(-1);
}

/*
 * Язык редактора:
 * # Комментарий
 * "вводимый текст
 * <пробел>вводимый текст
 * [{.}][Повторитель]Команда[ Аргумент][; ...]
 * . - означает, что нужно ввести Rt, если перед этим шла строка текста
 * Повторитель - число повторений, по умолчанию 1
 * Специальная команда, @, служит для ввода текста в строке
 * Если есть аргумент, формируется En;@ Текст;Команда или, что точнее
 * ------
 * En
 *  Текст
 * Команда
 * -------
 */
/*
 * Потом - в r.defs.h !!!
 * cmdstr - описатель команды
 */
struct cmdstr {
short cmd_cmd;    /* 0 - просто текст */
short cmd_rep;    /* Число повторений */
short cmd_ac;     /* Число параметров */
short cmd_break;  /* Признак - команда прерывает строку */
char **cmd_av;  /* Массив параметров */
};

/*
 * Макро
 * Sel1cmd - выбрать 1 команду из строки с разделителями ';'
 * Sel1cmd(ac1,av1,ac, av)
 */
# define Get1cmd(ac1, av1, ac, av) {\
for (ac1 = 0, av1 = av; ac > 0 && *av != semiColon;ac--, av++, ac1++);\
if ( ac > 0 ) ac--, av++;\
}

/*
 * char *ParseCmd(d_cmd, ac, av )
 * struct cmdstr *d_cmd
 * - разобрать 1 команду
 * и проверить ее правильность
 * Если ответ не NULL, то это - сообщение об ошибке
 */
char *ParseCmd( pd, ac, av)
register struct cmdstr *pd;
int ac;
char **av;
{
    register char *p;
    register int i;
    int l = 0; /* Длина числа повторов */
    p = av[0];
    i = 0;
    pd->cmd_break = 0;
    if ( *p == '.' )
    {
        p++;
        pd->cmd_break = 1;
    }
    while ( *p >= '0' && *p <= '9' )
    {
        i = i*10 + (*p - '0');
        p++;
        l++;
    }
    /* По умолчанию - команда повторяется 1 раз */
    if ( l == 0 ) i = 1;
    av++; 
    ac--;
    pd->cmd_rep = i;
    if ( p[0] == '@' && p[1] == 0 ) i = 0;
    else i = makecc(p);
    if ( i < 0 ) return("Illegal command");
    pd->cmd_cmd = i;
    /* Здесь надо бы проверить число аргументов, но пока оно д. быть 0 или 1 */
    if ( ac < 0 || ac > 1 ) return("Illegal opt. count");
    pd->cmd_ac = ac;
    if ( ac == 1 ) pd->cmd_av = av;
    else pd->cmd_av = (char **)0;
    return(NULL);
}


/*
 * char *ReadMFile (r_proc, err_proc)
 * Читать макро из инит-файла и
 * (*r_proc)(buf, len) - подпрограмма ввода строки
 * определить возвратить указатель на него
 * (*err_proc)(s1,s2)
 * - печатать сообщение об ошибке
 */
/*
 * 1. Отвести массив памяти
 * 2. Установить - был_текст = 0
 * 3. Цикл по чтению
 * 4. Дай!
 * 5. Если 1-й эл != ".", и был текст, то вставить команду Rt
 * 6. Если 1-й элемент ' ' или '"', то дописать текст, был_текст = 1, продолж
 * 7. Если 1-й элемент "'" или ".", сдвинуть на 1
 * 8. Если цифра, то дай_число_повторов
 * 9. команда =
 * 10. Цикл по повторам
 * 11. Если аргумент, то "En", Параметры
 * 12. Команда
 * 13. Конец цикла по повторам
 * 14. Конец цикла по дай!
 * 15. Разместить макро
 * 16. Выход (адрес_начала_макро)
 */
# ifndef MAX_MACLEN
# define MAX_MACLEN 512
# endif
# ifndef MAX_ILINE
# define MAX_ILINE  128
# endif
char *macrobuf, *mep;
char *mp;
# define MPUTC(c,Err) if ( mp  > mep ) goto Err; else *mp++ = (c)
# define MINIT(len) {mp = macrobuf = salloc(len);\
        mep = &macrobuf[len-2];}
# define MGET ((*mp++ = 0),macrobuf)
# define MFREE if(macrobuf) {zFree(macrobuf);macrobuf = (char *)0;}else

# ifdef PARSERDEB
# define DEB(s) ErrI(1,s)
#else
# define DEB(s) /* */
# endif
/* */
char *ReadMFile(r_proc,err_proc)
char *(*r_proc)();
void (*err_proc)();
{
    char line[MAX_ILINE+2],*args[MAX_IARGS+1], **av, **av1;
    register char *p;
    struct cmdstr scmd;
    int ac,ac1;
    int errors = 0;
    int rep;
    int was_text;
    was_text = 0;
    MINIT(MAX_MACLEN);
    DEB("Mread start");
    while ( (*r_proc)(line,MAX_ILINE) != NULL )
    {
        DEB(line);
        if ( line[0] == '#' ) continue;
        if ( line[0] == ' ' || line[0] == '"' )
        {
            if ( was_text ) MPUTC(CCRETURN,too_long);
            for( p = &line[1]; *p && *p != '\n' ;p++)
            {
                MPUTC(*p,too_long);
            }
            was_text = 1;
            continue;
        }
        DEB("Start select");
        ac = SelectArgs(line, args, MAX_IARGS);
        av = args;
        DEB("End select");
        if ( ac < 0 ) {
            (*err_proc)("Syntax error");
            errors = 1;
            continue;
        }
        while (ac>0)
        {
            DEB("Start GET1");
            Get1cmd(ac1,av1,ac,av);
            if ( ac1 <= 0 ) continue;
            p = ParseCmd( &scmd, ac1, av1);
            if ( p ) {
                (*err_proc)(p);
                errors = 1;
                continue;
            }
            if ( was_text && scmd.cmd_break ) MPUTC(CCRETURN,too_long);
            was_text = 0;
            while ( scmd.cmd_rep-- > 0 )
            {
                if ( scmd.cmd_ac > 0 ) {
                    if ( scmd.cmd_cmd)
                        MPUTC(CCENTER,too_long);
                    DEB("Arg");
                    DEB(*(scmd.cmd_av));
                    for(p = *(scmd.cmd_av);*p;p++)
                        MPUTC(*p, too_long);
                }
                if( scmd.cmd_cmd)
                    MPUTC(scmd.cmd_cmd,too_long);
            }
            /* Следующий элемент из строки */
        }
        /* Следующая строка */
    }
    p = NULL;
    if ( errors == 0 ) p = append(MGET,"");
    MFREE;
    return(p);
too_long:
    (*err_proc)("Too long macro");
    while ( (*r_proc)(line,MAX_ILINE) != NULL );
    MFREE;
    return(p);
}

/*
 * WriteMFile(char *macro, FILE *fd)
 * - записать макро-файл по дескримптору fd
 */
WriteMFile(mp, fd)
register char *mp;
FILE *fd;
{
    register char *cmd;
    register int lc,llc, count;
    char *amp;
    char qchar;
    int ko = 0;
    while ( lc = *mp++ )
    {
        lc &= 0377;
        /* Простой текст просто записываем, начиная с пробела */
        if ( !ISACMD(lc) && !ISMACRO(lc) )
        {
            putc('"',fd);
            putc(lc,fd);
            while ((lc = (*mp++ & 0377) ) && !ISACMD(lc) && !ISMACRO(lc) )
                putc(lc,fd);
            putc('\n',fd);
            mp--;
        }
        else
        {
            if ( lc == CCENTER )
            {
                amp = mp;
                while ((lc = (*amp++ & 0377)) && !ISACMD(lc) && !ISMACRO(lc) );
                /* Если это команда с аргументами - выводим в форме команды с аргументами */
                if ( lc && (cmd = makecs(lc) ) ) {
                    fputs(cmd,fd);
                    qchar = DQUOTA;
                    putc(' ',fd);
                    putc(qchar,fd);
                    while ( mp < amp-1 )
                    {
                        lc = *mp++;
                        if ( lc == qchar ) {
                            putc(qchar,fd);
                            qchar = (qchar == DQUOTA ? QUOTA :DQUOTA);
                            putc(qchar,fd);
                        }
                        putc(lc,fd);
                    }
                    putc(qchar,fd);
                    putc('\n',fd);
                    mp = amp;
                    continue;
                }
            }
            count = 1;
            while ( (*mp&0377) == lc )
            {
                count++;
                mp++;
            }
            cmd = makecs(lc);
            if (!cmd )
            {
                ko = -1;
                cmd = "??";
            }
            if ( count != 1 )
                fprintf(fd,"%d%s\n",count,cmd);
            else
                fprintf(fd,"%s\n",cmd);
        }
    }
    return(ko);
}

extern int rep_count;
/*
 * Param_cmd() - разбирает командную строку из paramv.
 * Ответ:
 * код ответа - сама команда, -1 - ошибка
 *            paramtype - тип, как в param(0)
 *              если paramtype == 2 - это команда без параметров
 *
 * Формат команды:    (<d> - пусто либо число повторений команды)
 * <d>КК - команда без параметров
 * <d>KK<пробел> - команда с пустым аргументом
 * <d>KK <аргумент> - команда с аргументом
 * 1. Выделим команду
 * 2. Определим, есть ли там пробел
 * 3. Определим длину остатка
 * 4. Установим paramv, paraml
 * 5. Установим paramtype
 * 6. Выйдем
 * В старой r.cmd.c <d> не используется
 */
Param_cmd()
{
        register char *cp;
        register char *pv;
        int is_param = 0;
        int cmd;
        char *cpv;
        char *paramv1;
        paramv1 =  s2i(paramv, &rep_count);
        if ( !paramv1 || rep_count <= 0 )
              paramv1 = paramv;
        cp = paramv1;
        while ( *cp && *cp != ' ') cp++;
        if ( *cp == ' ' ) {
                *cp == 0;
                is_param = 1;
                cp++;
        }
        cmd = makecc(paramv1);
        if ( cmd < 0 )
        {
                /* Восстановим старый paramv */
                if ( is_param ) cp[-1] = ' ';
                rep_count = 0;
                return(-1);
        }
        if ( *cp == 0 )
        {
                paraml = 0;
                zFree(paramv);
                paramtype = (is_param? 0 : 2 );
                return(cmd);
        }
        /* Осталось переразместить аргумент */
        paraml = paraml - ( cp - paramv1);
        cp = append(cp,"");
        zFree(paramv);
        paramv = cp;
        paramtype = 1;
        return(cmd);
}

