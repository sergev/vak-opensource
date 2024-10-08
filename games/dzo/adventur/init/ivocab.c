/*d* === ivocab === read in vocabulary === 13.02.85   version   27 */

/*  format: */
/*     d         direction word(s) */
/*     s         special word(s).  after it is of message to produce */
/*     m         message */

/*  vocabulary: */
/*     common /vocab/ ktab(vocw),      % числа - значения слов */
/*                    atab(vocw)       % слова - по 4 буквы */

#include "adv_ini.h"

int outt, mesimp;

ivocab() {
    static int direct, kword, mm, mark;

    if( (F3=fopen("adv_vocab","r")) == NULL )  fatal();
    printf("vocabulary words   :");
    while(getlin()) {
	mark=line(1);
	if( mark=='m' ) {                /* message */
	    rtext(mm)=putmes();
	} else {
	    if( mark=='d' ) {            /* directional words */
		direct=direct+1; kword=direct;
	    } else if( mark=='s' ) {       /* special words */
		mm=ranm-mesimp;
		kword=mm+(specwr*1000);
		mesimp=mesimp+1;
	    } else {
		printf("\n%s%.10s\n","неверный маркер: ",_line); fatal();
	    }
	    p=4                                /* read list of words */;
	    while(getwrd()) {
		if( vocab(_word) >= 0 ) {
		    printf("\n%s%.4s\n","дважды определено: ",_word);
		    fatal();
		}
		nvoc=nvoc+1;
		ktab(nvoc)=kword;  atab(nvoc) = *rword;
		screen(nvoc);
	    }
	}
    }
    printf("%8d  %s %6d  %s\n",nvoc,"of",vocw,"used");
    fclose(F3);
}
