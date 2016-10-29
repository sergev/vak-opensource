      subroutine tdmp(x,y,n,a,b,c,d,kode)
C...subroutine     tdmp      01/09/73
C...subroutine     tdmp      01/09/78
      dimension x(n),y(n),a(n),b(n),c(n),d(n)
      if (n.le.2) go to 7
      dy2=y(2)-y(1)
      c(2)=x(2)-x(1)
      if (c(2).le.0.) goto 7
      dy=dy2
      do 1 i=3,n
      dy1=dy2
      dy2=y(i)-y(i-1)
      c(i)=x(i)-x(i-1)
      if (c(i).le.0.) goto  7
      a(i-1)=c(i)
      a2=a(i-1)
      c2=c(i-1)
      b(i-1)=2.*(c2+a2)
   1  d(i-1)=3.*(a2*dy1/c2+c2*dy2/a2)
      a(1)=0.
      c(n)=0.
      if(kode+1) 5,3,2
   2  if (kode-1) 4,6,7
    3 b(1)=1.
      b(n)=1.
      c(1)=0.
      a(n)=0.
      return
   4  d(1)=0.
      d(n)=0.
   5  b(1)=2.
      b(n)=2.
      c(1)=1.
      a(n)=1.
      d(1)=3.*dy/c(2)-(c(2)*d(1))/2.
      d(n)=3.*dy2/a(n-1)-(a(n-1)*d(n))/2.
      return
   6  a(1)=-1.
      c(1)=0.
      d(1)=0.
      b(1)=1.
      a(n)=c(2)
      c(n)=a(n-1)
      b(n)=2.*(c(2)+a(n-1))
      d(n)=3.*((a(n)/c(n))*dy2+(c(n)/a(n))*dy)
   7  return
      end
