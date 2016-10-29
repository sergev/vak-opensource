      subroutine lsfit (x,y,ro,m,n,mpts)
C...subroutine     lsfit     01/09/73
C...subroutine     lsfit     01/09/78
      common /gftab/ irdb(5),rdb(17)
      dimension x(1),y(1),ro(1),b(6)
      if (irdb(2).lt.2) goto 42
      ma=iabs(m)
      na=iabs(n)+1
      alx=alog10(abs(x(ma)))
      call minmax (y,ma,ymin,ymax)
      aly=alog10(abs(ymax))
      alm=alog10(float(ma))
      alro=0.
      if (m) 141,145,172
 172  call minmax (ro,ma,romin,romax)
      alro=alog10 (romax)
 141  if (na.le.2) goto 42
      na=na-1
      if (18..lt.(alm+alro+2.*(na-1)*alx)) goto 141
      if (18..lt.(alm+alro+aly+(na-1)*alx)) goto 141
      if (na-iabs(n)) 144,149,145
 145  call grafer ('LSF1')
      return
 144  call grafer ('LSF2')
 149  n1=isign(na,n)
      call lesq (x,y,ro,m,b,n1)
      if (mpts) 10,2,12
   2  do 3 i=1,ma
   3  call pval (y(i),x(i),b,na)
      return
  10  l=-1
      k=-1
      dx=iabs (mpts)
      go to 13
  12  dx=mpts
      l=0
      k=1
  13  step=(x(ma)-x(1))/dx
      epsl=step/2.
      xt=x(1)
      j=1
  50  if (abs(x(ma)-xt).gt.epsl) goto 16
      j=2
      l=1
      xt=x(ma)
  16  call pval (yt,xt,b,na)
      if (l) 32,31,30
  30  ipen=1
      go to 40
  31  l=1
  32  ipen=0
  40  call tmf (xt,yt,xtf,ytf)
      call move (xtf,ytf,ipen)
      if (j.eq.2) goto 42
      xt=xt+step
      l=l*k
      go to 50
  42  return
      end
