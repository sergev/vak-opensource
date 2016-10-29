      subroutine  polreg(xofc,yofc,rin,rfn,theta1,theta2,jtext,m,h,j)
C...subroutine     polreg    01/09/73
C...subroutine           polreg          01/09/78
      dimension jtext(1)
      common /gftab/ irdb(5),rdb(17)
      equivalence (ct,cs),(st,sn),(t,t2)
      if (irdb(2).le.0) go to 1
      c=rdb(17)/rdb(1)
      if (rfn-rin.lt.c*.5) go to 1
      t=theta2-theta1-360.
      if (t.lt.(-355.)) go to 1
      if (t.gt.0.) go to 1
      irdb(2)=2
      rdb(7)=xofc
      rdb(8)=yofc
      rdb(9)=rin
      rdb(10)=rfn
      rdb(15)=theta1
      rdb(16)=theta2
      n=iabs(m)
      p=90.
      if (m.lt.0) p=0.
      if (j) 1,30,17
   17 if(t) 21,22,1
   21 t1=theta1*.01745329
      t2=theta2*.01745329
      ct=cos(t1)
      st=sin(t1)
      x1=xofc+rin*ct
      y1=yofc+rin*st
      call move(x1,y1,0)
      x=xofc+rfn*ct
      y=yofc+rfn*st
      call move(x,y,1)
      call circle(x,y,theta1,theta2,rfn,rfn,1)
      x=xofc+rin*cos(t2)
      y=yofc+rin*sin(t2)
      call move(x,y,1)
      if(rin)1,30,23
   23 call circle(x1,y1,theta1,theta2,rin,rin,1)
      go to 30
   22 t=p+105.
      t1=t*.01745329
      cs=cos(t1)
      sn=sin(t1)
      if(rin)1,24,25
   25 x=xofc+rin*cs
      y=yofc+rin*sn
      call circle(x,y,t,t+360.,rin,rin,1)
 24   x=xofc+rfn*cs
      y=yofc+rfn*sn
      call circle(x,y,t,t+360.,rfn,rfn,1)
   30 if (n.le.0) go to 1
      x1=h
      if (h.le.0.) x1=.5*c
      t=p*.01745329
      cs=cos(t)
      sn=sin(t)
      r=rfn+.5*c
      y1=x1*n/3.
      x=xofc-r*sn-y1*cs
      y=yofc+r*cs-y1*sn
      call  symbol(x,y,x1,jtext,n,p)
    1 return
      end
