       subroutine prap(p1,r,a,p2)
C............. a ah e to k  (p2) othoc te  ho to k  (p1)  p   a ahh x
C.............y" e (a)   pa  yce (r)
       dimension p1(2),p2(2)
       th=a*0.01745329
       p2(1)=p1(1)+r*cos(th)
       p2(2)=p1(2)+r*sin(th)
       return
       end
