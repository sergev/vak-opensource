/*d* === getans ===        07.01.85   version    9 */
/* ввод одного или 2-x слов */
/* out: word1,word2 - найденные по словарю значения слов */
/*      type1,type2 - их типы (-1 - слова het) */

getans(word1,word2,type1,type2)
    int *word1, *word2, *type1, *type2;
{
    int work, kk;
    char _wrd1[10],_wrd2[10];
#    define wrd2(x) _wrd2[(x)-1]
#    define wrd1(x) _wrd1[(x)-1]

    for(;;) {
	inpans(_wrd1,_wrd2);
	*word1 = *word2 = *type1 = *type2 = -1;
	if( wrd1(1)=='!' || wrd2(1)=='!' )  goto L100    /* спец-слова */;

	work=vocab(_wrd1);
	if( work >= 1 ) {
	    *word1=work%1000;
	    *type1=work/1000;
	}

	work=vocab(_wrd2);
	if( work >= 1 ) {
	    *word2=work%1000;
	    *type2=work/1000;
	}

    if( *type1>=0 ) break;          /* bce слова неизвестны */
	if( wrd1(1)==' ' ) {           /*     вообще het ни одного */
	    kk=61;
	} else {
L100:       kk=60;
	    if( pct(50) )  kk=61;
	    if( pct(33) )  kk=13;
	}
	rspeak(kk);
    }
}
