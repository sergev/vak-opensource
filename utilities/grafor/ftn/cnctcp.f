       subroutine cnctcp(c,p,r,n)
       common/gfgmc/c1(2),c2(2),c3(2),c4(2),b(8)
       common/gfgmp/p1(2),p2(2)
       dimension c(3),p(2),cr(3)
      cr(1)=p(1)
      cr(2)=p(2)
       cr(3)=r
       c(3)=c(3)+r
       call picc(cr,c,n1)
       c1(1)=p1(1)
       c1(2)=p1(2)
       c2(1)=p2(1)
       c2(2)=p2(2)
       c(3)=c(3)-2*r
       call picc(cr,c,n2)
       c3(1)=p1(1)
        c3(2)=p1(2)
       c4(1)=p2(1)
       c4(2)=p2(2)
       n=n1+n2
       c(3)=c(3)+r
        return
        end
