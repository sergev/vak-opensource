/*
 *      Редактор RED.
 *      файл r.cmd.c - основной цикл редактора RED.
 *      $Header: r.cmd.c,v 4.20 90/05/22 23:21:37 alex Exp $
 *      $Log:   r.cmd.c,v $
 * Revision 4.20  90/05/22  23:21:37  alex
 * First rev. red 4.2
 * 
 * Revision 4.10  90/02/05  19:52:16  alex
 * Base revision 4.1
 *
 * Revision 4.10  90/02/05  19:40:37  alex
 * Base revision 4.1
 *
 * Revision 4.1  88/03/31  22:00:36  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 *
 * Revision 3.13  88/03/30  17:27:41  alex
 * Исправлена ошибка в readch. Введены функции для удаления/вставки
 * строк на экране через ChangeScroll/InsertLine-DeleteLine.
 *
 * Revision 3.1.2.6  87/07/09  20:41:20  alex
 * REVISION
 *
 * Revision 3.1.2.5  87/07/03  22:01:30  alex
 * Graph_characters used in margin
 *
 * Revision 3.1.2.4  87/06/29  17:55:11  alex
 * Commans set to 0200-0277
 *
 * Revision 3.1.2.3  87/06/24  22:33:07  alex
 * New readch + gettc + tc table Tested
 *
 * Revision 3.1.2.2  87/06/23  18:48:22  alex
 * wYNESENA PEREMENNAQ lread1 I \TO OTLAVENO
 *
 * Revision 3.1.2.1  87/06/19  16:57:34  alex
 * Start revision for red/4
 *
 * Revision 3.12  87/06/12  18:08:21  alex
 * Исправлены ошибки в режиме vilcase и мелочи для vt-200
 *
 * Revision 3.11  87/06/09  19:45:45  alex
 * Исправлена работа с tty ~ VT200 и 15-ИЭ-0013
 *
 * Revision 3.10  87/06/06  16:48:07  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 *
 * Revision 3.9  87/06/05  23:49:03  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 *
 * Revision 3.8  87/06/04  23:42:15  alex
 * Scroll on -sr- or -al/dl- added
 *
 * Revision 3.7  87/04/02  18:35:04  alex
 * Версия для Utec слита с версией с выделением прописных букв
 *
 * Revision 3.7  87/04/01  19:13:17  alex
 * Revision for Utec: Cs, Ce, Ct, hR
 *
 * Revision 3.6  86/09/19  21:37:19  alex
 *
 *
 * Revision 3.5  86/09/19  19:49:42  alex
 * Версия для СМ-1700
 *
 * Revision 3.4  86/08/04  20:50:21  alex
 * Bepqh dk LMNQ/DELNQ 2
 *
 * Revision 3.3  86/07/24  21:07:54  alex
 * Исправлена ошибка в автоматическом сдвиге
 * (привнесена при об'единении).
 *
 * Revision 3.2  86/07/24  00:22:11  alex
 * Об'единены версии текстов для ЕС и СМ
 *
 * Revision 3.1.1.9  86/07/15  22:11:18  alex
 * RED 3.3/EC.
 *
 * Revision 3.1.1.8  86/06/25  19:40:32  alex
 * Введено автоматическое смещение экрана
 *
 * Revision 3.1.1.7  86/06/20  23:30:50  alex
 * Улучшен "editline"".
 *
 * Revision 3.1.1.6  86/06/19  00:09:42  alex
 * Отлажен на модели повтор сеанса
 *
 * Revision 3.1.1.5  86/06/16  22:15:50  alex
 * Первая версия с повтором сеанса (пока грязная)
 *
 * Revision 3.1.1.3  86/06/06  00:12:49  alex
 * Введена установка ключей
 *
 * Revision 3.1.1.2  86/06/05  18:52:38  alex
 * Отлажено_на_модели
 *
 * Revision 3.1.1.1  86/06/05  00:03:11  alex
 * Отлажено_на_модели
 *
 * Revision 3.1  86/04/20  23:40:29  alex
 * Базовая версия для ЕС.
 *
 * Revision 3.1  86/04/20  23:40:29  alex
 * *** empty log message ***
 *
 * Revision 1.4  86/04/13  21:59:31  alex
 */


#include "r.defs.h"

char use0flg; /* используется для блокировки преобразования первого имени файла при "lcase" и "red - */
int clrsw;  /* 1 - чистить окно параметров */
int csrsw;  /* 1 - показать текущую позицию меткой  */
extern char *ps_inmac;
extern int fullinsert;   /* Режим полной вставки / Return вставляет строку, BS убирает */
#   ifndef DEMOSES
#define NEWEDIT new_info = 1
extern int imodesw;/* 1 - режим вставки  */
#   else
/* int imodesw;/* 1 - режим вставки  */
extern int zerowsflag;
extern editline();
#define NEWEDIT newedit = 1
#   endif
int oldccol;           /* используется для отметки положения курсора */
int oldcline;          /* - - / / - - */
extern char Mac_info[];
extern char *Label;
int rep_count;  /* Пока не используется, хотя и запоминается */
char *Err_str;  /* Текст сообщения об ошибке */
char *settab();

/*
 * mainloop() -
 * основной цикл работы редактора
 */
mainloop()
{
    int i,m,first=1;
    register int j;
    int clsave,ccsave;
    int inf_wfile= -1, inf_iline= -1;
    int inf_cmark = -1, inf_lmark= -1;
    int oldautoi = -1;
    int oldatr;
    int k;
    register int lread1;
    int is_param = 0; /* 1 - у команды есть параметры, 0 - нет */
    char ich[8], *cp;
    /* Для команд с тремя вариантами аргументов */
    int (*lnfun)(),(*spfun)();
    int openlines(), openspaces(), closelines(),closespaces(),picklines(),
    pickspaces();
    /* === */
    extern int templ[4];
    struct viewport *oport;
/*
 * Определения для работы режима указания области
 */
#define NO_SELECT() if ( sel_flag ) goto no_select;else
#define END_SELECT() if ( sel_flag ) goto make_select;else;
    NEWEDIT;
    new_info = 1;
    /*
     * Обработка одного символа или команды
     * ====================================
     */
    if (cursorline== 0) oldcline = 1;
    if (cursorcol == 0) oldccol  = 1;
#ifndef lint
    goto funcdone;
#endif
    FOREVER
        {
        is_param = 0;
        lread1=readch();
#if 0
        if ( lread1 == CCHOME )
                {
                        dumpfsd("DUMP");
                        continue;
                }
#endif
        if ( lread1 < 0 ) goto funcdone; /* Для перевыдачи информации */
        if (errsw)
        {
            errsw = 0;
            clrsw = 1;
            unread1(lread1);
            goto errclear;
        }
        if (lread1 == CCQUIT)
        {
            putline(1);
            if (endit() == 0) goto funcdone;
            return;
        }
        /* Обработка RETURN  */
        if (lread1 == CCRETURN )
        {
#ifdef DEMOSES
            if ( cursorcol < 0 && curwksp->ulhccno != 0 )
                lread1 = CCLPORT;
            else if ( cursorcol > curport->rtext)
                lread1 = CCRPORT;
            else if ( cursorline < 0 )
                lread1 = CCMILINE;
            else if ( cursorline > curport->btext)
                lread1 = CCPLLINE;
            else
#endif
            {
            putline(0);
            if ( fullinsert && imodesw ) {NO_SELECT();splitline(ABS_LIN(cursorline),ABS_COL(cursorcol));};
            if ( !fix_screen )
            {
                      if ( !on_autoi )
                            j = 0;
                       else j = autoindent;
                            if ( (i = REL_COL(j)) < 0 || i > curport->rtext) movep(i);
            }
            if ( cursorline == curport->btext)
                movew( defreturn?defreturn:defplline,1);
            movecursor(lread1);
            lread1= -1;
            goto errclear;
            }
        }
        /*
         * Редактирование в строке. Обычные символы на ЕС
         * могут прийти из макро.
         */
        if (! ISACMD(lread1) )
        {
                NO_SELECT();
                if ( lineop(lread1)) goto contin;
                goto newnumber;
        }
        /* Если граница поля */
        if (cursorcol > curport->rtext) poscursor(curport->rtext,cursorline);
        switch (lread1)
        {
        case CCMOVELEFT:
        case CCMOVERIGHT:
        case CCTAB:
        case CCBACKTAB:
        case CCHOME:
        case CCMOVEUP:
        case CCMOVEDOWN:
             /*
              * Если команда перемещения
              */
              i=movecursor(fix_screen?lread1:lread1|MOVE_NOFIX);
              if(i != -1 ) unread1(i);
                      else
                      {
                               if ( lread1 > VMOTCODE+CC )
                                       autoindent = ABS_COL(cursorcol);
                               goto newnumber;
                      }
                      /* Получили новый код операции */
                      goto contin;
        case CCBACKSPACE:
              NO_SELECT();
              if ( fullinsert && (i = ABS_LIN(cursorline)) > 0 && ABS_COL(cursorcol) == 0 )
              {
                      /* Курсор в начало пред. строки */
                      getline(  --i );
                      cgoto( i, (j=ncline-1),-1,0);
                      combineline(i,j);
                      goto newnumber;
              }
        case CCCTRLQUOTE:
        case CCDELCH:
#ifdef DEMOSES
        case CCINSERT:
#endif
                NO_SELECT();
                if ( lineop(lread1)) goto contin;
                goto newnumber;
        case CCBEGIN:
        case CCEND:
        case CCWORDLEFT:
        case CCWORDRIGHT:
        case CCPSEARCH:
                linecset(lread1,(char *)0);
                goto newnumber;
        case CCENTER:
            if(need_box) drawport(curport,DRAW_HORI);
            goto gotarg;
        case CCLPORT:
            movep(- deflport);
            goto newnumber;
        case CCHELP:
            helpview(); NEWEDIT;
            goto funcdone;
        case CCUNDO:
            if (undoedit(0) < 0 )
                    error(DIAG("Can not undo changes","Не могу вернуть изменения"));
            goto funcdone;
        case CCSETFILE:
            NO_SELECT();
            switchfile(1); NEWEDIT;
            goto funcdone;
        case CCSELECT:
            do_select();
            goto funcdone;
        case CCCHPORT:
            NO_SELECT();
            chgport(-1);NEWEDIT;
            goto newnumber;
        case CCLINESPLIT:
                NO_SELECT();
                splitline(ABS_LIN(cursorline),ABS_COL(cursorcol));
                goto funcdone;
        case CCLINEJOIN:
                NO_SELECT();
                combineline(ABS_LIN(cursorline),ABS_COL(cursorcol));
                goto funcdone;
        case CCOPEN:
            END_SELECT();
            if (openwrite[curfile]==0)  goto nowriterr;
            openlines(curwksp->ulhclno + cursorline,definsert);
            goto newnumber;
        case CCMISRCH:
            search(-1);
            goto funcdone;
        case CCCLOSE:
            END_SELECT();
            if (openwrite[curfile]==0)
                goto nowriterr;
            closelines(curwksp->ulhclno + cursorline, defdelete);
            goto newnumber;
        case CCPUT:
            NO_SELECT();
            if (openwrite[curfile]==0)
                goto nowriterr;
            if (pickbuf->nrows == 0) goto nopickerr;
            put(pickbuf,curwksp->ulhclno+cursorline,
            curwksp->ulhccno+cursorcol);
            goto newnumber;
        case CCPICK:
            END_SELECT();
            picklines(curwksp->ulhclno + cursorline, defpick);
            goto newnumber;
#ifndef DEMOSES
        case CCINSMODE:
            imodesw = 1 - imodesw;  /* change it */
            goto funcdone;
#endif
        case CCGOTO:
            gtfcn(0);
            goto newnumber;
        case CCMIPAGE:
            movew(- defmipage * (1+curport->btext),0);
            goto newnumber;
        case CCPLSRCH:
            search(1);
            goto funcdone;
        case CCRPORT:
            movep(defrport);
            goto newnumber;
        case CCPLLINE:
            movew(PL1MOVEW,fix_screen);
            goto newnumber;
        case CCSAVEFILE:
            savefile(NULL,curfile);
            goto funcdone;
        case CCMILINE:
            movew(MI1MOVEW,fix_screen);
            goto newnumber;
        case CCDEFMACRO:
            MacCreate((char *)0);
            goto funcdone;
        case CCREADMACRO:
            MacRestore((char *)0);
            goto funcdone;
        case CCSAVEMACRO:
            MacSave((char *)0,1);
            goto funcdone;
        case CCDOCMD:
            goto notstrerr;
        case CCPLPAGE:
            movew(defplpage * (1+curport->btext),0);
            goto newnumber;
        case CCMAKEPORT:
            NO_SELECT();
            if(need_box) drawport(curport,DRAW_HORI);
            makeport(NULL); NEWEDIT;
            goto funcdone;
        case CCZOOMPORT:
            zoom_w(1);
            goto funcdone;
        case CCTABS:
            Err_str = settab(curwksp->ulhccno + cursorcol,(char *)0);
            goto funcdone;
        case CCRESCREEN:
            rescreen(0);
            goto funcdone;
            /*    case CCMOVELEFT:        */
            /*    case CCTAB:             */
            /*    case CCMOVEDOWN:        */
            /*    case CCHOME:            */
            /*    case CCRETURN:  */
            /*    case CCMOVEUP:  */
            /*    case CCINSERT:  */
        default:
            goto badkeyerr;
        }
/* Это - безобразие, как и вся данная функция! Но пока так! */
make_select:
        paramc0 = cursorcol;
        paramr0 = cursorline;
        if ( !set_param(sel_file,sel_lin,sel_col) ) goto funcdone;
        do_select();
        new_info = 1;
        goto yesarg;
        /* Повтор ввода аргумента */
reparg:
        lread1=readch();
        if ( lread1 < 0 ) goto funcdone; /* Пустая команда */
        if(ISACMD(lread1)) goto yesarg;
        else goto noargerr;
        /*
         * Дай аргумент!
         */
gotarg:
        lread1=param(0);
yesarg:
        is_param = 1;
        if (lread1 == CCQUIT )
        {
            if (paraml>0 && (dechars(paramv,paraml),*paramv) == 'a')
            {
                if (*(paramv+1) != 'd') return;
                cleanup();
                inputfile = -1; /* to force a dump */
                fatal("ABORTED");
            }
            if (endit() == 0) goto funcdone;
            return;
        }
        switch (lread1)
        {
        case CCENTER:
            if ( sel_flag ) do_select();
            goto funcdone;
        case CCLPORT:
            if (paramtype < 0)  goto notstrerr;
            if ( paramtype == 0 ) i = curport->rtext-cursorcol ;
            else if (s2i(paramv,&i)) goto notinterr;
            movep(-i);
            goto funcdone;
        case CCHELP:
            helpview(); NEWEDIT;
            goto funcdone;
        case CCSETFILE:
            NO_SELECT();
            if (paramtype < 0)  goto notstrerr;
            if (paramv == 0)
              {
                      switchfile(-1);
                      goto funcdone;
              }
            if ( use0flg || !inputfile)
                dechars(paramv,paraml);
            use0flg=1;NEWEDIT;
            editfile(paramv,0,0,1,1);
            goto funcdone;
        case CCCHPORT:
            NO_SELECT();
            if (paramtype <= 0)  goto notstrerr;
            if (s2i(paramv,&i)) goto notinterr;
            if (i <= 0) goto notposerr;
            chgport(i-1); NEWEDIT;
            goto funcdone;
        case CCOPEN:
            NO_SELECT();
            if (openwrite[curfile]==0)  goto nowriterr;
            if (paramtype == 0) {
                splitline(curwksp->ulhclno + paramr0,
                paramc0 + curwksp->ulhccno);
                goto funcdone;
            }
            else {
                lnfun = openlines;
                spfun = openspaces;
                goto spdir;
            };
        case CCMISRCH:
        case CCPLSRCH:
            if (paramtype <= 0)  goto notstrerr;
            if (paramv == 0) goto noargerr;
            if (searchkey) zFree((char *)searchkey);
            searchkey = paramv;
            paraml = 0;
            search(lread1==CCPLSRCH?1:-1);
            goto funcdone;
        case CCCLOSE:
            NO_SELECT();
            if (openwrite[curfile]==0)  goto nowriterr;
            if (paramtype == 0) combineline(curwksp->ulhclno + paramr0,
            paramc0 + curwksp->ulhccno);
            else {
                if(paramtype > 0 && paramv && paramv[0]=='>')
                {
                    msrbuf(deletebuf,paramv+1,0);
                    goto funcdone;
                }
                lnfun = closelines;
                spfun = closespaces;
                goto spdir;
            }
            goto funcdone;
        case CCPUT:
            NO_SELECT();
            if (paramtype >  0 && paramv && paramv[0]=='$' )
            {
                if (msrbuf(pickbuf,paramv+1,1))goto errclear;
                goto funcdone;
            }
            if (paramtype != 0)  goto notstrerr;
            if (openwrite[curfile]==0)
                goto nowriterr;
            if (deletebuf->nrows == 0) goto nodelerr;
            put(deletebuf,curwksp->ulhclno+cursorline,
            curwksp->ulhccno+cursorcol);
            goto funcdone;
        case CCTAB:
        case CCMOVELEFT:
        case CCMOVEDOWN:
        case CCHOME:
        case CCMOVEUP:
        case CCMOVERIGHT:
        case CCBACKTAB:
            if (s2i(paramv,&i)) goto notinterr;
            if (i <= 0) goto notposerr;
            m = ((lread1<=BT) ? lread1:0);
            while (--i >= 0) movecursor(m);
            goto funcdone;
        case CCRETURN:
            if(paramtype <=0|| !paramv) goto notimperr;
            dechars(paramv,paraml);
            /* Сначала посмотрим, нет ли у нас команды */
            lread1 = Param_cmd();
            if ( lread1 >= 0 )
            {
                if ( lread1 >= CCMAC && lread1 <= CCEMAC )
                {
                        do_macro(lread1);
                        while (rep_count-- > 1 )
                              do_macro(lread1);
                        goto funcdone;
                }
                if ( paramtype == 2 )
                {
                        unread1(lread1);
                        clrsw = 1;
                        goto contin;
                }
                goto yesarg;
            }
            if ( paramv[0] == 's' && paramv[1] == 'e' && paramv[2] == 't')
            {
                      do_set(paramv[3]? paramv+3: NULL);
                      goto funcdone;
            }
            switch (paramv[0])
            {
            case '!':
                run_shell(paramv+1);
                goto funcdone;
            case '-': case '+':
                lread1 = -1;
                new_info = 1;
                if ( !(lread1=setkeys(paramv))) goto noargerr;
                if(lread1 != -1) rescreen(0);
                goto funcdone;
            case '>':
                msvtag(paramv+1);
                goto funcdone;
            case '$':
                if(mdeftag(paramv+1)){
                    lread1= -1;
                    goto reparg;
                }
                else goto funcdone;
            case 'k':
                defkey();
                goto funcdone;
            case 's':
                tostop();
            case 'r':
                rescreen(0); goto funcdone;
            case 'd':
                if(paramv[1]==' ') defmac(&paramv[2]);
                goto funcdone;
            case 'q':
                lread1=CCQUIT;
                if(paramv[1]=='a') {
                    return;
                }
                unread1(lread1);
                goto contin;
            default:
                goto noargerr;
            }
        case CCPICK:
            NO_SELECT();
            if (paramtype == 0) goto notimperr;
            if (paramtype > 0 && paramv && paramv[0]=='>')
            {
                msrbuf(pickbuf,paramv+1,0);
                goto funcdone;
            }
            lnfun = picklines;
            spfun = pickspaces;
            goto spdir;
#ifndef DEMOSES
        case CCINSMODE:
            imodesw = 1 - imodesw;  /* Щелкнем!! */
#else
        case CCINSERT:
            if (paramtype > 0)
            {
                if (s2i(paramv,&i)) goto notinterr;
                dflinsert = i;
            }
            else dflinsert = 1;
#endif
            goto funcdone;
        case CCBEGIN:
        case CCEND:
        case CCWORDLEFT:
        case CCWORDRIGHT:
        case CCPSEARCH:
                linecset(lread1,paramtype==1?paramv:(char *)0);
                goto funcdone;
        case CCGOTO:
            if (paramtype == 0) gtfcn(nlines[curfile]);
            else if (paramtype > 0)
            {
                if(paramv && paramv[0]=='$') {
                    mgotag(paramv+1);
                    goto funcdone;
                }
                if (s2i(paramv,&i)) goto notinterr;
                gtfcn(i-1);
            }
            else goto noargerr;
            goto funcdone;
        case CCMIPAGE:
            if (paramtype <= 0)
            {
                movew(cursorline - curport->btext,0);
                goto funcdone;
            }
            if (s2i(paramv,&i)) goto notinterr;
            movew(- i * (1 + curport->btext),0);
            goto funcdone;
        case CCRPORT:
            if (paramtype < 0)  goto notstrerr;
            if ( paramtype == 0 ) i = (cursorcol - curport->ltext);
            else if (s2i(paramv,&i)) goto notinterr;
            movep(i);
            goto funcdone;
        case CCPLLINE:
            if (paramtype < 0)  goto notstrerr;
            else if (paramtype == 0)  movew(cursorline,1);
            else if (paramtype > 0)
            {
                if (s2i(paramv,&i)) goto notinterr;
                movew(i,1);
            }
            goto funcdone;
        case CCDELCH:
            NO_SELECT();
            lineop(CCTRUNCATE);
            goto funcdone;
        case CCSAVEMACRO:
            if (paramtype <  0)  goto notstrerr;
            if (paramv == 0) MacSave(NULL,2);
            else MacSave(paramv,0);
            goto funcdone;
        case CCREADMACRO:
            if (paramtype <=  0)  goto notstrerr;
            if (paramv == 0) goto noargerr;
            MacRestore(paramv);
            goto funcdone;
        case CCDEFMACRO:
            if (paramtype <=  0)  goto notstrerr;
            if (paramv == 0) goto noargerr;
            MacCreate(paramv);
            goto funcdone;
        case CCSAVEFILE:
            if (paramtype <=  0)  goto notstrerr;
            if (paramv == 0) goto noargerr;
            dechars(paramv,paraml);
            savefile(paramv,curfile);
            goto funcdone;
        case CCMILINE:
            if (paramtype < 0)  goto notstrerr;
            else if (paramtype == 0)  movew(cursorline - curport->btext,1);
            else if (paramtype > 0)
            {
                if (s2i(paramv,&i)) goto notinterr;
                movew(-i,1);
            }
            goto funcdone;
        case CCDOCMD:
            NO_SELECT();
            if(paramtype<=0) goto notstrerr;
            dechars(paramv,paraml);
            if (openwrite[curfile] == 0) goto nowriterr;
            callexec();
            goto funcdone;
        case CCPLPAGE:
            if (paramtype <= 0)
            {
                movew(cursorline,0);
                goto funcdone;
            }
            if (s2i(paramv,&i)) goto notinterr;
            movew(i * (1 + curport->btext),1);
            goto funcdone;
        case CCMAKEPORT:
            NO_SELECT();
            if (paramtype == 0)  removeport();
            else if (paramtype <  0)  goto notstrerr;
            else {
                dechars(paramv,paraml);
                makeport(paramv);
            }
            NEWEDIT;
            goto funcdone;
        case CCTABS:
            if ( paramtype == 0 ) Err_str = settab( curwksp->ulhccno + cursorcol,"");
            else
                Err_str = settab(curwksp->ulhccno + cursorcol,paramv);
            goto funcdone;
        case CCRESCREEN:
            rescreen(0);
            goto funcdone;
        case CCSELECT:
            do_select();
            goto funcdone;
        default:
            goto badkeyerr;
        }
spdir:
        if (paramtype > 0)
        {
            if(paramv[0] == '$')
            {
                if(mdeftag(paramv+1)) goto spdir;
                else goto funcdone;
            }
            if (s2i(paramv,&i)) goto notinterr;
            if (i <= 0) goto notposerr;
            (*lnfun)(curwksp->ulhclno + cursorline, i);
        }
        else
        {
            if (paramc1 == paramc0)
            {
                (*lnfun)(curwksp->ulhclno+paramr0,
                (paramr1-paramr0)+1);
            }
            else (*spfun)(curwksp->ulhclno + paramr0,
            curwksp->ulhccno + paramc0,
            (paramc1-paramc0),
            (paramr1-paramr0) + 1);
        }
        goto funcdone;
no_select:
        error(DIAG("Illegal op in select mode","В режиме выбора недопустимо") );
        sel_flag++;
        if ( sel_flag > 4 ) sel_flag = 0;
        new_info = 1;
        goto funcdone;
badkeyerr:
        error(DIAG("Illegal key or unnown macro","Неизвестная клавиша или макро"));
        goto funcdone;
notstrerr:
        error(DIAG("Argument must be a string.","Аргумент должен быть строкой"));
        goto funcdone;
noargerr:
        error(DIAG("Invalid argument.","Плохой аргумент"));
        goto funcdone;
notinterr:
        error(DIAG("Argument must be numerik.","Аргумент должен быть числом"));
        goto funcdone;
notposerr:
        error(DIAG("Argument must be positive.","Аргумент должен быть положительным"));
        goto funcdone;
nopickerr:
        error(DIAG("Nothing in the pick buffer.","Буфер вставок пуст"));
        goto funcdone;
nodelerr:
        error (DIAG("Nothing in the close buffer.","Буфер убранных строк пуст"));
        goto funcdone;
notimperr:
        error(DIAG("Feature not implemented yet.","Еще не определено."));
        goto funcdone;
nowriterr:
        error(DIAG("You cannot modify this file!","Вы не можете изменить этот файл."));
        goto funcdone;
funcdone:
        if ( Err_str )
        {
                error(Err_str);
                Err_str = (char *)0;
        }
        clrsw = 1;
newnumber:
        lread1 = -1;        /* signify char read was used */
errclear:
        oport = curport;
        k = cursorline;
#ifdef DEMOSES
        if ( newedit ) {
                newedit = 0;
                if ( openwrite[curfile] ) editwind(curport,editline,zerowsflag);
                else editwind((struct viewport *)0,(int (*)())0,0);
                }
#endif
/*
 * Алгоритм для перерисования рамок и номера -
 * если только что был сдвиг, то блокируем
 */
        if((need_box && need_box++ !=1) || (showtabs && chg_tabs)) drawport(curport,need_box? DRAW_HORI:DRAW_TABS);
        if(need_box != 2 ) {
            j = cursorcol;
            switchport(&paramport);
            paramport.redit = PARAMRINFO;   /* !!! */
            if (clrsw || new_info)
            {
                if (!errsw && !first)
                {
                    poscursor(0,0);
                    setatr(A_INFO);
                    /* if ( !putcha(COCLLIN ))        */
                           {
                              putblanks(PARAMREDIT);
                           }
                }
                ccsave = PARAMREDIT+2/* cursorcol */;
                clsave = 0 /* cursorline */;
                if (oport->wksp->wfile&&(oport->wksp->wfile != inf_wfile||new_info))
                {
                    poscursor(PARAMREDIT+8,0);
                    inf_wfile = oport->wksp->wfile;
                    i = strlen(openfnames[inf_wfile]) - (PARAMRINFO - PARAMREDIT - 17);
                    info(i>0?(DIAG(" file..."," файл...")):(DIAG(" file "," файл ") ),PARAMRINFO);
                    info(openfnames[inf_wfile] + (i>0? i : 0),PARAMRINFO);
                    setatr(A_INFO);
                    putblanks(PARAMRINFO-cursorcol);
                    infmesg(Mac_info,PARAMRMAC,A_ERROR);
                }
                first=0;
            }
            i = oport->wksp->ulhclno + k + 1; /* Рисуем номер строки */
            if ( i != inf_iline || new_info) {
                inf_iline = i;
                poscursor(ccsave,clsave);
                cp = ich + 6;
                *--cp = '\0';
                do
                    (*--cp = '0' + (i % 10));
                while (i = i/10);
                info(cp,PARAMRINFO);
                *cp = '\0';
                while (cp != ich) *--cp = ' ';
            info(ich,PARAMRINFO);
            }
            setatr(A_NORM);
            switchport(oport);
            paramport.redit = PARAMREDIT;
            i = REL_COL(autoindent);
            if (new_info ||
                 ((on_autoi || inf_lmark != -1)&&oldautoi != i)
                ) {
                switchport(&wholescreen);
                if(inf_lmark != -1) {
                        poscursor(inf_cmark,inf_lmark);
                        putch(B_MARG(inf_cmark - oport->ltext+oport->wksp->ulhccno),A_MARG);
                        inf_lmark = -1;
                }
                if(on_autoi&&oport->bmarg !=oport->ttext+oport->btext) {
                            inf_cmark = oport->ltext+i;
                            inf_lmark = oport->bmarg;
                            if ( inf_cmark < 0 ) inf_cmark = 0;
                            if ( inf_cmark > oport->rmarg )
                                      inf_cmark = oport->bmarg;
                            poscursor(inf_cmark,inf_lmark);
                            putcha(COOPEN);
                            putch(AMCH,A_INFO);
                            putcha(COCLOSE);
                            }
                switchport(oport);
                oldautoi = i;
                }
            poscursor(j,k);
            if (csrsw && !ps_inmac && !inputfile )
            {
                putch(COCURS,1);
                poscursor(j,k);
                dumpcbuf(1);
                sleep(1);
                out_win(k,k,j,j);
                poscursor(j,k);
            }
#ifndef DEMOSES
            if ((new_info || clrsw) && !errsw)
            {
#ifdef RED_CYRILL
                extern Cyin;
#else
#define Cyin 0
#endif
                char mkeys[3];
                if ( sel_flag ) {
                 char buf[72];
                 sprintf(buf,DIAG(" ** Select mode from line %d col %d","Режим выбора от стр. %d кол. %d"),sel_lin+1,sel_col+1);
                 telluser(buf,0);
                } else
                if ( Label ) {
                    telluser(Label,0);
                    Label = (char *)0;
                    clrsw = 2;
                } else if ( imodesw )
                telluser(DIAG("     ***** i n s e r t m o d e *****","  * * * * режим вставки * * * * "),0);
                mkeys[0] = fullinsert?'I':' ';
                mkeys[1] = Cyin?'R':' ';
                mkeys[2] = 0;
                infmesg(mkeys,PARAMRLANG,A_OUT);
            }
#endif
        csrsw =  new_info = 0;
        if ( clrsw >0 ) clrsw--;
        }
contin:
        ;
    }
}

/*
 * callexec() -
 * Выполнить команду "exec"
 * Ответ 1, если не было ошибок
 */
#define NARGS 20
callexec()
{
    register int i;
    char **execargs;
    register char *cp;
    int j,k,m,pipef[2];
    char pwbuf[100];
    /*
     * 1. Разбираем размер области.
     */
    i = curwksp->ulhclno + cursorline;
    m = 1;
    cp = paramv;
    if (*cp == '-' || (*cp >= '0' && *cp <= '9')) {
        cp = s2i(paramv,&m);
        if (cp == 0) goto noargerr;
    }
    m = -m;           /* По умолчанию - 1 параграф */
    if (*cp == 'l') {
        cp++;
        m = -m;
    }  /* nl ==  -n */
    /*
     * 2. Готовим строку аргументов.
     */
    execargs = (char **)salloc(NARGS*(sizeof (char *)));
    if ( SelectArgs(cp, execargs, NARGS) < 0 ) goto noargerr;
    /*
     * 3. Запускаем команду через pipe
     * (red) | (команда;red)
     * Вторая копия red занимается тем, что дочитывает
     * остаток информации из трубы.
     */
    if (pipe(pipef) == -1) goto nopiperr;
    if ((j = fork()) == -1) goto nopiperr;
    if (j == 0) {                   /* команда;red */
        close(0);               /* Замыкаем в станд. ввод */
        dup(pipef[0]);
        close(1);               /* Вывод в рабочий файл */
        open(tmpname,1);
        seek(1,tempfh,3);
        seek(1,tempfl,1);
        j = 2;
    /* Закрываем все, что осталось открыто */
        while ((k = dup(1)) >= 0) if (k > j) j = k;
        while (j >= 2) close(j--);
        if ((i = fork()) == -1) goto nopiperr;
        if (i != 0) {           /* ;red   */
            while (wait(&m) != i);  /* Ждем, затем читаем */
            while (read(0,pwbuf,100)); /* Пока не надоест */
            exit(m >> 8);        /* И возвр. статус      */
        }
        execr(execargs);
        /* exit теперь в EXECR, -1 если ошибка */
    }
    /* Отец: */
    telluser(DIAG("Executing ...","Выполнение ..."),0);
    dumpcbuf(1);
    zFree((char *)execargs);
    doreplace(i,m,j,pipef);
    return(1);
nopiperr:
    error(DIAG("Can not fork or write pipe.","Ошибка записи"));
    return(0);
noargerr:
    error(DIAG("Invalid argument.","ошибка в аргументе."));
    return(0);
}


/*
 * Высветить HELP-файл
 */
helpview()
{
    char *path;
    register char *cp;
    extern char *next_path(), *StrCalc();
    extern char *HelpPath;
    struct viewport *oport;
    static h_line, h_col;
    int lr;
    int cc, cl;
    int i = -1;
    oport = curport;
    cc = cursorcol;
    cl = cursorline;
    putcha(COERASE);
    switchport( &helpport);
    if ( !deffile )
    {
        path = HelpPath;
        while ( path != NULL && (cp = next_path(&path)))
        {
            cp = StrCalc(cp);
            if ((i = editfile(cp,0,0,0,1)) <= 0)
            {
                zFree(cp);
                continue;
            }
            deffile = cp;
            break;
        }
    }
    else
        i = editfile(deffile,h_line,h_col,0,1);
    if ( i < 0 )
    {
        error(DIAG("Help file gone: notify sys admin.","Нет файла инструкций."));
        sleep(2);
        goto retn;
    }
    infmesg(DIAG("HELP: ? - begin, n - NEXT, p - PREV, <Ent> - exit, ","HELP: ? - НАЧАЛО, n - СЛЕД, p - ПРЕД,  <Enter> - возврат"),
        0,A_ERROR);
    while ( lr = readch() )
    {
        switch ( lr )   {
        case CCGOTO:
        case '?':
              cgoto(0,defplline,0,1);
              break;
        case 'n':
        case 'N':
        case CCPLPAGE:
            movew(L_HELP);
            break;
        case 'P':
        case 'p':
        case CCMIPAGE:
            movew(-L_HELP,0);
            break;
        case '+':
        case CCMOVEDOWN:
            movew(1);
            break;
        case CCPLLINE:
            movew(PL1MOVEW,0);
            break;
        case '-':
        case CCMOVEUP:
             movew(-1,0);
             break;
        case CCMILINE:
            movew(MI1MOVEW,0);
            break;
        default:
            h_line = ABS_LIN(cursorline);
            h_col  = ABS_COL(cursorcol);
            goto zFree_help;
        }
    }
zFree_help:
        drop_wksp(curport);
retn:
    switchport(oport);
    poscursor(cc,cl);
    rescreen(0);
    return;
}

/*
 * do_select() - начать или кончить режим выбора области
 */
do_select()
{
    new_info = 1;
    if ( sel_flag ) {
        sel_flag = 0;
        out_mark(sel_lin,sel_col,0);
        return;
    }
    sel_flag = 1;
    sel_lin  = ABS_LIN(cursorline);
    sel_col  = ABS_COL(cursorcol);
    sel_file = curfile;
    sel_wspace = curwksp;
    msvtag("0");
    out_mark(sel_lin,sel_col,1);
    return;
}


