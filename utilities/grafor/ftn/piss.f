       subroutine piss(p1,p2,p3,p4,p,n)
C......ha t  koop  hat  to k   epece eh    byx  a ahh x otpe kob
        real p1(2),p2(2),p3(2),p4(2),p(2),l1(3),l2(3)
        call lpp(p1,p2,l1)
        call lpp(p3,p4,l2)
        call pill(l1,l2,p,n)
        return
        end
