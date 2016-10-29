      subroutine lperlp(l1,p,l2)
      real l1(3),p(2),l2(3)
      if(l1(1).eq.0.and.l1(2).eq.0.) return
      if(l1(2)) 1,3,2
  1   l2(1)=-l1(2)
      l2(2)=l1(1)
      l2(3)=l1(2)*p(1)-l1(1)*p(2)
      return
  2   l2(1)=l1(2)
      l2(2)=-l1(1)
      l2(3)=l1(1)*p(2)-l1(2)*p(1)
      return
  3   if(l1(1).gt.0.) go to 1
      go to 2
      end
