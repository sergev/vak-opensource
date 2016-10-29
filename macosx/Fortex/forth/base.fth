\ Portable Forth extended run-time library.
\ Copyright (C) 1990-1992 Serge Vakulenko, <vak@kiae.su>.

: [				( --		switch to run mode
	0 state ! ;
	immediate

: ]				( --		switch to compile mode
	1 state ! ;
	immediate

: binary			( --		set base to binary
	2 base ! ;

: octal				( --		set base to octal
	8 base ! ;

: decimal			( --		set base to decimal
	10 base ! ;

: hex				( --		set base to hex
	16 base ! ;

: <>				( a b -- rez	true if a != b
	= ! ;

: <=				( a b -- rez	true if a <= b
	> not ;

: >=				( a b -- rez	true if a >= b
	< not ;

: 0=                            ( a -- rez	true if a == 0
	0 = ;

: 0<                            ( a -- rez	true if a < 0
	0 < ;

: 0<=                           ( a -- rez	true if a <= 0
	0 <= ;

: 0>                            ( a -- rez	true if a > 0
	0 > ;

: 0>=                           ( a -- rez	true if a >= 0
	0 >= ;

: ?                             ( a --		print value of a variable
	@ . ;

: tuck				( a b -- b a b
	swap over ;

: nip				( a b -- b
	swap drop ;

: 3dup				( a b c -- a b c a b c
	dup 2over rot ;

: u.				( u --		print unsigned value
	<# #s #> type space ;

: .r				( d f --	print value aligned to right
	swap dup abs		( f d dabs
	<# #s swap sign #>	( f a n
	rot over -		( a n f-n
	dup 0> if spaces else drop then
	type ;

: u.r				( d f --	print uns. value aligned
	swap			( f d
	<# #s #>		( f a n
	rot over -		( a n f-n
	dup 0> if spaces else drop then
	type ;

: +!				( n a --	add number to variable
	dup @ rot + swap ! ;

: -!				( n a --	subtract number from variable
	dup @ rot - swap ! ;

: 1+!				( a --		increment variable
	1 swap +! ;

: 0!				( a --		assign zero to variable
	0 swap ! ;

: string			( c --		compile string to data area
	word count allot drop ;

100 constant rstacksz				( size of rstack in words
variable rpointer				( rstack pointer
create rstack rstacksz alloc			( rstack array
rstack rpointer - constant wordsz		( size of word

: rempty			( --		empty rstack
	rpointer 0! ;

: abort				( --		abort execution, empty stacks
	sempty rempty quit ;

: abort"			( --		print string then abort
	ascii " word count	( a n
	type cr abort ;

: r@				( -- n		get value from rstack
	rstack rpointer @ + wordsz - @ ;

: >r				( n --		push value from stack to rstack
	rpointer @ dup 0< if ." rstack underflow" cr abort then
	rstacksz >= if ." rstack overflow" cr abort then
	rstack rpointer @ + ! wordsz rpointer +! ;

: r>				( -- n		pop value from rstack to stack
	rpointer @ 0<= if ." rstack empty" cr abort then
	wordsz rpointer -! rstack rpointer @ + @ ;

: r.s				( --		print rstack
	rpointer @ 0 ?do
		rstack i + @ .
	wordsz +loop ;

: literal			( n --		compile number immediately
	ncompile ;
	immediate

: '				( -- i		get index of the next word
	bl word find		( i
	state @ if ncompile then ;
	immediate

: [compile]			( --		compile word immediately
	compile ;
	immediate

: myself			(		call itself recursively
	latest ncompile	compile execute ;
	immediate

freezedict			(		 freeze dictionary
