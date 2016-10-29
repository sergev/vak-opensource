       subroutine pldp(l,pp,d)
        common/gfgmp/p1(2),p2(2)
        real l(3),p(2) ,pp(2),l1(3)
       l1(1)=-l(2)
       l1(2)=l(1)
       l1(3)=l(2)*pp(1)-l(1)*pp(2)
       s=l1(2)*l(1)-l(2)*l1(1)
       p(1)=(l1(3)*l(2)-l(3)*l1(2))/s
       p(2)=(l(3)*l1(1)-l1(3)*l(1))/s
        if(l(2).eq.0.) goto 20
        alp=l(1)**2+l(2)**2
        bet=l(1)*l(3)+l(1)*l(2)*p(2)-l(2)**2*p(1)
        gam=l(2)**2*p(1)**2+(l(3)+l(2)*p(2))**2-l(2)**2*d**2
        e=bet**2-alp*gam
        p1(1)=(-bet+sqrt(e))/alp
        p1(2)=-(l(1)*p1(1)+l(3))/l(2)
        p2(1)=(-bet-sqrt(e))/alp
        p2(2)=-(l(1)*p2(1)+l(3))/l(2)
        return
  20    f=d**2-(l(3)/l(1)+p(1))**2
        p1(1)=-l(3)/l(1)
        p1(2)=p(2)-sqrt(f)
        p2(1)=-l(3)/l(1)
        p2(2)=p(2)+sqrt(f)
        return
         end
