        subroutine pcdp(pc,p,d)
        common/gfgmp/p1(2),p2(2)
        dimension pc(2),p(2)
        r=sqrt((p(1)-pc(1))**2+(p(2)-pc(2))**2)
        sita1=d/r
        sita2=-d/r
        p1(1)=(p(1)-pc(1))*cos(sita1)-(p(2)-pc(2))*sin(sita1)+pc(1)
        p1(2)=(p(1)-pc(1))*sin(sita1)+(p(2)-pc(2))*cos(sita1)+pc(2)
        p2(1)=(p(1)-pc(1))*cos(sita2)-(p(2)-pc(2))*sin(sita2)+pc(1)
        p2(2)=(p(1)-pc(1))*sin(sita2)+(p(2)-pc(2))*cos(sita2)+pc(2)
        return
        end
