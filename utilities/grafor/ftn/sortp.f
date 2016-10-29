       subroutine sortp(j,p)
       dimension p(2)
       common/gfgmp/p1(2),p2(2)
       if(j.le.0) goto 11
       go to (1,3,5,7),j
  1    if(p1(1)-p2(1)) 10,2,9
  2    if(p1(2)-p2(2)) 10,9,9
  3    if(p1(1)-p2(1)) 9,4,10
  4    if(p1(2)-p2(2)) 9,10,10
  5    if(p1(2)-p2(2)) 10,6,9
  6    if(p1(1)-p2(1)) 10,9,9
  7    if(p1(2)-p2(2)) 9,8,10
  8    if(p1(1)-p2(1)) 9,10,10
  9    p(1)=p1(1)
       p(2)=p1(2)
       goto 11
  10   p(1)=p2(1)
       p(2)=p2(2)
 11    return
       end
