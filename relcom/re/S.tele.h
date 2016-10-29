/*
 *      Редактор RED.
 *      ned.tdefs - определения для работы с терминалом (физ. уровень)
 *      $Header: S.tele.h,v 4.20 90/05/22 23:21:30 alex Exp $
 *      $Log:   S.tele.h,v $
 * Revision 4.20  90/05/22  23:21:30  alex
 * First rev. red 4.2
 * 
 * Revision 4.10  90/02/05  19:52:13  alex
 * Base revision 4.1
 * 
 * Revision 4.10  90/02/05  19:49:12  alex
 * Base revision 4.1
 * 
 * Revision 4.1  88/03/31  21:59:37  alex
 * Версия 4.1 - обкатка на UTEC, СМ
 * 
 * Revision 3.1.2.3  87/07/03  22:01:13  alex
 * Graph_characters used in margin
 * 
 * Revision 3.1.2.2  87/06/24  22:32:55  alex
 * New readch + gettc + tc table Tested
 * 
 * Revision 3.1.2.1  87/06/19  16:56:18  alex
 * Start revision for red/4
 * 
 * Revision 3.8  87/06/12  18:06:54  alex
 * Исправлены ошибки в режиме vilcase и мелочи для vt-200
 * 
 * Revision 3.8  87/06/09  19:45:19  alex
 * Исправлена работа с tty ~ VT200 и 15-ИЭ-0013
 * 
 * Revision 3.7  87/06/05  23:48:11  alex
 * Добавлен roll для терминалов с sr/sf и отображение
 *  табуляций в режиме -t + всякая мелочь
 * 
 * Revision 3.6  87/06/04  23:41:46  alex
 * Scroll on -sr- or -al/dl- added
 * 
 * Revision 3.5  87/04/02  18:31:51  alex
 * Версия для Utec слита с версией с выделением прописных букв
 * 
 * Revision 3.5  87/04/01  19:13:53  alex
 * Revision for Utec: Cs, Ce, Ct, hR
 * 
 * Revision 3.4  86/09/19  19:48:09  alex
 * Версия для СМ-1700
 * 
 * Revision 3.3  86/08/04  20:50:13  alex
 * Версия  для  Демос-2
 *
 * Revision 3.2  86/07/24  00:22:01  alex
 * Об'единены версии текстов для ЕС и СМ
 * 
 * Revision 3.1  86/04/20  23:40:23  alex
 * Базовая версия для ЕС.
 * 
 * Revision 3.1  86/04/20  23:40:23  alex
 * *** empty log message ***
 * 
 */

/*
 * Таблица клавиатуры ( команда / строка символов )
 */
struct ex_int {int incc;char *excc; }; extern struct ex_int inctab[];
#define HELP_NAME "hR"
#define ISCTRL(c) (((c)&0140) == 0 || (c)==0177)

#ifdef RED_CYRILL
int   Cyin, Cyout;
#define CHA_RUS 016
#define CHA_LAT 017
#define DFL_CYON "\016"
#define DFL_CYOFF "\017"
#define TABLENAME "Ct"
struct tablecod {
                  char ct_out[64];
                  char ct_in[96]; } tablecod
#ifdef TELE_INIT
= {
"@ABCDEFGHIJKLMNOPQRSTUVWXYZ\133\134\135\136\137`abcdefghijklmnopqrstuvwxyz\173\174\175\176\137",
" !\042#$%&'()*+,-./0123456789:;<=>?\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377"
}
#endif
;

#define OUTT(c) (tablecod.ct_out[(c)&077])
#define INTT(c) ((tablecod.ct_in[(c)-040])&0377)

#define iscyrill(c) (((c)&0300) == 0300)
#define isanychr(c) ((c)==' ' || ((c)>='0'&&(c)<='9'))

#endif /* RED_CYRILL */

char *ch_scroll; /* Как установить область роллинга */
short n0scroll, n1scroll; /* Текущая область роллинга - ttext, btext */
short can_scroll;         /* Вариант реализации роллинга */
#define ROLL_CS 01
#define ROLL_IL 02
#define ROLL_SF 03

/*
 * Таблицы графики - текущая и умолчаний
 * Число символов в текущей таблице
 */
extern char *g_table, g0table[];
int gchars;
int cur_atr;
#ifdef TELE_INIT
char g0table[G_END-G_START+2] = "-*!+-+!+!+-+&;<>";
char *g_table;
#endif

/* Размер буфера под termcap */
#ifndef TBUFSIZE
#define TBUFSIZE 2048
#endif
