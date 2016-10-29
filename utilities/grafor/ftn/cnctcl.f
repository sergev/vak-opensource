       subroutine cnctcl(c,l,r,n)
       common/gfgmc/c1(2),c2(2),c3(2),c4(2),c5(2),c6(2),c7(2),c8(2)
C...... epb e 4 okp-t  b common-  oke - kaca8  ec  okpy hoct
C......chapy  ,  oc e h e - bhytp
       common/gfgmp/p1(2),p2(2)
       real l(3),c(3),l1(3),l2(3)
       c(3)=c(3)+r
       call lparld(l,r,l1)
       call lparld(l,-r,l2)
       call pilc(l1,c,n1)
        c1(1)=p1(1)
        c1(2)=p1(2)
       c2(1)=p2(1)
       c2(2)=p2(2)
       call pilc(l2,c,n2)
        c3(1)=p1(1)
        c3(2)=p1(2)
        c4(1)=p2(1)
        c4(2)=p2(2)
       c(3)=c(3)-2*r
       call pilc(l1,c,n3)
       c5(1)=p1(1)
       c5(2)=p1(2)
       c6(1)=p2(1)
       c6(2)=p2(2)
       call pilc(l2,c,n4)
       c7(1)=p1(1)
       c7(2)=p1(2)
       c8(1)=p2(1)
       c8(2)=p2(2)
       n=n1+n2+n3+n4
       c(3)=c(3)+r
       return
       end
