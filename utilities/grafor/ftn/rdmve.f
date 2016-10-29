      subroutine rdmve(x0,y0,x,y)
C.....subroutine rdmve    01/06/75
      common/gfgel/gf(28)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      if(gf(i).ne.0.)goto 5
      x0=gf(i+1)
      y0=gf(i+2)
      x=gf(i+6)
      y=gf(i+7)
      goto 6
 5    call gffals(6)
  6   if(gf(9).gt.0.) call strmod(0)
      return
      end
