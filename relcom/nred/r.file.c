/*
 *      Редактор RED. ИАЭ им. И.В. Курчатова, ОС ДЕМОС
 *
 *      $Header: /home/sergev/Project/vak-opensource/trunk/relcom/nred/RCS/r.file.c,v 3.1 1986/04/20 23:41:27 alex Exp $
 *      $Log: r.file.c,v $
 *      Revision 3.1  1986/04/20 23:41:27  alex
 *      *** empty log message ***
 *
 *
 * r.file.c - Работа с файлами.
 *
 */

#include "r.defs.h"

/*
 * savefile(file,n) -
 * Запись файла из канала n в файл "file"
 * Если file не задан, используется свое родное имя
 * openfnames[n];
 * если файл не записывается, предлагает записать его в "."
 */
savefile(file,n)
char *file;
int n;
{
    register char *f1;
    char *f0, *f2;
    register int i, j;
    int newf,nowrbak=0;
    /* дай справочник */
    if (file) {
        f0=file;
    } 
    else {
        f0=openfnames[n]; 
        nowrbak=1;
    }
    for (f1=f2=f0; *f1; f1++) if (*f1 == '/') f2 = f1;
    if (f2 > f0)
    {
        *f2 = '\0';
        i = open(f0,0);
        *f2 = '/';
    }
    else i = open (".",0);
    if (i < 0)
    {
        error (DIAG("Directory does not exist.","Справочник не существует."));
        return(0);
    }
    j = checkpriv(i);
    close (i);
    if (j != 2)
    {
        if (file)
        {
            error (DIAG("Can't write in specified directory","Запись в справочник запрещена"));
            return(0);
        }
        if (f2 > f0)
        {
            telluser(DIAG("Hit <save> to use '.'","Нажмите <save> чтобы использовать '.'"),0);
            lread1 = -1; 
            nowrbak=0;
            read1();
            if (lread1 != CCSAVEFILE) return(-1);
            if ((i = open(".",0)) < 0)
            {
                error (DIAG("Directory '.' does not exist!","Справочник '.' не существует!"));
                return(0);
            }
            j = checkpriv(i);
            close (i);
            if (j != 2)
            {
                error (DIAG("Can't write in '.'","Запись в '.' невозможна"));
                return(0);
            }
            f0 = f2 +1;/* points to file name */
        }
        else
        {
            error (DIAG("Can't write in '.'","Запись в '.' невозможна"));
            return(0);
        }
    }
    /* Готовимся к записи файла f0 */
    f1 = append (f0,SUFFBAK);
    if (nowrbak && !movebak[n]) {
        nowrbak=0; 
        movebak[n]=1;
    }
    if (!nowrbak)
    {   
        unlink(f1);
        link (f0,f1);
    }
    unlink (f0);
    if ((newf = creat(f0,getpriv(n))) < 0)
    {
        error (DIAG("Creat failed!","Ошибка создания!"));
        return(0);
    }
    /*      chown(f0,userid);       */
    /* Собственно запись. */
    telluser(DIAG("save: ","зап: "),0);
    telluser(f0,6);
    return (fsdwrite(openfsds[n],077777,newf) == -1 ? 0 : 1);
}

/*
 * fsdwrite(f,nl,newf) -
 * Запись по цепочке описателей f в файл "newf"
 * Если nl # 0  - записывать только nl строк или
 * -nl абзацев текста (nl используется в случае
 * команды "exec".
 * Ответ - число записанных строк, или -1, если ошибка.
 */
fsdwrite(ff,nl,newf)
struct fsd *ff;
int nl, newf;
{
    register struct fsd *f;
    register char *c;
    register int i;
    int j,k,bflag,tlines;
    if (lcline < LBUFFER) excline(LBUFFER);
    f = ff;
    bflag = 1;
    tlines = 0;
    while (f->fsdfile && nl)
    {
        if (f->fsdfile > 0)
        {
            i = 0;
            c = &f->fsdbytes;
            for (j=f->fsdnlines; j; j--)
            {
                if (nl < 0) {
                    /* Проверяем счетчик пустых строк */
                    if (bflag && *c != 1) bflag = 0;
                    else if (bflag == 0 && *c == 1) {
                        bflag = 1;
                        if (++nl == 0) break;
                    }
                }
                if (*c&0200) i += 128 * (*c++&0177);
                i += *c++;
                ++tlines;
                /* Проверяем счетчик строк */
                if (nl > 0 && --nl == 0) break;
            }
            seek(f->fsdfile,f->seekhigh,3);
            seek(f->fsdfile,f->seeklow,1);
            while (i)
            {
                j = i < LBUFFER ? i : LBUFFER;
                read(f->fsdfile,cline,j);
                if (write(newf,cline,j) < 0)
                {
                    error(DIAG("DANGER -- WRITE ERROR","Внимание:ошибка записи"));
                    close(newf);
                    return(-1);
                }
                i -= j;
            }
        }
        else
        {
            j = f->fsdnlines;
            if (nl < 0) {
                if (bflag == 0 && ++nl == 0) j = 0;
                bflag = 1;
            }
            else {  
                if (j > nl) j = nl;
                nl -= j;
            }
            k = j;
            while (k) cline[--k] = NEWLINE;
            if (j && write(newf,cline,j) < 0)
            {
                error(DIAG("DANGER -- WRITE ERROR","Внимание: ошибка записи."));
                close(newf);
                return(-1);
            }
            tlines += j;
        }
        f = f->fwdptr;
    }
    close(newf);
    return tlines;
}

/*
 * editfile(file,line,col,mkflg,puflg) -
 * Открыть файл file для редактирования, начиная со строки
 * line и колонки col.
 * Файл открывается в текущем окне.
 * Если файла нет, а mkflg равен 1, то запрашивается
 * разрешение создать файл.
 * Код ответа -1, если файл не открыли и не создали.
 * Если putflg равен 1, файл тут же выводится в окно.
 */
editfile(file,line,col,mkflg,puflg)
char *file;
int line, col, mkflg, puflg;
{
    int i,j;
    register int fn;
    register char *c,*d;
    fn = -1;
    for (i=0; i<MAXFILES;++i) if (openfnames[i] != 0)
    {
        c = file;
        d = openfnames[i];
        while (*(c++) == *d) if (*(d++) == 0)
        {
            fn = i;
            break;
        }
    }
    if (fn < 0)
    {
        fn = open(file,0);  /* Файл существует? */
        if (fn >= 0)
        {
            if (fn >= MAXFILES)
            {
                error(DIAG("Too many files -- editor limit!","Слишком много файлов"));
                close(fn);
                return(0);
            }
            if ((j = checkpriv(fn)) == 0)
            {
                error(DIAG("File read protected.","Файл защищен по чтению."));
                close(fn);
                return(0);
            }
            openwrite[fn] = (j == 2 ? 1 : 0);
            telluser(DIAG("Use: ","Ред: "),0);
            telluser(file,5);
        }
        else if (mkflg)
        {
            telluser(DIAG("hit <use> (ctrl-d) to make: ","Создать(Y/N)?: "),0);
            telluser(file,DIAG(28,27));
            lread1 = -1;
            read1();
            if (lread1 != CCSETFILE&&lread1 != 'Y' &&lread1 != 'y') return(-1);
            /* Находим справочник */
            for (c=d=file; *c; c++)  if (*c == '/') d = c;
            if (d > file)
            {
                *d = '\0';
                i = open(file,0);
            }
            else i = open(".",0);
            if (i < 0)
            {
                error(DIAG("Specified directory does not exist.","Справочник не существует"));
                return(0);
            }
            if (checkpriv(i) != 2)
            {
                error(DIAG("Can't write in:","Не могу записать в:"));
                telluser (file,21);
                return(0);
            }
            close(i);
            if (d > file) *d = '/';
            /* Создаем файл */
            fn = creat(file,FILEMODE); 
            close(fn);
            if ((fn = open(file,0)) < 0)
            {
                error(DIAG("Create failed!","Ошибка создания файла."));
                return(0);
            }
            if (fn >= MAXFILES)
            {
                close(fn);
                error(DIAG("Too many files -- Editor limit!","Слишком много файлов."));
                return(0);
            }
            openwrite[fn] = 1;
            chown(file,userid,groupid);
        }
        else return (-1);
        paraml = 0;   /* so its kept around */
        openfnames[fn] = file;
    }
    /* Выталкиваем буфер, так как здесь долгая операция */
    dumpcbuf();
    switchwksp();
    if (openfsds[fn] == (struct fsd *)0)
        openfsds[fn] = file2fsd(fn);
    curwksp->curfsd = openfsds[fn];
    curfile = curwksp->wfile = fn;
    curwksp->curlno = curwksp->curflno = 0;
    curwksp->ulhclno = line;
    curwksp->ulhccno = col;
    if (puflg)
    {
        putup(0,curport->btext);
        poscursor(0,defplline);
    }
    return(1);
}


/*
 * endit() -
 * закончить работу и записать все
 * ответ = 0, если при записи были ошибки
 */
endit()
{
    register int i, ko = 1;
    for (i = 0; i < MAXFILES; i++)
        if (openfsds[i] && openwrite[i] == EDITED)
            if (savefile(NULL,i) == 0) ko = 0;
    return(ko);
}
