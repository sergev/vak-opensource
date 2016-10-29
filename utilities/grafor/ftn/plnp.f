       subroutine plnp(p1,l,p,d)
      real l(3),p1(2),p(2),l1(3)
      call lperlp(l,p1,l1)
      s=sqrt(l(1)*l(1)+l(2)*l(2))
      d=abs((l(1)*p1(1)+l(2)*p1(2)+l(3))/s)
      if(d.lt.1.e-6) go to 1
      a=l1(2)*l(1)-l(2)*l1(1)
      p(1)=(l1(3)*l(2)-l(3)*l1(2))/a
      p(2)=(l(3)*l1(1)-l1(3)*l(1))/a
      return
  1   p(1)=p1(1)
      p(2)=p1(2)
      return
      end
