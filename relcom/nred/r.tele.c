/*
 *      Редактор RED. ИАЭ им. И.В. Курчатова, ОС ДЕМОС
 *
 *      $Header: /home/sergev/Project/vak-opensource/trunk/relcom/nred/RCS/r.tele.c,v 3.1 1986/04/20 23:42:39 alex Exp $
 *      $Log: r.tele.c,v $
 *      Revision 3.1  1986/04/20 23:42:39  alex
 *      *** empty log message ***
 *
 * Revision 1.5  86/04/13  22:01:59  alex
 *
 * r.tele.c - Работа с терминалом - логический уровень
 *
 *
 */

#include "r.defs.h"

/*   putup(l0,lf) - выдать строки с l0 до lf
 *      Особый случай - если l0 отрицательно, то выдать только строку lf.
 *      При этом строка берется из cline, и выдавать только с колонки -l0.
 */
putup(lo,lf)
int lo,lf;
{
    register int i, l0, fc;
    int j,k,l1;
    char lmc,*cp,c,rmargflg;
    l0 = lo;
    lo += 2;
    if (lo > 0) lo = 0; /* Нач. колонка */
    l1 = lo;
    lmc = (curport->lmarg == curport->ltext ? 0 :
    curwksp->ulhccno == 0 ? LMCH : MLMCH);
    rmargflg = (curport->ltext + curport->rtext < curport->rmarg);
    while (l0 <= lf)
    {
        lo = l1;
        if (l0 < 0)
        {
            l0 = lf;
            lf = -1;
            i = 0;
        }
        else
        {
            if (l0 != lf && intrup())  return;
            if (i = wseek(curwksp,curwksp->ulhclno + l0)
                && lmc != 0) lmc = ELMCH;
        }
        if (lmc == curport->lmchars[l0] || lmc == 0 || lo < 0) poscursor(0,l0);
        else
        {
            poscursor(-1,l0);
            putch(lmc,0);
        }
        curport->lmchars[l0] = lmc;
        if (rmargflg != 0) rmargflg = RMCH;
        if (i != 0) i = 0;
        else
        {
            if (lf >= 0) c = chars(1);
            i = (ncline - 1) - curwksp->ulhccno;
            if (i < 0) i = 0;
            else if (i > curport->rtext)
            {
                if (i > 1 + curport->rtext && rmargflg)
                    rmargflg = MRMCH;
                i = 1 + curport->rtext;
            }
        }
        /*
         * Вывод символов.
         * Пытаемся пропустить начальные пробелы
         */
        if (lo == 0)
        {
            for (fc=0;cline[curwksp->ulhccno + fc]==' ';fc++);
            j=curport->rtext+1; 
            if(fc>j) fc=j; 
            if(fc>127) fc=127;
            lo = (curport->firstcol)[l0] > fc ?
            - fc : - (curport->firstcol)[l0];
            if (i+lo<=0) lo = 0;
            else curport->firstcol[l0] = fc;
        }
        if (lo) poscursor(-lo,l0);
        j = i + lo;
        cp = cline + curwksp->ulhccno - lo;
        while(j--) putcha(*cp++);
        cursorcol += (i + lo);
        if (curport->lastcol[l0] < cursorcol)
            curport->lastcol[l0] = cursorcol;
        /* Хвост строки заполняем пробелами */
        k = (j = curport->lastcol[l0]) - i;
        if (k > 0)
        {
            putblanks(k);
        }
        fixcurs();
        if (rmargflg && rmargflg != curport->rmchars[l0])
        {
            poscursor(curport->rmarg - curport->ltext, l0);
            putch(rmargflg,0);
        }
        else movecursor(0);
        curport->rmchars[l0] = rmargflg;
        curport->lastcol[l0] = (k > 0 ? i : j);
        ++l0;
    }
}

/*
 * poscursor(col,lin) -
 * Позиционирование курсора в текущем окне
 */
poscursor(col,lin)
int col,lin;
{ 
    register int scol,dcol,dlin;
    int slin;
    if (cursorline == lin)
    {
        if (cursorcol == col) return;
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
 *      0 - не операции (только проверить)
 */
movecursor(arg)
int arg;
{
    register int lin,col,i;
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
        --lin;
        break;
    case RN:
        col = curport->ledit;       /* break не нужен */
    case DN:
        ++lin;
        break;
    case RT:
        ++col;
        break;
    case LT:
        --col;
        break;
    case TB:
        i=0;
        col = col + curwksp->ulhccno;
        while (col >= tabstops[i]) i++;
        col = tabstops[i] - curwksp->ulhccno;
        break;
    case BT:
        i=0;
        col = col + curwksp->ulhccno;
        while (col >  tabstops[i]) i++;
        col = (i ? tabstops[i-1] - curwksp->ulhccno : -1);
        break;
    }
    if (col > curport->redit) col = curport->ledit;
    else if (col < curport->ledit) col = curport->redit;
    if (lin < curport->tedit) lin = curport->bedit;
    else if (lin > curport->bedit) lin = curport->tedit;
    poscursor(col,lin);
    return;
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
{
    if (flg && lread1 != ' ')
    {
        if ((curport->firstcol)[cursorline] > cursorcol)
            (curport->firstcol)[cursorline] = cursorcol;
        if ((curport->lastcol)[cursorline] <= cursorcol)
            (curport->lastcol)[cursorline] = cursorcol+1;
    }
    ++cursorcol;
    if (fixcurs() == 0) putcha(j);
    if (cursorcol <= 0) poscursor(curport->ledit,
    cursorline < curport->tedit ? curport->tedit :
    cursorline > curport->bedit ? curport->tedit :
    cursorline);
    movecursor(0);
}

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
 *      Возвращается указатель на введенную строку (paramv).
 *      Длина возвращается в переменной paraml.
 *      Если при очередном вызове paraml не 0, старый paramv
 *      освобождается, так что если старый параметр нужен,
 *      нужно обнулить paraml.
 */
#define LPARAM 20       /* Длина приращения длины */
char *param(macro)
int macro;
{
    register char *c1;
    char *cp,*c2;
    int c;
    register int i,pn;
    struct viewport *w;
    if (paraml != 0 && paramv != 0) free(paramv);
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
        lread1 = -1;
        read1();
    }
    while (lread1 == CCBACKSPACE);
    if(macro) goto rmac;
    if (CONTROLCHAR && lread1 && (lread1<=BT))
    {
        telluser(DIAG("arg: *** cursor defined ***",
        "arg: *** указание курсором ***"),0);
        switchport(w);
        poscursor(paramc0,paramr0);
t0:
        while (CONTROLCHAR && (i=((lread1<=BT)?lread1:0)))
        {
            movecursor(i);
            if (cursorline == paramr0 &&
                cursorcol  == paramc0)  goto back;
            lread1 = -1;
            read1();
        }
        if (CTRLCHAR && lread1 != CCBACKSPACE)
        {
            if(cursorcol > paramc0)paramc1 = cursorcol;
            else paramc0 = cursorcol;
            if(cursorline > paramr0)paramr1 = cursorline;
            else paramr0 = cursorline;
            paraml = 0; 
            paramv = NULL;
            paramtype = -1;
        }
        else
        {
            error(DIAG("Printing character illegal here","Этот символ здесь недопустим"));
            lread1 = -1;
            read1();
            goto t0;
        }
    }
    else if (CTRLCHAR)
    {
        paraml = 0; 
        paramv = NULL;
        paramtype=0;
    }
    else
    {
rmac:           
        paraml = pn = 0;
loop:
        c = read1();
        if (pn >= paraml)
        {
            cp = paramv;
            paramv = salloc(paraml + LPARAM+1); /* 1 for dechars */
            c1 = paramv;
            c2 = cp;
            for (i=0; i<paraml; ++i) *c1++ = *c2++;
            if (paraml) free(cp);
            paraml += LPARAM;
        }
        /* Конец ввода параметра */
        if ((!macro && CONTROLCHAR)|| c==CCBACKSPACE || c==CCQUIT)
        {
            if (c == CCBACKSPACE &&  cursorcol != curport->ledit)
                /* backspace */
            {
                if (pn == 0)
                {
                    lread1 = -1;
                    goto loop;
                }
                movecursor(LT);
                --pn;
                if((paramv[pn]&0340) ==0)
                {
                    putch(' ',0); 
                    movecursor(LT);
                    movecursor(LT);
                }
                paramv[pn] = 0;
                putch(' ',0);
                movecursor(LT);
                lread1 = -1;
                if (pn == 0) goto back;
                goto loop;
            }
            else c = 0;
        }
        if (c == 0177) c = 0;    /* del is a contol code  */
        paramv[pn++] = c;
        if (c != 0)
        {
            if((c&0140)==0){
                putch('^',0); 
                c = c | 0100;
            }
            putch(c,0);
            lread1 = -1;
            goto loop;
        }
        paramtype = 1;
    }
    switchport(w);
    putup(paramr0,paramr0);
    poscursor(paramc0,paramr0);
    return (paramv);
}

/*
 * drawport(newp,vertf) -
 * Рисовать окно newp (рамки). vertf говорит, нужно ли
 * рисовать верт. рамку
 */
drawport(newp,vertf)
struct viewport *newp;
int vertf;
{
    register struct viewport *newport;
    register int i;
    register char c;
    int j;
    newport = newp;
    switchport(&wholescreen);
    if(newport->tmarg != newport->ttext) {
        poscursor(newport->lmarg,newport->tmarg);
        for (i = newport->lmarg;i <= newport->rmarg;i++) putch(TMCH,0); 
    }
    if ( vertf )
        for (j = newport->tmarg + 1;j <= newport->bmarg - 1;j++)
        {
            if (c = newport->lmchars[j - newport->tmarg - 1])
            {
                poscursor(newport->lmarg,j);
                putch(c,0);
                poscursor(newport->rmarg,j);
                putch(newport->rmchars[j - newport->tmarg - 1],0);
            }
        }
    if(newport->tmarg != newport->ttext) {
        poscursor(newport->lmarg,newport->bmarg);
        for (i = newport->lmarg;i <= newport->rmarg;i++) putch(BMCH,0);
    }
    /*   poscursor(newport->lmarg + 1,newport->tmarg + 1);     */
    switchport(newport);
}

/*
 * error(msg) -
 * Выдать сообщение об ошибке
 */
error(msg)
char *msg;
{
        putcha(COBELL); 
        putcha(COBELL); 
        putcha(COBELL);
        telluser("**** ",0);
        telluser(msg,5);
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
    oldport = curport;
    c = cursorcol;
    l = cursorline;
    switchport(&paramport);
    if (col == 0)
    {
        poscursor(0,0);
        putblanks(paramport.redit);
    }
    poscursor(col,0);
    /*      while (*msg) putch(*msg++,0);   */
    info(msg, PARAMREDIT);
    switchport(oldport);
    poscursor(c,l);
    dumpcbuf();
}


/*
 * rescreen(nom) -
 * Восстановление сбитого окна
 * ( nom - пока не используется)
 */
/*ARGSUSED*/
rescreen(nom)int nom;
{       
    register int i;
    int j;
    register struct viewport *curp,*curp0 = curport;
    int col=cursorcol, lin=cursorline;
    switchport(&wholescreen);
    cursorcol = cursorline =0;
    putcha(COFIN);
    putcha(COSTART);
    putcha(COHO);
    for ( j=0; j<nportlist; j++) {
        switchport(portlist[j]); 
        curp = curport;
        for (i=0;i<curport->btext+1;i++)
        {
            curp->firstcol[i]=0; 
            curp->lastcol[i]= 0; /* curport->rtext;*/
            curp->lmchars[i] = curp->rmchars[i] =' ';
        }
        drawport(curp,0);
        putup(0,curp->btext); 
    }
    switchport(curp0);
    poscursor(col,lin);
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
    if( lcasef )
    { 
        char putbuf[PARAMRINFO+1], *si, *so=putbuf;
        int no=1;
        si=s; 
        while(*s++); /* na{li konec */
        exinss(&si, --s, &so, &no, PARAMRINFO);
        *so =0;
        s=putbuf;
    }
    while (*s && cursorcol < ml) putch( ( latf? *s++ &0177 : *s++ ),0);
}

/*   Руднев А.П. Москва, ИАЭ им. Курчатова, 1984 */
