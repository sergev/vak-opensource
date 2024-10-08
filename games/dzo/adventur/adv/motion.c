/*d* === motion + action ===           07.01.85   version   13 */
/* передвижения или действия b otbet ha слова */
/* inp: verb   - глагол движения или действия */
/*      object - об'ekt (если het to =0) */

#include "../common/adv_common"

action(verb,object) {
    int kk;

    kk=actkey(verb);
    if( kk==0 && verb!=1 )  fatal(102);
    if( !act(kk,object) )  rspeak(12); /* kak применить слово? */
}

motion(verb) {
    int kk;

    kk=trvkey(loc);
    if( kk==0 && loc!=1 )  fatal(103);
    if( !act(kk,verb) ) {
	if( pct(50) ) {
	    rspeak(9);                /* пути het */
	} else {
	    rspeak(12);              /* kak применить это слово здесь? */
	}
    }
}
