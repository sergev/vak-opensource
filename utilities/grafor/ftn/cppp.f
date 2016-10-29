      subroutine cppp(p1,p2,p3,c)
C............. octpoeh e okpy hoct  (c)  o tpem to kam (p1,p2,p3)
      dimension p1(2),p2(2),p3(2),c(3)
      if((p2(1)-p1(1))*(p3(2)-p1(2))-(p3(1)-p1(1))*(p2(2)-p1(2)).lt.
     *1e-6) return
       xm1=(p1(1)+p2(1))/2.
      ym1=(p1(2)+p2(2))/2.
      xm2=(p2(1)+p3(1))/2.
      ym2=(p2(2)+p3(2))/2.
      a=p1(1)-p2(1)
      b=p1(2)-p2(2)
      q=b*ym1+a*xm1
      d=p2(1)-p3(1)
      e=p2(2)-p3(2)
      f=e*ym2+d*xm2
       dv=a*e-b*d
         if(dv.eq.0.) goto 1
         c(1)=(q*e-b*f)/dv
         c(2)=(a*f-q*d)/dv
       c(3)=sqrt((p1(1)-c(1))**2+(p1(2)-c(2))**2)
      return
  1       c(1)=9999999.
          return
      end
