       subroutine sortl(l,j,c)
       common/gfgmc/c1(3),c2(3),c3(3),c4(3),q(4)
       dimension c(3),pt(2,4),dx(2,4),dy(2,4)
       real l(3)
       pt(1,1)=c1(1)
       pt(2,1)=c1(2)
       pt(1,2)=c2(1)
       pt(2,2)=c2(2)
       pt(1,3)=c3(1)
       pt(2,3)=c3(2)
       pt(1,4)=c4(1)
       pt(2,4)=c4(2)
       kp=j
       call sort3(pt,dx,1)
       do 3 i=1,2
       do 2 k=1,4
       pt(i,k)=dx(i,k)
  2    continue
  3    continue
       call sort3(pt,dy,2)
       if(kp.ge.5) go to 4
       c(1)=dx(1,kp)
       c(2)=dx(2,kp)
       go to 5
  4    kp=kp-4
       c(1)=dy(1,kp)
       c(2)=dy(2,kp)
 5     s=sqrt(l(1)*l(1)+l(2)*l(2))
       c(3)=abs(l(1)*c(1)+l(2)*c(2)+l(3))/s
       return
       end
