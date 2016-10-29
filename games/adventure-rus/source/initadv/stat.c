/* ВЫДАЧА СТАТИСТИКИ ПО ИСПОЛЬЗОВАНИЮ СЛОВАРЯ И ДР. */

#include "..\common\advexter.h"

extern char mesused[];
extern int mesimp;

extern char locused[];
#define locdef 1
#define locgo  2

static char stand_mes[] = {9,12,13,15,16,23,32,40,60,90,98,99,203,0};


stat()
{
    register int i, nm;
    register char *p;

    printf("vocabulary words   :");
    printf("%8d  %s %6d  %s\n",nvoc,"of",vocw,"used");

    for(nm=1, i=ranm; i--; )  if( _rtext[i] ) ++nm;
    printf("messages           :");
    printf("%8d  %s %6d  %s\n",nm,"of",ranm,"used");

    for(i=loct; i--; ) {
	switch( locused[i] ) {
	    case locgo:  printf("позиция %d не описана\n",i+1);  break;
	    case locdef: printf("позиция %d недоступна\n",i+1);  break;
	}
    }

    for( p=stand_mes; *p; ++p )  {
	if( !rtext((*p)&0377) ) {
	    printf("стандартное сообщение %d не определено\n",i+1);
	}
    }

    for(i=1; i<=ranm-mesimp; ++i) {
	for( p=stand_mes; *p; ++p )  {
	    if( ((*p)&0377) == i+1 )  goto st;
	}
	if( mesused[i]  &&  !_rtext[i] ) {
	    printf("сообщение %d не определено\n",i+1);
	}
	if( !mesused[i]  &&  _rtext[i] ) {
	    printf("сообщение %d не используется\n",i+1);
	}
st:;
    }
}
