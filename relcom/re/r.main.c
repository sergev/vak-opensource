/*
 *      Редактор RED. ИАЭ им. И.В. Курчатова, ОС ДЕМОС
 *
 *      $Header: r.main.c,v 4.20 90/05/22 23:22:10 alex Exp $
 *      $Log:   r.main.c,v $
 * Revision 4.20  90/05/22  23:22:10  alex
 * First rev. red 4.2
 * 
 * Revision 4.10  90/02/05  19:52:30  alex
 * Base revision 4.1
 *
 * Revision 4.10  90/02/05  19:49:18  alex
 * Base revision 4.1
 *
 * Revision 4.1  88/03/31  22:04:06  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 *
 * Revision 3.1.2.6  87/07/09  20:42:35  alex
 * REVISION
 *
 * Revision 3.1.2.5  87/07/03  22:02:45  alex
 * Graph_characters used in margin
 *
 * Revision 3.1.2.4  87/06/24  22:34:59  alex
 * New readch + gettc + tc table Tested
 *
 * Revision 3.1.2.3  87/06/23  20:51:36  alex
 * Automatic_revisions
 *
 * Revision 3.1.2.2  87/06/23  18:51:57  alex
 * wYNESENA PEREMENNAQ lread1 I \TO OTLAVENO
 *
 * Revision 3.1.2.1  87/06/19  17:01:14  alex
 * Start revision for red/4
 *
 * Revision 3.16  87/06/12  18:10:36  alex
 * Исправлены ошибки в режиме vilcase и мелочи для vt-200
 *
 * Revision 3.15  87/06/06  16:49:20  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 *
 * Revision 3.14  87/06/05  23:50:59  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 *
 * Revision 3.13  87/06/04  23:44:05  alex
 * Scroll on -sr- or -al/dl- added
 *
 * Revision 3.12  87/04/21  22:46:13  alex
 * Debug
 *
 * Revision 3.11  87/04/02  18:32:07  alex
 * Версия для Utec слита с версией с выделением прописных букв
 *
 * Revision 3.10  87/04/01  19:13:58  alex
 * Revision for Utec: Cs, Ce, Ct, hR
 *
 * Revision 3.9  86/11/25  08:06:07  alex
 * Исправлено ioctl(..,TIOCFLUSH,&flag)
 *
 * Revision 3.8  86/10/29  02:45:11  root
 * Версия от 27 октября.
 *
 * Revision 3.7  86/10/14  23:02:21  alex
 * Версия.
 *
 * Revision 3.6  86/09/19  21:38:58  alex
 *
 *
 * Revision 3.5  86/09/19  19:54:51  alex
 * Версия для СМ-1700
 *
 * Revision 3.4  86/08/04  20:52:34  alex
 * Bepqh dk LMNQ/DELNQ 2
 *
 * Revision 3.3  86/07/24  23:37:38  alex
 * Ob}aq wers
 * Общая версия названа 3.3
 *
 * Revision 3.2  86/07/24  19:04:32  alex
 * Об'единены версии текстов для ЕС и СМ
 *
 * Revision 3.1.1.6  86/07/15  22:13:04  alex
 * RED 3.3/EC.
 *
 * Revision 3.1.1.5  86/06/20  23:33:40  alex
 * Введен для отладки на СМ ключ "C" - не устанавливать режимы терминала.
 * Версия отлажена и получила номер 3.2 (пока на модели).
 *
 * Revision 3.1.1.4  86/06/16  22:19:26  alex
 * Первая версия с повтором сеанса (пока грязная)
 *
 * Revision 3.1.1.2  86/06/06  00:14:36  alex
 * Введена установка ключей
 *
 * Revision 3.1.1.1  86/06/05  00:05:55  alex
 * Отлажено_на_модели
 *
 * Revision 3.1  86/04/20  23:42:22  alex
 * Базовая версия для ЕС.
 * Revision 1.5  86/04/13  21:56:42  alex
 * .
 *
 *
 *  Главная программа - вход/выход, открытие окон,
 *  разбор параметров
 *
 */


/*
 * Аргументы:
 *
 *      red [ОПЦИИ] файл [номер строки]
 *
 *      либо
 *      red [ОПЦИИ] -R
 *      (повтор сеанса)
 *      либо
 *      red [ОПЦИИ] -r файл_протокол
 *      (повтор сеанса из файла протокол)
 *      либо
 *      red  [ОПЦИИ]
 *      (редактировать с того же места)
 *
 *      ОПЦИИ:
 *      -R - повтор сеанса
 *      -r имя_файла - повтор сеанса из файла
 *      -k-ключи+ключи - установка или снятие ключей
 *      -f - подавить чтение инициализационных файлов
 *      -i инит_файл - явное указание инит-файла или пути их
 *         поиска, перечисленного через :.
 *      -m - подавить чтение макро-файла
 *      Ключи могут задаваться через переменную REDKEYS. Описание ключей
 *      начинается со знака "-" или "+".
 */
#include "r.defs.h"
#include <signal.h>

#ifndef lint
static char *sccsid="@(#) RED / DEMOS-EC/SM $Header: r.main.c,v 4.20 90/05/22 23:22:10 alex Exp $";
#endif

/* Исходные (нерасширенные) имена рабочих файлов */
extern char *TTYnm ;
extern char *TTYtmp;
extern char *RFile ;
extern char *InitPath ;
extern char *HelpPath ;
extern char *RedKeys;
/* Имя файла умолчания для запоминания и считывания макро */
extern char *MacroFile ;

/* Вычисленные имена рабочих файлов */
extern char *ttynm, *ttytmp, *rfile;

struct savebuf pb,db;
int sig(),igsig(),testsig();
long lseek();
#ifdef SIGNOSPC
int werrsig();
int werrflag;
#endif
extern char *getvers();
static startup();

/* Переменная - признак того, что нач. установки прошли */
extern int chg_flag;

/*
 * Описания для getopt
 */
extern char *optarg;
extern int optind, opterr;
char **next_args;
/*
 * main(nargs,args)
 * Головная программа
 */
main(nargs,args)
int nargs;
char **args;
{
    int WorkMode;
    int c;
    char *cp, ichar,*getenv();
    char *arg0 = args[0];
    int no_macro = 0;
    WorkMode=0; /* режим работы 0 - норм, 1 - повторный, 2 - из файла %wtt.. */
    setbuf(stdout,NULL);
    /*
    pt_info();
    */
#ifdef ALLDIAG
    latdiag = ((cp = getenv ("MSG")) && *cp == 'l');
#endif
    /* Инициализировать переменные через ENVIRONMENT */
    VarEInit();
    if ( nargs == 2 && strcmp(args[1],"-") == 0 )
    {
                      WorkMode = 2; nargs--; args++;
    }
    while ( (c = getopt(nargs, args, "Rmr:k:fi:") ) >= 0 )
        switch ( c) {
        case 'f': InitPath = ""; break;
        case 'm': no_macro = 1; break;
        case 'R': WorkMode = 2;  break;
        case 'r': cp = optarg;
            if ((inputfile = open( cp,0)) < 0)
            {
                    printf("Can't open command file %s.\n",cp);
                    exit(1);
            }
            break;
        case 'k':
            RedKeys = append(RedKeys,optarg);
            break;
        case 'i':
            InitPath = optarg;
            break;
        case '?':
            printf("Using:\n");
            printf("%s [options] [files] - call editor\n%s - call with old state\n%s -R - repeat edition after system crash\n",arg0,arg0,arg0);
            printf("options:\n");
            printf("       -f - no read init file\n");
            printf("       -i path - change init_file_path to path\n");
            printf("       -m - no read macro/status file\n");
            printf("       -k -keys1+keys2 - unset keys1 and set keys2\n");
            printf("       -r file - read seans protocol from file\n");
            exit(1);
        }
    args += (optind -1);
    nargs -= (optind-1);
    if (WorkMode == 0 && (nargs <= 1)) {
        WorkMode=1;
        ichar='!';
    }
    else ichar = ' ';
    startup(WorkMode);
    if ( inputfile )
    {
        if (read(inputfile,&ichar,1) <= 0)
        {
            cleanup();
            printf("Command file is empty.\n");
            exit(1);
        }
    }
    else
        PUTP(ichar); /* Для пoвтopa ceaнca */
    if ( !no_macro )
          ReadInit(ichar == '!'? rfile : MacroFile);
    if ( nportlist == 0 ) makestate();
    next_args = &args[1];
    if (nargs > 1 && *args[1] != '\0')
    {
        extern char *Itoa();
        int i;
        i =  1;
        next_args++;
        if ( (nargs > 2) )
        {
              if ( s2i(args[2],&i) )
              {
                      i = 1;
              }
              else next_args++;
        }
        poscursor(curwksp->ccol,curwksp->crow);
        if (editfile(args[1],i-1,0,1,1)  <= 0 )
        {
            out_lines(0,-1);
            poscursor (curwksp->ccol,curwksp->crow);
        }
        else
        {
            writefile(CCENTER,args[1],CCSETFILE);
            writefile(CCENTER,Itoa(i),CCGOTO);
        }
    }
    else
    {
        out_lines(0,-1);
        poscursor (curwksp->ccol,curwksp->crow);
    }
    telluser( getvers(),0);
    mainloop();
    MacSave(rfile,2); /* Записать выходное состояние */
    cleanup();
    dumpcbuf(0);
    /*
    pt_info();
    */
    exit(0);
}

pt_info()
{
    printf("\n\n   In edit session press HELP or CTRL+\\ for HELP \n   %s\n",getvers());
}

char *ttyname();
int oldttmode;
/*
 * startup(mode) - Инициализация режимов и файлов
 */
static startup(f)
int f; /* 2 - повторить сеанс из ttyfile ;
        * 1 - начать заново
        */
{
    register int i;
    register char *c, *name;
    char *StrCalc();
    NICE; /* Установить приоритет (опред. в ned.?defs) */
    userid = GETUID;
    groupid = GETGID;
    setuid(userid);
    setgid(groupid);
    InitConsts();
    InitTabs();
    if ( RedKeys ) {
      setkeys(RedKeys);
      zFree(RedKeys);
      RedKeys = NULL;
    }
    /* Читаем инициализационный файл. */
    ReadInit(InitPath,1);
    /* Все, что меняется далее - нужно запоминать!! */
    chg_flag = 1;
    /* Если были ошибки, фиксируем сообщение об них */
    ErrI(-1,NULL);
    /* Устанавливаем режимы терминала */
    ttstartup();
    for (i=LINEL; i;) blanks[--i] = ' ';
    tmpname = StrCalc(TTYtmp);
    ttytmp  = StrCalc(TTYnm);
    rfile   = StrCalc(RFile);
    /* Устанавливаем описатель всего экрана. Здесь, так как нужно выдавать */
    setupviewport(&wholescreen,0,LINEL-1,0,NLINES-1,0);
    curport = &wholescreen;
    if ( f != 2 && close(open(tmpname,0)) >= 0 )
       {
              char buf[128];
              putcha(COERASE);
              poscursor(0,0);
              info(DIAG("Warning: old session was aborted possibly!","Предупреждение: предыдущий сеанс, похоже, кончился сбоем"),LINEL);
              poscursor(0,1);
              info(DIAG("You can repeat session by calling editor vith option -R","Вы можете повторить сеанс, вызвав редактор с ключом -R"),LINEL);
              poscursor(0,2);
              info(DIAG("Before you must to chdir to the same directory","Перед этим нужно перейти в тот же каталог, где выполнялся сбившийся сеанс"), LINEL);
              poscursor(0,3);
              info(DIAG("If you continue now, old session would be lost!","Если вы продолжите, сеанс будет потерян"),LINEL);
              buf[0] = 0;
              do
              {
                     poscursor(0,4);
                     info(DIAG("Enter: 1 - to cont, 0 - to abort ...","Введите: 1 - чтобы продолжить, 0 - чтобы выйти немедленно..."),LINEL);
                     dumpcbuf();
              }
               while(read(0,buf,127) > 0  && buf[0] != '0' && buf[0] != '1');
               if ( buf[0] != '1' )
               {
                      ttcleanup();
                      exit(1);
               }
               putcha(COERASE);
       }
    if ((i = (unlink(tmpname),creat(tmpname,0600))) < 0)
    {
        printf("Can't open temporary file %s.\n",tmpname);
        exit(1);
    }
    /*  Рабочий файл нужен на read/write - приходится переоткрыть */
    close(i);
    i = open(tmpname,2);
    openfnames[i] = tmpname;
    openwrite[i] = 1;
    tempfile = i;
    /* Теперь разберемся с файлом протокола */
    if (f) {
        ttyfile=open(ttytmp,2);
        if( ttyfile>=0 && f !=2) close(ttyfile);
    }
    if (f != 2) {
        unlink(ttytmp);
        ttyfile=creat(ttytmp,FILEMODE);
    }
    else inputfile = ttyfile;
    if (ttyfile <0) {
        printf("Can't create protocol file %s\n\r",ttytmp);
        ttyfile = -1;
    }
    /* файл '#' -- запоминание убранного и отмеченного  */
    openfnames[2] = "#";
    nlines[2] = 0;
    pickwksp = (struct workspace *)salloc(SWKSP);
    pickwksp->curfsd = openfsds[2] = (struct fsd *)salloc(SFSD);
    pickwksp->wfile = 2;
    pickbuf = &pb;
    deletebuf = &db;
    if ( RedKeys ) {
      setkeys(RedKeys);
      zFree(RedKeys);
      RedKeys = NULL;
    }
    /* Устанавливаем окно для HELP и т.п. */
    setupviewport( &zoomport, 0, LINEL-1, 0, ZOOML-1,0);
    /* Устанавливаем описатель окна параметров */
    setupviewport(&paramport,LMARG-1,LINEL-LMARG,NLINES-NPARAMLINES,NLINES-1,0);
    /* paramport.rtext -= 3;    */
    paramport.redit = PARAMREDIT - 5 - LMARG*2;
    /* Закрываем терминал на прием сообщений от других */
    /*
     * Переопределяем сигналы
     */
    for (i=SIGTERM; i; i--) signal(i,sig);
    signal(SIGINT,testsig);
    signal(SIGQUIT,igsig);
#ifdef SIGSTOP
    signal(SIGTSTP,SIG_IGN);
    signal(SIGCONT,SIG_IGN);
#endif
#ifdef SIGNOSPC
    signal(SIGNFILE,werrsig);
    signal(SIGNOSPC,werrsig);
#endif
#ifdef SV_INTERRUPT
    /* Открыть срыв чтения после сигнала прерывания */
    {
      struct sigvec veci;
      sigvec(SIGINT,(struct sigvec *)0, &veci);
      veci.sv_flags |= SV_INTERRUPT;
      sigvec(SIGINT, &veci, (struct sigvec *)0);
    }
#endif
    curport = &wholescreen;
    putcha(COSTART);
    putcha(COHO);
    dumpcbuf();
    excline(0);
    return;
}

/*
 * cleanup() -
 * Почиститься перед выходом
 */
cleanup()
{
        /* restore tty mode and exit */
        ttcleanup();
        close(tempfile);
        unlink(tmpname);
        close(ttyfile);
}

/*
 * makestate() -- создание начального состояния окон
 */
makestate()
{
        register struct viewport *port;
        if ( RedKeys )
        {
          setkeys(RedKeys);
          zFree(RedKeys);
          RedKeys = NULL;
        }
        nportlist = 1;
        port = portlist[0] = (struct viewport *)salloc(SVIEWPORT);
        setupviewport(portlist[0],0,LINEL-1,0,NLINES-NPARAMLINES-1,1);
        drawport(port,DRAW_HORI);
        if ( zoomflag )
        {
                port = portlist[nportlist++] = &zoomport;
                zoomedp = portlist[0];
                switchport(port);
        }
        poscursor(0,0);
}

/*
 * sig() -
 * Фатальный сигнал
 */
static char fat_sig[] = "..: Fatal signal\n";
sig(n)
{
fat_sig[0] = n/10+'0';
fat_sig[1] = n%10+'0';
fatal(fat_sig);
}

#ifdef SIGNOSPC
/* werrsig() - oбpaбoтaть oш. зaпиcи    */
werrsig()
{
        werrflag = 1;
        signal(SIGQUIT,werrsig);
        signal(SIGINT,testsig);
        signal(SIGNFILE,werrsig);
        signal(SIGNOSPC,werrsig);
}
#endif

/* igsig() - Установить игнорирование */
igsig()
{
        signal(SIGQUIT,igsig);
#ifdef SIGNOSPC
        signal(SIGNFILE,igsig);
        signal(SIGNOSPC,igsig);
#endif
}

/*
 * testsig() -
 * проверить, не было ли прерывания
 */
testsig()
{
/* В BSD 4.3 прерывание не сбрасывается */
#ifndef SV_INTERRUPT
        signal(SIGINT,igsig);
#endif
        if(intrflag) fatal(DIAG("RED WAS INTERUPTED\n","RED прерван\n"));
        igsig();
        intrflag=1;
}

/*
 * fatal(s) -- Печать аварийного состояния
 */
fatal(s)
char *s;
{
    putcha(COFIN);
    putcha(COBELL);
    putcha(COBELL);
    dumpcbuf(0);
    ttcleanup();
    printf(DIAG("\nFirst the bad news - the RED just    ","\n Сначала огорчим: Red слетел:"));
    if (s) {
        printf(DIAG("died:\n%s\n","сбой: \n%s\n"),s);
    }
    else printf(DIAG("ran out of space.\n","Исчерпал оперативную память"));
    if(userid%51==0) printf("\n ReD - KiAe MoScOw RuDnEv A.p.\n");
    printf(DIAG("\n Now the good news - your editing session can be reproduced\n  from file ","\n Сеанс можно воспроизвести из файла:"));
    printf(ttytmp);
    printf(DIAG("\n use command 're -R' to do it.\n","\nиспользуйте команду 're -R'\n"));
#ifndef WORK
    if (inputfile || (!isatty(1)) )
              dumpfsd();
#endif
    close(ttyfile);
    exit(1);
/*    abort(1);       */
}
/*   Руднев А.П. Москва, ИАЭ им. Курчатова, 1984 */

