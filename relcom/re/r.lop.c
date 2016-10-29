/*
 * r.lop.c - строковые операции (файл как последовательность байт)
 * $Header: r.lop.c,v 4.20 90/05/22 23:22:06 alex Exp $
 * $Log:        r.lop.c,v $
 * Revision 4.20  90/05/22  23:22:06  alex
 * First rev. red 4.2
 * 
 */
#include "r.defs.h"
#include <stdio.h>

int clrsw;  /* 1 - чистить окно параметров */
int csrsw;  /* 1 - показать текущую позицию меткой  */
int imodesw;/* 1 - режим вставки  */
extern rep_count;
/*
 * Информационные строки для работы по парам скобок
 */
char *left_parents ="({['`\"";
char *right_parents=")}]'`\"";
static SCALE s_parents[LSCALE(256)];
static int is_parents;
/* Помни о знаке !!! */
#define ISPARENT(i)   ISBIT(s_parents,(((int)i)&0377))
#define SETPARENT(i) SETBIT(s_parents,(((int)i)&0377))
/* Шаблон описания разделителей для работы по словам */
static SCALE s_delims[LSCALE(256)];
char *delimiters=" ~!@#$%^&*(-+=`\\\"';:.>,</?";
int is_delims;
#define ISDELIM(i)   ISBIT(s_delims,(((int)i)&0377))
#define SETDELIM(i) SETBIT(s_delims,(((int)i)&0377))
/* задания для поиска вперед/назад */
/* Вперед и назад на начало слова */
char *wDleft = "-1DI+1";/* */
char *wDright= "+ID"; /* */
/* А это то же, но на конец слова */
/* char *wDleft = "-1ID+i";   */
/* char *wDright= "+DI";      */
/* А это - от последнего аргумента */
char *w_left, *w_right;
/*
 * Подпрограммы, которые вызываются из команды установки переменных
 * new_parents
 * new_delims
 */
new_parents()
{
is_parents = 0;
}

new_delims()
{
is_delims = 0;
}


/*


/*
 * compl_delims() - формирует шкалу разделителей для работы по словам
 */
compl_delims()
{
        register int i;
        register char *l;
        for(i = 0; i < LSCALE(256);i++) s_delims[i] = 0l;
        for(l = delimiters;*l;l++)
        {
                i = *l;
                SETDELIM(i);
                /* Пробел и перевод строки считаем эквивалентными */
                if ( i == ' ' ) SETDELIM(012);
        }
        is_delims = 1;
        return;
}

/*
 * compl_parents() - формирует шкалу для работы со скобками и заодно
 * проверяет, что списки левых и правых скобок имеют одинаковую длину
 * ответ: 0 - нормально, -1 - ошибка в списках
 */
compl_parents()
{
        register int i;
        register char *l,*p;
        for(i = 0; i < LSCALE(256);i++) s_parents[i] = 0l;
        for(l = left_parents,p=right_parents;*l && *p;l++,p++)
        {
                i = *l;
                SETPARENT(i);
                i = *p;
                SETPARENT(i);
        }
        is_parents = 1;
        return ( (*l == 0 && *p == 0) ? 0 : -1);
}

/*
 * linecset(op,arg)
 * int op;
 * char *arg;
 * - выполнение операций CCBEGIN, CCEND, CCWORDLEFT, CCWORDRIGHT
 *   и других операций позиционирования в строке
 *   по контексту
 */
char *line_mesg;
linecset(op,arg)
int op;
char *arg;
{
        int ln,col;
        ln = ABS_LIN(cursorline);
        col= ABS_COL(cursorcol);
        line_mesg = (char *)0;
        if (line_cop(&ln, &col, op,arg) < 0 )
        {
              error(line_mesg);
              rep_count = 0;
              return(-1);
        }
        cgoto(ln,col,-1,0);
        return(0);
}

/* Для операций над строкой */
/* Указатели */
static char *ess, *ss, *bss;
/* S_setup(int *pl; int lin, col)
 * ответ - 0 - конец файла, -1 - нет такой строки, иначе код символа
 * если курсор за концом строки, то получаем код 012
 * Переход через конец строки дает код 012
 * S_next(l), S_prev(l) - макро "дай след/пред символ
 * аргумент - идентификатор с номером строки
 */
#define S_next(l) (++ss<=ess? (*ss) : S_setup(&l, l+1, 0))
#define S_prev(l) (--ss>=bss? (*ss) : S_setup(&l, l-1,-1))
static char S_setup(P_l, lin,col)
int *P_l;
int lin, col;
{
        int ko = -1; /* <0 - выдать *s */
        if ( lin < 0 ) return(0);
        if ( !getline(*P_l = lin) ) ko = 0;
        bss = cline;
        ess = cline + (ncline - 1);
        if ( col < 0 || col >= ncline -1 )
                ss =  ess;
        else
                ss = cline + col;
        if ( ko == 0 ) return(0);
        else return(*ss);
}


/*
 * Основная подпрограмма для работы со строками
 * line_cop(char *p_l, *P_c; int op; char *arg)
 * Результатом является установка p_l, p_c
 * -1 - операция не выполнена
 * 1  - выполнена, но не совсем та (если не было нужного контекста под курсором)
 * Строка диагностики помещается в переменную char *line_mesg;
 */
line_cop(p_l,p_c,op,str)
int op;
char *str;
int *p_l, *p_c;
{
    int ln;
    register char *s;
    char *es;
    int c;
    int col, slin, scol;
    ln = *p_l;
    col= *p_c;
next:
    getline(ln);
    switch (op ) {
    case CCBEGIN:
        for(s=cline;*s==' ';s++);
        scol = s - cline;
        if ( col <= scol && ln)
        {
            ln--;
            col = 32000;
            goto next;
        }
        *p_l = ln;
        *p_c = scol;
        /* 1 означает, что начало строки оказалось справа */
        return(col <= scol?1:0);
    case CCEND:
        for(s=cline+ncline-1;(*s==NEWLINE || *s == ' ') && s >= cline;s--);
        scol = s - cline +1 ;
        if ( col >= scol && ln < nlines[curwksp->wfile] )
        {
            col = -1;
            ln++;
            goto next;
        }
        *p_l = ln;
        *p_c = scol;
        return(col >= scol? 1 : 0);
    case CCPSEARCH:
        /*
         * 0. Если шкала-не-уст - установить шкалу скобок
         * 1. Установить тек. символ
         * 3. Проверить тек. символ. Сначала на левые скобки, затем
         *    на правые.
         * 4. Пока пред. символ - повторить 4.
         * 5. Если был-поиск - установить и выйти
         * 6. Если нашли: в левых -
         *    направление = вперед
         *    скобка1 = наша
         *    скобка2 - правая
         *    Иначе
         *    направление = назад
         *    скобка1 = наша
         *    скобка2 = левая
         * 7. Основной поиск. скобка1 ++, скобка2--
         *    если нашли то установить и выйти
         *    если 0, то выйти с кодом 1
         */
        if ( !is_parents && compl_parents() < 0 )
        {
err_ptab:
            line_mesg = DIAG("Error in parent tables","Ошибка в таблицах скобок");
            return(-1);
        }
        {
            register char c;
            char p1 = 0, p2; /* Наша и противоп. скобка */
            int fwd = 0; /* 0 - назад, 1 - вперед */
            int ko = 1;  /* Мы не на скобке, нашли скобку */
            int level;
            c = S_setup(&ln, ln,col);
            if ( c != 0 && ISPARENT(c) )
            {
                /* Левая скобка? */
                for(s=left_parents;*s && *s != c;s++);
                if ( *s )
                {
                    fwd = 1;
                    p1 = *s;
                    p2 = right_parents[s-left_parents];
                    goto find_p;
                }
                /* Правая скобка? */
                for(s=right_parents;*s && *s != c;s++ );
                if ( *s )
                {
                    fwd = 0;
                    p1 = *s;
                    p2 = 0377&left_parents[s-right_parents];
                    goto find_p;
                }
                /* Такого не бывает - в таблице нет, а в шкале есть */
                goto err_ptab;
find_p:
                level = 1; /* Мы уже внутри */
                ko = 0;
                while ( c = (fwd? S_next(ln): S_prev(ln) ) )
                {
                    if ( c == p2 )
                        level--;
                    else if ( c == p1 )
                        level++;
                    if ( level == 0 ) break;
                }
            }
            else
                /* Тек. символ - не скобка - ищем ближайшую скобку назад */
                while ( c = S_prev(ln) )
                {
                    if ( ISPARENT(c))
                    {
                        /* Левая скобка? Но только не равная правой! */
                        for(s=left_parents;*s && *s != c;s++);
                        if ( *s )
                        {
                              if ( *s == right_parents[s-left_parents] )
                                    continue; /* Совп. скобки - это не блок */
                            break; /* Нашли, что хотели */
                         }
                         /* Правая скобка? Найдем ее левую подругу, и продолжим  */
                         for(s=right_parents;*s && *s != c;s++ );
                         if ( *s )
                         {
                             level = 1;
                             p1 = *s;
                             p2 = 0377&left_parents[s-right_parents];
                         }
                         else goto err_ptab;
                         while ( c = S_prev(ln)  )
                         {
                             if ( c == p2 )
                                 level--;
                             else if ( c == p1 )
                                 level++;
                             if ( level == 0 ) break;
                         }
                         /* Нашли начало попавшихся скобок. Продолжим поиск блока */
                    }
                }
            /* А теперь у нас все стоит - установим положение и выйдем */
            if ( c ) {
                scol = ss - cline;
                if ( scol < 0 ) scol = 0;
                *p_l = ln;
                *p_c = scol;
                return(ko);
            } else
                line_mesg = DIAG("Not in parents","Не в скобках");
            return(-1);
        }
    case CCWORDLEFT:
    case CCWORDRIGHT:
        return(word_op(p_l, p_c, op,str) );
    default:
        line_mesg = DIAG("Not implemented yet","Операция не реализована");
        return(-1);
    }
}

word_op(p_l, p_c, op,arg)
int *p_l, *p_c;
int op;
char *arg;
{
    int lin, col, scol;
    int fwd;        /* 1 - вперед, 0 - назад */
    register char c;
    register int i;     /* Рабочая */
    int in1line = -1;   /* Если >= 0 ,    то это запрет на выход из строки     */
    int chk_cod;        /* 1 - проверяем на разделитель, 0 - на не разделитель */
    int cmd;            /* текущий аргумент автомата команд */
    int in1char;        /* 0 - while, 1 - 1 символ */
    lin = *p_l;
    col = *p_c;
    getline(lin);
    if ( !is_delims ) compl_delims();
    switch ( op )
    {
    case CCWORDRIGHT:
        fwd = 1;
        if ( !arg )  arg = w_right? w_right: wDright;
        goto avto;
    case CCWORDLEFT:
        fwd = 0;
        if ( !arg ) arg = w_left? w_left: wDleft;
        goto avto;
    default:
        line_mesg = DIAG("Ill. op","Неизвестная операция");
        return(-1);
    }
avto:
    /* Конечный автомат */
    /* Управляют: fwd = 1 - вперед, 0 - назад
     * cmd - тек. код шаблона
     * in1line - ограничиться текущей строкой, если >= 0.
     * Номер строки в in1line
     * Управление:
     * + - установить движение вперед
     * - - установить движение назад
     * I - пока не разделитель и не 0, двигаться
     * D - пока разделитель и не 0, двигаться
     * i - если не разделитель и не 0, продвинуться на 1
     * d - если разделитель и не 0, продвинуться на 1
     * 1 - продвинуться на 1 символ
     * L - установить режим одной ( текущей) строки
     * l - снять режим одной (текущей) строки
     *
     */
    c = S_setup( &lin, lin, col);
    while ( cmd = *arg++ )
    {
        switch (cmd)
        {
        case '+':
            fwd = 1;
            continue;
        case '-':
            fwd = 0;
            continue;
        case 'L':
            in1line = lin;
            continue;
        case 'l':
            in1line = -1;
            continue;
        case '1':
            c = (fwd? S_next(lin) : S_prev(lin));
            continue;
        case 'D':
            chk_cod = 1;
            goto c_while;
        case 'd':
            chk_cod = 1;
            goto c_1;
        case 'I':
            chk_cod = 0;
            goto c_while;
        case 'i':
            chk_cod = 0;
            goto c_1;
        default:
            line_mesg = DIAG(" :Bad control char"," :Недоп. упр. символ");
            line_mesg[0] = cmd;
            return(-1);
        };
c_1:  
        in1char = 1;
        goto do_while;
c_while:
        in1char = 0;
do_while:
        do
        {
            if ( in1char > 1 ) break;
            in1char += in1char;
            if ( in1line >= 0 && in1line != lin )
                goto set_1line;
            i = (ISDELIM(c) != 0 );
            if ( c == 0 || i != chk_cod ) break;
        }      while ( c = (fwd? S_next(lin) : S_prev(lin) ) );
        continue;
set_1line:
        if ( lin > in1line ) c = S_setup(&lin, in1line, -1); /* На конец */
        else c = S_setup( &lin, in1line, 0);
        continue;
    }
    scol = ss -cline;
    if ( scol < 0 ) scol = 0;
    *p_l = lin;
    *p_c = scol;
    return( *arg? 1 : 0);
}

#ifdef DEMOSES
/*
 * editline(номер,строка)
 * Редактировать строку "номер" в текущем окне.
 * Заменить ее на "строка"
 */
static editline(ln,str,lnos)
int ln,lnos;
char *str;
{
    register int i;register char *cp;
    int lno; /* Номер в файле */
    int slen; /* Длина str */
    int scolno; /* Начальная колонка str */
    int newnline; /* Новая длина строки */
    /*
     * 1. Дай строку

     */
    lno = ln + curwksp->ulhclno;
    getline(lno);
    /*
     * 2. Определить длину новой строки без хвостовых пробелов.
     */
    scolno = curwksp->ulhccno;
    if ( ncline-1 <= scolno+(curport->rtext+1) ){
        slen = lnos;
        newnline = scolno + slen+1;
    }
    else {
        slen = curport->rtext+1;
        newnline = ncline;
    }
    if ( lcline-2 < newnline ) excline(newnline+2);
    /*
     * 3. Заменяем часть строки
     */
    i = ncline-1;
    while ( i < scolno ) cline[i++] = ' ';
    for ( i = 0, cp = str; i < slen; i++ ) {
        cline[scolno+i] = (*cp?*cp++:' ');/* Becь xвocт зaбивaeм пpoбeлaми*/
    }
    ncline = newnline;
    cline[ncline-1]=012;
    fcline = 1;
 /*
  * Учитывая особенности терминала, устанавливаем режим - стирай все!
  */
    if ( !speccase )  {
        curport->firstcol[ln] = 0; curport->lastcol[ln]= slen;
        out_lines(ln,ln);
    } else
    out_lines(1, ln);
    return;
}

/*
 * lineop(символ) - для ЕС
 * Выполнение простейших операций в строке.
 * В конце сбрасывает lread1.
 */
lineop(lr1)
register lr1;
{
        register int i,k;
        int thiscol, thisrow;
        /* Введем курсор в его границы */
        movecursor(0);
        if (openwrite[curfile] == 0) goto nowriterr;
        /* Строки у нас нет? Дай! */
        getline(i);
        /* исключение символа */
        if (lr1==CCDELCH )
        {
                thiscol = cursorcol + curwksp->ulhccno;
                thisrow = cursorline;
                if (ncline < thiscol + 2)
                        goto contin;
                for (i=thiscol;i<ncline-2;i++) cline[i] = cline[i+1];
                ncline--;
                thiscol -= curwksp->ulhccno;
                fcline = 1;
                out_cols(thiscol,-1);
                poscursor(thiscol,thisrow);
                goto contin;
        }
        /* Проверка на границу окна - для МАКРО */
        if (cursorcol >= curport->rmarg )
        {
                if ( fcline )
                {
                        putline(0);
                        movep(defrport);
                        goto contin;
                }
                else  goto margerr;
        }
        fcline = 1;
        /* Если строка удлинилась - отведем еще память */
        if ((i = cursorcol + curwksp->ulhccno ) >=
            (lcline - 2)) excline(i+2);
        if (i >= ncline-1)
        {
                for (k=ncline-1; k<=i; k++) cline[k] = ' ';
                cline[i+1] = NEWLINE;
                ncline = i+2;
        }
        /* Вставка пробелов */
        if (lr1==CCINSERT)
        {
                thiscol = cursorcol + curwksp->ulhccno;
                thisrow = cursorline;
                if (ncline+dflinsert > lcline) excline(ncline+dflinsert);
                for (i=ncline-1;i >= thiscol;i--) cline[i+dflinsert]=cline[i];
                ncline += dflinsert;
                for ( i=0;i<dflinsert;i++) cline[i+thiscol] = ' ';
                thiscol -= curwksp->ulhccno;
                out_cols(thiscol,-1);
                poscursor(thiscol,thisrow);
                goto contin;
        }
        /* Выставим границу */
        if (cursorcol >= curport->rtext)
                curport->redit = curport->rtext + 1;
        /* Замена символа */
        if(lr1==CCCTRLQUOTE) lr1 = esc0;
        cline[i] = lr1;
        putch(lr1,1);
        /* Если переехали границу */
        curport->redit = curport->rtext;
        goto contin;
margerr:
        error("Margin stusk; move cursor to zFree.");
        goto errdone;
nowriterr:
        error(DIAG("You cannot modify this file!","Вы не можете изменить этот файл."));
        goto errdone;
errdone:
        /*lread1 = -1;  */
        return(1);
contin:
        /*lread1 = -1;  */
        return(0);
}

#else /* NOT DEMOSES */

/*
 * lineop(символ) - для СМ
 * Выполнение простейших операций в строке.
 * В конце сбрасывает lr1.
 */
lineop(lr1)
register lr1;
{
        register int i,k;
        int thiscol, thisrow,j;
        int old;
        int first_p = 0;
        if (openwrite[curfile] == 0) goto nowriterr;
repop:
        /* Отмена в 1 колонке */
        if (lr1 == CCBACKSPACE  &&  cursorcol == 0)
        {
              if ( (i=ABS_COL(0)) > 0 ) {
                      cgoto(ABS_LIN(cursorline),i-1,-1,0);
                      movecursor(CC+RT,0);
                      goto repop;
              }
              goto contin;
        }
        /* Строки у нас нет? Дай! */
        old = getline( (i=ABS_LIN(cursorline)) );
        /* исключение символа */
        if (lr1==CCDELCH || (imodesw && lr1==CCBACKSPACE) || lr1==CCTRUNCATE)
        {
                thiscol = cursorcol + curwksp->ulhccno;
                thisrow = cursorline;
                if (lr1 == CCBACKSPACE) thiscol--;
                if (ncline < thiscol + 2)
                {
                        if (lr1 == CCBACKSPACE) movecursor(LT);
                        lr1 = -1;
                        goto contin;
                }
                if(lr1 == CCTRUNCATE) {
                        cline[thiscol] = cline[ncline-1];
                        ncline = thiscol+1;
                } else
                {
                        for (i=thiscol;i<ncline-2;i++) cline[i] = cline[i+1];
                        ncline--;
                }
                thiscol -= curwksp->ulhccno;
                fcline = 1;
                out_cols(thiscol,-1);
                poscursor(thiscol,thisrow);
                goto contin;
        }
        /* Проверка на границу окна */
        if (cursorcol+curport->ltext >= curport->rmarg)
        {
                if (!first_p++) {
                        movep(defrport);
                        goto repop;
                }
                else  goto margerr;
        }
        fcline = 1;
        if (j = (lr1 == CCBACKSPACE))
        {
                movecursor(LT);
                lr1 = ' ';
        }
        if ((i = cursorcol + curwksp->ulhccno) >=
            (lcline - 2)) excline(i+2);
        if (i >= ncline-1)
        {
                for (k=ncline-1; k<=i; k++) cline[k] = ' ';
                cline[i+1] = NEWLINE;
                ncline = i+2;
        }
        else if (imodesw)
        {
                thiscol = cursorcol + curwksp->ulhccno;
                thisrow = cursorline;
                if (ncline >= lcline) excline(ncline+1);
                for (i=ncline;i>thiscol;i--) cline[i] = cline[i-1];
                ncline++;
                thiscol -= curwksp->ulhccno;
                out_cols((1+thiscol),-1);
                poscursor(thiscol,thisrow);
        }
        /* Выставим границу */
        if (cursorcol >= curport->rtext)
                curport->redit = curport->rtext + 1;
        /* Замена символа */
        if(lr1==CCCTRLQUOTE) lr1 = esc0;
        if (cursorcol == curport->rtext - 10) putcha(COBELL);
        cline[i] = lr1;
        putch(lr1,1);
        /* Если переехали границу */
        curport->redit = curport->rtext;
        if (j) movecursor(LT);
        lr1 = -1;
        goto contin;
margerr:
error("Margin stusk; move cursor to zFree.");
goto errdone;
nowriterr:
error(DIAG("You cannot modify this file!","Вы не можете изменить этот файл."));
goto errdone;
errdone:
return(0);
contin:
msvtag("1");
return(1);
}
#endif

/*
 * search(delta) -
 * поиск вперед/назад в файле
 * delta =  1  / -1
 * Установить окно, если нужно, и показать
 * место найденного текста.
 * Ищется "searchkey"
 */
search(delta)
int delta;
{
    register char *at,*sk,*fk;
    int ln,lkey,col,lin,slin,i;
    paraml = 0;
    if (searchkey == 0 || *searchkey == 0)
    {
        error(DIAG("Nothing to search for.","А что искать?"));
        return;
    }
    col = cursorcol;
    slin = lin = cursorline;
    if (delta == 1) telluser("+",0);
    else telluser("-",0);
    telluser(DIAG("search: ","поиск: "),1);
    i=lcasef;
/* --- */
    lcasef=0; /* Внутренняя форма текста: */
    telluser(searchkey,9);
    lcasef =i;
/* --- */
    putch(COCURS,1);
    poscursor(col,lin);
    dumpcbuf(1);
    lkey = 0;
    sk = searchkey;
    while (*sk++) lkey++;
    getline (ln = lin + curwksp->ulhclno);
    /* putline(0);      */
    at = cline + col + curwksp->ulhccno;
    FOREVER
        {
        at += delta;
        while (at <  cline || at >  cline + ncline - lkey)
        {
            /* Прервать, если было прерывание с терминала */
            if ((i=intrup()) || (ln += delta) < 0 ||
                (wposit(curwksp,ln) && delta == 1))
            {
                out_win(lin,lin,col,col);
                poscursor(col,lin);
                error(i?"Interup.":DIAG("Search key not found.","Текст не найден."));
                csrsw = 0;
                rep_count = 0;
                return;
            }
            getline(ln);
            /* putline(0);      */
            at = cline;
            if (delta < 0) at += ncline - lkey;
        }
        sk = searchkey;
        fk = at;
        while (*sk == *fk++ && *++sk);
        if (*sk == 0)
        {
            cgoto(ln,at-cline,slin,0);
            csrsw = 1;  /* put up a bullit briefly */
            return;
        }
    }
}

