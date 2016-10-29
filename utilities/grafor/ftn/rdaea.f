      subroutine rdaea(x0,y0,a,b,alpha,th0,thf)
C     subroutine    rdaea     01/04/76    01/01/80
      common/gfgel/gf(28)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      k=gf(i)
      if(k+20) 5,4,8
    8 if(k+2) 5,4,5
    4 th01=gf(i+6)*0.01745329
      a=gf(i+3)
      b=gf(i+4)
      alpha=gf(i+5)
      th0=gf(i+6)
      thf=gf(i+7)
      a1=abs(a)
      thf1=thf*0.01745329
      alpha1=alpha*0.01745329
      d=a1*b/sqrt((a1*sin(th01))**2+(b*cos(th01))**2)
      x0=gf(i+1)+d*cos(th01+alpha1)
      y0=gf(i+2)+d*sin(th01+alpha1)
      go to 6
    5 call gffals(6)
  6   if(gf(9).gt.0.) call strmod(0)
      return
      end
