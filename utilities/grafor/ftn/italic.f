	subroutine italic(j)
	common/gftab/itab(5),wtab(17)
      if(j)1,2,3
    1 wtab(2)=-0.4
      return
    2 wtab(2)=0.
      return
    3 wtab(2)=0.4
      return
      end
