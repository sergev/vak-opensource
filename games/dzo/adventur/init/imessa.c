/*d* === imessa === read in messages === 07.01.85   version   13 */

/*  format: */
/*     (number) (message) */

#include "adv_ini.h"

imessa() {
    static char buf[4];
    static int i, nmes;

    if( (F3=fopen("adv_messag","r")) == NULL )  fatal();
    printf("random messages    :");
    while(getlin()) {
	strncpy(buf,_line,3);  sscanf(buf,"%d",&nmes);
	screen(nmes);
	if(rtext(nmes)!=0)  printf("\nсообщение %d уже определено\n",nmes);
	rtext(nmes)=putmes();
    }
    printf("%8d  %s %6d  %s\n",nmes,"of",ranm,"used");
    fclose(F3);
}
