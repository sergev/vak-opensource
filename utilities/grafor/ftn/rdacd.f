      subroutine rdacd(x0,y0,xc,yc,phi)
C     subroutine    rdacd     01/04/76    01/01/80
      common/gfgel/gf(28)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      if(gf(i).lt.2.) goto 5
      th01=gf(i+6)*0.01745329
      xc=gf(i+1)
      yc=gf(i+2)
      x0=xc+abs(gf(i+3))*cos(th01)
      y0=yc+abs(gf(i+3))*sin(th01)
      phi=gf(i+7)-gf(i+6)
      goto 6
    5 call gffals(6)
  6   if(gf(9).gt.0.) call strmod(0)
      return
      end
