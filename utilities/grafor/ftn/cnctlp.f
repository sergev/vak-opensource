        subroutine cnctlp(l,p,r,n)
        common/gfgmp/pp1(2),pp2(2)
        real l(3),p(2),l1(3),p1(2),l2(3),c(3)
        call lperlp(l,p,l1)
        call pill(l,l1,p1,n1)
        r1=-r
        if(p1(1)-p(1)) 2,3,1
  1     call lparld(l,r1,l2)
      c(1)=p(1)
      c(2)=p(2)
      c(3)=r
        call pilc(l2,c,n)
        go to 4
  2     r1=r
        go to 1
  3     if(p(2)-p1(2)) 1,4,2
  4     return
        end
