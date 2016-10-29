/* === iactio === read actions description === 13.02.85   version   10 */

/*  format: */
/*  +\ (action names-synonyms) */
/*  a  (actions conditions) */

#include "adv_ext.h"

iactio() {
    static int aw;

    if( (F3=fopen("advactio","r")) == NULL )  fatal();
    printf("actions words      :");
    while(getlin()) {

        if( line(1)=='+' ) {
            aw=aw+1;
            p=4                                /* read list of words */;
            while(getwrd()) {
                if( vocab(_word) >= 0 ) {
                    printf("\n%s%.4s\n","дважды определено: ",_word);
                    fatal();
                }
                nvoc=nvoc+1;
                ktab(nvoc)=aw+(actnwr*1000);  atab(nvoc) = *rword;
            }
            screen(aw);

        } else if( line(1)=='a' ) {
            actkey(aw)=putcnd();

        } else {
            printf("\n%s%.10s\n","ошибка: ",&line(p));  fatal();
        }

    }
    printf("%8d  %s %6d  %s\n",aw,"of",actw,"used");

    fclose(F3);
}
