/*d* === putmes ===   07.01.85   version   16 */
/* store message into "adv:text" direct access file */
/* первая строка уже загружена в 'line' */
/* строки разделяются символом 'lf', на конце сообщения - 0. */
/* return: address of first record of message */

#include "adv_ext.h"

static FILE *F1;
long outt;

putmes() {
    static char _mrk[4],_mrk0[4];
#   define mrk0(x) _mrk0[(x)-1]
#   define mrk(x)  _mrk[(x)-1]
    static unsigned _putmes;
    static int i, en;
    long f3pos, ftell();

    _putmes = outt/2;
    for(i=1; i<=3; ++i)  mrk0(i)=line(i);
    do {
        for(i=1; i<=3; ++i)  mrk(i)=line(i);
    if( strncmp(_mrk,_mrk0,4) ) break;
        for(en=inplen; en>=4; --en) {
            if( line(en)!=' ' )  goto L9;
        }
        en=3;
L9:     for(i=4; i<=en; ++i)  _outt(line(i));
        _outt('\n');
        f3pos = ftell(F3);
    } while( getlin());
    fseek( F3, f3pos, 0 );
    _outt(0);

    return( _putmes );
}


_outt(byte)
    char byte;
{
    if( F1==NULL ) {                   /* открыть файл */
        if( (F1=fopen("text.adv","wb")) == NULL )  fatal();
    }
    fwrite( &byte, 1, 1, F1 );
    outt=outt+1;
}
