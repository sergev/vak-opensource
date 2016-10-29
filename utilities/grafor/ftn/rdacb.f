      subroutine rdacb(x0,y0,r,xf,yf,j)
C      rdacb    01/04/76         01/01/80
      common/gfgel/gf(28)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      if(gf(i).lt.2.)go to 9
      th01=gf(i+6)*gf(28)
      thf1=gf(i+7)*gf(28)
      r=gf(i+3)
      x0=gf(i+1)+abs(r)*cos(th01)
      y0=gf(i+2)+abs(r)*sin(th01)
      xf=gf(i+1)+abs(r)*cos(thf1)
      yf=gf(i+2)+abs(r)*sin(thf1)
      if(gf(i+6)-gf(i+7)) 1,3,2
  1   if(gf(i+7)-gf(i+6)-180.) 4,6,5
  2   if(gf(i+6)-gf(i+7)-180.) 5,7,4
  4   if(r) 7,3,6
  5   if(r) 6,3,7
  6   j=0
      go to 8
  7   j=1
      go to 8
  9   call gffals(6)
  8   if(gf(9).gt.0.) call strmod(0)
  3   return
      end
