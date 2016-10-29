/*
 * $Header: r.tabs.c,v 4.20 90/05/22 23:22:20 alex Exp $
 * $Log:        r.tabs.c,v $
 * Revision 4.20  90/05/22  23:22:20  alex
 * First rev. red 4.2
 * 
 *
 * r.tabs.c - работа с табуляциями
 */
#include "r.defs.h"

SCALE tabstps[LSCALE(MAXTCOL)];
int chg_tabs;
int deftabstep=  8;               /* Умолчание для табуляции */

/*
 * char *SetTab(&tcol,cmd) -
 * добавить точку табуляции
 * Ответ- NULL - O'K, иначе текст диагностики ошибки
 * Начальная колонка - tcol
 * Аргумент
 * "-номер" - вычистить таб. из колонки номер
 * "[+]номер" - установить таб. в колонку номер
 * "-" - вычистить все табуляции
 * "*номер" - установить таб. через номер колонок, нач. с текущей
 * "ac" == 0 - при av == 0 - снять тек. табуляцию
 *                  != 0 - установить
 * Остается признак - chg_tabs = 1;
 */
char *SetTab( tcol, cmd)
register int *tcol;
char *cmd;
{
        int set;        /* 0 - уст. тек, 1 - снять тек, 2 - уст. с шагом, 3 - снять все справа */
        int count;
        register int i;
        count = -1;
        if ( !cmd ) {
                set = 0;
                cmd = "";
        } else
        switch ( *cmd ) {
        case 0:
                set = 3;
                break;
        case '+':
                set = 0;
                cmd++;
                break;
        case '-':
                set = 1;
                cmd++;
                break;
        case '*':
                set = 2;
                cmd++;
                break;
        default:
                if ( *cmd < '0' || *cmd > '9' )
                        goto err;
                set = 0;
                break;
        }
        if ( *cmd && s2i(cmd, &count )) goto err;
        if ( set != 2 && count > 0 ) *tcol = count-1;
        if ( *tcol > MAXTCOL || *tcol < 0 ) goto err1;
        switch ( set ) {
        case 0:
                i = *tcol;
                SETTAB( i);
                break;
        case 1:
                i = *tcol;
                CLRTAB( i);
                break;
        case 2:
                if (  count <= 0 ) count = deftabstep;
                i = *tcol;
                while ( i < MAXTCOL )
                {
                        SETTAB(i);
                        i += count;
                }
                break;
        case 3:
                i = *tcol;
                /* Способ почистить все не самый быстрый, но и случай не частый */
                while ( i < MAXTCOL )
                {
                        CLRTAB(i);
                        i += 1;
                }
                break;
        }
        chg_tabs = 1;
        return((char *)0);
err:
        return(DIAG("Bad. [+-*][NUM] is correct","Ош. Правильно [+-*][ном]"));
err1:
        return(DIAG("Bad col number","Недоп. номер колонки"));
}

/*
 * Нач. установка - deftabstep
 * В стартовом файле можно изменить
 */
InitTabs()
{
        int col = 8;
        SetTab( &col, "*");
}

/*
 * TabWrite(fd) -
 * записать установку табуляций в файл
 */
TabWrite(fd)
FILE *fd;
{
    int t0 = -1;
    int count = -1;
    int isstar = 0;
    register int i;
    register int tcnt = 2;
    fprintf(fd,"tabset -1 \"\"");
    for(i = 1; i < MAXTCOL+1; i++ )
    {
        if ( !ISTAB(i-1) ) continue;
        /* Если продолжается все с шагом count - перезапишем t0, и все */
        if ( count > 0 && ( (i - t0) == count ) ) {
            t0 = i;
            if ( !isstar )
            {
                fprintf(fd," *%d",count);
                isstar = count;
            }
            continue;
        }
        isstar = 0;
        /* Места хватит? */
        if ( tcnt + 3 >= MAX_IARGS ) {
            tcnt = 0;
            fprintf(fd,"\ntabset");
        }
        /* Если чего то не хватало - выдать и запомнить */
        if ( t0 < 0 || count < 0 )
        {
            fprintf(fd," %d", i);
            tcnt++;
            if ( t0 > 0 ) count = i - t0;
            t0 = i;
            continue;
        }
        /* Шаг нарушился */
        fprintf(fd," %d %d \"\"",i,i+1);
        tcnt += 3;
        count = -1;
        t0 = -1;
    }
    fprintf(fd,"\n");
    return;
}

/*
 * char *settab(tcol,str) - то же, но делается еще и разбор строки
 *                   NULL - поставить тек.
 *                   ""   - снять тек.
 */
char *settab(tabcol,arg)
int tabcol;
char *arg;
{
        char *args[MAX_IARGS+1], **av;
        int ac;
        char *ko = (char *)0;
        av = args;
        /* Просто установить или снять сразу все */
        if ( !arg || !*arg ) return(SetTab(&tabcol,arg));
        ac = SelectArgs(arg,av,MAX_IARGS);
        while ( ac-- && !(ko = SetTab( &tabcol, *av++)) );
        return(ko);
}

