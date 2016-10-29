       subroutine pac(c,a,p)
C............. a ah e to k  (p) ha okpy hoct  (c)  p   a ahhom y" e (a)
       dimension c(3),p(2)
      th=a*0.01745329
       p(1)=c(1)+c(3)*cos(th)
       p(2)=c(2)+c(3)*sin(th)
       return
       end
