      subroutine licon(xt1,yt1,xt2,yt2,j)
C...licon      01/06/75   01/01/80
      common/gfgel/gf(28)
      if(gf(1).le.0.)goto 16
      if(gf(11).le.0.)goto 16
      delx=gf(12)-gf(2)
      dely=gf(13)-gf(3)
      r1=abs(gf(4))
      r2=abs(gf(14))
      th0=angler(delx,dely)
      dl=sqrt(delx**2+dely**2)
      if(ivest(r1+r2,dl,0.000001)  .le.0) goto 26
      if(r1-r2) 23,25,24
   23 if(ivest(dl,r1-r2,0.000001))   21,21,26
   24 if(ivest(dl,r2-r1,0.000001))   21,21,26
   25 if(ivest(dl,0.,.000001))   8,21,26
   26 if(j-11) 8,9,10
   10 if(j-22) 11,9,8
    9 if(r1-r2) 1,3,2
    1 sl=dl*r2/(r2-r1)
      z=r2/sqrt(sl**2-r2**2)
      alpha=atan(z)/gf(28)
      go to 4
    2 sl=dl*r1/(r1-r2)
      z=r1/sqrt(sl**2-r1**2)
      alpha=-atan(z)/gf(28)
      go to 4
    3 alpha=0.
    4 if(j-11) 8,5,6
    5 th=th0+(90.+alpha)
      go to 7
    6 th=th0-(90.+alpha)
    7 th1=th*gf(28)
      xt1=gf(2)+r1*cos(th1)
      yt1=gf(3)+r1*sin(th1)
      xt2=gf(12)+r2*cos(th1)
      yt2=gf(13)+r2*sin(th1)
      return
   11 xc=(r2*gf(2)+r1*gf(12))/(r1+r2)
      yc=(r2*gf(3)+r1*gf(13))/(r1+r2)
      if(ivest(r1+r2,dl,0.000001))   20,19,21
   19 xt1=xc
      yt1=yc
      xt2=xc
      yt2=yc
      return
   20 dl1=sqrt((xc-gf(2))**2+(yc-gf(3))**2)
      dl2=dl-dl1
      sl1=sqrt(dl1**2-r1**2)
      sl2=sqrt(dl2**2-r2**2)
      z=r1/sl1
      alpha=atan(z)/gf(28)
      if(j-12) 8,12,13
   12 th1=(th0+180.-alpha)*gf(28)
      th2=(th0-alpha)*gf(28)
      go to 15
   13 if(j.ne.21) goto 8
      alpha=-alpha
      go to 12
   15 xt1=xc+sl1*cos(th1)
      yt1=yc+sl1*sin(th1)
      xt2=xc+sl2*cos(th2)
      yt2=yc+sl2*sin(th2)
      return
   16 call gffals(6)
      return
   21 call gffals(5)
    8 return
      end
