      subroutine lpp(p1,p2,l)
       real      p1(2),p2(2),l(3)
       det=p2(1)*p1(2)-p1(1)*p2(2)
      if(abs(det).ge.1e-6) go to 3
      detx=p2(1)-p1(1)
      if(abs(detx).ge.1e-6) go to 1
      l(1)=1.
      l(2)=0.
      l(3)=0.
       return
  1   dety=p2(2)-p1(2)
      if(abs(dety).ge.1e-6) go to 2
      l(1)=0.
      l(2)=1.
      l(3)=0.
       return
  2   l(1)=1.
      l(2)=-detx/dety
       l(3)=0.
       return
  3   l(1)=(p2(2)-p1(2))/det
      l(2)=(p1(1)-p2(1))/det
      l(3)=1.
       return
      end
