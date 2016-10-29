      subroutine forfit (m,a,b,xbeg,xend,mpts)
C...subroutine     forfit    01/09/73
C...subroutine     forfit    01/09/78
      common /gftab/ irdb(5),rdb(17)
      dimension a(1),b(1)
      if (irdb(2).lt.2) goto 42
      if (irdb(3).le.0) goto 42
      b(1)=0.
      pi2=3.141593*2.
      l=0
      k=1
      ms=mpts
      if (mpts) 10,42,12
  10  ms=iabs(mpts)
      l=-1
      k=-1
  12  dx=(xend-xbeg)/ms
      epsl=dx/2.
      dz=pi2/ms
      xt=xbeg
      zt=0.
      j=1
  50  if (abs(xend-xt).gt.epsl) goto 16
      xt=xend
      zt=pi2
      l=1
      j=2
  16  yt=a(1)
      do 20 i=1,m
  20  yt=yt+a(i+1)*cos(i*zt)+b(i+1)*sin(i*zt)
      if (l) 32,31,30
  30  ipen=1
      go to 40
  31  l=1
  32  ipen=0
  40  call tmf (xt,yt,xtf,ytf)
      call move (xtf,ytf,ipen)
      xt=xt+dx
      zt=zt+dz
      l=l*k
      if (j.eq.1) goto 50
  42  return
      end
