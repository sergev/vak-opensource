      subroutine cirtac(r,xt,yt,j)
C.....subroutine cirtac  01/06/75      01/01/80
      common/gfgel/gf(28)
      common/gfbftr/xtr,ytr
      x0=xtr
      y0=ytr
      if(gf(9).gt.0.) call strmod(0)
      i=abs(gf(9))
      if(ivest(gf(i),0.,0.000001)  .le.0)  goto 14
      xc=gf(i+1)
      yc=gf(i+2)
      r1=abs(gf(i+3))
      delx=x0-xc
      dely=y0-yc
      cl=sqrt(delx**2+dely**2)
      if(ivest(cl,0.,0.000001))   17,18,19
  19  th=angler(delx,dely)
      if(ivest(cl,r1,0.000001))   4,6,7
  4   if(iabs(j)-1) 17,18,5
  5   p=(cl+r1)/2
      z=sqrt((p+r-r1)*(p-cl)/(p*(p-r)))
      if(ivest(cl+r1,2*r,0.000001))   18,12,12
  6   xt=x0
      yt=y0
      goto 15
  7   if(iabs(j)-1) 17,8,10
  8   if(ivest(cl-r1,2*r,0.000001)  .gt.0)  goto 18
      p=(2*r+r1+cl)/2
      z=sqrt((p-r-r1)*(p-cl)/(p*(p-r)))
      goto 12
  10  th=th+180.
      if(ivest(cl+r1,2*r,0.000001)  .gt.0)  goto 18
      p=(2*r-r1+cl)/2
      z=sqrt((p-r+r1)*(p-cl)/(p*(p-r)))
  12  th1=th*gf(28)+sign(2.,float(j))*atan(z)
      xt=xc+r1*cos(th1)
      yt=yc+r1*sin(th1)
      goto 15
  14  call gffals(6)
  15  if(gf(9).gt.0.) call strmod(0)
  17  return
  18  call gffals(5)
      goto 15
      end
