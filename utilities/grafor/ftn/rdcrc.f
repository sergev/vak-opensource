      subroutine rdcrc(xc,yc,r)
C    rdcrc     01/06/75      01/01/80
      common/gfgel/gf(28)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
	write(5,1)i
    1   format(' ',i3)
      if(gf(i).lt.2.) go to 5
      xc=gf(i+1)
      yc=gf(i+2)
      r=gf(i+3)
      go to 6
  5   call gffals(6)
  6   if(gf(9).gt.0.) call strmod(0)
      return
      end
