/*
 *      Редактор RED. ИАЭ им. И.В. Курчатова, ОС ДЕМОС
 *       $Header: /home/sergev/Project/vak-opensource/trunk/relcom/nred/RCS/r.ttyio.c, 3.1 1986/04/20 23:43:21 alex Exp $
 *
 *      $Log: r.ttyio.c, $
 *      Revision 3.1  1986/04/20 23:43:21  alex
 *      *** empty log message ***
 *
 *      Работа с терминалом : установка/снятие режимов, ввод/вывод.
 *      Ввод управляющих клавиш.
 *      Физический уровень
 */

#include "r.defs.h"
#include "r.tele.h"
#include <sgtty.h>

#define NPUTCBUF 256   /* Размер буфера вывода */

#ifdef TIOCSETA
#define stty(des,fil) ioctl(des,TIOCSETA,fil)
#define gtty(des,fil) ioctl(des,TIOCGETA,fil)
#endif
#ifndef ANYP
#define ANYP (ODDP|EVENP)
#endif
struct sgttyb templ;
#ifdef TIOCGETC
struct tchars tchars0;
#endif
#ifdef TIOCGETC
struct tchars tchars0;
#endif
int ospeed; /* Для termcap */

/*
 * ttstartup()
 * Установить режимы терминала
 */
ttstartup()
{
    struct sgttyb templw;
#ifdef TIOCGETC
    struct tchars tcharsw;
    ioctl(0,TIOCGETC, &tchars0);
    tcharsw=tchars0;
    if  (tcharsw.t_intrc=='\177')tcharsw.t_intrc=3; /* ETX - intrup of RE */
    tcharsw.t_eofc=tcharsw.t_quitc= -1;
    ioctl(0,TIOCSETC, &tcharsw);
#endif
    gtty(0,&templ);
    templw=templ;
    ospeed = templ.sg_ospeed;
    lcasef=LCFLG(templ.sg_flags);
    if(lcasef) latf=0;
    kioutf = KIOFLG(templ.sg_flags);
    templw.sg_flags=REDFLG(templw.sg_flags);
    if(lcasef) templw.sg_flags =REDFLGL(templw.sg_flags);
#ifdef TIOCSETA
    templw.sg_length=0;
    templw.sg_width=0;
#endif
    stty(0,&templw);
}

/*
 * ttcleanup()
 * Восстановить режимы терминала
 */
ttcleanup()
{
    stty(0,&templ);
#ifdef TIOCGETC
    ioctl(0,TIOCSETC, &tchars0);
#endif
}

/*
 * pcursor(col,lin) -
 * установить курсор в физические координаты на
 * экране. Ответ 0, если нет прямой адресации
 */
pcursor(col,lin)
int col,lin;
{
    register char *c,sy;
    if ((c=curspos)==NIL) return 0;
    if (agoto) c=(*agoto)(curspos,col,lin);
    if (*c=='O') return(0);
    while ((sy = *c++))
    {
        if(!agoto&&(sy&0200)){
            if (sy&0100) sy=(sy&077)+col;  /* 300 - col */
            else sy=(sy&077)+lin;
        }                        /* 200 - lin */
        putchb(sy);
    }
    return (1);
}

/* ===================
 * Работа с буфером символов
 * ===================
 */

char putcbuf[NPUTCBUF];
int iputcbuf=0;

/*
 * putcha(c) - выдать символ "c".
 * "c" может быть кодом управления.
 * Возвращается 0, если запрошенная операция невозможна
 */
putcha(c)
char c;
{
    register char cr, *s;
    cr=c & 0177;
    if (cr>=0 && cr<=COMCOD) {
        if(!(s=cvtout[cr])) return(0);
        while ((cr = *s++) != 0) putchb(cr);
        goto e;
    }
    putcbuf[iputcbuf++] =( rawf || latf || lcasef ? c&0177 : c );
    if (iputcbuf == NPUTCBUF) dumpcbuf();
e:
    return(1);
}

/*
 * putchb(c) -
 * Вывод символа без в безусловном режиме
 */
putchb(c)
char c;
{
    putcbuf[iputcbuf++] = c&0177;
    if (iputcbuf == NPUTCBUF) dumpcbuf();
    return;
}

/*
 * putblanks(k) -
 * Вывод строки пробелов
 */
putblanks(k)
register int k;
{       cursorcol += k;
    while (k--) {
        putcbuf[iputcbuf++] = ' ';
        if(iputcbuf == NPUTCBUF)  dumpcbuf();
    }
    dumpcbuf(); return;
}

/*
 * dumpcbuf() -
 * выталкивание буфера вывода
 */
dumpcbuf()
{
        if (iputcbuf != 0) write(2,putcbuf,iputcbuf);
        iputcbuf = 0;
}


/*
 * read1()
 * Чтание очередного символа с терминала.
 * Кроме того, здесь же разворачиваются макро и
 * происходит чтение из файла протокола.   Последние две
 * функции будут убраны повыше.
 * Ответ приходит в lread1.
 */
char *rmacl();
#define LBUFWSY 5
static char *sy0, knockdown=0, bufwsy[LBUFWSY];
extern char escch1, in0tab[];
int
read1()
#define GETSY1 if(read(inputfile,&sy1,1)!=1) goto readquit;if(rawf)sy1=sy1&0177;
{
    char sy1;
    int i,cntf=0;
    register int *lr1 = &lread1;
#define lread1 (*lr1)   /* Для ускорения */
    dumpcbuf();
    /* Если остался неиспользованным символ */
    if (lread1 != -1) goto retnl;
    /* Если еще не кончилось макро-расширение */
rmac:
    if (symac) {
        lread1= *symac++;
        if(*symac==0) symac=0;
        goto retnl;
    }
    /* Если идет чтение из файла */
    if (inputfile!=0 && readfc())
        goto retnm;
    /*
     * Ниже - то, что относится к терминалу
     * =====================================
     */
    /* Было преобразование и символы не кончились */
    if (sy0!=0) {
        lread1= *sy0++;
        if(*sy0==0) sy0=0;
        goto retn;
    }
    /* Чтение с клавиатуры */
new:
    intrflag=0;
    GETSY1;
    /* Если символ не управляющий */
    lread1=sy1;
    if(lread1==0177) {
        lread1=CCBACKSPACE;
        goto readycchr;
    }
    lread1 &= 0377;
    if ((lread1&0177) > 037) goto readychr;
    if (knockdown) {
        lread1 += 0100;
        goto readycchr;
    }
    /* Если символ - спец. признак? */
    if (lread1==escch1)
    {
        GETSY1;
        switch (sy1)
        {
        case '\177':
            lread1=CCQUIT;
            goto readycchr;
        case '+':;
        case ';':
            lread1=CCPLLINE;
            goto readycchr;
        case '-':;
        case '=':
            lread1=CCMILINE;
            goto readycchr;
        case '.':;
        case '>':
            lread1=CCGOTO;
            goto readycchr;
        case ' ':
            lread1=CCENTER;
            goto readycchr;
        case '$':
rmacname:
            GETSY1;
            if(sy1&0200) sy1 = STASCII(sy1);
            if(sy1 >= 'a' && sy1 <='z')
            {
                lread1 = (int)sy1 - 'a' + CCMAC+1;
                goto readycchr;
            }
            goto new;
        default:
            ;
        }
        lread1=((int)sy1)&037;
        goto corrcntr;
    }
    /* Веден управляющий символ - ищем команду в таблице */
    {
        int *i1,*i2, ts, k;
        i1=i2=0;
        ts=0;
        sy1=lread1;
        while ((k=findt(&i1,&i2,sy1,ts++))== CONTF)
        {
            GETSY1;
            if(sy1&0200) sy1= STASCII(sy1);
        }
        if ( k== BADF) {
            if (ts==1) goto corrcntr;
            else goto new;
        }
        lread1=k;
        if (lread1 == CCMAC) goto rmacname;
        goto readycchr;
    }
    /* ========================================================= */
corrcntr:
    if (lread1>0 && lread1<=BT) lread1=in0tab[lread1-1];
readycchr:
    cntf=1;
readychr:
    if (lread1==-1) goto new;
    if (lread1==0177) {
        cntf=1;
    }
    knockdown=0;
    if (lread1==CCCTRLQUOTE)
    {
        knockdown=1;
    }
    if(!cntf && !rawf && (lcasef||latf) && (lread1>='@'))
    {
        char bufr[2], *si=bufr, *so=bufwsy;
#ifndef MNOS
        if(lcasef&&(lread1>=0300)) lread1 ^= 040; /* из?за драйвера*/
#endif
        i=1;
        bufr[0]=lread1;
        exinss(&si, si+1, &so, &i, LBUFWSY-1);
        *so=0;
        lread1=((int)bufwsy[0])&0377;
        if(so!= bufwsy+1) sy0=bufwsy+1;
    }
retn:
    sy1=lread1;
    write (ttyfile,&sy1,1); /* ATTENSION - ONLY 8 BIT */
    /*
     * Конец того, что относится к терминалу (до quit).
     * ================================================
     */
retnm:
    if( lread1 >CCMAC && lread1 <= CCMAC+1+'z'-'a' && (symac=rmacl(lread1)))
        goto rmac;
retnl:
    return (lread1);
readquit:
    if(intrflag){
        lread1=CCENTER;
        intrflag=0;
        goto readycchr;
    }
    lread1=CCQUIT;
    goto readycchr;
}
#undef lread1

static int isy0f = -1;
/*
 * readfc()
 * - получение символа из файла протокола
 *   возвращается 0, если файл кончился
 */
readfc()
{
    char sy1 = CCQUIT;
    do {
        lread1=isy0f;
        if ( intrflag || (read(inputfile,&sy1,1) !=1))
        {
            if(inputfile != ttyfile) close (inputfile);
            else  lseek(ttyfile,(long)(-1),1);
            inputfile=0;
            intrflag=0;
            putcha(COBELL);
            dumpcbuf();
            return 0;
        }
        isy0f = ((int)sy1) & 0377;
    }
    while (lread1<0);
    return (1);
}

/*
 * intrup() -
 *       опрос, не было ли прерывания.
 */
intrup()
{
    char sy1;
    if( inputfile ){
        if(isy0f == CCINTRUP ){
            isy0f= -1;
            return(1);
        }
        else return(0);
    }
    else
    {
        if(intrflag){
            intrflag=0;
            sy1=CCINTRUP;
            write(ttyfile,&sy1,1);
            return(1);
        }
        return(0);
    }
}

#define CCDEL 0177
/* read2() -
 *     функция, читающая входные символы
 *     без их анализа.  Используется при
 *     переопределении клавиш.
 */
read2()
{
    char c;
    if (inputfile && readfc()) return(lread1);
    if (read(0,&c,1) != 1) {
        c=CCDEL;
        intrflag = 0;
    }
    c &= 0177;
    write(ttyfile,&c,1);
    return(c);
}



/*
 * findt (&fb,&fe,sy,ns) -
 * Поиск кода клавиши.
 * struct ex_int * (fb , fe) = NULL при подаче
 * первого символа с данной клавиши.
 * Дальше они используются при поиске кода.
 * Коды ответа:
 * CONTF - дай следующий символ,
 * BADF  - такой последовательности нет в таблице,
 * >=0   - код команды.
 */
char kioutf;
/*VARARGS*/
findt (fb,fe,sy,ns)
struct ex_int **fb, **fe;
char sy;
int ns;
{
    char sy1;
    register struct ex_int  *fi;
    fi=( *fb? *fb:inctab);
    *fb=0;
    if (sy==0) return BADF;
    if(kioutf&& (sy&0100)) sy |= 0240; /* Не различаем регистр букв */
    for (;(fi!= *fe); fi++ )
    {
        if (!(*fe) && !(fi->excc) ) goto exit;
        sy1=fi->excc[ns];
        if(kioutf&& (sy1&0100)) sy1 |= 0240; /* Не различаем регистр букв */
        if ( *fb) {
            if (sy!=sy1) goto exit;
        }
        else {
            if (sy==sy1) {
                *fb=fi;
            }
        }
    }
exit:
    *fe=fi; /* for "addkey" */
    if (! *fb) return BADF;
    fi= *fb;
    if(fi->excc[ns+1]) return CONTF;
    return ( fi->incc);
}

/* Тест findt
 main()
  { char *s = "\017abz"; int i,j,k,l,m,is;
        i=j=k=l=m=0;
        for(is=0;*s; is++)
        { k=findt(&i,&j,*s++,l++); if(k!=CONTF) goto ex1; }
ex1:   printf(" k= %d is= %d pt %o %o ", k,is,i,j);
  }
 t(i) int i; {return;}
 */

/*
 * addkey(cmd,key) -
 * добавления новой управляющей клавиши
 *      в таблицу кодов
 */
extern int nfinc; /* число свободных мест в таблице */
addkey(cmd,key)
int cmd;
char *key;
{
    struct ex_int *fb,*fe;
    register struct ex_int *fw;
    register int ns,i;
    ns=0;
    fb = fe = 0;
    while((i=findt(&fb,&fe,key[ns],ns))==CONTF && key[ns++]);
    if(i != BADF)
    {
        telluser(DIAG("key redefined","переопределение"),0);
        fw = fb;
        goto retn;
    }
    /* Код новый = нужно расширить таблицу */
    if (!nfinc) {
        error(DIAG("too many key's","много определений"));
        return(0);
    }
    fw = fe;nfinc--;
    while((fw++)->excc);
    do{
        *fw = *(fw-1);
    }
    while(--fw != fe);
retn:
#ifdef TEST
test("addkey out");
#endif
        fw ->excc = key;
        fw->incc = cmd;
        return(1);
}
#ifdef TEST
test(s) char *s;
{printf("test: %s\n",s); return(0);}
#endif
