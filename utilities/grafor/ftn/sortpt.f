        subroutine sortpt(j,pt1,pt2)
        common/gfgmc/p1(2),p2(2),p3(2),p4(2),p5(2),p6(2),p7(2),p8(2)
        dimension pt1(2),pt2(2)
        if(j.le.0) goto 5
        goto (1,2,3,4),j
   1    pt1(1)=p1(1)
        pt1(2)=p1(2)
        pt2(1)=p2(1)
        pt2(2)=p2(2)
        goto 5
   2    pt1(1)=p3(1)
        pt1(2)=p3(2)
        pt2(1)=p4(1)
        pt2(2)=p4(2)
        goto 5
   3    pt1(1)=p5(1)
        pt1(2)=p5(2)
        pt2(1)=p6(1)
        pt2(2)=p6(2)
        goto 5
   4    pt1(1)=p7(1)
        pt1(2)=p7(2)
        pt2(1)=p8(1)
        pt2(2)=p8(2)
   5    return
        end
