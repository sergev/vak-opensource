/*d* === функции sostoяния ===        07.01.85   version   19 */

/* dark()       - b данной точке temho */
/* at(object)   - здесь находится этот об'ekt */

#include "../common/adv_common"

int     darkflag1 = 0;

dark() {
    static int lamp, light;

    if( lamp== 0 ) {                       /* инициализация */
	lamp= ( vocab("lamp")   % 1000 );
	light=( vocab("!light") % 1000 );
    }
    if( darkflag1 )             /*@VG*/
	return(darkflag1 = 0);
    return( !at(light) && (!here(lamp) || prop(lamp)==0) );
}


at(object) {
    int _at, p;
    p = place(object);
    _at=0;
    if( p==loc ) {
	_at= 1;
    } else if( p<0 ) {
	p = -p;
	while(fixed(p)!=0) {
	    if( (fixed(p)&0377) == loc ) {
		_at= 1;
		return( _at );
	    }
	    p=p+1;
	}
    }
    return( _at );
}


here(object) {
    return( place(object) == caried || at(object) );
}
