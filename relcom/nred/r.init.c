/*
 *      Редактор RED. ИАЭ им. И.В. Курчатова, ОС ДЕМОС
 *      файл r.init.h - инициализация переменных.
 *      $Header: /home/sergev/Project/vak-opensource/trunk/relcom/nred/RCS/r.init.c,v 3.1 1986/04/20 23:42:03 alex Exp $
 *      $Log: r.init.c,v $
 *      Revision 3.1  1986/04/20 23:42:03  alex
 *      *** empty log message ***
 *
 * Revision 1.4  86/04/13  22:03:40  alex
 */


#define INIT_RE
#include "r.defs.h"

int tabstops[NTABS] = {
  -1, 0,  8, 16, 24, 32, 40, 48, 56, 64,
 72, 80, 88, 96,104,112,120,128,136, BIGTAB,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,};
int lread1 = -1;         /* -1 означает, что символ использован */

/* Параметры для движения по файлу */

int defplline = 10;               /* +LINE       */
int defmiline = 10;               /* -LINE       */
int defplpage =  1;                /* +PAGE      */
int defmipage =  1;                /* -PAGE      */
int deflport  = 30;                /* LEFT PORT  */
int defrport  = 30;                /* RIGHT PORT */
int definsert  = 1;                /* OPEN       */
int defdelete  = 1;                /* CLOSE      */
int defpick    = 1;                /* PICK       */
char deffile[] = "/usr/lib/re.std";   /* Файл подсказок */

/* Инициализации  */
int lcline  = 0;
int icline  = 20; /* Инкремент для расширения */
int clineno = -1;
char fcline = 0;

/* Файл / протокол */
int ttyfile  = -1;
int inputfile = 0;

