      subroutine tridig (u,n,a,b,c,d,kode,ier)
C...subroutine     tridig    01/09/73
C...subroutine     tridig    01/09/78
      dimension u(n),a(n),b(n),c(n),d(n)
      ier=0
      al=0.
      bt=0.
      gm=1.
      n1=n-1
      if (b(1).ne.0.) goto 2
      ier=1
      return
   2  do 5 i=1,n1
      s=a(i)*al+b(i)
      if(s.eq.0.) goto 4
      s1=1./s
      al=-c(i)*s1
      bt=(d(i)-a(i)*bt)*s1
      gm=-a(i)*gm*s1
      a(i)=al
      b(i)=bt
   5  c(i)=gm
      if (kode.eq.1) goto 8
      u(n)=(d(n)-a(n)*bt)/(a(n)*al+b(n))
      do 7 i=1,n1
    7 u(n-i)=a(n-i)*u(n-i+1)+b(n-i)
      return
   8  t=1.
      v=0.
      do 9 i=1,n1
      t=a(n-i)*t+c(n-i)
      v=a(n-i)*v+b(n-i)
      a(n-i)=t
   9  b(n-i)=v
      u(n)=(d(n)-c(n)*b(2)-a(n)*b(n-1))/(b(n)+c(n)*a(2)+a(n)*a(n-1))
      do 10 i=1,n1
  10  u(n-i)=a(n-i)*u(n)+b(n-i)
      return
   4  ier=2
      return
      end
