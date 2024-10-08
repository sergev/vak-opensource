/*d* === getans ===        07.01.85   version    9 */
/* ввод одного или 2-x слов */
/* out: word1,word2 - найденные по словарю значения слов */
/*      type1,type2 - их типы (-1 - слова het) */

getans(word1,word2,type1,type2)
    int *word1, *word2, *type1, *type2;
{
    int work, kk;
    char wrd[4];

    for(;;) {
	inp_ini();
	*word1 = *word2 = *type1 = *type2 = -1;
	kk = 0;

	while( *type2 == -1 ) {
	    inp_word( wrd );
	if( wrd[0] == ' ' )  break;
	    kk = 1;
	    if( wrd[0] == '!' ) goto L100;
	    if( (work=vocab(wrd)) >= 1 ) {
		if( *type1 == -1 ) {
		    *word1=work%1000;
		    *type1=work/1000;
		} else {
		    *word2=work%1000;
		    *type2=work/1000;
		}
	    }
	}


    if( *type1>=0 ) break;          /* есть известные слова */
	if( !kk ) {                 /*     вообще нет ни одного */
	    kk=61;
	} else {
L100:       kk=60;
	    if( pct(50) )  kk=61;
	    if( pct(33) )  kk=13;
	}
	rspeak(kk);
    }
}



/* ввод словa из ответа человека в буфер wrd */

static char buf[80], *ptr;


/* ПРЕОБРАЗОВАТЕЛЬ БОЛЬШИХ БУКВ В МАЛЕНЬКИЕ */

static inp_ini() {
    buf[80-1] = '\n';
    if( !read(0, buf, 80-1) )  strcpy( buf, "конец\n");
    tolower( ptr=buf, 80 );
}


static inp_word( wrd )
    char        *wrd;
{
    register int i;
    register char *ptr0;

    for(i=4; i--; ) wrd[i] = ' ';
    for( ; *ptr==' ' || *ptr=='\t'; ++ptr) ;
    if( *ptr != '\n' ) {
	ptr0 = ptr;
	while( *ptr!='\n' && *ptr!=' ' && *ptr!='\t' ) {
	    if( (i=ptr-ptr0) < 4 )  wrd[i] = *ptr;
	    ++ptr;
	}
    }
}
