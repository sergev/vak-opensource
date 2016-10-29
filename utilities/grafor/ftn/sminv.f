      subroutine sminv(a,v,n)
C...subroutine     sminv     01/09/73
C...subroutine     sminv     01/09/78
      dimension a(n),v(n)
      np1=n+1
      nb1=n-1
      nn=n*n
      do 7 k=1,n
      if (a(1).eq.0.) goto 9
      p=1/a(1)
      do 5 l=1,nb1
 5    v(l)=a(l*n+1)
      do 6 i=1,nb1
      a(i+nb1*n)=-v(i)*p
      y=a(i+nb1*n)
      do 6 j=1,nb1
 6    a(j*n+i-n)=a(j*n+i-n+np1)+v(j)*y
 7    a(nn)=-p
      do 8 i=1,n
      do 8 j=i,n
      a(j*n+i-n)=-a(j*n+i-n)
8     a(j+(i-1)*n)=a(j*n+i-n)
 9    return
      end
