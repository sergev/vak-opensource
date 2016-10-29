\ Draw recursive triangles.

: trirec			( bx by szx szy d --
	4 pick 3 pick 2 / +	( bx by szx szy d bx+szx/2
	4 pick 3 pick 2 / +	( bx by szx szy d bx+szx/2 by+szy/2
	6 pick over		( ... bx+szx/2 by+szy/2 bx by+szy/2
	7 grline drop drop 	( bx by szx szy d bx+szx/2
	5 pick 5 pick 4 pick	( bx by szx szy d bx+szx/2 bx by szy
	dup 2 / - + rot		( ... bx by+szy/2 bx+szx/2
	6 pick 5 pick +		( ... bx by+szy/2 bx+szx/2 by+szy
	7 grline drop swap drop	( bx by szx szy d by+szy/2
	5 pick 4 pick		( bx by szx szy d by+szy/2 bx szx
	dup 2 / - + swap	( bx by szx szy d bx+szx/2 by+szy/2
	over 6 pick 5 pick +	( ... bx+szx/2 by+szy/2 bx+szx/2 by+szy
	7 grline drop drop drop	( bx by szx szy d
	1- dup 0 > if		( call itself recursively if d>1
	4 pick 4 pick		( bx by szx szy d-1 bx by
	4 pick 2 / 4 pick 2 /	( ... bx by szx/2 szy/2
	4 pick myself		( bx by szx szy d-1
	4 pick 3 pick dup 2 / - + ( bx by szx szy d-1 bx+szx/2
	4 pick 3 pick dup 2 / - + ( bx by szx szy d-1 bx+szx/2 by+szy/2
	dup rot rot		( bx by szx szy d-1 by+szy/2 bx+szx/2 by+szy/2
	5 pick 2 / 5 pick 2 /	( ... bx+szx/2 by+szy/2 szx/2 szy/2
	5 pick myself		( bx by szx szy d-1 by+szy/2
	5 pick swap		( bx by szx szy d-1 bx by+szy/2
	4 pick 2 / 4 pick 2 /	( ... d-1 bx by+szy/2 szx/2 szy/2
	4 pick myself		( bx by szx szy d-1
	then drop drop drop drop drop ;

: tri   			( n --
	0 0 rot			( 0 0 n
	grinit drop		( 0 0 n maxx maxy
	over over 0 -rot swap	( 0 0 n maxx maxy 0 maxy maxx
	over 0 -rot 0 -rot	( 0 0 n maxx maxy 0 maxy 0 0 maxx maxy
	7 grline grline grline drop ( 0 0 n maxx maxy
	rot dup 0 > if
	    trirec
	else
	    drop drop drop drop drop
	then
	grend ;

7 tri
\ getch drop
