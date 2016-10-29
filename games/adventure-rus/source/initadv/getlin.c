/*d* === getlin === 07.01.85   version    6 */
/* get next line from file #3, skip comments */
/* return: true  - ok,  p=0 */
/*         false - end of file */

#include "adv_ext.h"
long *rword=(long *) _word;                  /* equivalence (word,rword) */

getlin() {
    register int c, _getlin;

    _getlin=0;
    do {
	for(p=1; (c=getc(F3))!=EOF && c!='\n'; ++p) {
	    if( c == '\t' ) {
		while( p%8 )  line(p++) = ' ';
		c = ' ';
	    }
	    line(p)=c;
	}
	if( c==EOF ) goto L2;
	for(; p<=inplen; ++p)  line(p)=' ';
    } while( line(1)=='*' || (line(1)==' ' && line(4)==' '));
    _getlin=1;  p=1;
L2: return( _getlin );
}


/* === getwrd === */
/* get next word from line 'line', from position 'p' */
/* return: true  - ok,  word is in 'word(4)', p - points to next */
/*         false - end of file */

int  getwrd() {
    register int i, _getwrd, ch, p0;

    _getwrd=0;
    if( scan() ) {
	_getwrd=1;
	for(i=4; i--; ) word(i+1)=' ';
	p0=p-1;
	while( p<=inplen) {
	    ch=line(p);
	if(!(ch!=' ' && ch!='=' && ch!=')')) break;
	    if( p-p0<=4 )  word(p-p0)=line(p);
	    p=p+1;
	}
    }
    return( _getwrd );
}


/* === scan === */
/* ищет первый пробел в 'line' начиная c позиции 'p' */
/* return: true  - ok,  p - позиция не-пробела */
/*         false - end of file */

scan() {
    int _scan;

beg:
    _scan=0;
    while(p<=inplen  &&  line(p)==' ')  p=p+1;
    if( p<=inplen ) _scan=1;
    if( line(p) == '\\' ) {
	if( !getlin() )  return( 0 );
	goto beg;
    }
    return( _scan );
}
