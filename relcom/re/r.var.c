/*
 *      Редактор RED.
 *
 * r.var.c - Программы для работы с переменными
 *
 *      $Header: r.var.c,v 4.20 90/05/22 23:22:25 alex Exp $
 *      $Log:   r.var.c,v $
 * Revision 4.20  90/05/22  23:22:25  alex
 * First rev. red 4.2
 * 
 */
#include "r.defs.h"

# ifndef MAX_STRLEN
# define MAX_STRLEN 128
# endif
/*
 * Таблица расширения переменных
 */
static char *UserName, *HomeDir, *TtyName,*TtyName2, *TmpDir;
char *TtyType;
extern char *KeyMode;
extern char *MacroFile;
char *Label; /* Вывеска */
static struct ctl_expand {
char left_c;
char **expand_c;
} ctl_expand[] =
{
'h', &HomeDir,
't', &TtyType,
'u', &UserName,
'd', &TtyName2,
'w', &TmpDir,
'k', &KeyMode,
0, (char **)0 };

/*
 * Таблица встроенных переменных
 */
extern char *TTYnm   ;
extern char *TTYtmp  ;
extern char *RFile   ;
extern char *InitPath;
extern char *HelpPath;
extern char *RedKeys;
int showtabs;
int imodesw;
int fullinsert;
int alltabs;
int chg_flag; /* 0 при Init-файле, 1  далее */
extern char *wDleft, *wDright;
extern char *delimiters, *left_parents, *right_parents;
extern new_delims(), new_parents();

# define VAR_BOOL 1
# define VAR_STR  2
# define VAR_NUM  3

#define CP (char **)
int rescreen();
int comp_selectors();
#define NOFUN (int(*)())0

struct int_vars {
char *I_name;  /* Имя в команде set */
char *I_ENV ;  /* Имя при установке из ENV */
char  I_type;  /* тип */
char  I_1key;  /* Однобуквенное имя, если существует */
char  I_changed; /* Признак - переменная менялась */
char  I_save;    /* Если 1 - запоминать в macro-файл */
char **I_ptr;   /* Указатель на переменную */
int (*I_func)(); /* Функция, которая вызывается после определения */
/*char *I_linst, *I_rinst; /* Инструкция по английски и по русски */
} int_vars [] =
{
/* Name       ENV_NAME    TYPE    KEY   CH SAVE Addr           Fun    */
"help",      "REDHELP", VAR_STR,   0,   0,   0, &HelpPath,     NOFUN,
"init",      "REDINIT", VAR_STR,   0,   0,   0, &InitPath,     NOFUN,
"macro",     "REDMACRO",VAR_STR,   0,   0,   0, &MacroFile,    NOFUN,
"tmp",       "REDTMP",  VAR_STR,   0,   0,   0, &TmpDir,       NOFUN,
"keys",      "REDKEYS", VAR_STR,   0,   0,   0, &RedKeys,      NOFUN,
"keymode",   "REDMODE", VAR_STR,   0,   0,   0, &KeyMode,      NOFUN,
"tmpfile",   NULL,      VAR_STR,   0,   0,   0, &TTYtmp,       NOFUN,
"savefile",  "REDSAVE", VAR_STR,   0,   0,   0, &RFile,        NOFUN,
"recfile",   NULL,      VAR_STR,   0,   0,   0, &TTYnm,        NOFUN,
"label",     NULL,      VAR_STR,   0,   0,   0, &Label,        NOFUN,
"search",    NULL,      VAR_STR,   0,   1,   1, &searchkey,    NOFUN,
"cols",      NULL,      VAR_NUM,   0,   0,   0, CP &LINEL,     NOFUN,
"lines",     NULL,      VAR_NUM,   0,   0,   0, CP &NLINES,    NOFUN,
"lboard",    NULL,      VAR_NUM,   0,   0,   1, CP &defedit,   NOFUN,
"noruscase", NULL,      VAR_BOOL,  'e', 0,   0, CP &latf,      rescreen,
"lcase",     NULL,      VAR_BOOL,  'l', 0,   0, CP &lcasef,    rescreen,
"insertmode",NULL,      VAR_BOOL,  'i', 0,   1, CP &imodesw,   NOFUN,
"fullinsert",NULL,      VAR_BOOL,  'I', 0,   1, CP &fullinsert,NOFUN,
"slowmode",  NULL,      VAR_BOOL,  0,   0,   0, CP &slowsw,    NOFUN,
"scrollon",  NULL,      VAR_BOOL,  's', 0,   1, CP &on_scroll, NOFUN,
"autoindent",NULL,      VAR_BOOL,  'a', 0,   1, CP &on_autoi,  NOFUN,
"indentcol", NULL,      VAR_NUM,     0, 1,   1, CP &autoindent,NOFUN,
"fixscreen", NULL,      VAR_BOOL,  'f', 0,   1, CP &fix_screen,NOFUN,
"tabsexpand",NULL,      VAR_BOOL,  't', 0,   1, CP &flgtabs,   rescreen,
"atabsexpand",NULL,     VAR_BOOL,  'T', 0,   1, CP &alltabs,   rescreen,
"showtabs",  NULL,      VAR_BOOL,  'S', 0,   1, CP &showtabs,  rescreen,
"zoomed",    NULL,      VAR_BOOL,  'z', 0,   1, CP &zoomflag,  rescreen,
"lparents",  NULL,      VAR_STR,     0, 0,   1, &left_parents, new_parents,
"rparents",  NULL,      VAR_STR,     0, 0,   1, &right_parents,new_parents,
"delims",    NULL,      VAR_STR,     0, 0,   1, &delimiters,   new_delims,
"wdleft",    NULL,      VAR_STR,     0, 0,   0, CP &wDleft,    NOFUN,
"wdright",   NULL,      VAR_STR,     0, 0,   0, CP &wDright,   NOFUN,
NULL,        NULL,      0,         0,   0, 0, (char **)0,      NOFUN
};

char *getenv();
/*
 * VarEInit() - служит для инициализации
 * переменных по ENVIRONMENT
 * Переменные часто могут освобождаться - разместим в куче
 */
VarEInit()
{
    register char *cp;
    register struct int_vars *pi;
    for(pi = int_vars;pi->I_name;pi++)
    {
        if ( pi->I_type == VAR_STR && pi->I_ENV && (cp=getenv(pi->I_ENV)))
            *(pi->I_ptr) = append(cp,NULL);
    }
}

/*
 * Функция выдает очередной элемент из /usr/man:/usr/local/man:...
 * Аргумент - адрес указателя на строку
 */
char *next_path( pps)
char **pps;
{
        register char *s= *pps;
        register char *s0;
        while ( *s == ':' ) s++;
        if ( *s == 0 ) return(NULL);
        s0 = s;
        while ( *++s && *s != ':' );
        *pps = s;
        if ( *s == ':' ) {
                *s = 0;
                *pps += 1;
        }
        return(s0);
}


/*
 * Инициализация переменных, используемых при
 * расширении текстовых констант
 */
InitConsts()
{
        extern char *getenv(), *ttyname(), *getnm();
        register char *s;
        if ( UserName ) return(1);
        UserName = getenv("USER");
        if ( !UserName ) UserName = getnm(userid);
        TtyType = getenv("TERM");
        if ( !TtyType ) TtyType = "";
        HomeDir = getenv("HOME");
        if ( ! HomeDir ) HomeDir = ".";
        TtyName = ttyname(0);
        if ( !TtyName ) TtyName = "nottyno";
        TtyName2 = TtyName + (strlen(TtyName) - 2);
	if ( !TmpDir ) TmpDir = "/usr/tmp/";
        return(0);
}

/*
 * Функция StrCalc(char *s)
 * возвращает расширение строки s
 * Подставляются %h - home dir
 *               %t - тип терминала
 *               %u - имя пользователя
 *               %d - последние буквы имени терминала
 * Строка размещается с помощью salloc
 * Макс. длина строки определяется переменной CPP MAX_STRLEN
 */
char *StrCalc(s)
register char *s;
{
        char sbuf[MAX_STRLEN + 2];
        register char *p;
        register struct ctl_expand *pctl;
        char *ss;
        p = sbuf;
        while ( *s && p < sbuf+MAX_STRLEN )
        {
                if ( *s == '%' && s[1] ) {
                        if ( s[1] == '%' ) {
                                *p++ = '%'; 
                                s += 2; 
                                continue;
                        }
                        for(pctl = ctl_expand; pctl->left_c && pctl->left_c != s[1];pctl++);
                        if ( pctl->left_c )
                        {
                                ss = s;
                                s = *(pctl->expand_c);
                                while ( *s && p < sbuf+MAX_STRLEN ) *p++ = *s++;
                                s = ss+2;
                                continue;
                        };
                };
                *p++ = *s++;
        }
        *p++ = 0;
        ss = salloc(p - sbuf + 1);
        p = sbuf;
        s = ss;
        while ( *s++ = *p++ );
        return(ss);
}

/*
 * char *SetVar ( name, av, ac ) -
 * установить переменную name.
 * av - вектор значений
 * ac - длина этого вектора
 * Ответ: NULL - O'key, STR - строка диагностики
 */
char *
SetVar ( name, av, ac)
char *name;
char *av[];
int ac;
{
    register char *cp;
    register struct int_vars *pi;
    int i;
    for(pi = int_vars;pi->I_name;pi++)
    {
        if ( strcmp(pi->I_name, name) == 0 ) break;
    }
    if ( pi->I_name == (char *)0 ) return("undefined");
    switch ( pi->I_type ) {
    case VAR_BOOL:
        if ( ac > 1 ) return("bad arg count");
        *((int *)(pi->I_ptr)) = ( ac == 1 && *av[0] == '0' ) ? 0 : 1;
        break;
    case VAR_STR:
        if ( ac != 1 ) return("bad arg count");
        *(pi->I_ptr) = append(av[0],"");
        break;
    case VAR_NUM:
        if ( ac != 1 ) return("bad arg count");
        if (  s2i(av[0], &i) != (char *)0) return("bad arg format");
        *((int *)(pi->I_ptr)) = i;
        break;
    default:
        return("internal error");
    }
    if ( pi->I_func != NOFUN ) (*pi->I_func)();
    pi->I_changed = chg_flag;
    return(NULL);
}


/*
 * setkeys(строка)
 * - установка ключей.
 */
setkeys(keys)
register char *keys;
{
    register int set=1, ko= -1;
    register char *cp;
    register struct int_vars *pi;
    int i;
    while ( i = *keys ) switch ( *keys++ ) {
    case '-': 
        set = 0;
        break;
    case '+': 
        set = 1;
        break;
    case 'w':
#ifdef DEMOSES
        newedit = 1;
#endif /* +DEMOSES */
        if(curwksp && curwksp->wfile )
            openwrite[curwksp->wfile]= set;
        break;
    default:
        for(pi = int_vars;pi->I_name;pi++)
        {
            if ( pi->I_1key == i && pi->I_type == VAR_BOOL ) break;
        }
        if ( pi->I_name == (char *)0 ) { 
            if ( ko < 0 ) ko = 0; 
            break;
        }
        *((int *)(pi->I_ptr)) = set;
        if ( pi->I_func != NOFUN ) (*pi->I_func)();
        pi->I_changed = chg_flag;
        break;
    }
    return(ko);
}

/*
 * char *getkeys(buf,len)
 * - выдает строку для установки ключей в текущем положении
 * Используется в записи макро и в записи/чтении текущего состояния
 *
 */
char *getkeys(buf0,len)
char *buf0;
int len;
{
        register char *cp;
        register struct int_vars *pi;
        int set = -1,i;
        char *buf = buf0;
        cp = buf;
        len--;
#define ADD(c) if ( len <= 0 ) goto retn;else (*buf++ = c),len--;
        for(pi = int_vars;pi->I_name;pi++)
        {
                if ( pi->I_1key != 0 && pi->I_type == VAR_BOOL )
                {
                        i = *((int *)(pi->I_ptr));
                        if ( i != set ) ADD(i?'+':'-');
                        set = i;
                        ADD(pi->I_1key);
                }
        }
retn:
        *buf++ = 0;
        return(buf0);
}

/*
 * do_set(str) - обрабатывает команду присвоения строки
 * Если str == NULL - используется диалоговый режим
 * (в будущем)
 */
do_set(str)
register char *str;
{
        char *args[10];
        register int i;
        register char *cp;
        if ( !str ) return(-1); /* Пока не реализовано */
        i = SelectArgs(str,args,10);
        if ( i < 0 ) return(-1);
        if ( cp = SetVar(args[0], args+1, i-1) )
        {
                ErrI(1,cp);
                return(-1);
        }
        return(0);
}

/*
 * WriteSet(fd) - записать изменившиеся переменные в файл fd
 * При записи значения текстовых переменных маскируются
 * Вызов с fd = 0 - не писать, а очистить признаки модификации
 */
WriteSet(fd)
FILE *fd;
{
    register char *cp;
    register struct int_vars *pi;
    register int i;
    for( pi = int_vars;pi->I_name; pi++)
    {
        /* Ключи, неизмененные и несохраняемые переменные - не пишем */
        if ( pi->I_1key || ! pi->I_changed || ! pi->I_save )
            continue;
        switch ( pi->I_type ) {
        case VAR_BOOL:
            fprintf(fd,"set %s %d\n",pi->I_name, ( *((int *)(pi->I_ptr)) ? 1 : 0 ) );
            continue;
        case VAR_NUM:
            fprintf(fd,"set %s %d\n",pi->I_name,  *((int *)(pi->I_ptr))  );
            continue;
        case VAR_STR:
            cp = *((char **)(pi->I_ptr));
            if ( !cp ) break;
            fprintf(fd,"set %s ",pi->I_name);
            {
                char qchar;
                qchar = DQUOTA;
                putc(qchar,fd);
                for( ; ; cp++ )
                {
                    i = *cp;
                    if ( !i ) break;
                    if ( i == qchar ) {
                        putc(qchar,fd);
                        qchar = (qchar == DQUOTA ? QUOTA :DQUOTA);
                        putc(qchar,fd);
                    }
                    putc(i,fd);
                }
                putc(qchar,fd);
                putc('\n',fd);
            }
            continue;
        default:
            break;
        }
    } /* Конец цикла по pi */
    return;
}
