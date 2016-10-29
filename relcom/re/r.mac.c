/*
 *      Редактор RED.
 *
 *      $Header: r.mac.c,v 4.20 90/05/22 23:22:08 alex Exp $
 *      $Log:   r.mac.c,v $
 * Revision 4.20  90/05/22  23:22:08  alex
 * First rev. red 4.2
 * 
 * Revision 4.10  90/02/05  19:52:27  alex
 * Base revision 4.1
 * 
 * Revision 4.1  88/03/31  22:03:40  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 * 
 * Revision 3.1.2.2  87/06/23  18:51:42  alex
 * wYNESENA PEREMENNAQ lread1 I \TO OTLAVENO
 * 
 * Revision 3.1.2.1  87/06/19  17:00:53  alex
 * Start revision for red/4
 * 
 * Revision 3.5  87/06/05  23:50:50  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.4  86/09/19  19:54:15  alex
 * Версия для СМ-1700
 * 
 * Revision 3.3  86/08/04  20:52:20  alex
 * Bepqh dk LMNQ/DELNQ 2
 * 
 * Revision 3.2  86/07/24  19:12:03  alex
 * Об'единены версии текстов для ЕС и СМ
 * 
 * Revision 3.1.1.2  86/06/05  18:53:55  alex
 * Отлажено_на_модели
 * 
 * Revision 3.1.1.1  86/06/05  00:05:14  alex
 * Отлажено_на_модели
 * 
 * Revision 3.1  86/04/20  23:42:10  alex
 * Базовая версия для ЕС.
 * 
 * Revision 3.1  86/04/20  23:42:10  alex
 * *** empty log message ***
 * 
 * Revision 1.4  86/04/13  22:01:27  alex
 */


/* Файл содержит программы, реализующие новые возможности
 *  редактора "RED", связанные с макропеременными
 *
 * Типы макросов
 TAG - точка в файле
 BUF - буфер вставки
 MAC - макро=вставка
 *
 * А вообще-то все это требует серьезной переделки
*/

#include "r.defs.h"

#define MTAG 1
#define MBUF 2
#define MMAC 3

struct tag {int line, col, nfile;};
#define MSBUF SSAVEBUF
#define MSTAG sizeof(struct tag)
#define MSMAC sizeof(char *)

#define LMAC ('z'-'a'+11)
/* Имена макро */
static char Mnames[]="abcdefghijklmnoqprstuvwxyz0123456789";
int csrsw; /* Для яркой отметки на экране */
union macro {struct savebuf mbuf; struct tag mtag; char *mstring;}
                *mtaba[LMAC], *mmtaba[LMAC];
char mtabt[LMAC];

/*
 * union macro *mname(name,typ,l)
 * Функция поиска описателя по имени
 * если l=0, то ищет и проверяет тип,
 * иначе создает новый описатель
 */
union macro *mname(name,typ,l)
register char *name;
int typ,l;
{
    register int i;char cname;
    register union macro **pm;
    cname = *name;
    if ( cname >= '0' && cname <= '9' ) cname = 'z'+ (cname-'0') +1;
    cname = (cname|040) & 0177;
    i= cname -'a';
    if(i >= LMAC || i < 0 || (*(name+1) != 0))
    {
        error(DIAG("ill.macro name","Недоп.имя макро"));
        goto err;
    }
    pm = (typ != MMAC? &(mtaba[i]): &(mmtaba[i]));
    if(l) {
        if(*pm) {
            if (typ == MMAC && (*pm)->mstring ) zFree((*pm)->mstring);
            zFree((char *)(*pm));
        }
        if ( typ != MMAC ) mtabt[i]=typ;
        *pm =(union macro *)salloc(l);
        goto retn;
    }
    if( typ == MMAC && !(*pm) ) {
        error( DIAG("undefined","неопределено"));
        goto err;
    }
    if( typ != MMAC && mtabt[i] != typ) {
        error( mtabt[i]?DIAG("ill.macro type","Ошиб.тип макро"):DIAG("undefined","неопределено"));
        goto err;
    }
retn:
    return(*pm);
err:
    return(0);
}

/*
 * msrbuf( sbuf, name,op)
 * Функция запоминает и выдает буфер вставки
 * op=1 - выдать, 0 - запомнить
 * ответ 1, если хорошо, иначе 0
 */
msrbuf( sbuf, name,op)
register struct savebuf *sbuf;
register char *name;
int op;
{
    register union macro *m;
    if ((m=mname(name,MBUF,(op?0:MSBUF))))
        {
        if(op) *sbuf = m->mbuf; else m->mbuf = *sbuf;
        return(1);
    }
    return(0);
}

/*
 * msvtag(name) -
 * Функция запоминает текущее положение курсора в файле под именем name.
 * Ее дефект в том, что tag (метка) не связана с файлом жестко и
 * перемещается при редактировании предыдущих строк файла
 */
msvtag(name)
register char *name;
{
        register union macro *m; register struct workspace *cws;
        cws = curwksp;
        if( !(m=mname(name,MTAG,MSTAG)) ) return(0);
        m->mtag.line = cursorline + cws->ulhclno;
        m->mtag.col  = cursorcol  + cws->ulhccno;
        m->mtag.nfile= cws->wfile;
        return(1);
}

/*
 * mgotag(name) -
 * Функция mgotag служит для установки курсора обратно в именованную
 * точку. cgoto является общей для нее и для нескольких других функций
 */
mgotag(name)
char *name;
{
    register int i;
    int fnew=0;
    register union macro *m,*m1;
    if( !(m=mname(name,MTAG,0))) return(0);
    /* Особое свойство перехода по метке 1 */
    /* Если переход вырожден, то идем на метку 2, иначе запишем метку 2 */
    if ( name[0] == '1' ) {
      if ( curwksp->wfile == m->mtag.nfile
           && ABS_LIN(cursorline) == m->mtag.line
           && ABS_COL(cursorcol)  == m->mtag.col
           && (m1 = mname("2",MTAG,0) ) ) m = m1;
      else
            msvtag("2");
    }
    if (curwksp->wfile != (i=m->mtag.nfile))
    {
        editfile(openfnames[i],0,0,0,0);
        fnew=1;
    }
    cgoto(m->mtag.line, m->mtag.col, -1, fnew);
/*    csrsw = 1;      */
    return(1);
}

/*
 * mdeftag(name)
 * Функция mdeftag вырабатывает параметры, описывающие область
 *  между текущим положением курсора и меткой "name". Она заполняет:
 *      paramtype = -2
 *      paramc1   =    соответствует точке "name"
 *      paramr1   =           -- // --
 */
mdeftag(name)
char *name;
{
    register union macro *m;
    if( !(m=mname(name,MTAG,0))) return(0);
    return(set_param(m->mtag.nfile,m->mtag.line,m->mtag.col));
}

set_param(nfile,line,col)
int nfile,line,col;
{
    register struct workspace *cws; 
    int cl,ln,f=0;
    cws = curwksp;
    if(nfile != cws->wfile) {
        error(DIAG("another file","Другой файл"));
        return(0);
    }
    paramtype= -2;
    paramr1 = line;
    paramc1 = col ;
    paramr0 += cws -> ulhclno;
    paramc0 += cws -> ulhccno;
    if( paramr0 > paramr1) {
        f++;
        ln = paramr1;
        paramr1 = paramr0; 
        paramr0 = ln;
    }
    else ln = paramr0;
    if( paramc0 > paramc1) {
        f++;
        cl = paramc1;
        paramc1 = paramc0; 
        paramc0 = cl;
    }
    else cl = paramc0;
    if( f ){
        cgoto(ln,cl,-1,0);
    }
    paramr0 -= cws -> ulhclno;
    paramr1 -= cws -> ulhclno;
    paramc0 -= cws -> ulhccno;
    paramc1 -= cws -> ulhccno;
    if (paramr1 == paramr0)
        telluser(DIAG("**:columns defined by tag","***Часть строки указана меткой***"),0);
    else if(paramc1 == paramc0)
        telluser(DIAG("**:lines defined by tag","***:строки указаны меткой"),0);
    else telluser(DIAG("**:square defined by tag","**:прямоугольник указан меткой"),0);
    return(1);
}

/*
 * defmac(name)
 * Программа определения макро - последовательности,
 */
defmac(name)
char *name;
{
    register union macro *m;
    if(!(m = mname(name,MMAC,MSMAC))) return(0);
    param(1);
    if(paramtype == 1 && paramv)
        {    
        m->mstring = paramv; 
        paraml = 0; paramv=NULL;
        return(1);
    }               
    return(0);
}

/*
 * NewMacro(name, macro)
 * Записать макро "macro" под именем name
 * macro должно уже быть размещено в памяти
 * Ответ - 0 - плохо, либо код команды для вызова этого макро
 */
NewMacro(name, macro)
char *name, *macro;
{
    register union macro *m;
    int i;
    /* Получили номер по имени */
    i = name[0];
    if ( i >= '0' && i <= '9' ) i = i - '0' + ('z'+1);
    i = i - 'a' + (CCMAC+1);
    if(!(m = mname(name,MMAC,MSMAC))) return(0);
    m->mstring = macro;
    return(i);
}

char Mac_info[3];
/*
 * char *rmacl(isy)
 * Выдается макро = последовательность или 0;
 * Имя определяется как один символ с кодом isy - 0200 + 'a'
 */
char *rmacl(isy)
int isy;
{
        char nm[2];
        register union macro *m;
        nm[0]=isy - (CCMAC+1) + 'a';
        nm[1] =0;
        if (!(m=mname(nm,MMAC,0))) return(0);
        return(m->mstring);
}

#define LKEY 20 /* Макс. число символов, генерируемых новой клавишей */
/*
 * defkey()
 * Функция служит для диалогового переопределения клавиатуры
 */
defkey()
{
    char bufc[LKEY+1], *buf;
    register int lc;
    struct viewport *curp;
    int curl,curc;
    register char *c,*c1;
    int lread1;
    curp = curport; 
    curc = cursorcol; 
    curl = cursorline;
    switchport(&paramport);
    poscursor(22,0);
    telluser(DIAG(" enter <new key><del>:","нажмите <новую клавишу><забой>:"),0);
    lc = 0;
    while((bufc[lc] = read2()) !='\177'  && lc++ < LKEY);
    if ( lc ==0 || lc == LKEY )
    {
        goto reterr;
    }
    bufc[lc] = 0;
    telluser(DIAG("enter <command> or <macro name>:","введите команду или имя макро"),1);
    poscursor(33,0);
    lread1= readch();
    if (!ISACMD(lread1)) {
        if(lread1 == '$') lread1 = CCMAC;
        else
            if(lread1 >= 'a' && lread1 <= 'z') lread1 += CCMAC+1 -'a';
            else {
                goto reterr;
            }
    }
    telluser("",0);
    c = buf = salloc(lc + 1);
    c1 = bufc;
    while ( *c++ = *c1++);
    lc = addkey(lread1,buf);
ret:    
    switchport(curp);
    poscursor(curc,curl);
    return(lc);
reterr:
    lc = 0; 
    error(DIAG("illegal","ошибка")); 
    goto ret;
}

/* Величина, на которую увеличивается буфер для макро */
# define MAC_EXLEN      64
/* Макс. длина одного макро */
# define MAC_MAXL      512
/*
 * Программы для работы с макро в режиме обучения
 *
 * MacCreate(char *Name) - начать заведение макро
 * если Name == NULL - имя по умолчанию
 *
 * MacExpand(cmd) - расширить буфер, если он переполнился,
 * и дописать туда "cmd"
 * Вызывается из программы ввода символа через макро
 * Код ответа 0 - символ уже потребили
 *
 * MacEnd(f!=0) - завершить ввод макро и занести его в таблицу
 *  f == 0 - не заносить в таблицу
 * MacSave(имя, режим) - запомнить все имеющиеся макро в
 * файле "имя". По умолчанию используется переменная "macro".
 * (точнее, ее первая компонента)
 *
 * MacRead(имя) - прочитать макро из файла
 *
 * Связь с readch
 * extern char *M_p, *M_e;
 * #define ADD_MACRO(c) (c != CCDEFMACRO && M_p < M_e? *M_p++ = c: MacExpand(c))
 * if ( M_p ) ADD_MACRO(lread1);
 * ---
 * В режиме ввода макро в правом нижнем углу выдается >n, где n - имя макро
 */
static char *M_buf; /* Начало буфера */
char *M_p;   /* Указатели текущего буфера и его конца */
char *M_e;
static union macro *M_mac; /* Указатель структуры тек. макро */
MacCreate(name)
char *name;
{
        /* Вообще то, такого не бывает, разве только командой из аргумента */
        if ( M_p ) {
                MacEnd(0);
                error(DIAG("Recursive macro def","Рекурсивное опр. макро"));
                return(-1);
        }
        if ( !name ) name = "0";
        M_mac = mname(name,MMAC, MSMAC);
        if ( !M_mac ) return(-1);
        M_mac->mstring = (char *)0;
        M_buf = M_p = salloc(MAC_EXLEN+1);
        M_e = M_p + MAC_EXLEN;
        Mac_info[0] = '>'; Mac_info[1] = name[0];
        infmesg(Mac_info,PARAMRMAC,A_ERROR);
        return(0);
}

/* MacExpand вызывается, когда не хватает текущего буфера
 * ответ = 0 - символ конца ввода макро, дальше его не передавать!
 */
MacExpand(cmd)
int cmd;
{
        int l;
        if ( cmd == CCQUIT || cmd == CCDEFMACRO )
        {
                MacEnd(1);
                return(0);
        }
        l = M_p - M_buf;
        if ( l > MAC_MAXL )
        {
                MacEnd(1);
                error(DIAG("Macro too long. Truncated","Макро слишком длинное."));
                return(0);
        }
        M_e = M_buf;
        M_buf = salloc(l*2+1);
        strcopy(M_buf,M_e);
        zFree(M_e);
        M_e = M_buf + l*2;
        M_p = M_buf + l;
        *M_p++ = cmd;
        return(cmd);
}

MacEnd(f)
int f; /* 0 - не записывать, 1 - записывать */
{
        Mac_info[0] = ' '; Mac_info[1] = ' ';
        infmesg(Mac_info,PARAMRMAC,A_INFO);
        if ( !M_buf || !M_mac ) {
                error(DIAG("Internal Error MAC","Сист. ош. макро"));
                return(-1);
        }
        if ( f && M_buf[0] )
        {
                M_mac->mstring = append(M_buf,"");
        }
        zFree(M_buf);
        M_buf = M_p = M_e = (char *)0;
        M_mac = (union macro *)0;
        return(0);
}

/*
 * MacWrite(fd) - записать все определенные макро в файл
 * с дескриптором fd
 * Код ответа - количество записанных макро
 */
MacWrite(fd)
FILE *fd;
{
        char *mm;
        register union macro *pm;
        int lc;
        int ko=0;
        for(lc = CCMAC+1;ISMACRO(lc);lc++)
        {
                pm = mmtaba[lc-CCMAC-1];
                if ( !pm ) continue;
                mm = pm->mstring;
                if ( !mm ) continue;
                ko++;
                fprintf(fd,"macro %c\n",Mnames[lc-CCMAC-1]);
                WriteMFile(mm,fd);
                fprintf(fd,".\n");
        }
        return(ko);
}
