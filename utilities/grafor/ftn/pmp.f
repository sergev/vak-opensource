       subroutine pmp(p1,p2,j)
C......ha t  koop  hat  to k , c mmetp  ho   ahho  othoc te  ho
C......ha a a koop  hat     oce
C......j -  p  hak b  opa c mmetp  ho  to k
C......j=0 - o otho eh 8 k ha a y koop  hat
C......j=1 -  o otho eh 8 k oc  x
C......j=2 -  o otho eh 8 k oc  y
       dimension p1(2),p2(2)
       if(j-1)1,2,3
  1    p2(1)=-p1(1)
       p2(2)=-p1(2)
       goto 4
  2    p2(1)=p1(1)
       p2(2)=-p1(2)
       goto 4
  3    p2(1)=-p1(1)
       p2(2)=p1(2)
  4    return
       end
