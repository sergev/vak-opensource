      subroutine picc(c1,c2,n)
       common/gfgmp/p1(2),p2(2)
       dimension c1(3),c2(3)
      r1=abs(c1(3))
      r2=abs(c2(3))
        d=sqrt((c2(1)-c1(1))**2+(c2(2)-c1(2))**2)
      n=0
      if(d.lt.1e-6.or.d.gt.r1+r2+1.e-6) return
      x=(r1**2-r2**2+d**2)/(2*d)
      st=(c2(2)-c1(2))/d
       ct=(c2(1)-c1(1))/d
       a=r1*r1-x*x
       if(abs(a).lt.1.e-5) goto 1
       if(a.lt.0.) return
       h=sqrt(a)
       n=2
      p1(1)=x*ct-h*st+c1(1)
      p1(2)=x*st+h*ct+c1(2)
       p2(1)=x*ct+h*st+c1(1)
       p2(2)=x*st-h*ct+c1(2)
       return
  1   n=1
        p1(1)=x*ct+c1(1)
       p1(2)=x*st+c1(2)
         p2(1)=p1(1)
         p2(2)=p1(2)
      return
       end
