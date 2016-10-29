        subroutine pvpl(l,p,d)
        common/gfgmp/p1(2),p2(2)
        real l(3),p(2)
        if(l(2).ne.0.) goto 10
        p1(1)=p(1)+d
        p1(2)=p(2)
        p2(1)=p(1)-d
        p2(2)=p(2)
        return
 10     if(l(1).ne.0.) goto 50
        p1(1)=p(1)
        p1(2)=p(2)+d
        p2(1)=p(1)
        p2(2)=p(2)-d
        return
  50    al=l(2)/l(1)
        be=p(2)-l(2)*p(1)/l(1)
        ga=1.+al**2
        de=al*be-al*p(2)-p(1)
        ep=p(1)**2+(be-p(2))**2-d**2
        h=de**2-ga*ep
        p1(1)=(-de+sqrt(h))/ga
        p1(2)=al*p1(1)+be
        p2(1)=(-de-sqrt(h))/ga
        p2(2)=al*p2(1)+be
        return
        end
