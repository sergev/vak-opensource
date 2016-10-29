      subroutine rdmvb(x0,y0,deltax,deltay)
C    rdmvb    01/06/75  01/01/80
      common/gfgel/gf(28)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      if(gf(i).ne.0.) go to 5
      x0=gf(i+1)
      y0=gf(i+2)
      deltax=gf(i+6)-gf(i+1)
      deltay=gf(i+7)-gf(i+2)
      go to 6
  5   call gffals(6)
  6   if(gf(9).gt.0.) call strmod(0)
      return
      end
