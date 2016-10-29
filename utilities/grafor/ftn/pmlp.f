       subroutine pmlp(l,p1,p2)
C.............to ka (p1), c mmetp  ha  to ke (p2)othoc te  ho  p mo  l
      real l(3),p1(2),p2(2)
      d=l(1)*l(1)+l(2)*l(2)
      p2(1)=(-2.*l(3)*l(1)-(l(1)*l(1)-l(2)*l(2))*p1(1)-
     12.*l(1)*l(2)*p1(2))/d
      p2(2)=(-2.*l(2)*l(3)-2.*l(1)*l(2)*p1(1)+(l(1)*l(1)-l(2)*l(2))*
     1p1(2))/d
      return
      end
