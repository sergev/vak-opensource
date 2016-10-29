/* ввод следующего байта из файла данных  a=get() */
/* установ начального адреса чтения   iniget(adr) */

#include <stdio.h>
#include <stdlib.h>
#include "..\common\advexter.h"

#define adv_data  "data.adv"
#define adv_data2 "\advent\data.adv"

static FILE *cb;
static char *bevents, *pevents;

iniget(adr)
    unsigned adr;
{
    if( cb == NULL ) {
        if( (cb = fopen( adv_data, "rb" )) == NULL ) {
            if( (cb = fopen( adv_data2, "rb" )) == NULL )  fatal();
        }
        bevents = (char*) malloc( eevent-tevent+2 );
        fseek( cb, (long) tevent, 0 );
        fread( bevents, eevent-tevent, 1, cb );
    }

    if( tevent<=adr && adr<eevent ) {
        pevents = bevents + (adr-tevent);
    } else {
        pevents = NULL;
        fseek( cb, (long) adr, 0 );
    }
}


int get()
{
    char c;
    if( pevents == NULL ) {
        fread( &c, 1, 1, cb);
    } else {
        c = *pevents++;
    }
    return( c & 0377 );
}
