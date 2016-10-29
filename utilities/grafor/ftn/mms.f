	subroutine mms
C.....subroutine mms 23/08/73,01/09/78,07/01/83
	common/gftab/itab(5),wtab(17)
	if(itab(2).eq.0)wtab(1)=0.1*wtab(17)
	return
	end
