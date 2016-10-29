/*
 *      Редактор RED.
 *
 * r.hlam.c Разные программы / всякий хлам
 *
 *      $Header: r.hlam.c,v 4.20 90/05/22 23:22:03 alex Exp $
 *      $Log:   r.hlam.c,v $
 * Revision 4.20  90/05/22  23:22:03  alex
 * First rev. red 4.2
 * 
 * Revision 4.10  90/02/05  19:52:25  alex
 * Base revision 4.1
 * 
 * Revision 4.10  90/02/05  19:49:16  alex
 * Base revision 4.1
 * 
 * Revision 4.1  88/03/31  22:03:09  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 * 
 * Revision 3.1.2.2  87/06/24  22:34:44  alex
 * New readch + gettc + tc table Tested
 * 
 * Revision 3.1.2.1  87/06/19  17:00:25  alex
 * Start revision for red/4
 * 
 * Revision 3.7  87/06/05  23:50:35  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.6  86/10/29  02:47:40  root
 * checkpriv для суперюзера теперь все же проверяет
 * право записи хотя бы для одного их "u-g-a"; чтение
 * разрешено всегда. Сделано для предотвращения случайной
 * порчи файлов (как правило, портили re.std).
 * 
 * Revision 3.5  86/09/19  21:38:27  alex
 * 
 * 
 * Revision 3.4  86/09/19  19:53:32  alex
 * Версия для СМ-1700
 * 
 * Revision 3.3  86/08/04  20:51:58  alex
 * Bepqh dk LMNQ/DELNQ 2
 * 
 * Revision 3.2  86/07/24  00:24:37  alex
 * Об'единены версии текстов для ЕС и СМ
 * 
 * Revision 3.1.1.5  86/07/15  22:12:42  alex
 * RED 3.3/EC.
 * 
 * Revision 3.1.1.4  86/06/16  22:18:22  alex
 * Первая версия с повтором сеанса (пока грязная)
 * 
 * Revision 3.1.1.2  86/06/05  18:53:33  alex
 * Отлажено_на_модели
 * 
 * Revision 3.1.1.1  86/06/05  00:09:28  alex
 * *** empty log message ***
 * 
 * Revision 3.1  86/04/20  23:41:51  alex
 * Базовая версия для ЕС.
 * 
 * Revision 3.1  86/04/20  23:41:51  alex
 * *** empty log message ***
 * 
 */
#include "r.defs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

/*   salloc -
 * выделить память, обнулить и проверить
 */
char *salloc(n)
unsigned n;
{
    register char *c, *i;
    extern char *zAlloc();
    if(!(c=zAlloc(n))) fatal(NULL);
    i = c + n;
    while (i != c) *--i = 0;
    return (c);
}

/*
 * checkpriv(file) - Проверить права. 0 - ни чтения, ни записи;
 *                   1 только чтение
 *                   2 и чтение, и запись
 */
checkpriv(fildes)
int fildes;
{
    register struct stat *buf;
    struct stat buffer;
    int anum, num;
    register int unum,gnum;
    buf = &buffer;
    fstat(fildes,buf);
    unum = gnum = anum = 0;
    if (userid == 0) unum = gnum = anum = 1;
    if (buf->st_uid == userid || userid == 0)
    {
        if (buf->st_mode & 0200) unum = 2;
        else if (buf->st_mode & 0400) unum = 1;
    }
    if (buf->st_gid == groupid || userid == 0)
    {
        if (buf->st_mode & 020) gnum = 2;
        else if (buf->st_mode & 040) gnum = 1;
    }
    if (buf->st_mode & 02) anum = 2;
    else if (buf->st_mode & 04) anum = 1;
    num = (unum > gnum ? unum : gnum);
    num = (num  > anum ? num  : anum);
    return(num);
}

/*
 * getpriv(fildes)
 * Дай режим
 */
getpriv(fildes)
int fildes;
{
        struct stat buffer,*buf;
        buf = &buffer;
        fstat(fildes,buf);
        return (buf->st_mode & 0777);
}

/* strcopy(a,b)
 * Копировать строку b в строку a
 */
strcopy(a,b)
char *a,*b;
{
        while(*a++ = *b++);
}

/* abs(n) -
 * Функция abs
 */
abs(number)
int number;
{
        return (number<0 ? -number : number);
}

/*
 * append(name,ext) - возвращает об'единение строк
 */
char *append(name,ext)
char *name,*ext;
{
    int lname;
    register char *c,*d,*newname;
    lname = 0;
    c = name; 
    while (*c++) ++lname;
    if ( ext )
    {
      c = ext;
      while (*c++) ++lname;
    }
    newname = c = salloc(lname+1);
    d = name; 
    while (*d) *c++ = *d++;
    if ( ext )
    {
      d = ext;
      while (*c++ = *d++);
    }
    return newname;
}

/*
 * s2i(s,*i) - Преобразование строки s в целое. Значение пересылается в *i.
 *      Возвращается указатель на первый символ в строке за числом, если
 *        там еще остались символы, или 0.
 */
char *s2i(s,i)
char *s;
int *i;
{
    register char lc,c;
    register int val;
    int sign;
    char *ans;
    sign = 1;
    val = lc = 0;
    ans = 0;
    while ((c = *s++) != 0) {
        if (c >= '0' && c <= '9') val = 10*val + c - '0';
        else if (c == '-' && lc == 0) sign = -1;
        else {
            ans = --s; 
            break; 
        }
        lc = c;
    }
    *i = val * sign;
    return ans;
}

/*
 * getnm() -    Подпрограмма выдает номер пользователя в текстовом виде.
 */
#define LNAME 8
static char namewd[LNAME+1];
char *getnm(uid)
int uid;
{
        register int i;
        i = LNAME; 
        namewd[LNAME]=0;
        while( i>1 && uid>0) {
                namewd[--i]= '0' + uid %10;
                uid /= 10;
        }
        return(&namewd[i]);
}

/*
 * get1c, get1w(fd) - дай байт/слово
 * put1c, put1w(w,fd) - положи байт/слово
 */
int get1w(fd)
int fd;
{ 
        int i; 
        if(read(fd,&i,SWORD)==SWORD) return(i);
        return(-1);
}

int get1c(fd)
int fd;
{
        char c; 
        if(read(fd,&c,1)==1) return((unsigned)c);
        return(-1);
}

put1w(w,fd)
int fd,w; 
{
        write(fd,&w,SWORD);
        return;
}

put1c(c,fd)
int fd; 
char c;
{
        write(fd,&c,1); 
        return;
}


/* seek - зaмeнa на lseek
 * В ДЕМОС/ЕС имя seek в локальных определениях заменено на oldseek
 */
seek(fd,shift,dsk)
{
        long i;
        i = shift;
        if ( dsk >= 3 ) dsk -= 3, i *= 512l;
        return((int)lseek(fd,i,dsk));
}

/*
 * tostop() -
 * Выйти на паузу (только в новом tty)
 */
tostop()
#if !defined(Y_SIGSTOP) || !defined(SIGSTOP)
{ 
        return(1);
}
#else
{
        ttcleanup();
        kill(0,SIGSTOP);
        ttstartup();
        return(0);
}
#endif

/*
 * run_shell(char *cmd)
 * выполнить команду UNIX и вернуться обратно
 */
run_shell(cmd)
char *cmd;
{
        char c;
        int i;
        int curl, curc;
        curl = cursorline;
        curc = cursorcol;
        ttcleanup();
        i = system(cmd);
        write(2,"\n\nPress ANY key to return to editor...",38);
        read(2,&c,1);
        ttstartup();
        poscursor(curc, curl);
        rescreen(0);
        return(i);
}

# define MI 20
char *Itoa(ii)
int ii;
{
        static char ib[MI+1];
        register char *s;
        register int j,i;
        s = &ib[MI];
        i = (ii >= 0 ? ii : -ii);
        while ( i )
        {
                j = i / 10;
                *--s = (i - j*10) + '0';
                i = j;
                if ( ib+2  > s ) return("**");
        }
        if ( s == &ib[MI] ) *--s = '0';
        if ( ii < 0 ) *--s = '-';
        return(s);
}
