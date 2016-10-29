/*
 *      Редактор RED.
 *      Основной файл описаний для редактора RED
 *      $Header: r.defs.h,v 4.20 90/05/22 23:21:26 alex Exp $
 *      $Log:   r.defs.h,v $
 * Revision 4.20  90/05/22  23:21:26  alex
 * First rev. red 4.2
 * 
 * Revision 4.10  90/02/05  19:52:10  alex
 * Base revision 4.1
 * 
 * Revision 4.10  90/02/05  19:40:33  alex
 * Base revision 4.1
 * 
 * Revision 4.1  88/03/31  21:58:59  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 * 
 * Revision 3.1.2.5  87/07/03  22:00:43  alex
 * Graph_characters used in margin
 * 
 * Revision 3.1.2.4  87/06/29  17:54:21  alex
 * Commans set to 0200-0277
 * 
 * Revision 3.1.2.3  87/06/24  22:31:57  alex
 * New readch + gettc + tc table Tested
 * 
 * Revision 3.1.2.2  87/06/23  18:43:29  alex
 * Вынесена переменная lread1 и это отлажено
 * 
 * Revision 3.1.2.1  87/06/19  16:58:36  alex
 * Start revision for red/4
 * 
 * Revision 3.11  87/06/12  18:09:23  alex
 * Исправлены ошибки в режиме vilcase и мелочи для vt-200
 * 
 * Revision 3.10  87/06/09  19:46:04  alex
 * Исправлена работа с tty ~ VT200 и 15-ИЭ-0013
 * 
 * Revision 3.9  87/06/06  16:48:40  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.8  87/06/05  23:49:32  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.7  87/06/04  23:42:53  alex
 * Scroll on -sr- or -al/dl- added
 * 
 * Revision 3.6  87/04/02  18:33:33  alex
 * Версия для Utec слита с версией с выделением прописных букв
 * 
 * Revision 3.5  87/04/01  19:15:01  alex
 * Revision for Utec: Cs, Ce, Ct, hR
 * 
 * Revision 3.4  86/09/19  19:46:39  alex
 * Версия для СМ-1700
 * 
 * Revision 3.3  86/08/04  20:49:12  alex
 * Версия для Демос / Мнос 2
 * 
 * Revision 3.2  86/07/24  00:20:10  alex
 * Об'единены версии текстов для ЕС и СМ
 * 
 * Revision 3.1.1.6  86/07/15  22:09:24  alex
 * RED 3.3/EC.
 * 
 * Revision 3.1.1.5  86/06/19  00:09:04  alex
 * Отлажен на модели повтор сеанса
 * 
 * Revision 3.1.1.4  86/06/17  19:25:13  alex
 * Правки.
 * 
 * Revision 3.1.1.3  86/06/16  22:14:49  alex
 * Первая версия с повтором сеанса (пока грязная)
 * 
 * Revision 3.1.1.2  86/06/09  22:25:18  alex
 * TTY OUTPUT IS TESTED
 * 
 * Revision 3.1.1.1  86/06/05  00:02:19  alex
 * Отлажено_на_модели
 * 
 * Revision 3.1  86/04/20  23:39:22  alex
 * Базовая версия для ЕС.
 * 
 * Revision 3.1  86/04/20  23:39:22  alex
 * Начальная версия для ЕС.
 * 
 * Revision 3.1  86/04/20  23:39:22  alex
 * *** empty log message ***
 * 
 *      Revision 1.4  86/04/13  21:55:13  alex
 */

#ifndef DEBUG
#define WORK
#endif

#include <stdio.h>

/* Локальные параметры в ned.ddefs */
#include LOCL

#ifndef MAXFILES
#define MAXFILES 14
#endif
#ifndef MAXPORTLIST
#define MAXPORTLIST 12
#endif

#ifdef RUSDIAG
#define DIAG(l,r) r
#else
#ifdef ALLDIAG
int latdiag;
#define DIAG(l,r) (latdiag?l:r)
#else
#define DIAG(l,r) l
#endif
#endif

#ifdef WORK
#define DEBUGCHECK /* */
#else
#define DEBUGCHECK checkfsd()   /* check fsd consistency for debugging */
#endif
#undef NULL
#define NIL 0
#define EDITED 2       /* Значение openwrite, если файл редактировался */
#define NULL ((char *)0)

#define esc0 COCURS
#define esc1 '\\'
#define esc2 037
# define QUOTA '\''
# define DQUOTA '"'

#define ISACMD(c)  ((c&0300) == 0200)
#define ISMACRO(c)  ((c)>CCMAC && (c) <= CCEMAC)

#ifndef DEMOSES
#define LINELM 256                /* макс. длина строки на экране */
#define LBUFFER 512     /* *** НЕ МЕНЬШЕ  fsdmaxl  **** */
#else
#define LINELM 264                /* макс. длина строки на экране */
#define LBUFFER 1024    /* *** НЕ МЕНЬШЕ  fsdmaxl  **** */
#endif

#define NLINESM 128               /* макс. число строк на экране  */
#define NEWLINE 012     /* newline  */
#define PARAMREDIT 40           /* редактируемая часть paramport */
#define PARAMRINFO 74           /* последняя колонка для информации в paramport */
#define PARAMRLANG 74           /* Место для вывода языка ввода */
#define PARAMRMAC 77            /* Место для вывода имени макро */
#define NPARAMLINES 1           /* число строк в PARAMPORT */
#define FILEMODE 0664           /* режим доступа к создаваемым файлам */

#define FOREVER for (;;)
#define BADF -1
#define CONTF -2

/* Внимание - коды движения курсора согласованы с командами ввода */
#define VMOTCODE 4  /* коды 1 - 4 уводят курсор из текущей строки */
#define UP 1 /* Up     */
#define DN 2 /* Down   */
#define RN 3 /* Return */
#define HO 4 /* Home   */
#define RT 5 /* Right  */
#define LT 6 /* Left   */
#define TB 7 /* Tab    */
#define BT 8 /* Backtab*/

 /* Признак - не переводить курсор через границу окна */
#define MOVE_NOFIX 01000
#define MOVE_MASK   0037
/* Специальные коды для смещения окна на 1. если ролл, и dflplline, если нет*/
#define PL1MOVEW 077777
#define MI1MOVEW 077776

  /* OUTPUT CODES */

#ifndef DEMOSES
#define COSTART 0
#define COUP 1
#define CODN 2
#define CORN 3
#define COHO 4
#define CORT 5
#define COLT 6
#define COCURS 7
#define COBELL 8
#define COFIN 9
#define COERASE 10
#define COOPEN  11
#define COCLOSE 12
#define COVIOPE 13
#define COVICLO 14
#define COCYON  15
#define COCYOFF 16
#define COILINE 17
#define CODELIN 18
#define COSRFWD 19
#define COSRBAK 20
#define COCLLIN 21
#define COCLSCR 22
#define COANORMAL 23     /* Согласовано с атрибутами */
#define COAINFO   24     /* Согласовано с атрибутами */
#define COAERROR  25     /* Согласовано с атрибутами */
#define COAMARG   26     /* Согласовано с атрибутами */
#define COAOUT    27     /* Согласовано с атрибутами */
#define COGSTART  28
#define COGEND    29
#define COMCOD  30 /* Число выходных кодов */
#else
# define COSTART 0
/* #define COUP 1                       */
/* #define CODN 2                          */
/* #define CORN 3                       */
# define COHO 4
/* #define CORT 5                          */
/* #define COLT 6                       */
# define COCURS 7
# define COBELL 8
# define COFIN 9
# define COERASE 10
# define COMCOD 11 /* Число выходных кодов */
#endif

/*
 * Атрибуты символов
 * Кодируются старшими битамии символов
 */
#define NUMA(c) ((c>>8)&017) + COANORMAL   /* Номер атрибута */
#define ATR(x)  (((x-COANORMAL)&(017))<<8)             /* Получение атрибута */
/*
 * Атрибуты
 */
#define A_INFO  ATR(COAINFO)
#define A_ERROR ATR(COAERROR)
#define A_MARG  ATR(COAMARG)
#define A_OUT   ATR(COAOUT)
#define A_NORM  ATR(COANORMAL)
int need_atr;      /* Требуемые атрибуты */
/*
 * Графические символы
 */
#define G_START 0241
#define G_GH    0241  /* Гориз линия */
#define G_GF    0242  /* Заполненный квадрат */
#define G_GV    0243  /* Верт. линия */
#define G_LD    0244  /*                                */
#define G_MD    0245  /*     LU MU  RU                  */
#define G_RD    0246  /*     LM CC  RM                  */
#define G_LM    0247  /*     LD MD  RD                  */
#define G_CC    0250  /*     - GH                       */
#define G_RM    0251  /*     | GV                       */
#define G_LU    0252  /*                                */
#define G_MU    0253  /*                                */
#define G_RU    0254  /*                                */
#define G_KU    0255  /* Стрелка вверх                  */
#define G_KD    0256  /* Стрелка вниз                   */
#define G_KL    0257  /* Стрелка влево                  */
#define G_KR    0260  /* Стрелка вправо                 */
#define G_END   0260
/* margin characters  ( см. B_MARG(col) там, где опис. табуляции ) */
#define LMCH  G_GV
#define RMCH  G_GV
#define TMCH  G_GH
#define BMCH  G_GH
#define BMTCH G_KU      /* BMCH в позиции табуляции ( при +T ) */
#define MRMCH G_KR
#define MLMCH G_KL
#define ELMCH G_KD
#define DOTCH G_GF
#define AMCH  G_KU

/* struct fsd -
 * Описатель сегмента файла. Описывает от 1 до 127 строк файла,
 * записанных подряд. Это минимальная компонента цепочки описателей
 */
#ifdef UN_CHAR
typedef short schar;
#else
typedef char schar;
#endif

#define FSDMAXL 127     /* Макс. число строк в fsd */

struct fsd
        {
        struct fsd *backptr;    /* Ссылка на пред. fsd   */
        struct fsd *fwdptr;     /* Ссылка на след. fsd   */
        char fsdnlines;         /* Число строк, которые описывает fsd */
                                /* 0 , если это конец цепи. */
        schar fsdfile;          /* Дecкpиптop фaйлa, 0, ecли этo кoнeц цeпи */
        int seekhigh;           /* Сдвиг в файле ( из двух слов): блок */
        int seeklow;            /*                              : символ */
        char fsdbytes;  /* Переменное число байт - столько, сколько нужно
                        для того, чтобы описать очередные fsdnlines-1 строк:
                        Интерпретация очередного байта:
                        1-127   смещение следующей строки от предыдущей
                        0       здесь помещается пустая строка
                        128+n   след. строка начинается с n*128+next байта
                                после начала предыдущей строки.
                        Имеется по меньшей мере fsdnlines-1 байтов, но может
                        быть и больше, если есть длинные строки
                        Отметим, что в принципе в одном fsd можно описать и
                        несмежные строки, но такая возможность не учтена
                        в функциях записи файла.               */
        };
/* Урезанный вариант - без fsdbyte */
struct fsdsht {struct fsd *backptr, *fwdptr;
               char fsdnlines; schar fsdfile;
               int seekhigh, seeklow;};
#define SFSD (sizeof (struct fsdsht))

struct fsd *openfsds[MAXFILES];
char *openfnames[MAXFILES],
     openwrite[MAXFILES],       /* 1 - файл можно записывать */
     movebak[MAXFILES];         /* 1 - файл уже двигали в .bak */
int  nlines[MAXFILES];          /* Число непустых строк в файле */

/* workspace - структура, которая описывает файл */
struct workspace
        {
        struct fsd *curfsd;     /* ptr на текущий fsd */
        int ulhclno;            /* номер строки, верхней на экране */
        int ulhccno;            /* номер колонки, которая 0 на экране */
        int curlno;             /* текущий номер строки */
        int curflno;            /* номер строки, первой в curfsd */
        char ws_flags;             /* не использовано        */
        char wfile;             /* номер файла, 0 - если нет вообще */
        int ccol;               /* curorcol, когда не активен */
        int crow;               /* cursorline, когда неактивен */
        struct workspace *next_wksp; /* Альтернативное workspace */
        };
#define SWKSP (sizeof (struct workspace))
#define ABS_LIN(l) ((l)+curwksp->ulhclno)
#define ABS_COL(c) ((c)+curwksp->ulhccno)
#define REL_LIN(l) ((l)-curwksp->ulhclno)
#define REL_COL(c) ((c)-curwksp->ulhccno)

struct workspace *curwksp, *pickwksp;
int curfile;


/* viewport - описатель окна на экране терминала
 * все координаты на экране, а также ltext и ttext, измеряются по отношению
 *      к (0,0) = начало экрана. Остальные 6 границ приводятся по отношению
 *      к ltext и ttext.
 *      Ниже границы, приведенные в координатах окна, помечены *w, а
 *      приведенные в координатах всего экрана - *s
 */

#define SVIEWPORT (sizeof (struct viewport))

struct viewport
        {
        struct workspace *wksp; /* Ссылка на workspace         */
        char prevport;          /* Номер пред. окна   */
        char flags;             /* Флаги / признаки окна */
        int ltext;              /*s Границы  текста (по отн. к 0,0) */
        int rtext;              /*w Длина в ширину                  */
        int ttext;              /*s Верхняя граница                 */
        int btext;              /*w Высота окна                     */
        int lmarg;              /*s границы окна == ..text или +1 */
        int rmarg;              /*s Левая, правая, верхняя, нижняя */
        int tmarg;              /*s */
        int bmarg;              /*s */
        int ledit;              /*w область редактирования в окне */
        int redit;              /*w по отношению к .text: начало, длина */
        int tedit;              /*w */
        int bedit;              /*w */
        char *firstcol;         /* Номера первых колонок, !=' '  */
        char *lastcol;          /*  -//-  последних -//-         */
        char *lmchars;          /* символы - левые ограничители  */
        char *rmchars;          /* символы - правые ограничители */
        };

struct viewport *portlist[MAXPORTLIST+1],
                *curport,       /* текущее окно */
                *zoomedp,       /* Окно, кот. было увеличено */
                wholescreen,    /* весь экран   */
                paramport,      /* окно для параметров */
                zoomport;       /* Большое окно - help и zoomfile */
int nportlist;
/* Параметры некоторых окон */
#define ZOOML   (NLINES - NPARAMLINES)
#define L_HELP ZOOML
#define helpport zoomport
/*
 * Описания флагов для окна
 * ROLL - 0 / неизвестно, Y - да, N - roll невозможен
 */
#define WF_ROLL  03
#define WF_YROLL 01
#define WF_NROLL 02

/*
 * Флаги для drawport
 */
#define DRAW_HORI 0
#define DRAW_TABS 1
#define DRAW_ALL  2

/* savebuf - структура, которая описывает буфер вставок */

#define SSAVEBUF (sizeof (struct savebuf))

struct  savebuf
        {
        int linenum;    /* Номер первой строки в "#" */
        int nrows;      /* Число строк               */
        int ncolumns;   /* Число колонок             */
        };
struct savebuf *pickbuf, *deletebuf;

/*
 * Управляющие символы
 */
#define CC                0200     /* Начало упр. кодов внутри red */
#define CCQUIT             000+CC     /* terminate editor run */

#define CCBACKTAB          BT+CC      /* tab left             */
#define CCHOME             HO+CC      /* home cursor          */
#define CCMOVEDOWN         DN+CC      /* move down 1 line     */
#define CCMOVELEFT         LT+CC      /* backspace            */
#define CCMOVERIGHT        RT+CC      /* forward move         */
#define CCMOVEUP           UP+CC      /* move up 1 lin        */
#define CCRETURN           RN+CC      /* return               */
#define CCTAB              TB+CC      /* tab                  */

#define CCGOTO             011+CC     /* goto linenumber      */
#define CCPLPAGE           012+CC     /* minus a page         */
#define CCMIPAGE           013+CC     /* plus a page          */
#define CCPLLINE           014+CC     /* minus a line         */
#define CCMILINE           015+CC     /* plus a line          */
#define CCLPORT            016+CC     /* port left            */
#define CCRPORT            017+CC     /* port right           */
#define CCPLSRCH           020+CC     /* plus search          */
#define CCMISRCH           021+CC     /* minus search         */
#define CCTABS             022+CC     /* set tabs             */
#define CCINSERT           023+CC     /* insert mode          */
#define CCINSMODE          024+CC     /* insert mode          */
#define CCCHPORT           025+CC     /* change port          */
#define CCOPEN             026+CC     /* insert               */
#define CCCLOSE            027+CC     /* delete               */
#define CCPUT              030+CC     /* put                  */
#define CCPICK             031+CC     /* pick                 */
#define CCDELCH            032+CC     /* character delete     */
#define CCBACKSPACE        033+CC     /* backspace and erase  */
#define CCCTRLQUOTE        034+CC     /* knockdown next char  */
#define CCTRUNCATE         035+CC     /* обрезать (пока только для lineop) */
#define CCENTER            036+CC     /* enter parameter      */
#define CCRESCREEN         037+CC     /* обновить экран*/
#define CCSETFILE          040+CC     /* set file             */
#define CCMAKEPORT         041+CC     /* make a viewport      */
#define CCSAVEFILE         042+CC     /* make new file        */
#define CCDOCMD            043+CC     /* execute a filter     */
#define CCHELP             044+CC     /* Help!                */
#define CCDEFMACRO         045+CC     /* Define Macro - '0' по умолч */
#define CCSAVEMACRO        046+CC     /* Save Macro           */
#define CCREADMACRO        047+CC     /* Read Macro           */
#define CCZOOMPORT         050+CC     /* ZOOM / UNZOOM Window */
#define CCBEGIN            051+CC     /* Cursor Begin         */
#define CCEND              052+CC     /* Cursor End           */
#define CCPSEARCH          053+CC     /* Parent Search        */
#define CCWORDLEFT         054+CC     /* Word Left */
#define CCWORDRIGHT        055+CC     /* На слово вправо */
#define CCWORDSELECT       056+CC     /* Отметить слово  */
#define CCWORDGET          057+CC     /* Взять слово в PARAM */
#define CCUNDO             060+CC     /* Восстановить измененную строку */
#define CCLINEJOIN         061+CC     /* Слить строки */
#define CCLINESPLIT        062+CC     /* Расщепить строку на две */
#define CCSELECT           063+CC     /* Начало/конец выборки текста */


#define CCINTRUP           077+CC     /* interuption (for ttyfile)     */

#ifdef DEMOSES
#define CCICURSOR         0076+CC  /* зaпиcь пoлoж. кypcopa */
#define CCILINE           0075+CC  /* нaчaлo oпиcaния cтpoки */
#define CCELINE           0074+CC  /* кoнeц oпиcaния cтpoки */
/* #define CCBACKSPACE       0244     /* backspace and erase  */
#endif

#define CCMAC             1     /* macro marka         */
                                /* 2 == $a, 27 == $z   */
#define CCLASTMACRO       28    /* $0 - Неименованный макро */
#define CCMACWORD         29    /* $1 - взятое слово        */
#define CCMACINIT         30    /* $2 - макро для нового файла */

#define CCEMAC            31    /* Последний код макро */

/*  */

int cursorline;         /* physical screen position of */
int cursorcol;          /*  cursor from (0,0)=ulhc of text in port */


extern char  cntlmotions[];

# ifndef MAX_IARGS
# define MAX_IARGS   30
# endif
/* Макс. длина строки определения ключей */
#define LKEYSTR 64

/* Макро для работы по битовым шкалам */
#define SCALE long
#define LSCALE(l) (((l)>>5)+1)
#define ISBIT(s,i)  (s[ (i) >>5] &  (1l<< ((i)&037) ) )
#define SETBIT(s,i) (s[ (i) >>5] |= (1l<< ((i)&037) ) )
#define CLRBIT(s,i) (s[ (i) >>5] &= ~(1l<<((i)&037) ) )

# define MAXTCOL 640


extern SCALE tabstps[LSCALE(MAXTCOL)];
#define ISTAB(i)   ISBIT(tabstps,i)
#define SETTAB(i) SETBIT(tabstps,i)
#define CLRTAB(i) CLRBIT(tabstps,i)
extern int deftabstep;
extern int chg_tabs;
extern int showtabs;
#define B_MARG(col) (showtabs && col < MAXTCOL && ISTAB(col) ? BMTCH : BMCH)

char blanks[LINELM];

extern int readch();             /* Текущий входной символ, -1 - дай еще! */
char intrflag; /* 1 - был сигнал INTERUP */

/* Умолчания */
extern int defplline,defplpage,defmiline,defmipage,deflport,defrport,
        definsert, defdelete, defpick;
extern char *deffile;
int defreturn;

int errsw;    /* 1 - в окне параметров сообщение об ошибке */

/*
 * Глобальные параметры для param():
 * paraml - длина параметра;
 * char *paramv - сам параметр,
 * paramtype - тип параметра,
 * paramc0, paramr0, paramc1< paramr1 -
 * размеры области при "cursor defined"
 */
int paraml;
char *paramv;int paramtype;
int paramc0, paramr0, paramc1, paramr1;

/*
 * cline 0- массив для строки, lcline - макс. длина,
 * ncline - текущая длина, icline  - следующее приращение
 * длины
 * fcline - флаг (было изменение), clineno - номер строки в файле
 */
char *cline;
int lcline;
int ncline;
int icline;
char fcline;
int clineno;
int clineexist;

/*
 * Описатели файлов:
 * tempfile, tempfh, tempfl - дескриптор и
 * смещение во временном файле
 * ttyfile - дескриптор файда протокола
 * inputfile - дескриптор файла ввода команд из протокола
 */
int tempfile;
int tempfh;
int tempfl;
int ttyfile;
int inputfile;

/*
 * Oпиcaния для зaпиcи пpoтoкoлa
 */
#define PUTP(c) put1c((char)c,ttyfile)

extern char *ps_inmac;
char *searchkey;

int userid,groupid;

char *tmpname;         /* name of file, for do command */

/* Управление выводом на терминал */
int LINEL,NLINES; /* SIZE OF THE SCREEN  - VT.C */
int eolflag;
extern char *curspos,*cvtout[];
extern video_mode; /* 0 - обычный режим, 1 - экранный режим */
extern cur_atr; /* Текущий атрибут текста */

#ifndef DEMOSES
char *(*agoto)();  /* for termcap definitions */
int (*atcread)();
int slowsw;   /* is 1 if slow speed terminal */
int lcasef, latf, flgtabs; /* terminal regim flags */
int kioutf; /* Не 0 если 15IE0013  с непереключенной клавиатурой*/
#ifdef RED_CYRILL
int Rcyflag;
#endif
int on_scroll; /* Флаг, разрешающий roll */
int fix_screen; /* Флаг запрещает автоматическую прокрутку экрана */
int on_autoi;   /* Флаг включехия авто-сдвига */
int autoindent; /* Auto-indent */
#else
int rawf, lcasef, latf, flgtabs, speccase; /* terminal regim flags */
#endif
int new_info; /* Признак - перерисовать info */
int need_box; /* Признак - перерисовать рамку */

char *append(), *salloc();
char *s2i();
struct fsd *file2fsd();
#ifdef DEMOSES
int newedit; /* Флаг - изменялось окно ввода */
int dflinsert;
int silentflag; /* He выдaвaть нa тepминaл */
extern int werrflag;
#endif
extern char **next_args; /* Очередной свободный аргумент */
int zoomflag;            /* Признак "zoom" для повтора по умолчанию */
/*
 * Переменные для режима выборки текста
 * флаг, абс. координаты, номер файла
 */
int sel_flag;
int sel_lin, sel_col, sel_file;
struct workspace *sel_wspace;
extern int defedit;
