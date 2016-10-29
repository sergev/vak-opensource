/*
 *      Редактор RED. ИАЭ им. И.В. Курчатова, ОС ДЕМОС
 *      Основной файл описаний для редактора RED
 *      $Header: /home/sergev/Project/vak-opensource/trunk/relcom/nred/RCS/r.defs.h,v 3.1 1986/04/20 23:39:22 alex Exp $
 *      $Log: r.defs.h,v $
 *      Revision 3.1  1986/04/20 23:39:22  alex
 *      *** empty log message ***
 *
 *      Revision 1.4  86/04/13  21:55:13  alex
 *
 *
 *
 *
 */

#ifndef DEBUG
#define WORK
#endif

#ifndef WORK
#include <stdio.h>
#endif

/* Локальные параметры в ned.ddefs */
#include "r.local.h"

#ifdef RUSDIAG
#define DIAG(l,r) r
#else
#define DIAG(l,r) l
#endif


#ifdef WORK
#define printf printf1
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
#define esc2 (('#'|'\200')&0377)

#define CONTROLCHAR (lread1 < 040)
#define CTRLCHAR   (((lread1>=0)&&(lread1<040)) || ((lread1 >= 0177)&& (lread1<=0240)))
#define LINELM 128                /* макс. длина строки на экране */
#define NLINESM 48                /* макс. число строк на экране  */
#define LBUFFER 256     /* *** НЕ МЕНЬШЕ  fsdmaxl  **** */
#define NEWLINE 012     /* newline  */
#define PARAMREDIT 40           /* редактируемая часть paramport */
#define PARAMRINFO 78           /* последняя колонка в paramport */
#define NPARAMLINES 1           /* число строк в PARAMPORT */
#define FILEMODE 0664           /* режим доступа к создаваемым файлам */
#define NTABS 30
#define BIGTAB 32767

#define FOREVER for (;;)
#define BADF -1
#define CONTF -2

#define VMOTCODE 4  /* коды 1 - 4 уводят курсор из текущей строки */
#define UP 1 /* Up     */
#define DN 2 /* Down   */
#define RN 3 /* Return */
#define HO 4 /* Home   */
#define RT 5 /* Right  */
#define LT 6 /* Left   */
#define TB 7 /* Tab    */
#define BT 8 /* Backtab*/

  /* OUTPUT CODES */

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
#define COMCOD 11 /* Число выходных кодов */

/* margin characters */
#define LMCH  '!'
#define RMCH  '!'
#define TMCH   '-'
#define BMCH   '-'
#define MRMCH '>'
#define MLMCH '<'
#define ELMCH ';'
#define DOTCH '+'

/* struct fsd -
 * Описатель сегмента файла. Описывает от 1 до 127 строк файла,
 * записанных подряд. Это минимальная компонента цепочки описателей
 */
#define FSDMAXL 127     /* Макс. число строк в fsd */

struct fsd
        {
        struct fsd *backptr;    /* Ссылка на пред. fsd   */
        struct fsd *fwdptr;     /* Ссылка на след. fsd   */
        char fsdnlines;         /* Число строк, которые описывает fsd */
                                /* 0 , если это конец цепи. */
        char fsdfile;           /* Дескриптор файла, 0, если это конец цепи */
        int seekhigh;           /* Сдвиг в файле ( из двух слов): блок */
        int seeklow;            /*                              : символ */
        char fsdbytes;  /* Переменное число байт - столько, сколько нужно
                        для того, чтобы описать очередные fsdnlines-1 строк:
                        Интерпретация очередного байта:
                        1-127   смещение следующей строки от предыдущей
                        0       здесь помещается пустая строка
                        -n      след. строка начинается с n*128+next байта
                                после начала предыдущей строки.
                        Имеется по меньшей мере fsdnlines-1 байтов, но может
                        быть и больше, если есть длинные строки
                        Отметим, что в принципе в одном fsd можно описать и
                        несмежные строки, но такая возможность не учтена
                        в функциях записи файла.               */
        };
/* Урезанный вариант - без fsdbyte */
struct fsdsht {struct fsd *backptr, *fwdptr;
               char fsdnlines; char fsdfile;
               int seekhigh, seeklow;};
#define SFSD (sizeof (struct fsdsht))
#define MAXFILES 14

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
        char flags;             /* не использовано        */
        char wfile;             /* номер файла, 0 - если нет вообще */
        int ccol;               /* curorcol, когда не активен */
        int crow;               /* cursorline, когда неактивен */
        };
#define SWKSP (sizeof (struct workspace))

struct workspace *curwksp, *pickwksp;
int curfile;


/* viewport - описатель окна на экране терминала
 * все координаты на экране, а также ltext и ttext, измеряются по отношению
 *      к (0,0) = начало экрана. Остальные 6 границ приводятся по отношению
 *      к ltext и ttext.
 */

#define SVIEWPORT (sizeof (struct viewport))

struct viewport
        {
        struct workspace *wksp; /* Ссылка на workspace         */
        struct workspace *altwksp;      /* Альтернативное workspace */
        int prevport;           /* Номер пред. окна   */
        int ltext;              /* Границы  текста (по отн. к 0,0) */
        int rtext;              /* Длина в ширину                  */
        int ttext;              /* Верхняя граница                 */
        int btext;              /* Высота окна                     */
        int lmarg;              /* границы окна == ..text или +1 */
        int rmarg;
        int tmarg;
        int bmarg;
        int ledit;              /* область редактирования в окне */
        int redit;
        int tedit;
        int bedit;
        char *firstcol;         /* Номера первых колонок, !=' '  */
        char *lastcol;          /*  -//-  последних -//-         */
        char *lmchars;          /* символы - левые ограничители  */
        char *rmchars;          /* символы - правые ограничители */
        };

#define MAXPORTLIST 10
struct viewport *portlist[MAXPORTLIST],
                *curport,       /* текущее окно */
                wholescreen,    /* весь экран   */
                paramport;      /* окно для параметров */
int nportlist;

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
Управляющие символы */

#define CCBACKTAB          BT      /* tab left             */
#define CCHOME             HO      /* home cursor          */
#define CCMOVEDOWN         DN      /* move down 1 line     */
#define CCMOVELEFT         LT      /* backspace            */
#define CCMOVERIGHT        RT      /* forward move         */
#define CCMOVEUP           UP      /* move up 1 lin        */
#define CCRETURN           RN      /* return               */
#define CCTAB              TB      /* tab                  */


#define CCCTRLQUOTE        000     /* knockdown next char  */
#define CCPICK             011     /* pick                 */
#define CCMAKEPORT         012     /* make a viewport      */
#define CCOPEN             013     /* insert               */
#define CCSETFILE          014     /* set file             */
#define CCCHPORT           015     /* change port          */
#define CCPLPAGE           016     /* minus a page         */
#define CCGOTO             017     /* goto linenumber      */
#define CCDOCMD            020     /* execute a filter     */
#define CCMIPAGE           021     /* plus a page          */
#define CCPLSRCH           022     /* plus search          */
#define CCRPORT            023     /* port right           */
#define CCPLLINE           024     /* minus a line         */
#define CCDELCH            025     /* character delete     */
#define CCSAVEFILE         026     /* make new file        */
#define CCMILINE           027     /* plus a line          */
#define CCMISRCH           030     /* minus search         */
#define CCLPORT            031     /* port left            */
#define CCPUT              032     /* put                  */
#define CCTABS             033     /* set tabs             */
#define CCINSMODE          034     /* insert mode          */
#define CCBACKSPACE        035     /* backspace and erase  */
#define CCCLOSE            036     /* delete               */
#define CCENTER            037     /* enter parameter      */
#define CCQUIT            0177     /* terminate editor run */
#define CCINTRUP          0240     /* interuption (for ttyfile)     */
#define CCMAC             0200     /* macro marka                   */

/*  */

int cursorline;         /* physical screen position of */
int cursorcol;          /*  cursor from (0,0)=ulhc of text in port */


extern char  cntlmotions[];



extern int tabstops[];
char blanks[LINELM];

extern int lread1;             /* Текущий входной символ, -1 - дай еще! */
char intrflag; /* 1 - был сигнал INTERUP */

/* Умолчания */
extern int defplline,defplpage,defmiline,defmipage,deflport,defrport,
        definsert, defdelete, defpick;
extern char deffile[];

int errsw;    /* 1 - в окне параметров сообщение об ошибке */
int gosw;     /* -- атавизм */

/*
 * Глобальные параметры для param():
 * paraml - длина параметра;
 * char *paramv - сам параметр,
 * paramtype - тип параметра,
 * paramc0, paramr0, paramc1< paramr1 -
 * размеры области при "cursor defined"
 */
int paraml;
char *paramv, paramtype;
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

char *searchkey,*symac;

int userid,groupid;

char *tmpname;         /* name of file, for do command */

/* codes for output on terminal, defined in ned.vt.c  */
int LINEL,NLINES; /* SIZE OF THE SCREEN  - VT.C */
int eolflag;
extern char *curspos,*cvtout[];
char *(*agoto)();  /* for termcap definitions */
int (*atcread)();
int slowsw;   /* is 1 if slow speed terminal */

int rawf, lcasef, latf, flgtabs; /* terminal regim flags */
char kioutf; /* Не 0 если 15IE0013  с непереключенной клавиатурой*/
char *append(), *salloc();
char *s2i();
struct fsd *file2fsd();
