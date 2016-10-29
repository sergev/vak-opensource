	include(masm.h)
	xta     var
	;------------
	ifregz(11)
		.ascii  "var is zero"
	else
		.ascii  "var is nonzero"
	endif
	;------------
	loop
		LOOP1
		breakloop
		LOOP2
		contloop
		LOOP3
	endloop
	xta     cont
