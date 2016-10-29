      subroutine splint (x,n,a,b,c,d,y,m)
C...subroutine     splint    01/09/73
C...subroutine     splint    01/09/78
      dimension x(n),a(n),b(n),c(n),d(n),y(m)
      m1=m-1
      xj=x(1)
      if(m1.le.0) goto 5
      y(1)=((a(1)*xj+b(1))*xj+c(1))*xj+d(1)
      dx=(x(n)-xj)/m1
      i=2
      do 4 j=2,m1
      xj=xj+dx
   2  if (xj.lt.x(i)) goto 4
      i=i+1
      go to 2
   4  y(j)=((a(i-1)*xj+b(i-1))*xj+c(i-1))*xj+d(i-1)
      y(m)=((a(n-1)*x(n)+b(n-1))*x(n)+c(n-1))*x(n)+d(n-1)
   5  return
      end
