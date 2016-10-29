/*
 *      Редактор RED.
 *
 *      $Header: r.tele.c,v 4.20 90/05/22 23:22:14 alex Exp $
 *      $Log:   r.tele.c,v $
 * Revision 4.20  90/05/22  23:22:14  alex
 * First rev. red 4.2
 * 
 * Revision 4.10  90/02/05  19:52:32  alex
 * Base revision 4.1
 * 
 * Revision 4.1  88/03/31  22:04:50  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 * 
 * Revision 3.9  88/03/30  17:26:53  alex
 * Исправлена ошибка в readch. Введены функции для удаления/вставки
 * строк на экране через ChangeScroll/InsertLine-DeleteLine.
 * 
 * Revision 3.1.2.6  87/07/09  20:43:27  alex
 * REVISION
 * 
 * Revision 3.1.2.5  87/07/03  22:03:22  alex
 * Graph_characters used in margin
 * 
 * Revision 3.1.2.4  87/06/29  17:56:51  alex
 * Commans set to 0200-0277
 * 
 * Revision 3.1.2.3  87/06/24  22:35:47  alex
 * New readch + gettc + tc table Tested
 * 
 * Revision 3.1.2.2  87/06/23  18:53:01  alex
 * wYNESENA PEREMENNAQ lread1 I \TO OTLAVENO
 * 
 * Revision 3.1.2.1  87/06/19  17:02:06  alex
 * Start revision for red/4
 * 
 * Revision 3.8  87/06/12  18:11:27  alex
 * Исправлены ошибки в режиме vilcase и мелочи для vt-200
 * 
 * Revision 3.7  87/06/06  16:49:42  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.6  87/06/05  23:51:24  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.5  87/06/04  23:44:33  alex
 * Scroll on -sr- or -al/dl- added
 * 
 * Revision 3.4  86/09/19  19:55:47  alex
 * Версия для СМ-1700
 * 
 * Revision 3.3  86/08/04  20:52:54  alex
 * Bepqh dk LMNQ/DELNQ 2
 * 
 * Revision 3.2  86/07/24  19:05:13  alex
 * Об'единены версии текстов для ЕС и СМ
 * 
 * Revision 3.1.1.6  86/07/15  22:13:45  alex
 * RED 3.3/EC.
 * 
 * Revision 3.1.1.5  86/06/20  23:35:44  alex
 * 
 * Revision 3.1.1.4  86/06/16  22:19:58  alex
 * Первая версия с повтором сеанса (пока грязная)
 * 
 * Revision 3.1.1.2  86/06/05  18:54:17  alex
 * Отлажено_на_модели
 * 
 * Revision 3.1.1.1  86/06/05  00:06:18  alex
 * Отлажено_на_модели
 * 
 * Revision 3.1  86/04/20  23:42:39  alex
 * Базовая версия для ЕС.
 * 
 * Revision 3.1  86/04/20  23:42:39  alex
 * *** empty log message ***
 * 
 * Revision 1.5  86/04/13  22:01:59  alex
 *
 * r.tele.c - Работа с терминалом - логический уровень
 *
 *
 */

#include "r.defs.h"

#ifdef DEMOSES
#define fixcurs() /* */
#endif

/*   out_win(l0,lf,c0,cf) - выдать строки с l0 до lf, колонки с c0 до cf
 *   если cf < 0 - выдать до конца окна
 *   если lf < 0 - выдать до конца окна
 *   out_lines(l0,lf ) == out_win(l0,lf,-1,-1)
 *   out_cols (c0,cf) == out_win(cursorline,cursorline,c0,cf)
 *   Особый случай: если c0 == cf, то выдавать символ всегда, даже
 *   если он пробел и на экране уже лежит пробел
 */
out_cols(c0,cf)
int c0,cf;
{
        out_win(cursorline,cursorline,c0,cf);
}
out_lines(l0,lf)
int l0,lf;
{
        out_win(l0,lf,0,-1);
}
out_win(l0,lf,c0,cf)
int l0,lf;
int c0,cf;
{
    register int col; /* Текущая колонка */
    register int i;   /* Рабочий регистр */
    int lin,alin;          /* Текущая строка - отн. и абс. номера */
    int fc, lc;       /* Первая и последняя колонки в строке (были) */
    char lmc,rmargflg;
    int uc;           /* Номер начальной колонки в окне */
    register char *cp;/* Указатель на строкy */
    char *ce;         /* Указ. на конец строки */
    if ( zoomflag && curport != &zoomport ) return;
    if ( cf < 0 ) cf = curport->rtext;
    if ( lf < 0 ) lf = curport->btext;
    uc = ABS_COL(0);
    lmc = (curport->lmarg == curport->ltext ? 0 :
        uc == 0 ? LMCH : MLMCH);
    rmargflg = (curport->ltext + curport->rtext < curport->rmarg);
    while ( (lin = l0++) <= lf)
    {
        if (lin != lf && intrup())  return;
        if (!getline(alin =  ABS_LIN(lin) ) )
        {
            if ( lmc ) lmc = ELMCH;
        }
        if (lmc != curport->lmchars[lin] && lmc != 0)
        {
            poscursor(-LMARG,lin);
            putch(lmc,A_MARG);
            curport->lmchars[lin] = lmc;
        }
        if (rmargflg != 0) rmargflg = RMCH;
        /* Установим начало и конец строки для выдачи */
        cp = cline + uc + c0;
        if ( uc + cf + 1 < ncline-1 )
            ce = cline + (uc + cf + 1);
        else
            ce = cline + (ncline-1);
        /* А теперь прочитаем для скорости fc, lc */
        fc = curport->firstcol[lin] & 0377;
        lc = curport->lastcol[lin]  & 0377;
        /* А теперь установим признак - курсор не стоит */
        i = 0;
        col = c0;
        /* Выдача! Сначала пройдем начало строки по бывшим пробелам */
        if ( fc >= c0 )
        {
            while ( cp < ce && *cp == ' ' )
            {
                if ( col >= fc )
                /* то у нас пробел, а на экране был не пробел  */
                {
                    if ( !i ) poscursor(col,lin), setatr(A_NORM);
                    i++;
                }
                col++;
                cp++;
            }
            if ( i > 0 ) putblanks(i);
            /* если 1. fc в нашем диапазоне или 2. есть еще непробелы */
            if ( col != fc && (fc <= cf || cp < ce) )
                    (curport->firstcol)[lin] = col;
        }
        /* Теперь выдадим содержательную часть */
        if ( cp < ce  )
        {
            if ( i == 0 )
                poscursor(col,lin), setatr(A_NORM);
            i = 0;
            while( cp < ce )
            {
                putcha(*cp++);
                i++;
            }
            cursorcol += i;
            col += i;
            /* Особый случай - длинная строка в окне без рамок */
            if ( col > curport->rtext  &&
                *cp != NEWLINE )
            {
                if ( !rmargflg )
                {
                        poscursor(col-1,lin);
                        setatr(A_INFO);
                        putcha(cp[-1]);
                }
                else
                rmargflg = MRMCH;
            }
        }
        /* А теперь нужно разобраться с оставшимися в хвосте пробелами */
        /* Особо отметили col==0 - пустая строка. Тогда не надо -1 */
        /* lc у нас показывает на первую пустую колонку в строке */
        /* i == 0, если не было ни нач. пробелов, ни содерж. части */
        if ( lc <= cf +1 )
            (curport->lastcol)[lin] = col;
        else
            lc = cf+1; /* Пока */
        /* Особый случай для затирания курсора - c0 == cf, нужно выдать ' ' всегда */
        /* 1. c0 == cf 2. Не было вывода */
           if ( cf == c0 && i == 0)
                lc = col+1; /* Для затирания курсора */
           if ( lc > col )
            {
                if ( !i ) poscursor(col,lin), setatr(A_NORM);
                putblanks(lc-col);
            }
        /* Если в тек. строке маркер выделения, выведем его */
        if ( sel_flag && curwksp == sel_wspace && sel_lin == alin &&
             (i = (sel_col-uc) ) >= c0 && i <= cf )
             {
                      poscursor(i,lin);
                      putch('+',A_ERROR+1);
             }
        /* Осталось довывести рамку */
        if (rmargflg && rmargflg != curport->rmchars[lin])
        {
            poscursor(curport->rmarg - curport->ltext, lin);
            putch(rmargflg,A_MARG);
            curport->rmchars[lin] = rmargflg;
        }
    }
}


/*
 * out_mark(lin,col,N)
 * Выдать марку по абсолютному адресу
 * 0 - выдать символ
 * 1 - марка курсора
 */
out_mark(lin,col,marka)
register int lin,col,marka;
{
    int l,c;
    lin = REL_LIN(lin);
    col = REL_COL(col);
    if ( lin < 0 || col < 0 || lin > curport->btext || col > curport->rtext )
        return;
    l = cursorline;
    c = cursorcol;
    if ( marka == 0 )
    {
        out_win(lin,lin,col,col);
    }
    else
    {
        poscursor(col,lin);
        putch('+',A_ERROR+1);
    }
    poscursor(c,l);
}

#ifndef DEMOSES
#include "S.tele.h"
#endif

/*
 * poscursor(col,lin) -
 * Позиционирование курсора в текущем окне
 */
poscursor(col,lin)
int col,lin;
#ifndef DEMOSES
{ 
    register int scol,dcol,dlin;
    int slin,alin;
    if ( cursorcol+curport->ltext >= LINEL ) goto direct;
    if (cursorline == lin)
    {
        if (cursorcol == col) return;
        if ( cur_atr != 0 ) goto direct;
        if ((cursorcol == col-1)&&(putcha(CORT)) )
        {
            ++cursorcol;
            return;
        }
        if ((cursorcol == col+1)&&(putcha(COLT)) )
        {
            --cursorcol;
            return;
        }
    }
    if (cursorcol == col)
    {
    /* Переход через scroll-границу запрещен */
        if ( n0scroll == (alin = cursorline+curport->ttext) ||  n1scroll == alin )
              goto direct;
        if ( cur_atr != 0 ) goto direct;
        if ((cursorline == lin-1)&&(putcha(CODN)))
        {
            ++cursorline;
            return;
        }
        if ((cursorline == lin+1)&&(putcha(COUP)) )
        {
            --cursorline;
            return;
        }
    }
direct:
    scol=col+curport->ltext; 
    slin=lin+curport->ttext; /* screen col, lin */
    dcol=col-cursorcol; 
    dlin=lin-cursorline;           /* delta col,lin   */
    cursorcol=col; 
    cursorline=lin;
    if (pcursor(scol,slin)) return; /* direct positioning */
    if ( (( abs(scol)+abs(slin)) < (abs(dcol)+abs(dlin))))
    { 
        putcha(COHO); 
        dcol=scol; 
        dlin=slin;
    }
    if (dcol>0) { 
        while (dcol--) putcha(CORT); 
    }
    else if (dcol<0) { 
        while (dcol++) putcha(COLT); 
    }
    if (dlin>0) { 
        while (dlin--) putcha(CODN); 
    }
    else if (dlin<0) { 
        while (dlin++) putcha(COUP); 
    }
    return;
}
#else /* +DEMOSES */
{ 
    register int scol, slin;
    if (cursorline == lin && cursorcol == col ) return;
    scol=col+curport->ltext; 
    slin=lin+curport->ttext; /* screen col, lin */
    cursorcol=col;cursorline=lin;
    pcursor(scol,slin);
    return;
}
#endif /* +DEMOSES */

/*
 *      Движение курсора в границах окна "curscreen"
 *      Значение аргумента:
 *     UP - Вверх
 *     CR - переход на начало строки
 *     DN - вниз на строку
 *     RT - вправо на колонку
 *     LT - влево на колонку
 *     TB - прямая табуляция
 *     BT - табуляция назад
 *     Большая часть этих команд используется на ЕС тольо в макро.
 *      0 - не операции (только проверить)
 */
movecursor(arg)
int arg;
{
    register int lin,col,i;
    int nofix,ko;
    nofix = (arg&MOVE_NOFIX) != 0;
    arg &= MOVE_MASK;
    ko = -1;
    lin=cursorline;
    col=cursorcol;
    switch (arg)
    {
    case 0: 
        break;
    case HO:
        col = lin = 0;
        break;
    case UP:
        if ( nofix && lin == curport->tedit + defedit )
        {
                ko = CCMILINE;
                goto retn;
        }
        --lin;
        break;
    case RN:
        col = curport->ledit;       /* break не нужен */
        if(on_autoi) col = REL_COL(autoindent);
    case DN:
        if ( nofix && lin == curport->bedit - defedit )
        {
                ko = CCPLLINE;
                goto retn;
        }
        ++lin;
        break;
    case RT:
        ++col;
        break;
    case LT:
        --col;
        break;
    case TB:
        col = col + curwksp->ulhccno;
        while (++col < MAXTCOL && !ISTAB(col));
        col -= curwksp->ulhccno;
        break;
    case BT:
        col = col + curwksp->ulhccno;
        while (--col > 0 &&  !ISTAB(col) );
        col = (col < 0 ? -1: col - curwksp->ulhccno);
        break;
    }
    if(nofix ) {
        if (col > curport->redit) col = curport->redit, ko=CCRPORT;
        else if (col < curport->ledit)  col = curport->ledit, ko=CCLPORT;
        if (lin < curport->tedit) lin = curport->tedit, ko = CCMILINE;
        else if (lin > curport->bedit) lin = curport->bedit, ko = CCPLLINE;
    } else {
fix:
        if (col > curport->redit) col = curport->ledit;
        else if (col < curport->ledit) col = curport->redit;
        if (lin < curport->tedit) lin = curport->bedit;
        else if (lin > curport->bedit) lin = curport->tedit;
    }
retn:
    poscursor(col,lin);
    return(ko);
}

/*
 * putch(c,flag) -
 *      поместить символ в текущую позицию.
 *      если установлен flag=1, то учесть его
 *      в размерах строки
 */
putch(j,flg)
int flg;
char j;
{   int atr;
    atr = flg & (~0377);
    flg &= 0377;
    if ( atr != cur_atr) setatr(atr);
    if (flg && j != ' ')
    {
        if ((curport->firstcol)[cursorline] > cursorcol)
            (curport->firstcol)[cursorline] = cursorcol;
        if ((curport->lastcol)[cursorline] <= cursorcol)
            (curport->lastcol)[cursorline] = cursorcol+1;
    }
#ifdef DEMOSES
    putcha(j);
    ++cursorcol;
#else /* -DEMOSES */
    ++cursorcol;
    if (fixcurs() == 0) putcha(j);
    if (cursorcol <= 0) poscursor(curport->ledit,
    cursorline < curport->tedit ? curport->tedit :
    cursorline > curport->bedit ? curport->tedit :
    cursorline);
    movecursor(0);
#endif /* -DEMOSES */
}

#ifndef DEMOSES
/*
 * fixcurs() -
 * Обработка граничных эффектов на экране.
 * Устанавливает cursorcol, cursorline равными корректным значениям
 * Управляется через eolflag (=1 - курсор фиксируется на границе экрана)
 * При достижении правой граници фиксируется cursorcol, если есть eolflag.
 * Правильная последовательность:
 * 1) Увеличить cursorcol; 2) Вызвать fixcurs; 3) Вывести символ;
 * 4) Если cursorcol <= 0, спозиционировать курсор. fixcurs возвращает 1,
 *    если курсор переместился на следующую строку.
 */

fixcurs()
{
    if (eolflag && curport->ltext+cursorcol>=LINEL)    /* for VT-52 */
    { 
        cursorcol=LINEL-curport->ltext; 
        return 0;
    }
    if (curport->ltext + cursorcol >= LINEL) {
        cursorcol = - curport->ltext;
        if (curport->ttext + ++cursorline >= NLINES) {
            cursorline = - curport->ttext;
            putcha(COHO);
            return (1);
        }
    }
    return (0);
}


/*
 *  param() - Запрос параметра
 *      Три типа параметров.
 *              paramtype = -1 -- определение области
 *              paramtype = 0  -- пустой аргумент
 *              paramtype = 1  -- строка.
 *              при использовании макро бывает paramtype = -2 - tag defined
 *      Возвращается код введенной команды, в paramv - указ. на строку.
 *      Возвращается указатель на введенную строку (paramv).
 *      Длина возвращается в переменной paraml.
 *      Если при очередном вызове paraml не 0, старый paramv
 *      освобождается, так что если старый параметр нужен,
 *      нужно обнулить paraml.
 *      Вариант "param" для ЕС записан в отдельном файле
 */
#define LPARAM 20       /* Длина приращения длины */
int param(macro)
int macro;
{
    register char *c1;
    char *cp,*c2;
    register int i,pn;
    int lread1;
    struct viewport *w;
    int chr;
    if (paraml != 0 && paramv != 0) zFree(paramv);
    paraml = 0;
    paramc1 = paramc0 = cursorcol;
    paramr1 = paramr0 = cursorline;
    putch(COCURS,1);
    poscursor(cursorcol,cursorline);
    w = curport;
back:
    telluser(macro?"mac: ":"arg: ",0);
    switchport(&paramport);
    poscursor(5,0);
    do
        {
        lread1=readch();
    }
    while (lread1 == CCBACKSPACE);
    if(macro) goto rmac;
    if (ISACMD(lread1) && (lread1>= CC && (lread1<=BT+CC)) ||
        (lread1 == CCBEGIN || lread1 == CCEND || lread1 == CCWORDLEFT ||
         lread1 == CCWORDRIGHT ))
    {
        switchport(w);
        poscursor(paramc0,paramr0);
        unread1(lread1);
        return(CCSELECT);
    }
    else if (ISACMD(lread1))
    {
        paraml = 0; 
        paramv = NULL;
        paramtype=0;
    }
    else
    {
rmac:           
        unread1(lread1);
        paraml = pn = 0;
loop:
        chr= lread1 = readch();
        if (pn >= paraml)
        {
            cp = paramv;
            paramv = salloc(paraml + LPARAM+1); /* 1 for dechars */
            c1 = paramv;
            c2 = cp;
            for (i=0; i<paraml; ++i) *c1++ = *c2++;
            if (paraml) zFree(cp);
            paraml += LPARAM;
        }
        /* Конец ввода параметра */
        if ((!macro && ISACMD(lread1) && lread1 != CCCTRLQUOTE )|| lread1==CCBACKSPACE || lread1==CCQUIT)
        {
            if (lread1 == CCBACKSPACE &&  cursorcol != curport->ledit)
                /* backspace */
            {
                if (pn == 0)
                {
                    goto loop;
                }
                movecursor(LT);
                --pn;
                if((paramv[pn]&0340) ==0)
                {
                    putch(' ',A_OUT);
                    movecursor(LT);
                    movecursor(LT);
                }
                paramv[pn] = 0;
                putch(' ',A_OUT);
                movecursor(LT);
                if (pn == 0) goto back;
                goto loop;
            }
            else chr = 0;
        }
        if (chr == 0177) chr = 0;    /* del is a contol code  */
        paramv[pn++] = chr;
        if (chr)
        {
            if((chr&0140)==0){
                putch('^',0); 
                chr = chr | 0100;
            }
            putch(chr,A_OUT);
            goto loop;
        }
        paramtype = 1;
    }
    switchport(w);
    out_win(paramr0,paramr0,paramc0, paramc0);
    poscursor(paramc0,paramr0);
    setatr(A_NORM);
    return (lread1);
}
#endif /* DEMOSES */

/*
 * drawport(newp,vertf) -
 * Рисовать окно newp (рамки). vertf говорит, нужно ли
 * рисовать верт. рамку или табуляции: DRAW_HORI, DRAW_ALL, DRAW_TABS (не исп)
 */
drawport(newp,vertf)
struct viewport *newp;
int vertf;
{
    register struct viewport *newport;
    register int i;
    register char c;
    int j;
    int cc,cl;
    int lu,ld,ru,rd;
    newport = newp;
    if ( zoomflag && curport != &zoomport ) goto retn;
    switchport(&wholescreen);
    cc = cursorcol; cl = cursorline;
    setscroll(&wholescreen,0);
    if(newport->tmarg == 0 ) { lu = G_LU; ru = G_RU; }
    else { lu = (newport->lmarg?G_LU:G_LM);
           ru = (newport->rmarg!=wholescreen.rmarg?G_RU:G_RM);
         }
    if(newport->bmarg == wholescreen.bmarg-NPARAMLINES ) {
         ld = G_LD; rd = G_RD; }
    else { ld = (newport->lmarg?G_LD:G_LM);
           rd = (newport->rmarg!=wholescreen.rmarg?G_RD:G_RM);
         }
    if(vertf != DRAW_TABS && newport->tmarg != newport->ttext) {
        poscursor(newport->lmarg,newport->tmarg);
        putch(lu,A_MARG);
        for (i = newport->lmarg+1;i <= newport->rmarg-1;i++) putch(TMCH,A_MARG);
        putch(ru,A_MARG);
    }
    if ( vertf == DRAW_ALL)
        for (j = newport->tmarg + 1;j <= newport->bmarg - 1;j++)
        {
            if (c = newport->lmchars[j - newport->tmarg - 1])
            {
                poscursor(newport->lmarg,j);
                putch(c,A_MARG);
                poscursor(newport->rmarg,j);
                putch(newport->rmchars[j - newport->tmarg - 1],A_MARG);
            }
        }
    if( newport->bmarg != newport->ttext+newport->btext) {
        poscursor(newport->lmarg,newport->bmarg);
        putch(ld,A_MARG);
        for (j = newport->wksp->ulhccno,i = newport->lmarg+1;i <= newport->rmarg-1;j++,i++)
                putch(B_MARG(j),A_MARG);
        putch(rd,A_MARG);
    }
    poscursor(cc, cl);
retn:
    switchport(newport);
    need_box = 0;
    chg_tabs = 0;
}

static tellatr = A_INFO;
/*
 * error(msg) -
 * Выдать сообщение об ошибке
 */
error(msg)
char *msg;
{
       if ( video_mode == 0 )
        {
                printf(DIAG("Error: %s\n","Ошибка: %s\n"),msg);
                return;
        }
        ps_inmac = (char *)0;
        putcha(COBELL); 
        putcha(COBELL); 
        putcha(COBELL);
        tellatr = A_ERROR;
        telluser("**** ",0);
        telluser(msg,5);
        tellatr = A_INFO;
        errsw = 1;
}

/*
 * telluser(msg,col) -
 * Выдать сообщение с колонки  col. Если col =0 -
 * очистить окно параметров
 */
telluser(msg,col)
char *msg;
int col;
{
    struct viewport *oldport;
    register int c,l;
    int oldledit;
    int oldatr;
    if ( video_mode == 0 )
     {
             printf(DIAG("Mesg: %s\n","Сообщ: %s\n"),msg);
             return;
     }
    oldport = curport;
    c = cursorcol;
    l = cursorline;
    switchport(&paramport);
    oldatr = setatr(A_INFO);
    oldledit=paramport.ledit;
    paramport.ledit = 0;
    if (col == 0)
    {
        poscursor(0,0);
        putblanks(paramport.redit);
    }
    poscursor(col,0);
    /*      while (*msg) putch(*msg++,0);   */
    info(msg, PARAMREDIT);
    paramport.ledit=oldledit;
    switchport(oldport);
    setatr(oldatr);
    poscursor(c,l);
    dumpcbuf(0);
}

/*
 * infmesg(msg,col,atr) -
 * Выдать сообщение с колонки  col атриьут atr в поле сообщений.
 */
infmesg(msg,col,atr)
char *msg;
int col;
{
    struct viewport *oldport;
    register int c,l;
    int oldledit;
    int j;
    if ( video_mode == 0 )
     {
             return;
     }
    oldport = curport;
    c = cursorcol;
    l = cursorline;
    switchport(&paramport);
    poscursor(col,0);
    j = setatr(atr);
    while (*msg) putcha(*msg++);/* ,atr); */
    j = setatr(j);
    switchport(oldport);
    poscursor(c,l);
    dumpcbuf(0);
}



/*
 * info(ss,ml)
 * - выдать (с преобразованием во внешнюю форму) строку
 * ss в окно параметров, макс. длина "ml"
 */
info(ss,ml)
char *ss;
int ml;
{ 
    register char *s=ss;
      char putbuf[PARAMRINFO+5], *si, *so;
      int oldatr;
      so = putbuf;
    if( lcasef )
    { 
        int no=1;
        si=s; 
        while(*s++); /* na{li konec */
        exinss(&si, --s, &so, &no, PARAMRINFO);
        *so =0;
        s=putbuf;
    }
    while (*s && cursorcol < ml) putch( ( latf? *s++ &0177 : *s++ ),tellatr);
}

#ifndef DEMOSES
/*
 * shiftview(n0, nl) - пересчитать описатель окна после сдвига
 * строк после n0-й на nl строк вверх (вниз, если <0)
 */
shiftview(n0,nl)
int n0,nl;
{
        register int i,j;
        register struct viewport *cp;
        cp=curport;
        if(nl>0) {
                for(i=n0,j=n0+nl;j<=cp->btext;i++,j++) {
                        cp->lmchars[i] = cp->lmchars[j];
                        cp->rmchars[i] = cp->rmchars[j];
                        cp->firstcol[i] = cp->firstcol[j];
                        cp->lastcol[i] = cp->lastcol[j];
                }
                for(;i<= cp->btext;i++){
                        cp->lmchars[i] = cp->rmchars[i] = ' ';
                        cp->firstcol[i] = cp->lastcol[i] = 0;
                }
                return(1);
        } 
        else {
                for(i=cp->btext,j=cp->btext+nl;j>=n0;i--,j--) {
                        cp->lmchars[i] = cp->lmchars[j];
                        cp->rmchars[i] = cp->rmchars[j];
                        cp->firstcol[i] = cp->firstcol[j];
                        cp->lastcol[i] = cp->lastcol[j];
                }
                for(;i>= n0;i--){
                        cp->lmchars[i] = cp->rmchars[i] = ' ';
                        cp->firstcol[i] = cp->rtext;
                        cp->lastcol[i] = 0;
                }
                return(1);
        }
}
/*   Руднев А.П. Москва, ИАЭ им. Курчатова, 1984 */
#endif
