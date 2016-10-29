      subroutine arcolc(r,xt1,yt1,xt2,yt2,j)
C...arcolc     01/06/75   01/01/80
      common/gfgel/gf(28)
      common/gfbftr/xtr,ytr
      x0=xtr
      y0=ytr
      if(gf(1)) 2,4,1
    1 if(gf(11)) 2,5,2
    4 if(gf(11)) 2,2,6
    5 xc=gf(2)
      yc=gf(3)
      r1=abs(gf(4))
      i=11
      go to 7
    6 xc=gf(12)
      yc=gf(13)
      r1=abs(gf(14))
      i=1
    7 delx=-(gf(i+3)/(gf(i+3)**2+gf(i+4)**2))*(gf(i+3)*xc+gf(i+4)*yc+gf(
     1i+5))
      dely=-(gf(i+4)/(gf(i+3)**2+gf(i+4)**2))*(gf(i+3)*xc+gf(i+4)*yc+gf(
     1i+5))
      th=angler(delx,dely)
      h=sqrt(delx**2+dely**2)
      if(ivest(h,r1,0.000001)  .lt.0) goto 20
      if(iabs(j)-10) 18,10,9
  9   if(iabs(j)-20) 3,12,3
   10 if(ivest(h-r1,2*r,0.000001)  .gt.0) goto 18
      a=r+r1
      go to 14
   12 if(ivest(h+r1,2*r,0.000001)  .gt.0) goto 18
      a=r-r1
   14 b=h-r
      j1=j
   19 c=sqrt(a**2-b**2)
      e=gf(9)
      if(ivest(h,0.,.000001))   3,32,31
   31 rt=sqrt(c**2+h**2)
      z=c/h
      if(gf(1)) 2,15,16
  15  th1=th*gf(28)+isign(1,j)*atan(z)
   39 xt1=xc+rt*cos(th1)
      yt1=yc+rt*sin(th1)
      call move(xt1,yt1,0)
      call strmod(1)
      jc=j1/10
      if(ivest(rt,0.,.000001))   3,45,41
   41 call cirtac(r,xt2,yt2,jc)
      go to 17
  16  th1=th*gf(28)-isign(1,j)*atan(z)
      xt2=xc+rt*cos(th1)
      yt2=yc+rt*sin(th1)
      call move(xt2,yt2,0)
      call strmod(2)
      jc=-j1/10
      call cirtac(r,xt1,yt1,jc)
   17 call move(x0,y0,0)
      gf(9)=e
      return
  20  if(iabs(j).ge.10) go to 18
      if(iabs(j)-4) 24,22,3
   22 if(ivest(h+r1,2*r,0.000001)  .lt.0) goto 18
      a=r1-r
      b=h-r
      j1=isign(20,j)
      go to 19
  24  if(iabs(j)-3) 26,25,3
   25 a=r1+r
      b=h-r
      j1=isign(10,j)
      go to 19
  26  if(iabs(j)-2) 29,27,3
   27 if(ivest(r1-h,2*r,0.000001)  .lt.0) goto 18
      a=r1-r
      b=h+r
      j1=-isign(20,j)
      go to 19
  29  if(iabs(j).ne.1) go to 3
      a=r1+r
      b=h+r
      j1=-isign(10,j)
      go to 19
   32 rt=c
      if(gf(1)) 3,33,34
   33 i=1
      go to 35
   34 i=11
   35 th=angler(gf(i+6)-gf(i+1),gf(i+7)-gf(i+2))
      if(j) 36,3,37
   36 th1=th
      go to 38
   37 th1=th+180.
   38 if(gf(1)) 3,39,40
   40 xt2=xc+rt*cos(th1)
      yt2=yc+rt*sin(th1)
      call move(xt2,yt2,0)
      call strmod(2)
      jc=j1/10
      if(ivest(rt,0.,.000001))   3,43,42
   42 call cirtac(r,xt1,yt1,jc)
      go to 17
   43 if(abs(r1-2*r).ge.1.e-6) goto 18
      th2=th1-sign(90.,float(j))
      xt1=xc+r1*cos(th2)
      yt1=xc+r1*sin(th2)
      go to 17
   45 if(abs(r1-2*r).ge.1.e-6) goto 18
      th2=th1-sign(90.,float(j))
      xt2=xc+r1*cos(th2)
      yt2=yc+r1*sin(th2)
      go to 17
    2 call gffals(6)
      return
   18 call gffals(5)
    3 return
      end
