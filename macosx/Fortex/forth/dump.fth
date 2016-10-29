\ Hexadecimal dump of memory.

." Defining decode ... "
: decode					( i -- c
	dup bl < if drop ascii . else
	dup ascii ~ > if drop ascii . then
	then
; ." done." cr

." Defining dump ... "
: dump						( addr, len -- 
	over base @ hex 2swap                   ( a b a n
	+ rot                                   ( b a+n a
	do i
		<#
		bl hold                         ( b ai
		0 15 do
			dup i + c@              ( b ai w
			decode hold             ( b ai
		-1 +loop                        ( b ai
		0 15 do
			bl hold                 ( b ai
			dup i + c@              ( b ai w
			# # drop                ( b ai
		-1 +loop                        ( b ai
		bl hold # # # # #>              ( b at nt
		type cr
	16 +loop
	base !
; ." done." cr
