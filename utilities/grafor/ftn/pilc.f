       subroutine pilc(l,c,n)
      common/gfgmp/p1(2),p2(2)
      real      l(3),c(3)
      s=sqrt(l(1)**2+l(2)**2)
      if(s.eq.0.) go to 3
      p=(l(1)*c(1)+l(2)*c(2)+l(3))/s
       st=l(1)/s
      ct=-l(2)/s
      b=abs(p)
      if(abs(b-c(3)).lt.1e-6) goto 1
      if(b-c(3)) 2,1,3
  1   n=1
      p1(1)=-p*st+c(1)
      p1(2)=p*ct+c(2)
       p2(1)=p1(1)
        p2(2)=p1(2)
      return
  2   n=2
      x=sqrt(c(3)**2-p**2)
      p1(1)=x*ct-p*st+c(1)
      p1(2)=x*st+p*ct+c(2)
      p2(1)=-x*ct-p*st+c(1)
       p2(2)=-x*st+p*ct+c(2)
       return
  3   n=0
      return
      end
