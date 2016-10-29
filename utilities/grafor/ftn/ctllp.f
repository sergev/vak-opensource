        subroutine ctllp(l1,l2,p,n)
C.........ha t  okpy hoct , kaca8  ec   byx  a ahh x  p m x
C......... poxo    e  epe   a ahhy8 to ky
        common/gfgmc/c1(3),c2(3),c(10)
        common/gfgmp/pp1(2),pp2(2)
         real l(3),ll(3),l3(3),l4(3),l5(3)
         dimension p1(2),p2(2),p3(2)
        real l1(3),l2(3),p(2),c3(3),c4(2)
        a13=l1(1)*p(1)+l1(2)*p(2)+l1(3)
        a23=l2(1)*p(1)+l2(2)*p(2)+l2(3)
        d=l1(1)*l2(2)-l2(1)*l1(2)
        if(abs(a13).lt.1.e-6) goto 16
        if(abs(a23).lt.1.e-6) goto 19
        b1=sqrt(l1(1)*l1(1)+l1(2)*l1(2))
        b2=sqrt(l2(1)*l2(1)+l2(2)*l2(2))
        n=0
        k1=0
        k2=0
        k3=0
        k4=0
        if(l1(2).eq.0.) goto 12
        s1=-l1(1)/l1(2)
        y1=s1*p(1)-l1(3)/l1(2)
        if(y1.gt.p(2)) k2=1
         if(abs(s1).lt.1.e-6) goto 14
         if(s1.lt.0.) k1=1
  1     if(l2(2).eq.0.) goto 13
        s2=-l2(1)/l2(2)
        y2=s2*p(1)-l2(3)/l2(2)
       if(y2.gt.p(2)) k4=1
         if(abs(s2).lt.1.e-6) goto 15
         if(s2.lt.0.) k3=1
  2     m=k1+k2+k3+k4
        if(m.eq.1.or.m.eq.3) go to 9
        b3=l1(1)*b2-l2(1)*b1
        b4=l1(2)*b2-l2(2)*b1
        b5=a13*b2-a23*b1
  3     if(b3.eq.0..and.b4.eq.0.) return
        if(b3.eq.0.) go to 6
        b6=-b4/b3
        b7=-b5/b3
        b8=l1(1)*b6+l1(2)
        b9=l1(1)*b7+a13
        b10=b8*b8-b1*b1*(b6*b6+1)
        b11=2*b8*b9-2*b1*b1*b6*b7
        b12=b9*b9-b1*b1*b7*b7
        if(b10.eq.0.) go to 5
        a=b11*b11-4*b10*b12
        if(abs(a).lt.1.e-6) go to 4
        if(a.lt.0.) return
        n=2
        c3(2)=(-b11+sqrt(a))/(2*b10)
        c3(1)=b6*c3(2)+b7
        c4(2)=(-b11-sqrt(a))/(2*b10)
        c4(1)=b6*c4(2)+b7
        goto 10
  4     n=1
        c3(2)=-b11/(2*b10)
        c3(1)=b6*c3(2)+b7
        goto 11
  5     n=1
        c3(2)=-b12/b11
        c3(1)=b6*c3(2)+b7
        goto 11
  6     c3(2)=-b5/b4
        f1=l1(1)*l1(1)-b1*b1
        f2=2*l1(1)*(l1(2)*c3(2)+a13)
        f3=c3(2)*c3(2)*(l1(2)*l1(2)-b1*b1)+a13*(2*l1(2)*c3(2)+a13)
        if(f1.eq.0.) goto 7
        r=f2*f2-4*f1*f3
        if(abs(r).lt.1.e-6) goto 8
        if(r.lt.0.) return
        n=2
        c3(1)=(-f2+sqrt(r))/(2*f1)
        c4(2)=c3(2)
        c4(1)=(-f2-sqrt(r))/(2*f1)
        go to 10
  7     n=1
        c3(1)=-f3/f2
        goto 11
  8     n=1
        c3(1)=-f2/(2*f1)
        goto 11
  9     b3=l1(1)*b2+l2(1)*b1
        b4=l1(2)*b2+l2(2)*b1
        b5=a13*b2+a23*b1
        goto 3
  10    c2(3)=sqrt(c4(1)**2+c4(2)**2)
        c2(1)=c4(1)+p(1)
        c2(2)=c4(2)+p(2)
  11    c1(3)=sqrt(c3(1)**2+c3(2)**2)
        c1(1)=c3(1)+p(1)
        c1(2)=c3(2)+p(2)
        return
 12     x1=-l1(3)/l1(1)
        if(x1.lt.p(1)) k2=1
        go to 1
 13     x2=-l2(3)/l2(1)
        if(x2.lt.p(1)) k4=1
        goto 2
 14     k1=1
        goto 1
 15     k3=1
        goto 2
  16     do 17 i=1,3
         l(i)=l2(i)
  17     ll(i)=l1(i)
         if(abs(d).lt.1.e-6) goto 21
  18     n=2
         call pill(l1,l2,p1,k)
         c3(1)=p1(1)
         c3(2)=p1(2)
         c3(3)=sqrt((p1(1)-p(1))**2+(p1(2)-p(2))**2)
         call pilc(l,c3,k)
         p2(1)=pp1(1)
         p2(2)=pp1(2)
         p3(1)=pp2(1)
         p3(2)=pp2(2)
          call lperlp(l,p2,l3)
          call lperlp(l,p3,l4)
          call lperlp(ll,p,l5)
          call pill(l3,l5,c1,k)
          call pill(l4,l5,c2,k)
          c1(3)=sqrt((c1(1)-p(1))**2+(c1(2)-p(2))**2)
          c2(3)=sqrt((c2(1)-p(1))**2+(c2(2)-p(2))**2)
          return
  19      do 20 i=1,3
          l(i)=l1(i)
  20      ll(i)=l2(i)
          if(abs(d).gt.1.e-6) goto 18
  21      n=1
          call lperlp(ll,p,l3)
          call pill(l3,l,p3,k)
          c1(1)=(p(1)+p3(1))/2.
          c1(2)=(p(2)+p3(2))/2.
          c1(3)=sqrt((c1(1)-p(1))**2+(c1(2)-p(2))**2)
          return
          end
