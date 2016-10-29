      subroutine arcocc(r,xt1,yt1,xt2,yt2,j)
C...arcocc     01/06/75   01/01/80
      common/gfgel/gf(28)
      if(gf(1).le.0.)goto 2
      if(gf(11).le.0.)goto 2
      delx=gf(12)-gf(2)
      dely=gf(13)-gf(3)
      r1=abs(gf(4))
      r2=abs(gf(14))
      th0=angler(delx,dely)
      dl=sqrt(delx**2+dely**2)
      if(abs(dl).lt.1.e-6) goto 37
      if(ivest(dl,r1+r2,0.000001))   23,28,5
  5   if(iabs(j)-11) 37,12,6
  6   if(iabs(j)-12) 3,9,7
  7   if(iabs(j)-21) 3,10,8
  8   if(iabs(j)-22) 3,11,3
    9 r2=-r2
      go to 12
   10 r1=-r1
      go to 12
   11 r1=-r1
      r2=-r2
   12 if(ivest(dl,r1+r2+2*r,0.000001)  .gt.0) goto 37
      p=(2*r+dl+r1+r2)/2
      z=sqrt((p-r-r1)*(p-dl)/(p*(p-r-r2)))
      if(j) 13,3,14
   13 th1=th0*gf(28)+2*atan(z)
      go to 15
   14 th1=th0*gf(28)-2*atan(z)
   15 xc=gf(2)+abs(r+r1)*cos(th1)
      yc=gf(3)+abs(r+r1)*sin(th1)
      th1=th1+3.141593
      delx2=gf(12)-xc
      dely2=gf(13)-yc
      th02=angler(delx2,dely2)
      th2=th02*gf(28)
      xt1=xc+r*cos(th1)
      yt1=yc+r*sin(th1)
      xt2=xc+r*cos(th2)
      yt2=yc+r*sin(th2)
      return
   23 if(ivest(dl,abs(r1-r2),0.000001)  .le.0) goto 24
      if(iabs(j)-1) 3,27,17
  17  if(iabs(j)-2) 3,29,18
  18  if(iabs(j)-3) 3,25,19
  19  if(iabs(j)-4) 3,12,20
  20  if(iabs(j)-5) 3,11,21
  21  if(iabs(j)-10) 3,37,37
  24  if(iabs(j)-30) 37,38,3
   25 if(ivest(r1+r2-dl,2*r,0.000001))   37,46,26
   26 p=(dl+r1+r2-2*r)/2
      z1=sqrt((p+r-r1)*(p-dl)/(p*(p+r-r2)))
      z2=sqrt((p+r-r2)*(p-dl)/(p*(p+r-r1)))
      go to 31
   27 if(ivest(r1-r2+dl,2*r,0.000001))   37,46,28
   28 p=(dl+r1+r2)/2
      z1=sqrt((p+r-r1)*(p-dl)/(p*(p-r-r2)))
      z2=sqrt((p+r-r2)*(p-dl)/(p*(p+r-r1)))
      go to 31
   29 if(ivest(r2-r1+dl,2*r,0.000001))   37,46,30
   30 p=(dl+r1+r2)/2
      z1=sqrt((p-r-r1)*(p-dl)/(p*(p+r-r2)))
      z2=sqrt((p+r-r2)*(p-dl)/(p*(p-r-r1)))
  31  th1=th0*gf(28)-isign(2,j)*atan(z1)
      th2=th0*gf(28)+3.141593+isign(2,j)*atan(z2)
   47 xt1=gf(2)+r1*cos(th1)
      yt1=gf(3)+r1*sin(th1)
      xt2=gf(12)+r2*cos(th2)
      yt2=gf(13)+r2*sin(th2)
      return
   38 if(r1-r2) 40,3,39
   39 rg=r1
      rl=r2
      j1=j
      go to 41
   40 rg=r2
      rl=r1
      j1=-j
      th0=th0+180.
   41 if(ivest(rg-rl-dl,2*r,0.000001))   42,46,37
   42 if(ivest(rg-rl+dl,2*r,0.000001)  .lt.0) goto 37
      p=(rg+rl+dl)/2
      z1=sqrt((p-rg+r)*(p-dl)/(p*(p-rl-r)))
      z2=sqrt((p-rl-r)*(p-dl)/(p*(p-rg+r)))
      thg=th0*gf(28)-isign(2,j1)*atan(z1)
      thl=th0*gf(28)+3.141593+isign(2,j1)*atan(z2)
      if(r1-r2) 45,3,44
   44 th1=thg
      th2=thl
      go to 47
   45 th1=thl
      th2=thg
      go to 47
   46 th1=th0*gf(28)
      th2=th0*gf(28)
      go to 47
    2 call gffals(6)
      return
   37 call gffals(5)
    3 return
      end
