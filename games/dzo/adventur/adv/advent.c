/*d* === adventure === head modul == 07.01.85   version   25 */

#include "../common/adv_common"

int moves;

main() {
    int word1, word2, type1, type2;
    int oldob, oldobj;

    ini();                                /* initiate data base */

    for(;;) {
	events();                          /* ��������� ������� */
	getans(&word1,&word2,&type1,&type2);
	++moves;
	oldob=0;

	if(        type1 == specwr ) {      /* ����-����� */
	    rspeak(word1);
	} else if( type2 == specwr ) {
	    rspeak(word2);

	} else if( type1 == movewr ) {      /* ������������ */
	    motion(word1);

	} else if( type2 == movewr ) {
	    motion(word2);
	} else {

	    if( type1 == objcwr ) {            /* ��"ekta het ����� */
		if( !here(word1) )  goto L10;
	    }
	    if( type2 == objcwr ) {
		if( !here(word2) ) {
L10   :             rspeak(203);
		    goto L20;
		}
	    }

	    if( type1 == actnwr ) {           /* �������� + */
		if( type2 == objcwr ) {         /*     + ��"ekt */
		    action(word1,word2);
		} else if( oldobj != 0 ) {        /*     + ������ ��"ekt */
		    action(word1,oldobj);
		} else {                          /*     + het ��"ekta */
		    action(word1,255);
		}
	    } else if( type2 == actnwr ) {      /* ��"ekt + �������� */
		action(word2,word1);

	    } else if( type1 == objcwr ) {      /* ��"ekt */
		rspeak(90);                     /*    ��� ������ c ? */
		oldob=word1;
	    }
L20:;
	}
	oldobj=oldob;
    }
}
