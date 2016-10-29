      subroutine rdaca(x0,y0,r,th0,thf)
C    rdaca      01/04/76         01/01/80
      common/gfgel/gf(28)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      if(gf(i).lt.2.)go to 5
      th01=gf(i+6)*gf(28)
      x0=gf(i+1)+abs(gf(i+3))*cos(th01)
      y0=gf(i+2)+abs(gf(i+3))*sin(th01)
      r=gf(i+3)
       th0=gf(i+6)
       thf=gf(i+7)
      go to 6
  5   call gffals(6)
  6   if(gf(9).gt.0.) call strmod(0)
      return
      end
