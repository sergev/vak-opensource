        subroutine cncpp(p1,p2,r,n)
        common/gfgmp/pp1(2),pp2(2)
        dimension p1(2),p2(2),p3(2),c(3)
        real l1(3),l2(3)
        n=0
        if(p1(1).eq.p2(1).and.p1(2).eq.p2(2)) return
        p3(1)=(p1(1)+p2(1))/2.
        p3(2)=(p1(2)+p2(2))/2.
        call lpp(p1,p2,l1)
        call lperlp(l1,p3,l2)
      c(1)=p1(1)
      c(2)=p1(2)
      c(3)=r
        call pilc(l2,c,n)
        return
        end
