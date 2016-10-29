/*
 *      Редактор RED.
 *      r.wind.c - работа с окнами
 *      $Header: r.wind.c,v 4.20 90/05/22 23:22:20 alex Exp $
 *      $Log:   r.wind.c,v $
 * Revision 4.20  90/05/22  23:22:20  alex
 * First rev. red 4.2
 * 
 * Revision 4.10  90/02/05  19:52:36  alex
 * Base revision 4.1
 * 
 * Revision 4.2  88/03/31  22:49:23  alex
 * Исправлена ошибка при работе на терминале Электроника.
 * 
 * Revision 4.1  88/03/31  22:05:29  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 * 
 * Revision 3.8  88/03/30  17:27:23  alex
 * Исправлена ошибка в readch. Введены функции для удаления/вставки
 * строк на экране через ChangeScroll/InsertLine-DeleteLine.
 * 
 * Revision 3.1.2.1  87/06/19  17:03:04  alex
 * Start revision for red/4
 * 
 * Revision 3.7  87/06/05  23:51:43  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.6  87/06/04  23:44:54  alex
 * Scroll on -sr- or -al/dl- added
 * 
 * Revision 3.5  86/09/19  19:57:36  alex
 * Версия для СМ-1700
 * 
 * Revision 3.3  86/07/24  23:38:32  alex
 * Исправлена ошибка в makeport.
 * 
 * Revision 3.2  86/07/24  19:06:50  alex
 * Об'единены версии текстов для ЕС и СМ
 * 
 * Revision 3.1.1.1  86/06/05  00:07:27  alex
 * Отлажено_на_модели
 * 
 * Revision 3.1  86/04/20  23:43:41  alex
 * Базовая версия для ЕС.
 * 
 * Revision 3.1  86/04/20  23:43:41  alex
 * *** empty log message ***
 * 
 *
 *
 */


#include "r.defs.h"

/*
 *   movew(nl) -
 *    сдвинуть текущее окно на nl строк вниз (+) или вверх
 */
movew(nl,fix)
int nl,fix;
{
        register int cc,cl,i;
        int nladd;
        cc = cursorcol;
        cl = cursorline;
        nladd = 0;
        if( nl == PL1MOVEW ) {
                nl = 1; nladd = defplline-1;
        }
        if( nl == MI1MOVEW ) {
                nl = -1; nladd = -defmiline+1;
        }
rep:
        if ((curwksp->ulhclno += nl) < 0) {
                cl += curwksp->ulhclno;
                nl -= curwksp->ulhclno;
                curwksp->ulhclno = 0;
                if ( cl < 0 ) cl = 0;
        };
        if( nl == 0) goto afix;
        putline(1);
        if( on_scroll && scroll(nl,0))
        {
                if(nl>0)
                        out_lines(curport->btext-nl+1, curport->btext);
                else
                        out_lines(0,-nl-1);
             goto afix;
        };
        if ( nladd ) {
                nl = nladd; nladd = 0; goto rep;
        }
        out_lines(0,curport->btext);
afix:
        if(fix && (i=cl-nl)>=0 && i<=curport->btext) cl=i;
        poscursor(cc,cl);
}

/*
 * movep(nc) -
 *  сдвинуть окно на nc строк вправо
 */
movep(nc)
int nc;
{
    register int cl,cc;
    cl = cursorline;
    cc = cursorcol;
    if ((curwksp->ulhccno + nc) < 0) nc = - curwksp->ulhccno;
    if (nc == 0) return;
    curwksp->ulhccno += nc;
    out_lines(0,-1);
    if ((cc -= nc) < curport->ledit) cc = curport->ledit;
    else if (cc > curport->redit) cc = curport->redit;
    if ( showtabs ) drawport(curport,DRAW_TABS);
    poscursor(cc,cl);
}

/*
 *  gtfcn(number) -
 *  перейти к строке number
 */
gtfcn(number)
int number;
{
    register int i;
    movew(number - curwksp->ulhclno - defplline,0);
    if ((i = number - curwksp->ulhclno) >= 0) {
        if (i > curport->btext) i = curport->btext;
        poscursor(cursorcol,i);
    }       
}
/*
 * cgoto(ln,col,slin,lkey) -
 * Подвести тек. окно search и mgotag
 * slin - номер строки, на которой был курсор раньше
 * (для стирания отметки курсора)
 * lkey = 0 - не двигать окно, если можно.
 */
cgoto(ln,col,slin,lkey)
int ln,col,slin,lkey;
{
    register int lin;
    lin  = ln - curwksp->ulhclno;
    if (lkey || lin  < 0 || lin  > curport->btext)
    {
        lkey = -1;
        lin = defplline;
        if ((curwksp->ulhclno = ln - defplline) < 0)
        {
            lin += curwksp->ulhclno;
            curwksp->ulhclno = 0;
        }
    }
    col -= curwksp->ulhccno;
    if (col < 0 || col > curport->rtext)
    {
        curwksp->ulhccno += col-(curport->rtext/2);
        col = (curport->rtext/2);
        lkey = -1;
    }
    /* Если нам приходится смещать окно, попробуем сдвинуть его влево до упора */
    if ( lkey && curwksp->ulhccno + col < curport->rtext )
    {
        col += curwksp->ulhccno;
        curwksp->ulhccno = 0;
    }
    if (lkey) out_lines(0,-1);
    else  if (slin >=0) out_lines(slin,slin);
    poscursor(col,lin);
}


/*
 * switchport(w) -
 * перейти к окну "w".
 * cursorcol, cursorline пересчитываются к новому окну
 */
switchport(ww)
struct viewport *ww;
{
    register struct viewport *w=ww;
    cursorcol  -= (w->ltext - curport->ltext);
    cursorline -= (w->ttext - curport->ttext);
    curport = w;
    if (curwksp = curport->wksp) curfile = curwksp->wfile;
}

/*
 * setupviewport() -
 *  создать новое окно
 *  c = 1 если окно имеет границы по краям
 */
setupviewport(ww,cl,cr,lt,lb,c)
struct viewport *ww;
int cl,cr,lt,lb,c;
{
    register int i,size;
    register struct viewport *w;
    w = ww;
    w->lmarg = cl;
    w->rmarg = cr;
    w->tmarg = lt;
    w->bmarg = lb;
    if (c)
    {
        w->ltext = cl + LMARG;
        w->rtext = cr - cl - 2*LMARG;
        w->ttext = lt + 1;
        w->btext = lb - lt - 2;
    }
    else
    {
        w->ltext = cl;
        w->rtext = cr - cl;
        w->ttext = lt;
        w->btext = lb - lt;
    }
    w->ledit = w->tedit = 0;
    w->redit = w->rtext;
    w->bedit = w->btext;
    /* eventually this extra space may not be needed */
    w->wksp = (struct workspace *)salloc(SWKSP);
    w->wksp->next_wksp = w->wksp;
    w->firstcol = salloc(size = NLINES - lt + 1);
    for (i=0;i<size;i++) (w->firstcol)[i] = w->rtext + 1;
    w->lastcol = salloc(size);
    w->lmchars = salloc(size);
    w->rmchars = salloc(size);
    w->wksp->curfsd = openfsds[2]; /* "#" - так как он всегда есть */
}

/*
 * to_home (cp) - загнать курсор в начало всех подокон
 * данного рабочего пространства
 */
to_home(cp,port)
struct workspace *cp;
struct viewport *port;
{
        register  struct workspace *pws;
        pws = cp;
        do
        {
                if (pws->ccol > port->rtext )
                {
                      pws->ccol = 0;
                }
                if (pws->crow > port->btext )
                {
                      pws->crow = 0;
                }
                pws = pws->next_wksp;
        }
        while (pws != cp);
}

/* makeport() -
 *  создание нового окна
 */
makeport(file)
char *file;
{
    register struct viewport *oldport, *newport;
    int ln=0, cn=0;
    char horiz;             /* 1 - если горизонтально */
    register int i;
    char *curfname;
    struct workspace *pws;
    int portnum;
    unzoom(1);  /* Там же делается putline(1) */
    /* Есть ли место */
    if (nportlist >= MAXPORTLIST)
    {
        error(DIAG("Can't make any more windows.","Слишком много окон"));
        return;
    }
    if ( *next_args )
          curfname = *next_args++;
    else
          curfname = openfnames[curwksp->wfile];
    if ( !file ) {
              file = curfname;
              ln = curwksp->ulhclno + cursorline;
              cn = curwksp->ulhccno + cursorcol;
              }
    if (cursorcol == 0 && cursorline > 0
        && cursorline < curport->btext) horiz = 1;
    else if (/* cursorline == 0 && */ cursorcol > 0
        && cursorcol < curport->rtext-1) horiz = 0;
    else
    {
        error(DIAG("Can't put a window there.","Здесь нельзя создать окно"));
        return;
    }
    oldport = curport;
    curwksp->ccol = cursorcol;
    curwksp->crow = cursorline;
    newport = portlist[nportlist++]=(struct viewport *)salloc(SVIEWPORT);
    /* Ищем номер curport */
    for (portnum=0;portlist[portnum]!=curport;portnum++);
    newport->prevport = portnum;
    if (horiz)
    {
        setupviewport(newport,oldport->lmarg,
        oldport->rmarg,
        oldport->ttext + cursorline,
        oldport->bmarg,1);
        oldport->bmarg = oldport->ttext + cursorline;
        oldport->btext = oldport->bedit = cursorline - 1;
        for (i=0;i <= newport->btext;i++)
        {
            newport->firstcol[i] =
                oldport->firstcol[i+cursorline+1];
            newport->lastcol[i] =
                oldport->lastcol[i+cursorline+1];
        }
    }
    else
    {
        setupviewport(newport,oldport->lmarg + cursorcol + 3-LMARG,
        oldport->rmarg,
        oldport->tmarg,
        oldport->bmarg,1);
        oldport->rmarg = oldport->lmarg + cursorcol + 2-LMARG;
        oldport->rtext = oldport->redit = cursorcol - LMARG;
        for (i=0;i <= newport->btext;i++)
        {
            if (oldport->lastcol[i] > oldport->rtext + 1)
            {
                newport->firstcol[i] = 0;
                newport->lastcol[i] = oldport->lastcol[i] -
                    cursorcol - 2;
                oldport->lastcol[i] = oldport->rtext + 1;
                oldport->rmchars[i] = MRMCH;
            }
        }
    }
    newport->flags &= ~WF_ROLL;
    oldport->flags &= ~WF_ROLL;
    /* Обнуляем тек. координаты в остальных wksp на нашем окне */
    switchport(newport);
    to_home(oldport->wksp,oldport);
    defplline = defmiline = (newport->btext)/ 3 + 1;
    if (editfile(file,ln,cn,1,1) <= 0 && editfile(curfname,ln,cn,0,1) <= 0)
        error(DIAG("Default file gone: notify sys admin.","Нет файла инструкций."));
    drawport(oldport,DRAW_ALL);
    drawport(newport,DRAW_ALL);
    poscursor(0,0);
}

/*
 * drop_wksp(port)
 * - уничтожает текущее рабочее пространство, переходя на следующее
 * Если это единственное рабочее пространство, ответ 1, иначе 0
 */
drop_wksp(port)
struct viewport *port;
{
    register struct workspace *pwsp, *pwsp0;
    pwsp0 = pwsp = port->wksp;
    if ( !pwsp ) return(1);
    while ( pwsp->next_wksp != pwsp0 )
        pwsp = pwsp->next_wksp;
    /* 1 случай. Там больше 1-го wksp */
    if ( pwsp != pwsp0 )
    {
        pwsp->next_wksp = pwsp0->next_wksp;
        port->wksp = pwsp;
        zFree(pwsp0);
        return(0);
    }
    /* 2 случай. там 1 wksp */
    pwsp->wfile = 0;
    return(1);
}


/*
 * removeport() -
 *  уничтожить окно, созданное последним
 */
removeport()
{
    int j, pnum;
    register int i;
    register struct viewport *theport, *pport;
    struct workspace *pws;
    unzoom(1);  /* Там же делается putline(1) */
    if (nportlist == 1)
    {
        error (DIAG("Can't remove remaining port.","Нельзя уничтожить последнее окно"));
        return;
    }
    pport = portlist[pnum = (theport = portlist[--nportlist])->prevport];
    if (pport->bmarg != theport->bmarg)
    {
        /* Vertical */
        pport->firstcol[j = pport->btext+1] = 0;
        pport->lastcol[j++] = pport->rtext+1;
        for (i=0;i<=theport->btext;i++)
        {
            pport->firstcol[j+i] = theport->firstcol[i];
            pport->lastcol[j+i] = theport->lastcol[i];
        }
        pport->bmarg = theport->bmarg;
        pport->bedit = pport->btext = theport->ttext+theport->btext - pport->ttext;
    }
    else
    {
        /* Горизонтальное */
        for (i=0;i<=pport->btext;i++)
        {
            pport->lastcol[i] = theport->lastcol[i] +
                theport->lmarg - pport->lmarg;
            if (pport->firstcol[i] > pport->rtext)
                pport->firstcol[i] = pport->rtext;
        }
        pport->rmarg = theport->rmarg;
        pport->redit = pport->rtext = pport->rmarg - pport->lmarg - 2*LMARG;
    }
    pport->flags &= ~WF_ROLL;
    defplline = defmiline = (pport->btext)/ 3 + 1;
    drawport(pport,DRAW_ALL);
    chgport(pnum);
    out_lines(0,-1);
    poscursor(0,0);
    DEBUGCHECK;
    zFree(theport->firstcol);
    zFree(theport->lastcol);
    zFree(theport->lmchars);
    zFree(theport->rmchars);
    for(      pws = theport->wksp;
              pws->next_wksp != theport->wksp;
              pws = pws->next_wksp)
          zFree((char *)pws);
    zFree((char *)theport);
    DEBUGCHECK;
}

/*
 * chgport(portnum) -
 * перейти в следующее окно
 */
chgport(portnum)
int portnum;
{
    register struct viewport *oldport, *newport;
    unzoom(1);  /* Там же делается putline(1) */
    oldport = curport;
    if (portnum < 0) for (portnum=0;portnum<nportlist &&
        oldport != portlist[portnum++];);
        oldport->wksp->ccol = cursorcol;
    oldport->wksp->crow = cursorline;
    newport = portlist[portnum%nportlist];
    if (newport == oldport) return;
    switchport(newport);
    defplline = defmiline = (newport->btext) / 3 + 1;
    poscursor(curport->wksp->ccol,curport->wksp->crow);
}

/*
 * redisplay(w,w->wfile,from,to,delta) -
 * Перевыдать изменившиеся окна после сдвига строк в файле.
 * w - Рабочее пространство;
 * fn - номер имя измененного файла;
 * from, to, delta - диапазон изменившихся строк и
 * общее изменение числа строк в нем
 * fromd - строка, начиная с которой сделан сдвиг delta, <0 - сдвиг экрана
 *         запрещен
 * Делается следующее:
 *  1. Перевыдаются все окна, в которые попал измененный участок;
 *  2. Получают новые ссылки на fsd в тех рабочих пространствах,
 *     которые изменялись (из за breakfsd они могли измениться);
 *  3. Пересчитываются текущие номера строк в рабочих областях, если они
 *     отображают хвосты изменившихся файлов.
 */
redisplay(w,fn,from,to,delta,fromd)
struct workspace *w;
int from,to,delta,fn,fromd;
{
    register struct workspace *tw;
    register int i,j;
    int k,l,m,jd;
    int ul,tl;
    struct viewport *oport;
    for (i = 0; i < nportlist; i++)
    {
        for(tw = portlist[i]->wksp->next_wksp;
            tw->next_wksp != portlist[i]->wksp; tw = tw->next_wksp)
        {
            if (tw->wfile == fn && tw != w)
            {
                /* Исправим указатель на fsd. */
                tw->curlno = tw->curflno = 0;
                tw->curfsd = openfsds[fn];
                /* Исправить номер строки */
                j = delta >= 0 ? to : from;
                if (tw->ulhclno > j) tw->ulhclno += delta;
            }
        }
        if ((tw = portlist[i]->wksp)->wfile == fn && tw != w)
        {
            /* Исправляем указатель на fsd. */
            tw->curlno = tw->curflno = 0;
            tw->curfsd = openfsds[fn];
            /* Исправляем номер строки и позиции на экране */
            j = delta >= 0 ? to : from;
            if (tw->ulhclno > j) tw->ulhclno += delta;
            /* В режиме zoom рисовать только само zoom-окно */
            if ( zoomedp && portlist[i] != (&zoomport) ) continue;
            ul = tw->ulhclno;
            /* Если изменилось, перевыдать окно */
            j = (from > ul ? from : ul);
            if ((k =  ul + portlist[i]->btext ) > to) k = to;
            if (j <= k)
            {
                oport = curport;
                l = cursorcol;
                m = cursorline;
                switchport(portlist[i]);
                if(fromd>=0 && fromd<=k && delta)
                {
                    jd = fromd;
                    if(jd < j) jd = j;
                    if(scroll(-delta,-1-(jd-ul)) <= 0) goto putall;
                    if( delta > 0 ) {
                        out_lines(j-ul, k-ul);
                    } else {
                        if(jd > j)
                            out_lines(j-ul, jd-1-ul);
                        out_lines(curport->btext-(-delta),curport->btext);
                    }
                    goto next;
                }
putall:
                out_lines(j - ul, delta == 0 ?
                    k - ul : curport->btext);
next:
                switchport(oport);
                poscursor(l,m);
            }
        }
    }
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
    extern int clrsw;
    if ( video_mode == 0 || nportlist <= 0 ) return;
    putline(1);
    switchport(&wholescreen);
    setscroll(&wholescreen,0);
    cursorcol = cursorline =0;
    putcha(COFIN);
    new_info = 1;
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
        if ( zoomedp && curp != (&zoomport )) continue;
        drawport(curp,DRAW_HORI);
        if ( nom != -1 || curp != curp0 )
        out_lines(0,-1);
    }
    switchport(curp0);
    poscursor(col,lin);
    clrsw = 1;
}

/*
 * Работа с увеличенным окном
 * zoom_w(flag) - перевести текущее окно в ZOOM-режим
 *          если мы уже в этом режиме и flag=1 - вернуть все назад
 * unzoom(flag) - вернуться назад к нормальному режиму
 *              flag = 1 - рисовать, 0 - не рисовать
 * Заодно в unzoom сбрасываются атрибуты текущей строки (может исчезнуть wksp )
 */
zoom_w(back)
int back;
{
        register struct viewport *cp, *zp;
        int cl,cc;
        if ( zoomedp )
        {
                if ( back ) unzoom(1);
                return(0);
        }
        putline(1);
        cp = zoomedp = curport;
        zoomflag = 1;
        portlist[nportlist++] = zp = &zoomport;
        cl = cursorline;
        cc = cursorcol;
        /* Теперь поменяем wksp у ZOOM-окна и у текущего местами */
        curwksp = cp->wksp;
        cp->wksp = zp->wksp;
        zp->wksp = curwksp;
        /* А теперь переключимся на ZOOM - окно */
        switchport(zp);
        putcha(COERASE);
        new_info = 1;
        out_lines(0,-1);
        poscursor(cc,cl);
        return(1);
}

unzoom(draw)
int draw;
{
        register int cc,cl;
        putline(1);
        if ( !zoomedp ) return(0);
        nportlist--;
        curwksp->ccol = cursorcol;
        curwksp->crow = cursorline;
        curport->wksp = zoomedp->wksp;
        zoomedp->wksp = curwksp;
        switchport(zoomedp);
        zoomedp = (struct viewport *)0;
        zoomflag = 0;
        to_home(curwksp,curport);
        poscursor(curwksp->ccol,curwksp->crow);
        if ( !draw ) return(1);
        rescreen(0);
        new_info = 1;
        return(2);
}
