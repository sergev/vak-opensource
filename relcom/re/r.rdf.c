/*
 * $Header: r.rdf.c,v 4.20 90/05/22 23:22:26 alex Exp $
 * $Log:        r.rdf.c,v $
 * Revision 4.20  90/05/22  23:22:26  alex
 * First rev. red 4.2
 * 
 * r.rdf.c - файл, содержащий программы по работе с
 *           инициализационным файлом и всем остальным, что имеет
 * такой же формат (в дальнейшем так будут писаться все файлы, кроме
 * файла протокола, а пока это только инит-файл и файл с макро)
 */
#include "r.defs.h"

extern char *InitPath;
extern char *MacroFile;
extern char *RedKeys;
extern char *SetVar(), *AddKey(), *AddCmd();
extern char *StrCalc();
extern char *SetTab();
extern char *getkeys();

/*
 * read_init(path) -
 * читать инит-файл
 * path - path для поиска файла инициализации
 *
 * Понимаются операторы инициализации:
 * # - комментарий
 * set VAR VALUE - присвоить переменную
 * keytab {|add|replace}
 * .....
 * .
 * - доопределение или переопределение таблицы клавиатуры
 * termcap
 * ...
 * .
 * - вставка местного termcap-a
 * (в этом случае поиск по переменной TERMCAP уже не ведется)
 * .
 * macro Letter [Key]
 * Макроопределение
 * .
 * - определение макроопределения
 *   Эта программа работает еще в обычном режиме терминала
 *   ( до gettc )
 *
 */
# define INI_SET        1
# define INI_KEY        2
# define INI_READ       3
# define INI_TERMCAP    4
# define INI_MACRO      5
# define INI_ATR        6
# define INI_TABS       7
# define INI_STAT       8

/* Шкала - допустимое число аргументов */
# define ARGS(N) (01<<(N))
# define ARGSA   017777
static struct ini_keys {
char *i_keyw;
int   i_args;
int   i_kod; } ini_keys[] = {
"set", ARGS(2)|ARGS(1) ,   INI_SET,
"keytab", ARGS(0)|ARGS(1), INI_KEY,
"read",   ARGS(1),         INI_READ,
"termcap",ARGSA,           INI_TERMCAP,
"macro",  ARGS(1)|ARGS(2), INI_MACRO,
"attr",   ARGSA,           INI_ATR,
"read",   ARGS(1),         INI_READ,
"tabset", ARGSA,           INI_TABS,
"status", ARGS(0),         INI_STAT,
(char *)0,    0,           0 };
# ifndef MAX_ILINE
# define MAX_ILINE      128
# endif
# ifndef MAX_IARGS
# define MAX_IARGS      10
# endif
/*
 * Данные для разбора макро и т.п.
 */
extern char *ReadMFile();
FILE *curr_ifd;
char *curr_ifname;
int curr_iline;
static char *ini_rline();
static void mac_perr();
ReadInit(path,sysinit)
char *path;
int sysinit; /* 1 - чтение при старте */
{
        char *cp;
        /* Файл */
        FILE *ifd;
        char *ifname;
        char *rname;
        int iline;
        /* Буфер и аргументы */
        char buf[MAX_ILINE];
        char *iargs[MAX_IARGS+1],**av;
        int i;
        int ac;
        char *next_path();
        register struct ini_keys *pit;
        /* Разбор команды */
        /* 1. Ищем файл инициализации */
        while (  path !=NULL &&  (ifname = next_path( &path)) )
        {
                ifname = StrCalc ( ifname);
                if ( (ifd = fopen(ifname,"r")) != (FILE *)0 ) goto read_file;
                zFree( ifname);
        }
        return(0);
read_file:
        curr_ifd = ifd;
        curr_ifname = ifname;
        curr_iline = 0;
        while ( ini_rline(buf,MAX_ILINE))
        {
                if ( buf[0] == '#' ) continue;
                ac = SelectArgs(buf, iargs, MAX_IARGS);
                if ( ac < 0 ) continue;
                for(pit=ini_keys;pit->i_keyw != NULL;pit++)
                        if ( strcmp(pit->i_keyw, iargs[0]) == 0 ) break;
                if ( pit->i_keyw == NULL )
                {
                        ErrI(1,"Unknown  init command: ");
                        continue;
                }
                /* Нашли команду. Проверим аргументы */
                if ( (ARGS(ac) | pit->i_args) == 0 )
                {
                        ErrI(1,"Bad options count");
                        continue;
                }
                switch ( pit->i_kod )
                {
                case INI_SET:
                        if (  (cp = SetVar(iargs[1], &iargs[2], ac-2)) != NULL )
                                ErrI(1,cp);
                        continue;
                case INI_TABS:
                        av = &iargs[1]; ac -= 1;
                        i = 0;  /* Вместо номера колонки */
                        while ( ac-- && (cp = SetTab( &i, *av++)) == NULL) ;
                        if (  cp )
                                ErrI(1,cp);
                        continue;
                case INI_KEY:
                        if ( ac == 2 && strcmp(iargs[1],"replace") == 0 )
                                ClearKeys(0);
                        else if ( ac == 2 && strcmp(iargs[1],"add") != 0 )
                        {
                                ErrI(1,"Bad option in `keytab'");
                                continue;
                        }
                        while ( ini_rline(buf,MAX_ILINE))
                        {
                                ac = SelectArgs(buf, iargs, MAX_IARGS);
                                if ( sysinit && (cp = AddKey(ac, iargs)))
                                        ErrI(1,cp);
                        }
                        curr_ifd = ifd;
                        break;
                case INI_TERMCAP:
                        if ( MatchTname(ac-1,iargs+1) && sysinit )
                                GetTC(ifd,&curr_iline);
                        else
                                while(ini_rline(buf,MAX_ILINE) );
                        break;
                case INI_MACRO:
                        /* Отвести память под макро */
                        cp = ReadMFile( ini_rline, mac_perr);
                        /* Дочитаем, если что */
                        while(ini_rline(buf,MAX_ILINE) );
                        /* Запишем макро */
                        if ( cp )
                        {
                                i = NewMacro(iargs[1],cp);
                                if ( i <= 0 ) {
                                        ErrI(1,"Illegal macro name");
                                        zFree(cp);
                                        break;
                                }
                                if ( ac > 2 && sysinit)
                                {
                                        cp = AddCmd(i,iargs[2]);
                                        if ( cp ) ErrI(1,cp);
                                }
                        }
                        break;
                case INI_READ:
                        iline = curr_iline;
                        rname = iargs[1];
                        if ( strcmp(rname,"NEXT") == 0 ) rname = path;
                        if ( strcmp(rname,"MACRO") == 0 ) rname = MacroFile;
                        if ( ReadInit(rname,sysinit) <= 0 /* && rname != path */)
                        {
                                ErrI (1,"Can not find init file: ");
                        }
                        curr_ifd = ifd;
                        curr_ifname = ifname;
                        curr_iline = iline;
                        continue;
                case INI_STAT:
                        /* Восстановить состояние редактора */
                        if ( nportlist == 0 )
                              StateRead(ini_rline,mac_perr);
                        /* Дочитаем, если что */
                        while(ini_rline(buf,MAX_ILINE) );
                        break;
                default:
                        break;
                }
                /* Восстановим режим чтения */
                curr_ifd = ifd;
        }
        if ( ifd ) fclose(ifd);
        if ( ifname ) zFree(ifname);
        curr_ifname = NULL;
        return(1);
}

/*
 * ini_rline(buf,len) - чтение очередной строки
 * NULL - файл кончился или найдена строка "."
 */
static char *ini_rline(buf,len)
char *buf;
{
    register char *p;
    if ( !curr_ifd) return(NULL);
next:
    curr_iline++;
    p = fgets(buf,len,curr_ifd);
    if ( p == NULL || p[0] == '.' && (p[1] == 0 || p[1] == '\n') )
    {
        curr_ifd = (FILE *)0;
        return(NULL);
    }
/* Строки комментария и пустые игнорируем */
    if ( p[0] == '#' || p[0] == '\n' ) goto next;
    return(p);
}

/*
 * ErrI(kod,s);
 * - печать сообщения об ошибке на этапе инициализации
 * - kod = 0 - Warn
 * -       1 - No fatal
 *         2 - fatal
 * Если ko < 0, то это означает конец этапа инициализации
 */
static errkod = -1;
ErrI(ko, s)
int ko;
char *s;
{
    char c;
    char err_buf[256];
    char *Itoa();
    if ( ko > errkod ) errkod = ko;
    if ( ko < 0 && errkod < 0 ) return(0);
    if ( ko > 0 )
    {
        if ( !s ) s = "";
        if ( video_mode != 0 )
        {
                if ( curr_ifname )
                      sprintf(err_buf,"Err `%s'(%d): %s",curr_ifname,curr_iline,s);
                else
                      sprintf(err_buf,"Err: %s", s);
                putcha(COBELL);
                error(err_buf);
                sleep(1);
        }
        else
        {
                sprintf(err_buf,"Error in `%s'(%d): %s",curr_ifname,curr_iline,s);
                printf("%s\n",err_buf);
        }
    }
    if ( ko  < 0 && errkod == 1 )
        {
                printf("Press any key to continue!..");
                read(0,&c,1);
                errkod = 0;
        }
    return(1);
}

/*
 * Печать сообщения об ошибке
 */
static void mac_perr(s1,s2)
char *s1,*s2;
{
        ErrI(1,s1);
}

/*
 * MatchTname(ac,av) - 1, если имя терминала есть в аргументах
 */
MatchTname(ac,av)
int ac;
char **av;
{
        extern char *TtyType;  /* Здесь уже определен */
        if ( ac <= 0 ) return(1);
        while ( ac && strcmp(*av, TtyType) )
        {
                ac--;
                av++;
        }
        if ( ac > 0 ) return(1);
            return(0);
}

/*
 * MacSave(path,howstate) -
 * записать текущие макросы в файл "path"
 * берется первый элемент из списка path, причем
 * проводится подстановка расширений "%Буква"
 * howstate - как писать статус
 * 0 - не писать вообще
 * 1 - писать, но открывать только файлы, нач. с "/"
 * 2 - писать статус полностью
 */
MacSave(path,howstate)
char *path;
int howstate;
{
        char *sname;
        FILE *fd;
        extern char *next_path(), *StrCalc();
        char keybuf[LKEYSTR];
        if ( !path ) path = MacroFile;
        if ( path[0] == '!' ) {
              howstate = 2;
              path++;
              if ( !path[0] ) path = MacroFile;
        }
        sname = StrCalc(next_path( &path) );
        if ( sname) unlink(sname);    /* Без проверки */
        if ( !sname || !(fd = fopen(sname,"w")) )
        {
                sname = append(DIAG("Can't create ","Не могу создать "),sname);
                error(sname);
                zFree(sname);
                return(-1);
        }
        telluser(DIAG("MSave: ","МЗап: "),0);
        telluser(sname,6);
        zFree(sname);  /* Больше имя нам не нужно */
        MacWrite(fd);
        TabWrite(fd);
        fprintf(fd,"set keys \"%s\"\n",getkeys(keybuf,LKEYSTR-1));
        WriteSet(fd);
        if ( howstate )
              StateWrite(fd,howstate);
        fclose(fd);
        return(0);
}

/*
 * Чтение файла с макро - MacRead(path)
 *
 */
MacRestore(path)
char *path;
{
        if ( !path ) path = MacroFile;
        telluser(DIAG("MRead:","МЧтен:"),0);
        telluser(path,7);
        ReadInit(path,0);
        return;
}


/*
 * StateWrite(fd,howstate) - Запись состояния окон для
 *             - продолжения работы с того же места
 * howtstate == 0 - не делать, 1 - только файлы с полным именем,
 *                                 находящиеся прямо на окне,
 *              2 - полностью
 * Формат:
 * status
 * Либо
 * Pl,r,t,b,m,p - left,right, top,bottom,marg, prev_port
 * Либо
 * PZ,p - это ZOOM порт
 *      r < 0 - это r-LINEL, b<0 - это b-NLINES
 * fl0,c0,l,c, Имя
 * Либо то же самое с буквой F
 * ...
 * P...
 * F... Буква F означает, что нужно высветить
 * ...
 * Cномер_текущего_порта
 * ZНомер_увеличенного_порта
 * . - точка - признак конца информации
 */
StateWrite(fd,howstate)
FILE *fd;
int howstate;
{
    int i;
    register int portnum;
    char *fname;
    register struct viewport *port;
    if ( !howstate || (howstate == 1 && (nportlist == 1 || (zoomedp && nportlist == 2) ) ) ) return;
    curwksp->ccol = cursorcol;
    curwksp->crow = cursorline;
    fprintf(fd,"status\n");
    for (i=0;i<nportlist;i++)
    {
        port = portlist[i];
        if ( port == &zoomport )
                fprintf(fd,"PZ,");
        else
                fprintf(fd,"P%d,%d,%d,%d,%d,",
                        port->lmarg,
                        (port->rmarg >= LINEL-1? port->rmarg-LINEL:port->rmarg),
                        port->tmarg,
                        (port->bmarg + NPARAMLINES >= NLINES-1? port->bmarg-NLINES:port->bmarg),
                        (port->lmarg != port->ltext));
        fprintf(fd,"%d\n",port->prevport);
        curwksp = port->wksp;
        do
        {
            char c;
            curwksp = curwksp->next_wksp;
            fname = openfnames[curwksp->wfile];
            c = (port != curport && curwksp == port->wksp &&
                           (!zoomedp || port== &zoomport)
                           ?'F':'f');
            if ( !fname || (howstate < 2 && (c != 'F'||fname[0] != '/') ) ) continue;
            fprintf(fd,"%c%d,%d,%d,%d,",
                       c,
                       curwksp->ulhclno,
                       curwksp->ulhccno,
                       curwksp->crow,
                       curwksp->ccol);
            fprintf(fd," %s\n",fname?fname:"");
        }
        while ( curwksp != port->wksp );
    }
    for (portnum=0; portnum < nportlist; portnum++)
        if (portlist[portnum] == curport) break;
    fprintf(fd,"C%d\n",portnum);
    if ( zoomedp ) {
        for (portnum=0; portnum < nportlist; portnum++)
            if (portlist[portnum] == zoomedp) break;
            fprintf(fd,"Z%d\n",portnum);
    }
    fprintf(fd,".\n");
    return;
}

/*
 * StateRead(rline,err)
 * Восстановление статуса
 * Для чтения строк используется (*rline)(s,len)
 * Для печати ошибок - (*err)(s1,s2)
 */
StateRead(rline,errpt)
char *(*rline)();
int (*errpt)();
{
    char buf[MAX_ILINE];
    int int_args[10];
    register int i;
    register struct viewport *port;
    int j,l,r,t,b,m,c,l0,c0;
    char *cp;
    if ( RedKeys )
    {
      setkeys(RedKeys);
      zFree(RedKeys);
      RedKeys = NULL;
    }
    if ( nportlist > 0 ) return;
    while ( (*rline)(buf,MAX_ILINE) )
    {
        i = buf[0];
        switch (i)
        {
        case 'C':
            if(sscanf(buf+1,"%d\n",&j) != 1 ||  j < 0 || j >= nportlist )
                goto bad_args;
            switchport(portlist[j]);
            break;
        case 'P':
            if ( nportlist >= MAXPORTLIST )
            {
                (*errpt)(DIAG("Too many ports","Слишком много окон"),NULL);
                break;
            }
            if ( buf[1] == 'Z' ) {
                if ( sscanf(buf+2,"%d\n",&j ) != 1 ) j = 0;
                portlist[nportlist++] = &zoomport;
                switchport( &zoomport );
            }
            else
            {
                if ( sscanf(buf+1,"%d,%d,%d,%d,%d,%d\n",&l,&r,&t,&b,&m,&j) != 6 )
                    goto bad_args;
                if ( r < 0 ) r += LINEL;
                if ( b < 0 ) b += NLINES;
                port = portlist[nportlist++] = (struct viewport *)salloc(SVIEWPORT);
                setupviewport(port,l,r,t,b,m);
                drawport(port,DRAW_HORI);
            }
            port->prevport = j;
            break;
        case 'f':
        case 'F':
            if ( sscanf(buf+1,"%d,%d,%d,%d,%c%s\n",&l0,&c0,&l,&c,&j,buf) != 6 )
                goto bad_args;
            cp = append(buf,NULL);
            editfile(cp,l0+defplline,c0,0,(i == 'F'?1:0) );
            curwksp->ccol = c;
            curwksp->crow = l;
            poscursor(c,l);
            break;
       case 'Z':
            if ( sscanf(buf+1,"%d\n",&j ) != 1 ) goto bad_args;
            if ( j < 0 || j >= nportlist-1 ) goto bad_args;
            zoomedp = portlist[j];
            zoomflag = 1;
            break;
        default:
            (*errpt)(DIAG("Unknown status line","Недопустимая команда в статусе"));
            break;
        }
        continue;
bad_args:
        (*errpt)(DIAG("Bad args in status line","Плохие аргументы в описании статуса"),NULL);
        continue;
    }
    return;
}
