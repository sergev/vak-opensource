       subroutine sortcn(m,j,c)
       common/gfgmc/c1(2),c2(2),c3(2),c4(2),c5(2),c6(2),c7(2),c8(2)
       dimension c(2),pt(2,4),dx(2,4),dy(2,4)
       if(m.ne.1) go to 5
       pt(1,1)=c1(1)
       pt(2,1)=c1(2)
       pt(1,2)=c2(1)
       pt(2,2)=c2(2)
       pt(1,3)=c3(1)
       pt(2,3)=c3(2)
       pt(1,4)=c4(1)
       pt(2,4)=c4(2)
  1    kp=j
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
       return
  4    kp=kp-4
       c(1)=dy(1,kp)
       c(2)=dy(2,kp)
       return
 5     pt(1,1)=c5(1)
       pt(2,1)=c5(2)
       pt(1,2)=c6(1)
       pt(2,2)=c6(2)
       pt(1,3)=c7(1)
       pt(2,3)=c7(2)
       pt(1,4)=c8(1)
       pt(2,4)=c8(2)
       go to 1
       end
