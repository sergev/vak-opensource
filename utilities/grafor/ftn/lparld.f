      subroutine lparld(l1,d,l2)
      real l1(3),l2(3)
      if(l1(1).eq.0.and.l1(2).eq.0.) return
      th=sqrt(l1(1)*l1(1)+l1(2)*l1(2))
       if(l1(1)) 2,3,1
  1   l2(1)=l1(1)
       l2(2)=l1(2)
       l2(3)=l1(3)-d*th
      d=abs(d)
      return
  2   l2(1)=-l1(1)
      l2(2)=-l1(2)
      l2(3)=-l1(3)-d*th
      d=abs(d)
      return
  3   if(l1(2).lt.0.) go to 2
      go to 1
      end
