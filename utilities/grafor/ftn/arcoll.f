      subroutine arcoll(r,xt1,yt1,xt2,yt2,j)
C...arcoll     01/06/75   01/01/80
      common/gfgel/gf(28)
      if(gf(1).ne.0.)goto 2
      if(gf(11).ne.0.)goto 2
      d=gf(5)*gf(14)-gf(15)*gf(4)
      if(d.eq.0.)goto 18
      xc=(gf(6)*gf(15)-gf(16)*gf(5))/d
      yc=(gf(16)*gf(4)-gf(6)*gf(14))/d
      th01=angler(gf(7)-gf(2),gf(8)-gf(3))
      th02=angler(gf(17)-gf(12),gf(18)-gf(13))
      th1=th01*gf(28)
      if(ivest(th01,th02,0.000001))   6,3,8
    6 if(ivest(th02-th01,180.,0.000001))   10,3,7
    7 th02=th02-180.
      go to 10
    8 if(ivest(th01-th02,180.,0.000001))   9,3,10
    9 th02=th02+180.
   10 th2=th02*gf(28)
      if(j-1) 3,17,11
   11 if(j-2) 3,12,13
   12 th1=th1+3.141593
      go to 17
   13 if(j-3) 3,14,15
   14 th1=th1+3.141593
      th2=th2+3.141593
      go to 17
   15 if(j.ne.4) goto 3
      th2=th2+3.141593
   17 th=abs((th1-th2)/2)
      rt1=r*cos(th)/sin(th)
      rt=abs(rt1)
      xt1=xc+rt*cos(th1)
      yt1=yc+rt*sin(th1)
      xt2=xc+rt*cos(th2)
      yt2=yc+rt*sin(th2)
      return
    2 call gffals(6)
      return
   18 call gffals(5)
    3 return
      end
