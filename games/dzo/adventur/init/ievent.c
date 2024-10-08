/*d* === ievent === read in initial actions === 07.01.85   version    9 */

/* format: */
/*    i  initialisation */
/*    e  events */

#include "adv_ini.h"

unsigned dpoint;

ievent() {
    if( (F3=fopen("adv_events","r")) == NULL )  fatal();
    printf("initial + events   :");
    while(getlin()) {
	if( line(1)=='i' ) {
	    tiniti=putcnd();
	} else if( line(1)=='e' ) {
	    tevent=putcnd();
	    eevent=dpoint;
	} else {
	    printf("\n%s%.10s\n","ошибка: ",&line(p));  fatal();
	}
    }
    printf("%8d   + %6d bytes\n", tevent-tiniti, dpoint-tevent);
    fclose(F3);
}
