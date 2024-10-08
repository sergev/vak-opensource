/* ðòåïâòáúï÷áôåìø âïìøûéè âõë÷ ÷ íáìåîøëéå */
/* ÷ÎÉÍÁÎÉÅ: KOI8 ËÏÄ! */

tolower(adr,lgt)
    register char *adr;
    int lgt;
{
    register char *oldadr, c;

    oldadr = adr;
    while( lgt-- ) {
	c = *adr;
	if(c>='A' && c<='Z')
		*adr |= 040;
	else if ( (c&0340)==0340 )
		*adr ^= 040;
	++adr;
    }

    return( oldadr );
}
