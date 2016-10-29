/*d* === freeze ===             19.01.85   version    9 */
/* freeze game into file 'frozen.adv' */

#include "..\common\advexter.h"
#include <stdio.h>

extern FILE *F1;

freeze() {
    F1=fopen("frozen.adv","wb");
    fwrite( &abb,   sizeof abb,     1, F1);
    fwrite( _abrev, sizeof _abrev,  1, F1);
    fwrite( &loc,   sizeof loc,     1, F1);
    fwrite( &tally, sizeof tally,   1, F1);
    fwrite( _place, sizeof _place,  1, F1);
    fwrite( _prop,  sizeof _prop,   1, F1);
    fwrite( &rndini,sizeof rndini,  1, F1);
    fwrite( &tevent,sizeof tevent,  1, F1);
    fwrite( &tiniti,sizeof tiniti,  1, F1);
    fclose(F1);
}


/* === loadfr === */
/* load frozen game from file 'adv:frozen' */

loadfr() { 
    fread(  &abb,   sizeof abb,     1, F1);
    fread(  _abrev, sizeof _abrev,  1, F1);
    fread(  &loc,   sizeof loc,     1, F1);
    fread(  &tally, sizeof tally,   1, F1);
    fread(  _place, sizeof _place,  1, F1);
    fread(  _prop,  sizeof _prop,   1, F1);
    fread(  &rndini,sizeof rndini,  1, F1);
    fread(  &tevent,sizeof tevent,  1, F1);
    fread(  &tiniti,sizeof tiniti,  1, F1);
    fclose( F1 );
    unlink( "frozen.adv" );
}
