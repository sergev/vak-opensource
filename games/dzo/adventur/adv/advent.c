/*d* === adventure === head modul == 07.01.85   version   25 */

#include "../common/adv_common"

int moves;

main() {
    int word1, word2, type1, type2;
    int oldob, oldobj;

    ini();                                /* initiate data base */

    for(;;) {
	events();                          /* случайные события */
	getans(&word1,&word2,&type1,&type2);
	++moves;
	oldob=0;

	if(        type1 == specwr ) {      /* спец-слово */
	    rspeak(word1);
	} else if( type2 == specwr ) {
	    rspeak(word2);

	} else if( type1 == movewr ) {      /* передвижения */
	    motion(word1);

	} else if( type2 == movewr ) {
	    motion(word2);
	} else {

	    if( type1 == objcwr ) {            /* об"ekta het рядом */
		if( !here(word1) )  goto L10;
	    }
	    if( type2 == objcwr ) {
		if( !here(word2) ) {
L10   :             rspeak(203);
		    goto L20;
		}
	    }

	    if( type1 == actnwr ) {           /* действие + */
		if( type2 == objcwr ) {         /*     + об"ekt */
		    action(word1,word2);
		} else if( oldobj != 0 ) {        /*     + старый об"ekt */
		    action(word1,oldobj);
		} else {                          /*     + het об"ekta */
		    action(word1,255);
		}
	    } else if( type2 == actnwr ) {      /* об"ekt + действие */
		action(word2,word1);

	    } else if( type1 == objcwr ) {      /* об"ekt */
		rspeak(90);                     /*    что делать c ? */
		oldob=word1;
	    }
L20:;
	}
	oldobj=oldob;
    }
}
