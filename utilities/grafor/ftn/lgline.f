      subroutine lgline(x,y,n,lg,nm,js,l)
C...subroutine     lgline    01/12/73
C...subroutine          lgline           01/09/78
      common /gftab/ irdb(5),rdb(17)
      dimension x(1),y(1)
      if (irdb(2).ne.2) go to 1
      if (irdb(3).le.0) go to 1
      m=iabs(n)
      call where (xt,yt,x1)
C  * * * * * * * * *
      jst1=0
      mark=1
      jstep=js
      if(jstep) 30,31,32
  30  ipen=0
      jstep=-jstep
      go to 33
  31  mark=2
  32  ipen=1
C  * * * * * * * * *
  33  call tmlgf(x(1),y(1),lg,n,x1,y1)
      call tmlgf(x(m),y(m),lg,n,x2,y2)
      a=amax1(abs(xt-x1),abs(yt-y1))
      b=amax1(abs(xt-x2),abs(yt-y2))
      j=1
      jj=1
      if (a.le.b) go to 35
      j=m
      jj=-1
      x1 = x2
      y1 = y2
  35  call move(x1,y1,0)
C  * * * * * * * * *
      do 40 i = 2,m
      j=j+jj
      if (mark.eq.2) go to 42
      jst1=jst1-1
      if (jst1.gt.0) go to 42
      call marker(nm)
       jst1=jstep
  42  call tmlgf(x(j),y(j),lg,n,xt,yt)
      call move(xt,yt,ipen)
  40  continue
C  * * * * * * * * *
      if (mark.eq.1) call marker(nm)
      if (l.gt.0) call move(x1,y1,ipen)
   1  return
      end
