/*d* === savecm ===             07.01.85   version    2 */
/* load/store common data for adventure into file 'adv:data' */

#include "advexter.h"
#include <stdio.h>

FILE *F1;

savecm() {
/*    int    fwrite(); */
    if( (F1=fopen("common.adv","wb")) == NULL )  fatal();
    slcm( &fwrite );
}

loadcm() {
/*    int    fread(); */
    if( (F1=fopen("common.adv","rb")) == NULL ) {
        if( (F1=fopen("\advent\common.adv","rb")) == NULL ) fatal(2);
    }
    slcm( &fread );
}

slcm(f)
    int (*f)();
{
    (*f) (_rtext, sizeof _rtext,  1, F1);
    (*f) (_ltext, sizeof _ltext,  1, F1);
    (*f) (_stext, sizeof _stext,  1, F1);
    (*f) (_ctext, sizeof _ctext,  1, F1);
    (*f) (_cval,  sizeof _cval,   1, F1);
    (*f) (_ptext, sizeof _ptext,  1, F1);
    (*f) (_pstat, sizeof _pstat,  1, F1);
    (*f) (_trvkey,sizeof _trvkey, 1, F1);
    (*f) (_actkey,sizeof _actkey, 1, F1);
    (*f) (&nvoc,  sizeof nvoc,    1, F1);
    (*f) (_ktab,  sizeof _ktab,   1, F1);
    (*f) (_atab,  sizeof _atab,   1, F1);
    (*f) (&tally, sizeof tally,   1, F1);
    (*f) (&treasr,sizeof treasr,  1, F1);
    (*f) (_place, sizeof _place,  1, F1);
    (*f) (_fixed, sizeof _fixed,  1, F1);
    (*f) (_prop,  sizeof _prop,   1, F1);
    (*f) (&rndini,sizeof rndini,  1, F1);
    (*f) (&tevent,sizeof tevent,  1, F1);
    (*f) (&eevent,sizeof eevent,  1, F1);
    (*f) (&tiniti,sizeof tiniti,  1, F1);
    fclose(F1);
}
