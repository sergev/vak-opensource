/* вывод сообщения по заданному адресу */

#include "..\common\advexter.h"

#define blksiz    512
#define mask      (~(blksiz-1l))

#define adv_text  "text.adv"
#define adv_text2 "\advent\text.dat"
#define empty     '>'

mes(iadr)
    unsigned iadr;
{
    static int cb = -1;
    static char buf[blksiz+1];
    register char *ptr, *zeroptr, *p;
    static long block = -1, nblock, adr;

    if( cb == -1 ) {
        if( (cb = open( adv_text, 0 )) == -1 )  {
            if( (cb = open( adv_text2, 0 )) == -1 )  fatal(1);
        }
    }

    adr = (long) iadr * 2l;
    nblock = adr & mask;
    if( block != nblock ) {
        block  = nblock;
        lseek( cb, block, 0);
        read(  cb, buf, blksiz );
    }

    ptr = &buf[ (unsigned) (adr & (blksiz-1l)) ];
    if( !*ptr )  ++ptr;

    if( *ptr != empty ) {
        for(;;) {
            for(zeroptr=ptr;  *zeroptr;  ++zeroptr) ;
            write( 1, ptr, zeroptr-ptr );
        if( zeroptr != &buf[blksiz] ) break;
            read( cb, buf, blksiz);  ++block;
            ptr = buf;
        }
    }
}
