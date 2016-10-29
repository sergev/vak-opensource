      subroutine pill(l1,l2,p,n)
C..........to ka (p),  o y ehha   p   epece eh    byx  p m x (l1   l2)
C..........n=0 - het to k   epece eh   ( p m e  apa  e  h )
C..........n=1 - o ha to ka  epece eh
      real l1(3),l2(3),p(2)
      d=l1(2)*l2(1)-l2(2)*l1(1)
      n=0
      if(abs(d).lt.1e-6) return
      n=1
      p(1)=(l1(3)*l2(2)-l2(3)*l1(2))/d
      p(2)=(l2(3)*l1(1)-l1(3)*l2(1))/d
      return
      end
