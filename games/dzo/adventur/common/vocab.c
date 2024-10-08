/* === vocab === 08.08.84 version 2 */
/*  look up 'word' in the vocabulary ('atab' real array) */
/*  and return its "definition" (ktab), or -1 if not found */
/* out: vocab=-1  - слово he найдено */
/*      vocab     - код слова */

#include "adv_common"

vocab(word)
    char *word;
{
    register int _vocab, i;

    i=1;
    for(;;) {
	_vocab=ktab(i);
    if( _vocab == 0 ) break;
	if( !strncmp( &atab(i), word, 4 ) )  return( _vocab );
	i=i+1;
    }
    return( -1 );
}
