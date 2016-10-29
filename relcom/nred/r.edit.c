/*
 *      Редактор RED. ИАЭ им. И.В. Курчатова, ОС ДЕМОС
 * Функции по манипулированию содержимым файлов для RED
 *      $Header: /home/sergev/Project/vak-opensource/trunk/relcom/nred/RCS/r.edit.c,v 3.1 1986/04/20 23:40:53 alex Exp $
 *      $Log: r.edit.c,v $
 *      Revision 3.1  1986/04/20 23:40:53  alex
 *      *** empty log message ***
 *
 * Revision 1.5  86/04/13  21:57:54  alex
 *
 */

#include "r.defs.h"

int charskh,charskl,charscol;   /* Положение символа ( для chars) */
#define NBYMAX 150      /* Макс. размер байтов для fsdbytes, +1 */
struct fsd *temp2fsd(),*delete(),*pick(),*blanklines(),*writemp(),*copyfsd();

/*
 * struct fsd *file2fsd(fname)
 * Создать цепочку описателей для файла fname
 */
struct fsd *file2fsd(fname)
int fname;
{
    charsin(fname,0,0);
    return temp2fsd(fname);
}

/* struct fsd *temp2fsd(chan)
 * - Разложить файл на fsd-цепь.
 * работает с текущего места в файле chan
 */
struct fsd *temp2fsd(chan)
int chan;
{
    register struct fsd *thisfsd, *lastfsd;
    struct fsd *firstfsd;
    register int nby;
    char *bpt;
    int c;
    char fby[NBYMAX+1];
    int i,lct,nl,sh,sl,kh,kl;
    firstfsd = thisfsd = lastfsd = 0;
    /* основной цикл. c - очередной символ, но -1 означает
     * конец файла, а -2 - вход в цикл.
     */
    c = -2;
    sh = sl = 0;
#ifdef lint
    nby = 0;
    nl = 0;
#endif
    FOREVER
        {
        if ( (c < 0) || (nby >= NBYMAX) || (nl == FSDMAXL))
        {
            if (c != -2)
            {
                lastfsd = thisfsd;
                thisfsd = (struct fsd *)salloc(nby + SFSD);
                if (firstfsd == 0) firstfsd = thisfsd;
                else lastfsd->fwdptr = thisfsd;
                thisfsd->backptr = lastfsd;
                thisfsd->fwdptr = (struct fsd *)0;
                thisfsd->fsdnlines = nl;
                nlines[chan] += nl;
                thisfsd->fsdfile = chan;
                thisfsd->seekhigh = sh;
                thisfsd->seeklow = sl;
                bpt = &(thisfsd->fsdbytes);
                for (i=0; i<nby; ++i) *(bpt++) = fby[i];
            }
            if (c == -1)
            { /* Поместим блок конца и выйдем */
                thisfsd->fwdptr = lastfsd = (struct fsd *)salloc(SFSD);
                lastfsd->backptr = thisfsd;
                return (firstfsd);
            }
            sh = charskh;
            sl = charskl;
            nl = nby = lct = 0;
        }
        kh = charskh;
        kl = charskl;
        c = chars(0);
        lct = (charskl - kl) + 512 * (charskh - kh);
        if (c != -1 || lct) {
            if (lct > 127)
            {
                fby[nby++] = (lct / 128)|0200;
                lct = lct % 128;
            }
            fby[nby++] = lct;
            ++nl;
        }
    }
}

/*
 * Настройка программы чтения из файла
 */
#define ALIGBUF                 /* Читать по границе листа!! */
#define CHARBUFSZ 512           /* >= 512, если ALIGBUF */


int chkl,chkh;  /* position of next read from charsfi */
int charsfi,charsi,charsn;

/*
 * charsin(file,h,l) - настроить программу чтения символов "char" на
 * файл "fi" с позиции (h,l) = (блок, сдвиг).
 */
charsin(fi,h,l)
int fi,h,l;
{
    if (fi <= 0) {
        charsfi = fi;
        return;
    }
    if((charsfi != fi)||(chkh != h)||(l > chkl)||(l < chkl - charsn))
    {
        chkh = h;
        seek(fi,h,3);
#ifndef ALIGBUF
        seek(fi,l,1);
        chkl = l;
        charsi = charsn = 0;
#else ALIGBUF
        chkl = 0;
        charsi = l;
        charsn = 0;
#endif ALIGBUF
    }
    else charsi = charsn + l - chkl;
    ncline = 0;
    charsfi = fi;
    charskh = chkh;
    charskl = chkl + charsi - charsn;
    charskl=normhl(charskl, &charskh);
}

/*
 * int chars(flg)
 * Основная функция для чтения очередной строки из файла.
 * Управляется глобальными переменными chkх, chkl, и charsfi.
 * (сдвиг в файле и дескриптор).
 * Возвращает: последний символ либо -1 при конце файла.
 * Если flg = 1 - заполняется строка "cline".
 * Длина строки возвращается в ncline, с учетом символа конца.
 * Строка оканчивается символом LINE FEED или -1, если конец файла.
 * ============================================================
 * При вводе строка разворачивается из файловой в экранную форму.
 * При этом заменяются символы табуляции, неграфические символы,
 * а в режимах "lcase" или "lat" часть символов преобразуются в пару
 * символов.
 * Обратное преобразование (из экранной в файловую форму)
 * делается функцией "dechar".
 *
 * Если flg = 0 - строка не формируется, этот режим служит для
 * составления цепи дескрипторов.
 */
int chars(flg)
{
    register char *c,*se;
    register int ko;
    char *si, *so;
    static char charsbuf[CHARBUFSZ];
    if (charsfi <= 0)
    {
        if (lcline == 0) excline(1);
        ncline = 1;
        cline[0] = NEWLINE;
        return (NEWLINE);
    }
    so=cline;
    ko=(charsi>=charsn? 1 : 2 ) ;
    do
        {
        if (ko==1)
#ifndef ALIGBUF
        {
            charsn=read(charsfi, charsbuf, CHARBUFSZ);
            charsi=0;
#else ALIGBUF
        {
            charsi -= charsn;
            charsn=read(charsfi, charsbuf, CHARBUFSZ);
#endif ALIGBUF
            if(charsn<0) {
                error(DIAG("Read Error","Oшибка чтения"));
                charsn=0;
            }
            chkl += charsn;
        }
        if (charsn<=charsi) {
            ko=1;
            break;
        }/* read buf empty */
        si=charsbuf+charsi;
        se=charsbuf+charsn;
        if ( !flg)
        {
            c=si;
            while ( *c++ !=NEWLINE && c != se);
            charsi=c-charsbuf;
            ko=(*(c-1)==NEWLINE ? 0 : 1 );
            continue;
        }
        while (!lcline||(ko=exinss(&si, se, &so, &ncline, lcline))==2)
        {
            ncline=so-cline;
            excline(0);
            so=cline+ncline;
        }
        charsi=si-charsbuf;
    }
    while (ko);
    /* ko=0 - NEWLINE, 1 - end of file */
    chkl=normhl(chkl,&chkh);
    charskh  = chkh;
    charskl = chkl + charsi - charsn;
    charskl=normhl(charskl, &charskh);
    if (ko==1)
        charsfi = 0;
    /* Убрать хвостовые пробелы */
    if (flg)
    {
        *so=' ';
        c= so;
        while (*c==' ' && c--!=cline);
        *++c=NEWLINE;
        ncline=(c-cline)+1;
    }
    return( ko? -1 : NEWLINE);
}

static char *seit="\\\\\140'{(})|!~^";
/*
 * exinss(&si,&se,&so,&no,&mo) -
 * Преобразование строки из внешней во внутреннюю форму.
 * *si - Текущий входной символ
 * *(se-1) - Последний ранее считанный входной символ
 * *so - текущее место в строке вывода
 *  no - текущая колонка на выводе, +1 (1, если пусто)
 *  mo - макс. номер колонки в олученной строке
 *
 * Код ответа:
 *       0 - end of line
 *       1 - end of input string
 *       2 - the output line overflow
 */
exinss(si,se,so,no,mo)
char **si,*se,**so;
int *no,mo;
{
    register char *st, *sf;
    register unsigned sy;
    int s,s1,n,i;
    char *sft;
    int ir=0;
    sf= *si;
    st= *so;
    /*==== se +=1; ====*/
    n= *no;
    /* main loop */
    while((sy= *sf)!='\n' && sf!=se)
    {
        if((n+2)>mo) {
            ir=2;
            break;
        }
        if( sy=='\11' )
        {
            i=(n&(~07))+8;
            if(i>mo) {
                ir=2;
                break;
            }
            for(; n<i; n++) *st++ =' ';
            goto next;
        }
        /* DEL */
        if(sy=='\177') {
            *st++ =esc0;
            *st++ ='#';
            n += 2;
            goto next;
        }
        /* CNTRL X */
        s1= (sy)&0340;
        if(s1==0)  {
            *st++ =esc0;
            *st++ =sy|0100;
            n += 2;
            goto next;
        }
        /* letters and sim. */
        if(s1!=040)
        {
            if(s1==0200||s1==0240||(sy==S_NO1)||(lcasef && sy==S_NO2))
            /* \377 */              {
                if (n+4>mo) {
                    ir=2;
                    break;
                }
                *st++ =esc0;
                *st++ =(sy>>6)&3 + '0';
                *st++ =(sy>>3)&7 + '0';
                *st++ =((sy) &7) + '0';
                n += 4;
                goto next;
            }
            /* russion letters */
            if (latf && ((sy&0300)==0300) )
            {
                *st++ = esc2;
                *st++ = (sy==0337?sy&0177:(sy&0177)^040);
                n += 2;
                goto next;
            }
            /* \A  A \( \) \! ... */
            if (lcasef)
            {
                *st=esc1;
                s= ((int)sy) & 0377;
                if (s>='A' && s<= 'Z')
                {
                    *++st = s;
                    st++;
                    n += 2;
                    goto next;
                }
                if (RLPRO(s))
                {
                    *++st = STK7(s);
                    st++;
                    n += 2;
                    goto next;
                }
                if (s>='a' && s<='z')
                {
                    *st++ = (s&0177) ^ 040;
                    n++;
                    goto next;
                }
                if (RLSTRO(s))
                {
                    *st++ = STK7(s);
                    n++;
                    goto next;
                }
                /* table twice chars */
                sft=sf;
                sf=seit;
                while ( *sf && *sf++!=sy) sf++;
                if( *sf) {
                    *++st = *sf;
                    st++;
                    n += 2;
                    sf=sft;
                    goto next;
                }
                sf=sft;
            }
        }
        *st++ = sy;
        n++;
next:
        sf++;
    }
    ir= ( ir?  ir : ((sy=='\n')?(sf++,0):1) );
    *si=sf;
    *so=st;
    *no=n;
    *st=0;
    return(ir);
}

int flgtabs=1;
/*
 * dechars(line,n) -
 * Преобразование строки line из внутренней (экранной) формы
 * во внешнюю. Преобразование делается прямо в строке line.
 * n - длина строки. В строке должно быть не менее n+1 символов,
 * так как в конец полученной строки пишется NEWLINE.
 * Замена пробелов табуляциями управляется через flgtabs
 *      0 - нет, 1 - начальные табуляции, 2 - все пробелы.
 * Возвращает число символов в полученной строке.
 * Хвостовые пробелы исключаются.
 */
int dechars(line,n)
char *line;
int n;
{
    register char *fm,*to;  /* pointers for move */
    register unsigned cc;   /* current character */
    int lnb;       /* 1 + last non-blank col */
    int cn;                 /* col number */
    int i,j;
    char *to1;
    line[n] = NEWLINE;
    fm = to = line;
    cn = -1;
    lnb = 0;
    while ((cc = *fm++) != NEWLINE)
    {
        cn++;
        if ( cc != ' ')
        {
            if(flgtabs==2 || flgtabs==1 && lnb==0)
                /* Пробелы заменяем табуляциями */
                while (8+(lnb&(~07)) <= cn)
                {
                    *to++ = (lnb&7)==7 ? ' ' : '\t';
                    lnb &= (~07);
                    lnb += 8;
                }
            while ( ++lnb <= cn ) *to++ = ' ';
            /* расшифровка символов */
            if (cc==esc0)
            {
                if( *fm >='@' || (unsigned)(*fm) > 0200 )
                {
                    *to++ = *fm++ & 037;
                    continue;
                }
                if ( *fm == '#') {
                    *to++ = '\177';
                    *fm++;
                    continue;
                }
                /* <esc>XXX */
                i=0;
                j=0;
                while (j++ <3 && (*fm >= '0' && *fm <= '7'))
                    i= (i<<3) + (*fm++ - '0');
                *to++ = i & 0377;
                continue;
            }
            else if (lcasef)
            {
                if (cc>='A' && cc<= 'Z')
                    cc=cc^040;
                else if (cc>=0140 && cc <= 0176 )
                    cc=K7TS(cc)^040;
                else if (cc==esc1 && (cc= *fm) !=NEWLINE )
                {
                    *fm++;
                    if (cc>=0140 && cc<=0176)
                        cc=K7TS(cc);
                    else if (cc<'A' || cc>'Z')
                    {
                        to1=to;
                        to=seit-1;
                        while (*++to && *++to != cc);
                        if (*to) cc= *--to;
                        to=to1;
                    }
                }
                *to++ =cc;
                continue;
            }
            else if (latf && cc==esc2 && *fm != NEWLINE )
            {
                cc= *fm++ ^ 0240;
                if (cc==0377) cc= 0337;
            }
            *to++ =cc;
        } /* while - continue - not skip */
    }
    *to++ = NEWLINE;
    return (to - line);
}

/*
 * excline(n) -
 * расширить массив cline.
 * cline содержит текущую строку, причем
 * lcline - содержит максимальную длину массива cline;
 * ncline - длина текущей строки.
 */
excline(n)
int n;
{
    register int j;
    register char *tmp;
    j = lcline + icline;
    if (j < n) j = n;
    tmp = salloc(j+1);
    icline += icline >> 1;
    while (--lcline >= 0) tmp[lcline] = cline[lcline];
    lcline = j;
    if (cline != 0) free(cline);
    cline = tmp;
}

/*
 * putbks(col,n) -
 * вставить n пробелов в колонке col текущей строки
 */
putbks(col,n)
int col,n;
{
    register int i;
    if (n <= 0) return;
    if (col > ncline-1)
    {
        n += col - (ncline-1);
        col = ncline-1;
    }
    if (lcline <= (ncline += n)) excline(ncline);
    for (i = ncline - (n + 1); i >= col; i--) cline[i + n] = cline[i];
    for (i = col + n - 1; i >= col; i--) cline[i] = ' ';
}

/*
 * wseek(wksp,lno) -
 * Установить файл и смещение для ввода строки nlo
 * из рабочего пространства wksp.
 * После этого chars(0) считает требуемую строку.
 * Код ответа = 1, если такой строки нет, 0 если ОК
 */
int wseek(wksp,lno)
struct workspace *wksp;
int lno;
{
    register char *cp;
    int h,i;
    register int j,l;
    /* 1. Получим fsd, в котором "сидит" данная строка */
    if (wposit(wksp,lno)) return (1);
    /* Теперь вычислим смещение */
    h = wksp->curfsd->seekhigh;
    l = wksp->curfsd->seeklow;
    i = lno - wksp->curflno;
    cp = &(wksp->curfsd->fsdbytes);
    while (i-- != 0)
    {
        if ((j = *(cp++)) &0200)
        {
            l += 128*(j&0177);
            j = *(cp++);
        }
        l += j;
    }
    l=normhl(l, &h);
    charsin(wksp->curfsd->fsdfile,h,l);
    return (0);
}

/*
 * wposit(wksp,lno) -
 * Установить указатель curfsd структуры wksp на fsd,
 * к которому относится строка lno.
 */
wposit(wk,lno)
struct workspace *wk;
int lno;
{
    register struct workspace *wksp;
    wksp = wk;
    if (lno < 0) fatal("Wposit neg arg");
    while (lno >= (wksp->curflno + wksp->curfsd->fsdnlines))
    {
        if (wksp->curfsd->fsdfile == 0)
        {
            wksp->curlno = wksp->curflno;
            return (1);
        }
        wksp->curflno += wksp->curfsd->fsdnlines;
        wksp->curfsd = wksp->curfsd->fwdptr;
    }
    while (lno < wksp->curflno)
    {
        if ((wksp->curfsd = wksp->curfsd->backptr) == 0)
            fatal("Wposit 0 backptr");
        wksp->curflno -= wksp->curfsd->fsdnlines;
    }
    if (wksp->curflno < 0) fatal("WPOSIT LINE CT LOST");
    wksp->curlno = lno;
    return 0;
}

/*
 * switchfile() -
 *  Переключиться на альтернативный файл.
 *
 */
switchfile()
{
    if (curport->altwksp->wfile == 0)
    {
        if (editfile(deffile,0,0,0,1) <= 0)
            error(DIAG("Default file gone: notify sys admin.","Нет файла инструкций."));
        return;
    }
    switchwksp();
    putup(0,curport->btext);
    poscursor(curwksp->ccol,curwksp->crow);
}

/*
 * switchwksp() -
 *  Переключить wksp на альтернативное.
 *
 */
switchwksp()
{
    register struct workspace *tempwksp;
    curwksp->ccol = cursorcol;
    curwksp->crow = cursorline;
    tempwksp = curwksp;
    curwksp = curport->wksp = curport->altwksp;
    curfile = curwksp->wfile;
    curport->altwksp = tempwksp;
}

/*
 * openlines(from,number) -
 * Вставить со строки from number строк.
 */
openlines(from,number)
int from, number;
{
    if (from >= nlines[curfile]) return;
    nlines[curfile] += number;
    insert(curwksp,blanklines(number),from);
    redisplay((struct workspace *)NULL,curfile,from,from+number-1,number);
    poscursor(cursorcol,from - curwksp->ulhclno);
}

/*
 * openspaces(line,col,number, nl) -
 * Вставить пробелы, nubmer колонок, nl строк.
 */
openspaces(line,col,number, nl)
int line, col, number, nl;
{
    register int i,j;
    for (i=line;i<line+nl;i++)
    {
        getlin(i);
        putbks(col,number);
        fcline = 1;
        putline(0);
        if((j=i - curwksp->ulhclno)<= curport ->btext)
            putup(j,j);
    }
    poscursor(col - curwksp->ulhccno, line - curwksp->ulhclno);
}

/*
 * splitline(line,col) -
 * Расщепить строку "line,col"
 */
splitline(line,col)
int line,col;
{
    register int nsave;
    register char csave;
    if (line >= nlines[curfile]) return;
    nlines[curfile]++;
    getlin(line);
    if (col >= ncline - 1) openlines(line+1,1);
    else
    {
        csave = cline[col];
        cline[col] = NEWLINE;
        nsave = ncline;
        ncline = col+1;
        fcline = 1;
        putline(0);
        cline[col] = csave;
        insert(curwksp,writemp(cline+col,nsave-col),line+1);
        redisplay((struct workspace *)NULL,curfile,line,line+1,1);
    }
    poscursor(col - curwksp->ulhccno, line - curwksp->ulhclno);
    return;
}

/*
 * closelines(frum,number) -
 * Убрать строки из файла.
 * frum < 0 - не делать redisplay, from = -frum
 * (используется при "exec").
 */
closelines(frum,number)
int frum, number;
{
    register int n,from;
    register struct fsd *f;
    if ((from = frum) < 0) from = -from-1;
    if (from < nlines[curfile]) if ((nlines[curfile] -= number) <= from)
        nlines[curfile] = from + 1;
    f = delete(curwksp,from,from+number-1);
    if (frum >= 0) redisplay((struct workspace *)NULL,curfile,from,from+number-1,-number);
    insert(pickwksp,f,n = nlines[2]);
    redisplay((struct workspace *)NULL,2,n,n,number);
    deletebuf->linenum = n;
    deletebuf->nrows = number;
    deletebuf->ncolumns = 0;
    nlines[2] += number;
    poscursor(cursorcol,from - curwksp->ulhclno);
    return;
}

/*
 * closespaces(line,col,number,nl) -
 * Убрать область из файла
 */
closespaces(line,col,number,nl)
int line, col, number, nl;
{
        pcspaces(line,col,number,nl,1);
}


/*
 * combineline(line,col) -
 * Слепить строку со следующей
 */
combineline(line,col)
int line,col;
{
    register char *temp;
    register int nsave,i;
    if (nlines[curfile] <= line-2) nlines[curfile]--;
    getlin(line+1);
    temp = salloc(ncline);
    for (i=0;i<ncline;i++) temp[i] = cline[i];
    nsave = ncline;
    getlin(line);
    if (col+nsave > lcline) excline(col+nsave);
    for (i=ncline-1;i<col;i++) cline[i] = ' ';
    for (i=0;i<nsave;i++) cline[col+i] = temp[i];
    ncline = col + nsave;
    fcline = 1;
    putline(0);
    free((char *)temp);
    delete(curwksp,line+1,line+1);
    redisplay((struct workspace *)NULL,curfile,line,line+1,-1);
    poscursor(col - curwksp->ulhccno, line - curwksp->ulhclno);
}

/*
 * picklines(from,number) -
 * взять строки из файла
 */
picklines(from,number)
int from, number;
{
    register int n;
    register struct fsd *f;
    f = pick(curwksp,from,from+number-1);
    redisplay((struct workspace *)NULL,curfile,from,from+number-1,0); /* because of breakfsd */
    insert(pickwksp,f,n = nlines[2]);
    redisplay((struct workspace *)NULL,2,n,n,number);
    pickbuf->linenum = n;
    pickbuf->nrows = number;
    pickbuf->ncolumns = 0;
    nlines[2] += number;
    poscursor(cursorcol,from - curwksp->ulhclno);
    return;
}

/*
 * pickspaces(line,col,number,nl) -
 * взять кусок из файла
 */
pickspaces(line,col,number,nl)
int line, col, number, nl;
{
        pcspaces(line,col,number,nl,0);
}

/*
 * pcspaces(line,col,number,nl,flg) -
 * Взять (flg=0) / убрать (flg = 1)
 * Общая функция
 */
pcspaces(line,col,number,nl,flg)
int line, col, number, nl, flg;
{
    register struct fsd *f1,*f2;
    struct fsd *f0;
    char *linebuf, *bp;
    register int i;
    int j, n,line0,line1;
    if (charsfi == tempfile) charsfi = 0;
    linebuf = salloc(number+1);
    f0=f2=0;
    line1 = (line0=line) + nl;
    while( nl = (line1 - line0))
    {
        if(nl > FSDMAXL) nl=FSDMAXL;
        f1 = (struct fsd *)salloc(SFSD+(number>127?nl*2:nl));
        if(f2) {
            f2->fwdptr = f1;
            f1->backptr = f2;
        }
        else f0= f1;
        bp = &(f1->fsdbytes);
        f1->fsdnlines = nl;
        f1->fsdfile = tempfile;
        f1->seekhigh = tempfh;
        f1->seeklow = tempfl;
        for (j=line0;j<line0+nl;j++)
        {
            getlin(j);
            if (col+number >= ncline)
            {
                if (col+number >= lcline) excline(col+number+1);
                for (i=ncline-1;i<col+number;i++) cline[i] = ' ';
                cline[col+number] = NEWLINE;
                ncline = col + number + 1;
            }
            for (i=0;i<number;i++) linebuf[i] = cline[col+i];
            linebuf[number] = NEWLINE;
            seek(tempfile,tempfh,3);
            seek(tempfile,tempfl,1);
            if (charsfi == tempfile) charsfi = 0;
            write(tempfile,linebuf,n = dechars(linebuf,number));
            if ( n > 127 ) *bp++ = (n/128)|0200;
            *bp++ = n%128;
            tempfl += n;
            tempfl=normhl(tempfl, &tempfh);
        }
        f2 = f1;
        line0 = line0 + nl;
    }
    (f2->fwdptr = (struct fsd *)salloc(SFSD))->backptr = f2;
    nl = line1 - line;
    if (flg) for (j=line;j<line+nl;j++)
    {
        getlin(j);
        if (col+number >= ncline)
        {
            if (col+number >= lcline) excline(col+number+1);
            for (i=ncline-1;i<col+number;i++) cline[i] = ' ';
            cline[col+number] = NEWLINE;
            ncline = col + number + 1;
        }
        for (i=col+number;i<ncline;i++)
            cline[i-number] = cline[i];
        ncline -= number;
        fcline = 1;
        putline(0);
        if((i=j - curwksp->ulhclno) <= curport->btext)
            putup(i,i);
    }
    insert(pickwksp,f0,n = nlines[2]);
    redisplay((struct workspace *)NULL,2,n,n,nl);
    if (flg)
    {
        deletebuf->linenum = n;
        deletebuf->nrows = nl;
        deletebuf->ncolumns = number;
    }
    else
    {
        pickbuf->linenum = n;
        pickbuf->nrows = nl;
        pickbuf->ncolumns = number;
    }
    nlines[2] += nl;
    free(linebuf);
    poscursor(col - curwksp->ulhccno, line - curwksp->ulhclno);
}

/*
 * put(buf,line,col) -
 * Поместить на указанное место взятое "buf"
 * Если  buf->ncolumns == 0 , вставляются строки,
 * иначе прямоугольная область.
 */
put(buf,line,col)
struct savebuf *buf;
int line, col;
{
        if (buf->ncolumns == 0) plines(buf,line);
        else pspaces(buf,line,col);
}

/*
 * plines(buf,line) -
 * Поместить на указанное место строки из buf.
 * Должно быть buf->ncolumns == 0 .
 */
plines(buf,line)
struct savebuf *buf;
int line;
{
    int lbuf, cc, cl;
    struct fsd *w0, *w1;
    register struct fsd *f, *g;
    register int j;
    cc = cursorcol;
    cl = cursorline;
    breakfsd(pickwksp, buf->linenum + buf->nrows,1);
    w1 = pickwksp->curfsd;
    breakfsd(pickwksp, buf->linenum,1);
    w0 = pickwksp->curfsd;
    f = g = copyfsd(w0,w1);
    lbuf = 0;
    while (g->fsdfile)
    {
        lbuf += g->fsdnlines;
        g = g->fwdptr;
    }
    insert(curwksp,f,line);
    redisplay((struct workspace *)NULL,curfile,line,line,lbuf);
    poscursor(cc,cl);
    if ((nlines[curfile] += lbuf) <= (j=line+lbuf)) nlines[curfile] = j+1;
}

/*
 * pspaces(buf,line,col)    -
 * Поместить на указанное место взятое "buf"
 * Должно быть buf->ncolumns != 0 .
 */
pspaces(buf,line,col)
struct savebuf *buf;
int line, col;
{
    struct workspace *oldwksp;
    char *linebuf;
    int nc, i, j;
    linebuf = salloc(nc = buf->ncolumns);
    oldwksp = curwksp;
    for (i=0;i<buf->nrows;i++)
    {
        curwksp = pickwksp;
        getlin(buf->linenum + i);
        if (ncline-1 < nc) for (j=ncline-1;j<nc;j++) cline[j] = ' ';
        for (j=0;j<nc;j++) linebuf[j] = cline[j];
        curwksp = oldwksp;
        getlin(line+i);
        putbks(col,nc);
        for (j=0;j<nc;j++) cline[col+j] = linebuf[j];
        fcline = 1;
        putline(0);
        if ((j = line+i-curwksp->ulhclno) <= curport->btext)
            putup(j,j);
    }
    free(linebuf);
    poscursor(col - curwksp->ulhccno, line - curwksp->ulhclno);
    return;
}

/* ========= Подпрограммы нижнего уровня ========
 *         = Для работы с описателями fsd =
 */
/*
 * insert(wksp,f,at) -
 * Вставить описатель f в файл, описываемый wksp,
 * перед строкой at.
 * Вызывающая программа должна вызвать redisplay с
 * соответствующими аргументами.
 */
insert(wksp,f,at)
struct workspace *wksp;
struct fsd *f;
int at;
{
    register struct fsd *w0, *wf, *ff;
    DEBUGCHECK;
    /* determine length of insert */
    ff = f;
    /* ln = 0;      */
    while (ff->fwdptr->fsdfile)
    {
        /*  ln += ff->fsdnlines;  */
        ff = ff->fwdptr;
    }
    /* ln += ff->fsdnlines; */
    breakfsd(wksp,at,1);
    wf = wksp->curfsd;
    w0 = wf->backptr;
    free((char *)ff->fwdptr);
    ff->fwdptr = wf;
    wf->backptr = ff;
    f->backptr = w0;
    wksp->curfsd = f;
    wksp->curlno = wksp->curflno = at;
    if (openwrite[wksp->wfile]) openwrite[wksp->wfile] = EDITED;
    catfsd(wksp);
    DEBUGCHECK;
}


/*
 * struct fsd *delete(wksp,from,to) -
 * Исключить указанные строки из wksp.
 * Возвращает указатель на fsd - цепь исключенных строк,
 * с добавленнык концевым блоком.
 * Требует redisplay.
 */
struct fsd *delete(wksp,from,to)
struct workspace *wksp;
int from,to;
{
    struct fsd *w0;
    register struct fsd *wf,*f0,*ff;
    breakfsd(wksp,to+1,1);
    DEBUGCHECK;
    wf = wksp->curfsd;
    breakfsd(wksp,from,1);
    f0 = wksp->curfsd;
    ff = wf->backptr;
    w0 = f0->backptr;
    wksp->curfsd = wf;
    wf->backptr = w0;
    f0->backptr = 0;
/* do both in one line */
    (ff->fwdptr = (struct fsd *)salloc(SFSD))->backptr = ff;
    catfsd(wksp);
    openwrite[wksp->wfile]=  EDITED;
    DEBUGCHECK;
    return (f0);
}

/*
 * struct fsd *pick(wksp,from,to) -
 * Возвращает указатель на fsd - цепь указанных строк,
 * с добавленным концевым блоком.
 * Требует redisplay.
 */
struct fsd *pick(wksp,from,to)
struct workspace *wksp;
int from,to;
{
    struct fsd *wf;
    breakfsd(wksp,to+1,1);
    wf = wksp->curfsd;
    breakfsd(wksp,from,1);
    return(copyfsd(wksp->curfsd,wf));
}


/*
 * struct fsd *breakfsd(w,n,reall) -
 * разломать fsd по строке n в пространстве
 * w. curlno = curflno при возврате, и curfsd указывает на первую
 * строку после разрыва (которая может быть строкой из конечного
 * блока). Исходный fsd остается, возможно, с неиспользуемым
 * остатком списка длин.
 * Если функция применяется к концу файла, текущая
 * позиция остается в конце файла, на конечном блоке.
 * Если указанная строка выходит за пределы файла, добавляются
 * описатели пустых строк (с каналом -1).
 * Если "reall=1" то блок перед разрывом переразмещается в памяти с
 * целью экономии места.
 * ВНИМАНИЕ: breakfsd может нарушить корректность указателей в  workspace.
 * Поэтому после всех операций нужно вызывать "redisplay".
 */
int breakfsd(w,n,reall)
struct workspace *w;
int n,reall;
{
    int nby,i,j,jj,k,offs,lfb0;
    register struct fsd *f,*ff;
    struct fsd *fn;
    register char *c;
    char *cc;
    DEBUGCHECK;
    if (wposit(w,n))
    {
        f = w->curfsd;
        ff = f->backptr;
        free((char *)f);
        fn = blanklines(n - w->curlno);
        w->curfsd = fn;
        fn->backptr = ff;
        if (ff) ff->fwdptr = fn;
        else openfsds[w->wfile] = fn;
        wposit(w,n);
        return (1);
    }
    f = w->curfsd;
    cc = c = &f->fsdbytes;
    offs = 0;
    ff = f;
    nby = n - w->curflno;
    if (nby != 0)
    {
        /* get down to the nth line */
        for (i=0; i<nby; i++)
        {
            if ((j = *c++)&0200)
            {
                offs += 128*(j&0177);
                j = *c++;
            }
            offs += j;
        }
        /* now make a new fsd from the remainder of f */
        i = j = jj = f -> fsdnlines - nby; /* number of lines in new fsd */
        lfb0 = c - cc;
        cc = c;
        while (--i >= 0) if (*cc++ < 0) {
            j++;
            cc++;
        }
        ff = (struct fsd *)salloc(SFSD + j);
        ff->fsdnlines = jj;
        ff->fsdfile = f->fsdfile;
        offs += f->seeklow;
        ff->seeklow = offs % 512;
        ff->seekhigh = f->seekhigh + offs/512;
        cc = &ff->fsdbytes;
        for (k=0; k<jj; k++) if ((*cc++ = *c++) < 0) *cc++ = *c++;
        if ((ff->fwdptr = f->fwdptr)) ff->fwdptr->backptr = ff;
        ff->backptr = f;
        f->fwdptr = ff;
        f->fsdnlines = nby;
        if (reall && (jj>4) && (f->backptr))
        {
            ff=(struct fsd *)salloc(SFSD+lfb0);
            *ff = *f;
            c= &(ff->fsdbytes);
            cc= &(f->fsdbytes);
            while (lfb0--) {
                *c++ = *cc++;
            }
            ff->backptr->fwdptr = ff->fwdptr->backptr = ff;
            free((char *)f);
            f=ff;
            ff=f->fwdptr;
        }
    }
    w->curfsd = ff;
    w->curflno = n;
    DEBUGCHECK;
    return (0);
}

/*
 * struct fsd *writemp(buf,n)
 * Дописать строку buf длиной n во врем. файл.
 * Возвращает описатель этой строки.
 */
struct fsd *writemp(buf,n)
char *buf;
int n;
{
    register struct fsd *f1,*f2;
    register char *p;
    if (charsfi == tempfile) charsfi = 0;
    if (tempfh)
    {
        seek(tempfile,tempfh,3);
        seek(tempfile,tempfl,1);
    }
    else seek(tempfile,tempfl,0);
    write(tempfile,buf,n = dechars(buf,n-1));
    /* now make fsd */
    f1 = (struct fsd *)salloc(2 + SFSD);
    f2 = (struct fsd *)salloc(SFSD);
    f2->backptr = f1;
    f1->fwdptr = f2;
    f1->fsdnlines = 1;
    f1->fsdfile = tempfile;
    f1->seekhigh = tempfh;
    f1->seeklow = tempfl;
    if (n<=127) f1->fsdbytes = n;
    else
    {
        p = &f1->fsdbytes;
        *p++ = (n / 128)|0200;
        *p = n % 128;
    }
    tempfl += n;
    tempfl=normhl(tempfl, &tempfh);
    return (f1);
}

/*
 * getlin(ln) -
 * Дай строку ln из текущего curwksp.
 * строка попадает в cline, длина - в ncline.
 */
getlin(ln)
int ln;
{
    fcline = 0;
    clineno = ln;
    if (wseek(curwksp, ln))
    {
        if (lcline == 0) excline(1);
        cline[0] = NEWLINE;
        ncline = 1;
    }
    else chars(1);
}

/*
 * putline(nl) -
 * Поместить строку из cline в curwksp, строка nl.
 */
putline(n)
int n;
{
    struct fsd *w0,*cl;
    register struct fsd *wf, *wg;
    register struct workspace *w;
    int i;
    char flg;
    DEBUGCHECK;
    if (fcline == 0)
    {
        clineno = -1;
        return;
    }
    if (nlines[curfile] <= clineno) nlines[curfile] = clineno + 1;
    fcline = 0;
    cline[ncline-1] = NEWLINE;
    cl = writemp(cline+n,ncline-n);
    w = curport->wksp;             /* w s can be replaced by curwksp */
    i = clineno;
    flg = breakfsd(w,i,1);
    wg = w->curfsd;
    w0 = wg->backptr;
    if (flg == 0)
    {
        breakfsd(w,i+1,0);
        wf = w->curfsd;
        free((char *)cl->fwdptr);
        cl->fwdptr = wf;
        wf->backptr = cl;
    }
    free((char *)wg);
    cl->backptr = w0;
    w->curfsd = cl;
    w->curlno = w->curflno = i;
    openwrite[w->wfile] = EDITED;
    clineno = -1;
    catfsd(w);
    redisplay(w,w->wfile,i,i,0);
    DEBUGCHECK;
}

/*
 * struct fsd *copyfsd(f,end) -
 * Возвращает копию цепи fsd, от f до end, не включая end.
 * Если end = NULL - до конца файла.
 */
struct fsd *copyfsd(f,end)
struct fsd *f,*end;
{
    struct fsd *res,*ff,*rend;
    register int i;
    register char *c1,*c2;
    res = 0;
#ifdef lint
    rend = (struct fsd *)0; /* При первом заходе res = FALSE */
#endif
    while (f->fsdfile && f != end)
    {
        c1 = &f->fsdbytes;
        for (i = f->fsdnlines; i; i--) if (*c1++&0200) c1++;
        c2 = (char *)f; /* !!! Подсчет места !!!*/
        i = c1 - c2;
        ff = (struct fsd *)(c2 = salloc(i));
        c2 += i;
        while (i--) *--c2 = *--c1;
        if (res)
        {
            rend->fwdptr = ff;
            ff->backptr = rend;
            rend = ff;
        }
        else res = rend = ff;
        f = f->fwdptr;
    }
    if (res)
    {
        (rend->fwdptr = (struct fsd *)salloc(SFSD))->backptr = rend;
        rend = rend->fwdptr;
    }
    else res = rend = (struct fsd *)salloc(SFSD);
    if (f->fsdfile == 0) rend->seeklow = f->seeklow;
    return res;
}

/*
 * freefsd(f) -
 * Почистить цепочку fsd.
 */
freefsd(f)
struct fsd *f;
{
    register struct fsd *g;
    while (f) {
        g = f;
        f = f->fwdptr;
        free((char *)g);
    }
}

/*
 * blanklines(n) -
 * создай дескриптор n пустых строк
 */
struct fsd *blanklines(n)
int n;
{
    int i;
    register struct fsd *f,*g;
    register char *c;
    f = (struct fsd *)salloc(SFSD);
    while (n) {
        i = n > FSDMAXL ? FSDMAXL : n;
        g = (struct fsd *)salloc(SFSD + i);
        g->fwdptr = f;
        f->backptr = g;
        g->fsdnlines = i;
        g->fsdfile = -1;
        c = &g->fsdbytes;
        n -= i;
        while (i--) *c++ = 1;
        f = g;
    }
    return (f);
}

/*
 * catfsd(w)
 *  программа пытается слить несколько fsd - блоков в один
 *  для экономии памяти.
 *  делается попытка слить в один блок w->curfsd->backptr и
 *  w->curfsd, если они описывают смежную информацию.
 */
catfsd(w)
struct workspace *w;
{
    register struct fsd *f0, *f;
    struct fsd *f2;
    register char *c;
    char *cc;
    int i,j,l0=0 ,l1=0,lb0=0 ,lb1,dl,nl0,nl1,fd0,kod=0;
    /*      l0, l1 : число байтов в участке файла, описываемом f0,f;
     *      lb0,lb1: длина описателя длин в fsd
     *      nl0,nl1 : число строк в fsd */
    f=w->curfsd;
    if ((f0=f->backptr)==0) {
        openfsds[w->wfile]=f;
        return(0);
    }
    f0->fwdptr = f;
    fd0=f0->fsdfile;
    nl0=f0->fsdnlines;
    while( fd0>0 && fd0==f->fsdfile && (nl0+(nl1=f->fsdnlines)< FSDMAXL))
    {
        i=f0->seekhigh;
        j=f->seekhigh;
        if (i>j || (j-i > 20 )) return(kod);
        dl=(j-i)*512 + f->seeklow - f0->seeklow;
        /*  подсчитываем длину блока, eсли она неизвестна */
        if (l0==0)
        {
            i=nl0;
            cc=c= &(f0->fsdbytes);
            while(i--)
            {
                if ((j= *c++)&0200) j= (j&0177)*128 + *c++;
                l0 += j;
            }
            lb0= c - cc;
        }
        if (dl != l0) return(kod);
        /* сливаем два fsd  и пытаемся повторить */
        i=nl1;
        cc=c= &(f0->fsdbytes);
        while(i--)
        {
            if((j= *c++ ) &0200) j= (j&0177)*128 + *c++;
            l1 += j;
        }
        lb1 = c - cc;
        f2=f;
        f=(struct fsd *)salloc(SFSD + lb0 + lb1);
        *f = *f0;
        f->fwdptr = f2->fwdptr;
        w->curfsd=f;
        w->curflno -= nl0;
        nl0=f->fsdnlines = nl0 + nl1;
        c= &(f->fsdbytes);
        i=lb0;
        cc= &(f0->fsdbytes);
        while (i--) *c++ = *cc++;
        i=lb1;
        cc= &(f2->fsdbytes);
        while (i--) *c++ = *cc++;
        lb0 += lb1;
        l0 += l1;
        kod=1;
        free((char *)f2);
        free((char *)f0);
        f->fwdptr ->backptr =f;
        if( f0=f->backptr) f0->fwdptr = f;
        else openfsds[w->wfile]=f;
        f0=f;
        f=f0->fwdptr;
    }
    return(kod);
}

/*
 * int normhl(ll, hh)
 * int ll,*hh;
 * Пересчитать номер блока и сдвиг (ll, hh) так,
 * чтобы ll указывал сдвиг в блоке hh.
 * hh пишется на место, ll возвращается
 */
int normhl(ll, hh)
int ll,*hh;
{
        register int l=ll,*h=hh;
        while(l<0) {
                l += 512;
                (*h)--;
        }
        while(l>=512) {
                l -= 512;
                (*h)++;
        }
        return(l);
}


/*
 * doreplace(line,m,jproc,pipef)
 * Заменить строки с "line" m строк, через фильтр jproc, канал pipef.
 */
doreplace(line,m,jproc,pipef)
int line,m,jproc,*pipef;
{
    register struct fsd *e,*ee;
    register int l;
    int n;
    close(pipef[0]);
    breakfsd(curwksp,line,0);
    if (m == 0) close(pipef[1]);
    else {
        m = fsdwrite(curwksp->curfsd,m,pipef[1]);
        if (m == -1) {
            error(DIAG("Can't write on pipe.","Ошибка записи в pipe."));
            kill(jproc,9);
        }
    }
    while (wait(&n) != jproc);          /* wait for completion */
    if ((n & 0177400) == 0157400) {
        error(DIAG("Can't find program to execute.","Не найдена программа"));
        return;
    }
    if ((n & 0177400) == 0177000 || (n & 0377) != 0) {
        error(DIAG("Abnormal termination of program.","Плохой код конца программы."));
        return;
    }
    charsfi = -1;                   /* forget old position before fork */
    if (m) closelines(-1-line,m);
    charsin(tempfile,tempfh,tempfl);
    ee = e = temp2fsd(tempfile);
    tempfh = charskh;
    tempfl = charskl;
    if (e->fsdnlines) {
        l = 0;
        while (e->fsdfile) {
            l += e->fsdnlines;
            e = e->fwdptr;
        }
        insert(curwksp,ee,line);
        nlines[curfile] += l;
    }
    redisplay((struct workspace *)NULL,curfile,line,line+m,l-m);
    poscursor(cursorcol,cursorline);
}

/*
 * execr(args) -
 * char **args
 * запустить команду с параметрами args
 */
execr(args)
char **args;
{
    execvp(*args,args);
    exit(0337); /* Код ответа согласован с doreplace */
}

#ifndef WORK
/*  #ifndef WORK
 *  printfsd(f) -
 * Отладочная печать цепочек fsd
 */
printfsd(f)
struct fsd *f;
{
    int i;
    register char *c;
    register struct {
        char *s0;
        char *s1;
    }
    *cp;
    printf("\n**********");
    while (f) {
        printf("\nfsdnl=%d chan=%d hi=%d lo=%d at %o",
        f->fsdnlines,f->fsdfile,f->seekhigh,
        f->seeklow,f);
        if (f->fwdptr && f != f->fwdptr->backptr) printf("\n*** next block bad backpointer ***");
        c = &(f->fsdbytes);
        for (i=0; i<f->fsdnlines; i++) {
            if ((i % 20) == 0) putchar('\n');
            printf(" %d",*(c++));
        }
        f = f->fwdptr;
    }
}

/*
 * checkfsd() -
 * Проверка корректности цепочек fsd.
 */
checkfsd()
{
    register struct fsd *f;
    register int nl;
    struct {
        int s0;
        char *s1;
    };
    register char *g;
    char *gg;
    nl = 0;
    f = openfsds[curfile];
    while (f) {
        if (curwksp->curlno >=nl && curwksp->curlno <
            nl + f->fsdnlines && curwksp->curfsd != f &&
            curwksp->curfsd->backptr ) {
            fatal("CKFSD CURFSD LOST");
        }
        if (f->fwdptr && f->fwdptr->backptr != f) fatal("CKFSD BAD CHAIN");
        nl += f->fsdnlines;
        f = f->fwdptr;
    }
    allock();
}
#endif

/*   Руднев А.П. Москва, ИАЭ им. Курчатова, 1984 */
