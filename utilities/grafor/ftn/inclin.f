      subroutine inclin (xbeg,dxex,jx,y,n,nm,js)
C...subroutine     inclin    01/09/73
C...subroutine     inclin    01/09/78
      dimension y(n)
       common /gftab/ irdb(5),rdb(17)
      if (irdb(2).ne.2) goto 14
      if (irdb(3).le.0) goto 14
      na=iabs(n)
      call where (xt,yt,x1)
      xor=rdb(7)
      yor=rdb(8)
      xr=xor+rdb(9)
      yr=yor+rdb(10)
      if (jx) 14,20,21
  20  dx=dxex
      xend=xbeg+dx*(na-1)
      go to 22
  21  xend=dxex
      dx=(xend-xbeg)/(na-1)
  22  jst=0.
      mark=1
      jstep=js
      if (jstep) 3,4,5
    3 ipen=0
      jstep=-jstep
      go to 6
    4 mark=2
    5 ipen=1
    6 call tmf (xbeg,y(1),x1f,y1f)
      call tmf (xend,y(na),xnf,ynf)
      a=amax1(abs(xt-x1f),abs(yt-y1f))
      b=amax1(abs(xt-xnf),abs(yt-ynf))
      j=1
      jj=1
      xj=xbeg
      dxj=dx
      if (a.le.b) goto 80
      j=na
      jj=-1
      xj=xend
      dxj=-dx
      x1f=xnf
      y1f=ynf
  80  if (n.le.0) goto 8
      x1f=amin1(xr,amax1(xor,x1f))
      y1f=amin1(yr,amax1(yor,y1f))
    8 call move (x1f,y1f,0)
      do 12 i=2,na
      xj=xj+dxj
      j=j+jj
      if (mark.eq.2) goto 11
      jst=jst-1
      if (jst.gt.0) goto 11
      call marker (nm)
      jst=jstep
   11 call tmf (xj,y(j),xt,yt)
      if (n.le.0) goto 112
      xt=amin1(xr,amax1(xor,xt))
      yt=amin1(yr,amax1(yor,yt))
 112  call move (xt,yt,ipen)
   12 continue
      if (mark.eq.2) goto 14
      call marker(nm)
   14 return
      end
