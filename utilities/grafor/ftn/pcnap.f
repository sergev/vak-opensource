       subroutine pcnap(pc,p1,alpha,p)
C...........ha t  koop  hat  to k  ha okpy hoct  c   becth m pa  ycom,
C...........coctab  8 e  o pe e . y"o  c  a ahho  to ko  ha okpy hoct
       dimension pc(2),p1(2),p(2)
       a=3.141593/180.*alpha
       p(1)=(p1(1)-pc(1))*cos(a)-(p1(2)-pc(2))*sin(a)+pc(1)
       p(2)=(p1(1)-pc(1))*sin(a)+(p1(2)-pc(2))*cos(a)+pc(2)
       return
       end
