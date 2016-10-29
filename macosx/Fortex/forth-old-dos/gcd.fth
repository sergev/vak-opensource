\ Greatest common divisor.

: gcd   			( a b -- gcd
	2dup = if drop exit then
	2dup < if swap then     ( max:a,b min:a,b
	dup -rot - myself ;
