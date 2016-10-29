       subroutine cnctll(l1,l2,r,n)
       common/gfgmc/c1(2),c2(2),c3(2),c4(2),c(8)
       real l1(3),l2(3),l3(3),l4(3),l5(3),l6(3)
       r1=r
       r2=-r
       call lparld(l1,r1,l3)
       call lparld(l2,r1,l4)
       call pill(l3,l4,c1,n1)
       call lparld(l2,r2,l5)
       call lparld(l1,-r2,l6)
       call pill(l3,l5,c2,n2)
       call pill(l4,l6,c3,n3)
       call pill(l6,l5,c4,n4)
       n=n1+n2+n3+n4
       return
       end
