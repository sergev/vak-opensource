/*d* === score === выдача счета игры        07.01.85   version    9 */

#include "..\common\advexter.h"

score() {
    int scor, maxsco, obj, i;

    scor=maxsco=0;
    for(obj=treasr; obj<=objt; ++obj) {
	if( place(obj)>0 ) {
	    maxsco=maxsco+20;
	    if( place(obj)==3 && prop(obj)==0 ) {    /* сокровище b доме */
		scor=scor+20;
	    } else if( (prop(obj)&0377)!=inipro ) {   /* сокровище видел */
		scor=scor+5;
	    }
	}
    }
    rspeak(32);
    mscore(scor,maxsco);

    obj = 1;
    for(i=1; i<=plcl; ++i)  if( cval(i) && scor>=cval(i) ) obj = i;
    mes(ctext(obj));
}
