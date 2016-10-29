/*
 *      Редактор RED.
 *       $Header: S.ttyio.c,v 4.20 90/05/22 23:22:28 alex Exp $
 *
 *      $Log:   S.ttyio.c,v $
 * Revision 4.20  90/05/22  23:22:28  alex
 * First rev. red 4.2
 * 
 * Revision 4.10  90/02/05  19:52:38  alex
 * Base revision 4.1
 * 
 * Revision 4.10  90/02/05  19:49:23  alex
 * Base revision 4.1
 * 
 * Revision 4.5  88/04/07  12:19:20  alex
 * Очередная правка в роллировании.
 * 
 * Revision 4.4  88/04/07  12:02:13  alex
 * Исправили ошибку в работе setscroll.
 * 
 * Revision 4.3  88/04/04  22:16:14  alex
 * Исправлена ошибка в работе при расщеплении строки (режим CS).
 * 
 * Revision 4.2  88/03/31  22:50:29  alex
 * Исправлена ошибка при работе на терминале Электроника.
 * 
 * Revision 4.1  88/03/31  22:06:09  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 * 
 * Revision 3.17  88/03/30  17:25:07  alex
 * Исправлена ошибка в readch. Введены функции для удаления/вставки
 * строк на экране через ChangeScroll/InsertLine-DeleteLine.
 * 
 * Revision 3.1.2.5  87/07/09  20:44:11  alex
 * REVISION
 * 
 * Revision 3.1.2.4  87/07/03  22:03:56  alex
 * Graph_characters used in margin
 * 
 * Revision 3.1.2.3  87/06/24  22:36:46  alex
 * New readch + gettc + tc table Tested
 * 
 * Revision 3.1.2.2  87/06/23  18:55:33  alex
 * Выкинута lread1 как класс.
 * 
 * Revision 3.1.2.1  87/06/19  16:56:43  alex
 * Start revision for red/4
 * 
 * Revision 3.16  87/06/12  18:07:21  alex
 * Исправлены ошибки в режиме vilcase и мелочи для vt-200
 * 
 * Revision 3.15  87/06/09  19:45:28  alex
 * Исправлена работа с tty ~ VT200 и 15-ИЭ-0013
 * 
 * Revision 3.14  87/06/05  23:48:29  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.13  87/06/04  23:41:52  alex
 * Scroll on -sr- or -al/dl- added
 * 
 * Revision 3.12  87/04/21  22:46:30  alex
 * Debug
 * 
 * Revision 3.11  87/04/21  22:28:57  alex
 * Исправлена ошибка в writefile.
 * 
 * Revision 3.10  87/04/02  18:29:44  alex
 * Версия для Utec слита с версией с выделением прописных букв
 * 
 * Revision 3.9  87/04/01  19:14:19  alex
 * Revision for Utec: Cs, Ce, Ct, hR
 * 
 * Revision 3.8  86/11/25  08:06:26  alex
 * Исправлено ioctl(..,TIOCFLUSH,&flag)
 * 
 * Revision 3.7  86/10/29  02:46:37  root
 * При неверной escape-последовательности сбратываем всю очередь ввода.
 * Сделано для борьбы с пропаданием символов.
 * 
 * Revision 3.6  86/10/14  23:02:52  alex
 * Исправлены ошибки с: RUSDIAG (не описан latdiag), и с LCASEO
 * (KI40BIT нужно задавать так, чтобы при lcasef была 1).
 * В СМ-1700 было 2 ошибки, в рез. все работало на LCASE правильно.
 * 
 * Revision 3.5  86/09/19  21:39:35  alex
 * 
 * 
 * Revision 3.4  86/09/19  19:59:39  alex
 * Версия для СМ-1700
 * 
 * Revision 3.3  86/08/04  20:55:13  alex
 * Bepqh dk LMNQ/DELNQ 2
 * 
 * Revision 3.2  86/07/24  19:07:48  alex
 * Об'единены версии текстов для ЕС и СМ
 *
 * Revision 3.1  86/04/20  23:43:21  alex
 * Базовая версия для ЕС.
 *
 * Revision 3.1  86/04/20  23:43:21  alex
 * *** empty log message ***
 *
 *      Работа с терминалом : установка/снятие режимов, ввод/вывод.
 *      Ввод управляющих клавиш.
 *      Физический уровень
 */

#include "r.defs.h"
#include "S.tele.h"

#ifdef TERMIO
#include <termio.h>
#define  stty(des,fil) ioctl(des,TCSETA,fil)
#define  gtty(des,fil) ioctl(des,TCGETA,fil)
#define  sgttyb termio
#undef TIOCSETA
#undef TIOCGETC
#undef TIOCSLTC
#else
#include <sgtty.h>
#endif

#ifndef TERMIO
#include <sys/types.h>
#include <sys/file.h>
#endif

#define NPUTCBUF 256   /* Размер буфера вывода */

#ifdef TIOCSETA
#define stty(des,fil) ioctl(des,TIOCSETA,fil)
#define gtty(des,fil) ioctl(des,TIOCGETA,fil)
#endif
#ifdef SGTTYB /* Признак ДЕМОС-2 */
#define sgttyb SGTTYB
#endif

#ifndef ANYP
#define ANYP (ODDP|EVENP)
#endif
struct sgttyb templ;
#ifdef TIOCGETC
struct tchars tchars0;
#endif
#ifdef TIOCSLTC
static struct ltchars tmpltc,oldltc;
#endif
int local, localo;
int ospeed; /* Для termcap */
int cy40bit; /* Для переворота в упр. последовательностях */
int lcasef0; /* Внутренний признак lcase для вывода */
int latdiag;
int vilcasef;
int video_mode; /* 0 - обычный режим, 1 - экранный режим */
int firststart=1;  /* 0 - не первый заход в startup */
/*
 * ttstartup()
 * Установить режимы терминала
 */
ttstartup()
{
    static struct sgttyb templw;
    char *msg, *getenv();
#ifdef TIOCGETC
    static struct tchars tcharsw;
    if ( !firststart ) goto set_start;
    ioctl(2,TIOCGETC, &tchars0);
    tcharsw=tchars0;
    if  (tcharsw.t_intrc=='\177')tcharsw.t_intrc=3; /* ETX - intrup of RE */
    tcharsw.t_quitc = -1;
#else
    if ( !firststart ) goto set_start;
#endif
#ifdef TIOCSLTC
    ioctl(2,TIOCGLTC,&tmpltc);
    oldltc = tmpltc;
    tmpltc.t_suspc = tmpltc.t_dsuspc = tmpltc.t_flushc = -1;
#endif
    gtty(2,&templ);
# ifdef LOCALSET
    ioctl(2,TIOCLGET, &local);
    localo = local;
    local = LOCALSET(local);
# endif
#ifdef TERMIO
        templw = templ;
        lcasef = LCASEF(templw);
        ospeed = templw.c_cflag & CBAUD;
        latf   = LATFLG(templw);
        kioutf = KIOFLG(templw);
#else
    templw=templ;
        ospeed = templ.sg_ospeed;
        lcasef=lcasef0=LCFLG(templ.sg_flags);
        latf = LATFLG(templ.sg_flags);
        if(lcasef) latf=0;
        kioutf = KIOFLG(templ.sg_flags);
#ifdef KI40BIT
        cy40bit = KI40BIT(templ.sg_flags);
#endif
#endif
    if( atcread)
    {   register int i;
        i=(*atcread)();
        atcread = (int (*)())NULL;
        if(i) {
            if ( latf ) latdiag = 1;
            printf((i==1?
            DIAG("unknown term capabilities\r\n please, set TTY type (`setenv TERM type' for csh) r\n and repeat red\r\n","Не задано описание возможностей терминала, \r\n установите переменную TERM (`setenv TERM тип' в csh)\r\n и повторите вызов\r\n"):
            DIAG("re can not work with this terminal\r\n","red не может работать с этим терминалом\r\n")));
            exit(1);
        }
    }
    if (latf) latdiag = 1;
#ifndef TERMIO
    templw.sg_flags=REDFLG(templw.sg_flags);
    if(lcasef) templw.sg_flags =REDFLGL(templw.sg_flags);
    if(lcasef && cvtout[COVIOPE] ) {
       vilcasef = 1; lcasef = 0;
     }
#   ifdef TIOCSETA
    templw.sg_length=0;
    templw.sg_width=0;
#       ifdef Y_SIGSTOP
    templw.sg_suspc = templw.sg_dsuspc = -1;
#       endif
#       ifdef TIOCSLTC
    templw.sg_flushc = -1;
#       endif
#   endif
#else
        if(templw.c_cc[VINTR] == 0177) templw.c_cc[VINTR] = 3;
        {
                register int i;
                for(i=1;i<NCC;i++) templw.c_cc[i] = 0377;
        }
        templw.c_iflag = IXON|IXOFF|((INPCK)&templw.c_iflag);
        templw.c_oflag = (templw.c_oflag&(~OPOST));
        if (latf)
            templw.c_cflag = ((templw.c_cflag)|CREAD);
        else
            templw.c_cflag = ((templw.c_cflag&(~PARENB))|CS8|CREAD);
        templw.c_lflag = ECHOK|ISIG;
        templw.c_cc[VMIN ] = 1;
        templw.c_cc[VTIME] = 2;
    if(lcasef && cvtout[COVIOPE] ) {
       vilcasef = 1; lcasef = 0;
     }
#endif
set_start:
#ifdef TIOCGETC
    ioctl(2,TIOCSETC, &tcharsw);
#endif
#ifdef TIOCSLTC
    ioctl(2,TIOCSLTC,&tmpltc);
#endif
# ifdef LOCALSET
    ioctl(2,TIOCLSET, &local);
# endif
    stty(2,&templw);
    video_mode = 1;
    firststart = 0;
}

/*
 * ttcleanup()
 * Восстановить режимы терминала
 */
ttcleanup()
{
    setscroll(&wholescreen,0);
    putcha(COGEND);
    setatr(A_NORM);
    putcha(COFIN);
    dumpcbuf(0);
    printf("\n");
    stty(2,&templ);
#ifdef TIOCSLTC
    ioctl(2,TIOCSLTC, &oldltc);
#endif
#ifdef TIOCGETC
    ioctl(2,TIOCSETC, &tchars0);
#endif
#ifdef LOCALSET
    ioctl(2,TIOCLSET,&localo);
#endif
    video_mode = 0;
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
static int graphcase = 0;
putcha(c)
register int c;
{
    register char cr, *s;
    static int vicase=0;
    int vineed, needgraph;
    int i;
    c &= 0377;
    if (c>=0 && c<=COMCOD) {
        if ( c == COSTART || c == COFIN ) n0scroll = n1scroll = -1;
        if ( c == COERASE || c == COFIN || c == COILINE || c == CODELIN )
              {
                      if ( graphcase ) putcha(COGEND);
                      setatr(A_NORM);
              }
        if ( c == COGSTART ) graphcase = 1;
        if ( c == COGEND   ) graphcase = 0;
        if(!(s=cvtout[c])) return(0);
        while ((cr = *s++) != 0) putchb(cr);
        goto e;
    }
    if ((char)c == (char)(esc2)) c='#';
    if(vilcasef) {
      vineed = 1;
      if( c>= 'A' && c<= 'Z' ) c= c+'a'-'A';
      else if( RLPRO(c))  c= c+'б'-'Б';
      else if( c=='`')  c='\'';
      else if( c=='|')  c='!';
      else if( c=='}')  c=')';
      else if( c=='{')  c='(';
      else if( c=='~')  c='^';
      else vineed = 0;
      if( vineed != vicase) putcha((vicase=vineed)?COVIOPE:COVICLO);
      }
      if ( c >= G_START && c <= G_END)
        {
        if( (i = g_table[(c) - G_START]) ) {
                c = i;
                needgraph = 1;
                }
                else {
                c = g0table[c-G_START];
                needgraph = 0;
                }
         } else needgraph = 0;
         if ( needgraph != graphcase ){
              if ( needgraph )
              {
                      if ( cur_atr != A_NORM ) {
                      i = setatr(A_NORM);
                      putcha(COGSTART);
                      setatr(i);
                      }
                      else putcha(COGSTART);
              }
              else
              {
                      putcha(COGEND);
              }
          }
#ifdef RED_CYRILL
    if( Rcyflag ) {
        if (iscyrill(c) ) {
                if(!Cyout) putcha(COCYON);
                           Cyout= 1;
                           c = OUTT(c);
                }
        else if( !isanychr(c) && Cyout) {
                           putcha(COCYOFF);
                           Cyout=0;
               }
    }
#endif
#ifdef LCASEO
    if ( lcasef0 && !vilcasef ) c=(c>='A'&&c<='Z'?c+040:(c>= 0140 && c<= 0176?c+0140:c));
#endif
    putcbuf[iputcbuf++] = c;
    if (iputcbuf >= NPUTCBUF) dumpcbuf();
e:
    return(1);
}

/*
 * putchb(c) -
 * Вывод символа в безусловном режиме
 */
putchb(c)
char c;
{
    if((c&0377)==0200) c = 0;
#ifdef LCASEO
    if ( lcasef0 ) c=(c>='A'&&c<='Z'?c+040:(c>= 0140 && c<= 0176?c+0140:c));
#endif
    if (c == 0200) c = 0;
    putcbuf[iputcbuf++] = c;
    if (iputcbuf >= NPUTCBUF) dumpcbuf();
    return;
}

/*
 * putblanks(k) -
 * Вывод строки пробелов
 */
putblanks(k)
register int k;
{       cursorcol += k;
    if ( graphcase ) {
      graphcase = 0;
      putcha(COGEND);
    }
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
 * setatr(атрибут) - установить видео-атриьут
 * Возвращает старый атрибут
 */
setatr(atr)
int atr;
{
 register int i = NUMA(atr);
 register int j;
 j = cur_atr;
 if ( graphcase && atr == A_NORM   ) putcha(COGEND); /* Запрещено устанавливать норм. моду из псевдографики */
 if ( atr != A_NORM && j != A_NORM ) setatr(A_NORM);
 if ( atr != cur_atr && i <= COAOUT ) putcha(i);
 cur_atr = atr;
 return( j );
}

extern char *M_p, *M_e;
#define ADD_MACRO(c) ((c != CCQUIT && c != CCDEFMACRO && M_p < M_e ) ?  (*M_p++ = c) : MacExpand(c))
char *ps_inmac, *ps_exin;
int (*pf_wmac)();
char *rmacl();
static int litchar;
static int oldchar  = -1;
static int all_errors = 0;

/*
 * Макро - они могут быть вложены, так что заводим стек на 20 позиций
 */
char *rmacl();
# define MAXSTEK 64
static char *b_stekm[MAXSTEK+1], **p_stekm = b_stekm;
#define S_IN(p) {if ( p && p_stekm < &b_stekm[MAXSTEK] ) *p_stekm++ = p;}
#define S_OUT(p) {if ( p_stekm > b_stekm ) p = *--p_stekm;else p = NULL;}
/*
 * do_macro(cmd) - подставить макро "lc" (lc - внутренний код макро
 * ответ > 0 - макро есть, < 0 - макро нет
 */
do_macro(lc)
int lc;
{
        S_IN(ps_inmac);
        if (! (ps_inmac = rmacl(lc)) )
        {
                S_OUT(ps_inmac);
                return(0);
        }
        return(1);
}

/*
 * readch()
 * Чтение очередного символа с терминала.
 * Кроме того, здесь же разворачиваются макро и
 * происходит чтение из файла протокола.   Последние две
 * функции будут убраны повыше.
 * Переменные:
 * oldchar - символ, возвращенный для повторного чтения п/п unread1()
 * ps_inmac - указатель на читаемое макро
 * ps_exin - указатель на читаемое расширение символа
 * (*pf_wmac)() - текущая запись в макро, она же отрабатывает CCQUIT в конце
 * litchar - признак (след. символ - литерал)
 * rmacl(name) - дает макро по имени
 */
int readch()
{
#define GETSY1(c,g) if( read(inputfile,&c,1) != 1 || intrflag ) goto g; else
        register int lc;
        char sy;
        if(oldchar != -1) { 
                lc = oldchar; 
                oldchar = -1; 
                return(lc); 
        }
        dumpcbuf();
rmacro:
    if(inputfile && (lc = readfc()) != -1) goto after_proto;
/* Весь след. кусок всегда выходит через запись протокола */
    if( ps_inmac) {
        lc = (*ps_inmac++) &0377;
        if(intrflag || *ps_inmac == 0) S_OUT(ps_inmac);
        if(lc > CCMAC && lc <= CCEMAC)
        {
                (void)do_macro(lc);
                goto rmacro;
        }
        goto w_proto;
        }
r_exin:
    if( ps_exin) {
        lc = (*ps_exin++) &0377;
        if(*ps_exin == 0) ps_exin = NULL;
        goto after_in;
        }
/*
 * Чтение очередного символа
 */
new:
#ifdef RED_CYRILL
    if (Rcyflag && Cyin != Cyout )
        {
               infmesg(Cyin?"R":" ",PARAMRLANG+1,A_OUT);
               putcha(COCYON + 1 - Cyin); Cyout = Cyin;
               dumpcbuf();
        }
#endif /* RED_CYRILL */
    intrflag = 0;
    GETSY1(sy, readquit);
    lc = sy & 0377;
    if(litchar) {
        if( lc < 040)
                lc = (lc&037)|'@';
        litchar = 0;
        goto after_in;
        }
#ifdef RED_CYRILL
    if ( Rcyflag) {
        lc &= 0377;
        if( lc==CHA_RUS) {Cyin=1; goto new; }
        if( lc==CHA_LAT) {Cyin=0; goto new; }
        if( Cyin==1 && lc >= 040 && lc < 0177) lc = INTT(lc);
    }
#endif /* RED_CYRIILL */
    if( ISCTRL(lc)) {
        int *i1,*i2, ts, k;
        i1=i2=0;
        ts=0;
        sy=lc;
        while ((k=findt(&i1,&i2,sy,ts++))== CONTF)
        {
            GETSY1(sy,readquit);
            if(sy&0200) sy = STASCII(cy40bit?(sy^040):sy);
        }
        if ( k== BADF)
        {
#ifndef TERMIO
            int flag=FREAD; putcha(COBELL); dumpcbuf();
            ioctl(inputfile,TIOCFLUSH,&flag);
#else
            putcha(COBELL);dumpcbuf();
            ioctl(inputfile,TCFLSH,0);
#endif
            if ( all_errors++ < 2 )
                      goto new;
            k = CCHELP;
        }
        all_errors = 0;
        lc=k;
        if (lc == CCMAC) {
            GETSY1(sy, readquit);
            if(sy&0200) sy = STASCII(cy40bit?(sy^040):sy);
            if(sy >= 'a' && sy <='z')
                {
                lc = (int)sy - 'a' + CCMAC+1;
                } else goto new;
            }
            goto after_in;
    }
    /* ========================================================= */
    /* Преобразование символов при вводе в lcase */
    if((lcasef||latf) && (lc >= '@'))
    {
#define LBUFWSY 5
        static char bufwsy[LBUFWSY+1];
        char bufr[2], *si=bufr, *so=bufwsy;
        int i;
#ifndef MNOS
#ifndef LCASEO
        if(lcasef&&(lc>=0300)) lc ^= 040; /* из?за драйвера*/
#endif
#endif
        i=1;
        bufr[0]=lc;
        exinss(&si, si+1, &so, &i, LBUFWSY-1);
        *so=0;
        lc=((int)bufwsy[0])&0377;
        if(so!= bufwsy+1) ps_exin = bufwsy+1;
    }
/*
 * Точки выхода из подпрограммы
 */
after_in:
/* Запись макро */
    if ( M_p && !ADD_MACRO(lc)) goto new; /* Символ съели */
/* Опознание макро */
    if(lc > CCMAC && lc <= CCEMAC)
        {
        if (do_macro(lc)) goto rmacro;
        else goto new;
        }
    if( lc == CCCTRLQUOTE) {
        litchar = 1;
        }
w_proto:
    if(ttyfile > 0) {
        sy = lc;
        write(ttyfile,&sy, 1);
        }
after_proto:
    return(lc);
readquit:
    if(intrflag){
        lc=CCENTER;
        intrflag=0;
    }   else
        lc=CCQUIT;
    goto w_proto;
}

unread1(ch)
int ch;
{
oldchar = ch&0377;
}

static int isy0f = -1;
/*
 * readfc()
 * - получение символа из файла протокола
 *   возвращается 0, если файл кончился
 */
readfc()
{
    char sy1 = CCQUIT;
    register int lread1;
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
            return (-1);
        }
        isy0f = ((int)sy1) & 0377;
    }
    while (lread1<0);
    return (lread1);
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
            S_OUT(ps_inmac);
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
            S_OUT(ps_inmac);
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
    register int lread1;
    if (inputfile && (lread1=readfc())!= -1) return(lread1);
    if (read(0,&c,1) != 1) {
        c=CCDEL;
        intrflag = 0;
    }
    c &= 0177;
    write(ttyfile,&c,1);
    return(c);
}

/*
 * writefile -
 * Записать команду с аргументами в файл протокола
 */

writefile(code1,str,code2)
int code1, code2;
char *str;
{
        char cd1=code1, cd2=code2;
        write(ttyfile,&cd1,1);
        for(;*str;str++) write(ttyfile,str,1);
        write(ttyfile,&cd2,1);
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
int kioutf;
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

/*
 * scroll(n,bl) - прокрутить экран на n строк вверх (вниз), начиная со
 * строки bl. bl <0 == bl = -1-bl,но особый случай - не прокрутить, а удалить
 * выдается 0, если не сделано, иначе 1
 */
scroll(n,bl)
int n,bl;
{
        register int i;
        int dll,ill,nl;
        int no_sf;      /* Флаг - признак запрета сдвига по sf */
#define PUTCC(c,n) for(i= n;i>0;i--) putcha(c)
        if(!(curport->flags&WF_ROLL)) {
                /*
                 * Установить признак возможности roll
                 */
                i=WF_NROLL;
                if(!can_scroll) goto ex;
                if(curport->lmarg != 0 || curport->rmarg != wholescreen.rmarg) goto ex;
                if(can_scroll == ROLL_SF
                    && (curport->tmarg != 0
                    || curport->bmarg != wholescreen.bmarg - NPARAMLINES)) goto ex;
                i = WF_YROLL;
ex: 
                curport->flags |= i;
        }
        if(!(curport->flags & WF_YROLL) ) return(0);
        if ( n == 0 ) return(1);
        no_sf = 0;
        if (bl < 0 ) { no_sf = 1; bl = -bl -1; }
        if( n+bl>curport->btext-2 || -n+bl > curport->btext-2) return(0);
        switch(can_scroll) {
        case ROLL_CS:
                if(!setscroll(curport,bl))  return(0);
                if(n>0) {
                        poscursor(0, curport->btext);
                        PUTCC(COSRFWD,n);
                } 
                else {
                        poscursor(0,bl);
                        PUTCC(COSRBAK, -n);
                }
                if (bl) setscroll(curport,0);
                break;
        case ROLL_IL:
                if(n>0) {
                        nl=n;
                        dll=bl;
                        ill=curport->btext - nl+1;
                }
                else
                {
                        nl = -n;
                        dll = curport->btext - nl+1;
                        ill=bl;
                }
                poscursor(0,dll);
                PUTCC(CODELIN,nl);
                poscursor(0,ill);
                PUTCC(COILINE,nl);
                break;
        case ROLL_SF:
                if(no_sf) return(0);
                {
                        register struct viewport *w;
                        w = curport;
                        if ( n > 0 )
                        {
                                switchport(&wholescreen);
                                poscursor(0,wholescreen.bmarg);
                                PUTCC(COSRFWD,n);
                                switchport(w);
                                if(need_box != 2 )
                                {
                                        poscursor(-w->ltext, w->btext-n+1);
                                        putcha(COCLSCR);
                                }
                        } 
                        else {
                                if(need_box != 2 )
                                {
                                        poscursor(-w->ltext,w->btext+n+1);
                                        putcha(COCLSCR);
                                }
                                switchport(&wholescreen);
                                poscursor(0,0);
                                for(i= -n; i; i--)
                                        putcha(COCLLIN), putcha(COSRBAK);
                                switchport(w);
                        }
                        need_box = (n==1||n== -1)?1:2;
                        new_info = 1;
                        break;
                }
        default: 
                return(0);
        }
        shiftview(bl,n);
        return(1);
}

/*
 * setscroll(viewp,begl) - установка границ роллинга экрана
 * на область text данного окна
 * начиная со строки begl
 */
setscroll(viewp,begl)
register struct viewport *viewp;
{
        register char *c;
        extern char *tgoto();
        register l0, l1;
        struct viewport *cur_p;
        l0 = viewp->ttext;
        l1 = viewp->btext + l0;
        l0 += begl;
        if (l0 >= l1) return(0); /* Терминалы не допускают cs на 1 строку */
        if(n0scroll==l0 && n1scroll==l1)
                return(1);
        if( !ch_scroll) return(0);
        c= tgoto(ch_scroll,  l1, l0);
        if(!c) return(0);
        cur_p = curport;
        switchport(&wholescreen);
        while(*c) putchb(*c++);
        putcha(COHO);
        cursorcol = cursorline = 0;
        switchport(cur_p);
        n0scroll = l0;
        n1scroll = l1;
        return(1);
}
