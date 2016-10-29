       subroutine cnctcc(c1,c2,r,n)
        common/gfgmc/c3(2),c4(2),c5(2),c6(2),c7(2),c8(2),c9(2),c10(2)
       common/gfgmp/p1(2),p2(2)
        dimension c1(3),c2(3)
       c1(3)=c1(3)+r
        c2(3)=c2(3)+r
        call picc(c1,c2,n1)
        c3(1)=p1(1)
        c3(2)=p1(2)
        c4(1)=p2(1)
        c4(2)=p2(2)
        c2(3)=c2(3)-2*r
         call picc(c1,c2,n2)
        c5(1)=p1(1)
        c5(2)=p1(2)
        c6(1)=p2(1)
         c6(2)=p2(2)
        c1(3)=c1(3)-2*r
       call picc(c1,c2,n3)
       c7(1)=p1(1)
      c7(2)=p1(2)
       c8(1)=p2(1)
       c8(2)=p2(2)
       c2(3)=c2(3)+2*r
       call picc(c1,c2,n4)
       c9(1)=p1(1)
       c9(2)=p1(2)
       c10(1)=p2(1)
       c10(2)=p2(2)
       n=n1+n2+n3+n4
       c1(3)=c1(3)+r
       c2(3)=c2(3)-r
       return
       end
