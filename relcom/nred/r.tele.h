/*
 *      Редактор RED. ИАЭ им. И.В. Курчатова, ОС ДЕМОС
 *      ned.tdefs - определения для работы с терминалом (физ. уровень)
 *      $Header: /home/sergev/Project/vak-opensource/trunk/relcom/nred/RCS/r.tele.h,v 3.1 1986/04/20 23:40:23 alex Exp $
 *      $Log: r.tele.h,v $
 *      Revision 3.1  1986/04/20 23:40:23  alex
 *      *** empty log message ***
 *
 */

/*
 * Таблица клавиатуры ( команда / строка символов )
 */
struct ex_int {int incc;char *excc; }; extern struct ex_int inctab[];
