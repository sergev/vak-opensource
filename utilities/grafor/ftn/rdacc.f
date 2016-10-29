      subroutine rdacc(x0,y0,xm,ym,xf,yf,j)
C     subroutine    rdacc   01/04/76         01/01/80
      common/gfgel/gf(28)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      if(gf(9).lt.2.) go to 5
      th01=gf(i+6)*0.01745329
      thf1=gf(i+7)*0.01745329
      thm1=(th01+thf1)/2.
      r1=abs(gf(i+3))
      x0=gf(i+1)+r1*cos(th01)
      xm=gf(i+1)+r1*cos(thm1)
      xf=gf(i+1)+r1*cos(thf1)
      y0=gf(i+2)+r1*sin(th01)
      ym=gf(i+2)+r1*sin(thm1)
      yf=gf(i+2)+r1*sin(thf1)
      j=0
      go to 6
    5 call gffals(6)
  6   if(gf(9).gt.0.) call strmod(0)
      return
      end
