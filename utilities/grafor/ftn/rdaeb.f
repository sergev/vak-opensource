      subroutine rdaeb(x0,y0,a,b,alpha,xf,yf)
C    rdaeb     01/04/76      01/01/80
      common/gfgel/gf(28)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      k=gf(i)
      if(k+20) 5,4,8
  8   if(k+2) 5,4,5
  4   th01=gf(i+6)*0.01745329
      thf1=gf(i+7)*0.01745329
      alpha=gf(i+5)
      alpha1=gf(i+5)*0.01745329
      a=gf(i+3)
      b=gf(i+4)
      a1=abs(a)
      b1=abs(b)
      d0=a1*b1/sqrt((a1*sin(th01))**2+(b1*cos(th01))**2)
      df=a1*b1/sqrt((a1*sin(thf1))**2+(b1*cos(thf1))**2)
      x0=gf(i+1)+d0*cos(th01+alpha1)
      y0=gf(i+2)+d0*sin(th01+alpha1)
      xf=gf(i+1)+df*cos(thf1+alpha1)
      yf=gf(i+2)+df*sin(thf1+alpha1)
      go to 6
  5   call gffals(6)
  6   if(gf(9).gt.0.) call strmod(0)
      return
      end
