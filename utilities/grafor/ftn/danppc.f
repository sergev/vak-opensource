       subroutine danppc(p1,p2,pc,d,alpha)
       dimension p1(2),p2(2),pc(2)
       s=(p1(1)-p2(1))**2+(p1(2)-p2(2))**2
       r=(p1(1)-pc(1))**2+(p1(2)-pc(2))**2
       r1=sqrt(r)
       r2=sqrt((p2(1)-pc(1))**2+(p2(2)-pc(2))**2)
      if(abs(r1-r2).gt.1e-6) return
       h=r-s/4.
       if(abs(h).lt.1e-6) go to 2
       al=2.*atan(sqrt(s/h)/2.)
  1    d=al*sqrt(r)
       alpha=180./3.141593*al
       return
  2    al=3.141593
       go to 1
       end
