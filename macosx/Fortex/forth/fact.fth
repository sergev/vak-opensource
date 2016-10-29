\ Iterative factorial function.

." Defining fact function ... " 
: fact  ( n -- n! )
	dup 2 < if drop 1 else
	dup begin 1- swap over * swap dup 1 = until
	drop then
; ." done." cr
