       subroutine ctccp(c1,c2,p,n)
C......okpy hoct , kaca8 a c   byx  a ahh x okpy hocte     poxo   a
C...... epe   a ahhy8 to ky
       dimension c1(3),c2(3),p(2)
       common/gfgmc/c3(3),c4(3),c(10)
       s1=(p(1)-c1(1))**2+(p(2)-c1(2))**2-c1(3)**2
       s2=(p(1)-c2(1))**2+(p(2)-c2(2))**2-c2(3)**2
       n=0
       if(abs(s1).lt.1.e-6.and.abs(s2).lt.1.e-6) return
       c1(1)=c1(1)-p(1)
       c1(2)=c1(2)-p(2)
       c2(1)=c2(1)-p(1)
       c2(2)=c2(2)-p(2)
       b1=2*c2(1)*c1(3)-2*c1(1)*c2(3)
       b2=2*c2(2)*c1(3)-2*c1(2)*c2(3)
       b3=c2(3)*(c1(1)**2+c1(2)**2-c1(3)**2)-c1(3)*(c2(1)**2+c2(2)**2-
     *c2(3)**2)
       if(b1.eq.0.) go to 1
       b4=-b2/b1
       b5=-b3/b1
       b6=(-c1(1)*b4-c1(2))/c1(3)
       b7=(-2*c1(1)*b5+c1(1)**2+c1(2)**2-c1(3)**2)/(2*c1(3))
       b8=b6*b6-b4*b4-1
       b9=2*b6*b7-2*b4*b5
       b10=b7*b7-b5*b5
       if(b8.eq.0.) go to 4
       a=b9*b9-4*b8*b10
       if(abs(a).lt.1.e-6) go to 5
       if(a.lt.0.) return
       n=2
       c3(2)=(-b9+sqrt(a))/(2*b8)
       c3(1)=b4*c3(2)+b5
       c4(2)=(-b9-sqrt(a))/(2*b8)
       c4(1)=b4*c4(2)+b5
       go to 6
  1    c3(2)=-b3/b2
       b11=-c1(1)/c1(3)
       b12=(-2*c1(2)*c3(2)+c1(1)**2+c1(2)**2-c1(3)**2)/(2*c1(3))
       b13=b11*b11-1
       b14=2*b11*b12
       b15=b12*b12-c3(2)**2
       if(b13.eq.0.) go to 2
       a1=b14*b14-4*b13*b15
       if(abs(a1).lt.1.e-6) go to 3
       if(a1.lt.0.) go to 6
       n=2
       c4(2)=c3(2)
       c3(1)=(-b14+sqrt(a1))/(2*b13)
       c4(1)=(-b14-sqrt(a1))/(2*b13)
       go to 6
 2     n=1
       c3(1)=-b15/b14
       go to 7
 3    n=1
       c3(1)=-b14/(2*b13)
       go to 7
 4    n=1
       c3(2)=-b10/b9
       c3(1)=b4*c3(2)+b5
       go to 7
 5     n=1
       c3(2)=-b9/(2*b8)
       c3(1)=b4*c3(2)+b5
       goto 7
 6     c4(3)=sqrt(c4(1)*c4(1)+c4(2)*c4(2))
        c4(1)=c4(1)+p(1)
        c4(2)=c4(2)+p(2)
  7     c3(3)=sqrt(c3(1)*c3(1)+c3(2)*c3(2))
        c3(1)=c3(1)+p(1)
        c3(2)=c3(2)+p(2)
        c1(1)=c1(1)+p(1)
        c1(2)=c1(2)+p(2)
        c2(1)=c2(1)+p(1)
        c2(2)=c2(2)+p(2)
        return
        end
