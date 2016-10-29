	subroutine inches
C	subroutine inches   23/08/73  01/09/78	 07/01/83
	common/gftab/itab(5),wtab(17)
	if(itab(2).eq.0)wtab(1)=2.54*wtab(17)
	return
	end
