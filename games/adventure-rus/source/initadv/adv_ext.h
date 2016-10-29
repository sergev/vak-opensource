/* INCLUDE-FILE ДЛЯ ГЕНЕРАТОРА БАЗЫ ДАННЫХ "ADVENTURE" */

#include <stdio.h>
#include "..\common\advexter.h"

#define  inplen  81
extern int      p;
extern char     _line[inplen], _word[4];            /* buffer: for input line */
extern long *rword;                          /* equivalence (word,rword) */
#define  line(x) _line[(x)-1]
#define  word(x) _word[(x)-1]
extern FILE    *F3;
