      subroutine ctlpp(p1,p2,l1,n)
C........ha t  okpy hoct ,  poxo    e  epe   be  a ahh e to k
C........  kaca8  ec   a ahho   p mo
      common/gfgmc/c1(3),c2(3),c(10)
      dimension p1(2),p2(2),p3(2)
      real l1(3),l(3),l2(3)
      s1=l1(1)*p1(1)+l1(2)*p1(2)+l1(3)
      s2=l1(1)*p2(1)+l1(2)*p2(2)+l1(3)
      n=0
      if(abs(s1).lt.1.e-6.and.abs(s2).lt.1.e-6) return
      if(s1*s2.lt.0.) return
       n=1
      call lpp(p1,p2,l)
      p3(1)=(p1(1)+p2(1))/2.
      p3(2)=(p1(2)+p2(2))/2.
      call lperlp(l,p3,l2)
      b1=l1(2)*l1(2)
      b2=l1(1)*l1(1)
      b3=-2.*l1(1)*l1(2)
      b4=-2.*(p1(1)*(b1+b2)+l1(1)*l1(3))
      b5=-2.*(p1(2)*(b1+b2)+l1(2)*l1(3))
       b6=(b1+b2)*(p1(1)*p1(1)+p1(2)*p1(2))-l1(3)*l1(3)
       if(l2(2).eq.0.) go to 2
      if(l2(1).eq.0.) go to 3
      b7=-l2(2)/l2(1)
      b8=-l2(3)/l2(1)
      b9=b1*b7*b7+b3*b7+b2
      b10=2*b1*b7*b8+b3*b8+b4*b7+b5
      b11=b1*b8*b8+b4*b8+b6
      if(b9.eq.0.) go to 1
      a=b10*b10-4*b9*b11
      if(a.lt.0.) return
      c1(2)=(-b10-sqrt(a))/(2*b9)
      c1(1)=b7*c1(2)+b8
      if(abs(a).lt.1.e-6) go to 7
      n=2
      c2(2)=(-b10+sqrt(a))/(2*b9)
      c2(1)=b7*c2(2)+b8
      go to 6
 1    c1(2)=-b11/b10
      c1(1)=b7*c1(2)+b8
      go to 7
 2    c1(1)=-l2(3)/l2(1)
      if(l1(1).eq.0.) go to 5
      if(l1(2).eq.0.) go to 9
      b12=c1(1)*b3+b5
      b13=c1(1)*c1(1)*b1+c1(1)*b4+b6
      a=b12*b12-4*b2*b13
      if(a.lt.0.) return
      c1(2)=(-b12-sqrt(a))/(2*b2)
      if(abs(a).lt.1.e-6) go to 7
      c2(1)=c1(1)
      c2(2)=(-b12+sqrt(a))/(2*b2)
      n=2
      go to 6
 3    c1(2)=-l2(3)/l2(2)
      if(l1(2).eq.0.) go to 4
      if(l1(1).eq.0.) go to 8
      b14=c1(2)*b3+b5
      b15=c1(2)*c1(2)*b2+c1(2)*b4+b6
      a=b14*b14-4*b1*b15
      if(a.lt.0.) return
      c1(1)=(-b14-sqrt(a))/(2*b1)
      if(abs(a).lt.1.e-6) go to 7
      c2(2)=c1(2)
      c2(1)=(-b14+sqrt(a))/(2*b1)
      n=2
      goto 6
 4    c1(1)=-(c1(2)*c1(2)*b2+c1(2)*b4+b6)/b5
      go to 7
 5    c1(2)=-(c1(1)*c1(1)*b1+c1(1)*b4+b6)/b5
      go to 7
 6    c2(3)=sqrt((c2(1)-p1(1))**2+(c2(2)-p1(2))**2)
  7   c1(3)=sqrt((c1(1)-p1(1))**2+(c1(2)-p1(2))**2)
      return
  8   c1(1)=p3(1)
      go to 7
  9   c1(2)=p3(2)
      go to 7
      end
