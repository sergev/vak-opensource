      subroutine cirtal(r,xt,yt,j)
C...cirtal     01/06/75   01/01/80
      common/gfgel/gf(28)
      common/gfbftr/xtr,ytr
      x0=xtr
      y0=ytr
      if(gf(9).gt.0.)call strmod(0)
       i=abs(gf(9))
      if(abs(gf(i)).gt.1e-6) goto 14
      delx=-(gf(i+3)/(gf(i+3)**2+gf(i+4)**2))*(gf(i+3)*x0+gf(i+4)*y0+gf(
     1i+5))
      dely=-(gf(i+4)/(gf(i+3)**2+gf(i+4)**2))*(gf(i+3)*x0+gf(i+4)*y0+gf(
     1i+5))
      h=sqrt(delx**2+dely**2)
      if(ivest(h,0.,.000001))   17,13,11
   11 if(ivest(h,2*r,0.000001)  .gt.0) goto 18
      h1=h-r
      a=sqrt(r**2-h1**2)
      z=a/h
      alpha1=atan(z)
      r1=a/sin(alpha1)
      th=angler(delx,dely)
      th1=th*gf(28)-isign(1,j)*alpha1
      xt=x0+r1*cos(th1)
      yt=y0+r1*sin(th1)
      go to 15
   13 xt=x0
      yt=y0
      go to 15
   14 call gffals(6)
   15 if(gf(9).gt.0.)call strmod(0)
   17 return
   18 call gffals(5)
      go to 15
      end
