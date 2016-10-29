/*
 *      Редактор RED.
 *      файл r.init.h - инициализация переменных.
 *      $Header: r.init.c,v 4.20 90/05/22 23:22:05 alex Exp $
 *      $Log:   r.init.c,v $
 * Revision 4.20  90/05/22  23:22:05  alex
 * First rev. red 4.2
 * 
 * Revision 4.10  90/02/05  19:52:26  alex
 * Base revision 4.1
 * 
 * Revision 4.10  90/02/05  19:49:17  alex
 * Base revision 4.1
 * 
 * Revision 4.1  88/03/31  22:03:30  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 * 
 * Revision 3.1.2.2  87/06/23  18:51:35  alex
 * wYNESENA PEREMENNAQ lread1 I \TO OTLAVENO
 * 
 * Revision 3.1.2.1  87/06/19  17:00:46  alex
 * Start revision for red/4
 * 
 * Revision 3.6  87/06/05  23:50:43  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.5  87/04/02  18:37:53  alex
 * Версия для Utec слита с версией с выделением прописных букв
 * 
 * Revision 3.5  87/04/01  19:16:06  alex
 * Revision for Utec: Cs, Ce, Ct, hR
 * 
 * Revision 3.4  86/09/19  19:54:01  alex
 * Версия для СМ-1700
 * 
 * Revision 3.3  86/08/04  20:52:12  alex
 * Bepqh dk LMNQ/DELNQ 2
 * 
 * Revision 3.2  86/07/24  00:24:59  alex
 * Об'единены версии текстов для ЕС и СМ
 * 
 * Revision 3.1.1.3  86/06/16  22:18:41  alex
 * Первая версия с повтором сеанса (пока грязная)
 * 
 * Revision 3.1.1.1  86/06/05  00:05:06  alex
 * Отлажено_на_модели
 * 
 * Revision 3.1  86/04/20  23:42:03  alex
 * Базовая версия для ЕС.
 * 
 * Revision 3.1  86/04/20  23:42:03  alex
 * *** empty log message ***
 * 
 * Revision 1.4  86/04/13  22:03:40  alex
 */


#define INIT_RE
#include "r.defs.h"

/* Путь поиска файлов инициализации */
# ifndef INIT_PATH
#  define    INIT_PATH "%h/.redi.%t:%h/.redi.dflt:/usr/local/lib/red/init%k.%t:/usr/local/lib/red/init%k.dflt"
# endif
/* Путь поиска файлов справки */
# ifndef HELP_PATH
#  define    HELP_PATH "%h/.redh.%t:%h/.redh.dflt:/usr/local/lib/red/help%k.%t:/usr/local/lib/red/help%k.dflt"
# endif
# ifndef MACRO_PATH
#  define    MACRO_PATH "%h/.redmacro"
# endif

/* Исходные (нерасширенные) имена рабочих файлов */
char *TTYnm = "%wrett%d.%u";
char *TTYtmp= "%wretm%d.%u";
char *RFile = "/usr/tmp/resv.%u";
char *InitPath = INIT_PATH;
char *HelpPath = HELP_PATH;
char *MacroFile= MACRO_PATH;
char *RedKeys;
char *KeyMode = "";
/* Имя файла умолчания для запоминания и считывания макро */

/* Вычисленные имена рабочих файлов */
char *ttynm, *ttytmp, *rfile;

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
char *deffile /* = "/usr/local/lib/red/help.DFLT" */ ;   /* Файл подсказок */
int defedit    = 2;                /* Умолчания для границы сдвига экрана */

/* Инициализации  */
int lcline  = 0;
int icline  = 20; /* Инкремент для расширения */
int clineno = -1;
char fcline = 0;

/* Файл / протокол */
int ttyfile  = -1;
int inputfile = 0;

#ifdef DEMOSES
int dflinsert=4;
#endif /* DEMOSES */

