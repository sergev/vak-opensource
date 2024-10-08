/*  === getobj === input of object */
/* inp: line(p) - beginning of word in form: */
/*                bird - object name */
/*                **** - zero object (kod 255) */
/*                123  - object #123 */
/* out: getobj = object number, line(p) - next symbol after object */
/*             = -1 - no more words */

#include "adv_ini.h"

int  getobj() {
    static char buf[5];
    int _getobj;

    if( getwrd() ) {
	if( word(1)>='0' && word(1)<='9' ) {      /* число */
	    strncpy(buf,_word,4);  sscanf(buf,"%d",&_getobj);
	} else if(word(1)=='*') {                       /* **** */
	    _getobj=255;
	} else {                                        /* bird */
	    _getobj=vocab(_word);
	    if( _getobj>=0 ) {
		_getobj = _getobj%1000;
	    } else {
		printf("\n%s%.4s\n","unknown word=",_word);  fatal();
	    }
	}

    } else {
	_getobj = -1;
    }
    return( _getobj );
}
