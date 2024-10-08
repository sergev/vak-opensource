/*d* === iclass === read in class messages === 13.02.85   version    5 */

/*  format: */
/*     (number) (message) */

#include "adv_ini.h"

iclass() {
    static char buf[4];
    static int  nmes;

    if( (F3=fopen("adv_class","r")) == NULL )  fatal();
    printf("class messages     :");
    while(getlin()) {
	nmes=nmes+1;
	screen(nmes);
	strncpy(buf,_line,3);  sscanf(buf,"%d",&cval(nmes));
	ctext(nmes)=putmes();
    }
    printf("%8d  %s %6d  %s\n",nmes,"of",plcl,"used");
    fclose(F3);
}
