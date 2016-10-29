/*d* === icave === read in cave description === 07.01.85   version    6 */

/*  format: */
/*  +\ (location number) */
/*  s  (short description) */
/*  l  (long description) */
/*  t  (travel conditions) */

#include "adv_ext.h"

char locused[loct];
#define locdef 1
#define locgo  2

icave() {
    static int lc;

    if( (F3=fopen("advcave","r")) == NULL )  fatal();
    printf("cave locations     :");
    while(getlin()) {
        if( line(1)=='+' ) {
            p=3;
            lc=getobj();
            if( lc<0 ) {
L999:           printf("\n%s%.10s\n","icave: ошибка: ",&line(p));  fatal();
            }
            screen(lc);
            if( locused[lc-1] & locdef ) {
                printf("\nпозиция %d уже определена\n", lc);
            }
            locused[lc-1] |= locdef;
        } else if( line(1)=='s' ) {
            stext(lc)=putmes();
        } else if( line(1)=='l' ) {
            ltext(lc)=putmes();
        } else if( line(1)=='t' ) {
            trvkey(lc)=putcnd();
        } else {
            goto L999;
        }
    }
    printf("%8d  %s %6d  %s\n",lc,"of",loct,"used");
    fclose(F3);
}
