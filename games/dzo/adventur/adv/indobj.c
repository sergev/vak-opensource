/*d* === indobj ===        07.01.85   version   16 */
/* indicate objects at this location */

#include "../common/adv_common"

indobj() {
    int obj, kk, p;
    if( !dark() ) {
	for(obj=1; obj<=objt; ++obj) {
	    if( ptext(obj)!=0 ) {
		kk = prop(obj) & 0377;
		if( kk==inipro )  kk=0;
		kk=pstat(ptext(obj)+kk+1);
		p=place(obj);

		if( p == loc ) {                 /* подвижный об"ekt */
		    if( (prop(obj)&0377) == inipro ) { /* впервые увидел */
			prop(obj)=0     /*        сокровище */;
			tally=tally-1;
		    }
		    mes(kk);

		} else if( p < 0 ) {                      /* не -"- -"- */
		    p = -p;
		    while(fixed(p)!=0) {
			if((fixed(p)&0377)==loc)  mes(kk);
			p=p+1;
		    }
		}
	    }
	}
    }
}
