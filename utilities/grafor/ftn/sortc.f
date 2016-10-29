       subroutine sortc(j,c)
       dimension c(3)
       common/gfgmc/c1(3),c2(3),c3(10)
       if(j.le.0) return
       go to (3,5,7,9),j
  3    if(c1(1)-c2(1)) 12,4,11
  4    if(c1(2)-c2(2)) 12,11,11
  5    if(c1(1)-c2(1)) 11,6,12
  6    if(c1(2)-c2(2)) 11,12,12
  7    if(c1(2)-c2(2)) 12,8,11
  8    if(c1(1)-c2(1)) 12,11,11
  9    if(c1(2)-c2(2)) 11,10,12
  10   if(c1(1)-c2(1)) 11,12,12
  11   c(1)=c1(1)
       c(2)=c1(2)
       c(3)=c1(3)
       return
  12   c(1)=c2(1)
       c(2)=c2(2)
       c(3)=c2(3)
       return
       end
