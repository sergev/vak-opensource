       subroutine ctcpp(p1,p2,c,n)
       common/gfgmc/c1(3),c2(3),c3(10)
       dimension p1(2),p2(2),c(3)
       s1=(p1(1)-c(1))**2+(p1(2)-c(2))**2-c(3)*c(3)
       s2=(p2(1)-c(1))**2+(p2(2)-c(2))**2-c(3)*c(3)
       n=0
       if(abs(s1).lt.1e-6.and.abs(s2).lt.1e-6) return
       xm=(p1(1)+p2(1))/2.
       ym=(p1(2)+p2(2))/2.
       if(p1(2).ne.p2(2)) go to 2
       aa=2.*p1(2)-2.*c(2)
       aa=aa/2./c(3)
       bb=((2.*p1(1)-2.*c(1))*xm+c(1)*c(1)+c(2)*c(2)-p1(1)*p1(1)-
     *p1(2)*p1(2)+c(3)*c(3))/c(3)/2.
       ad=aa*aa-1.
       bd=aa*bb+c(2)
       cd=bb*bb-(xm-c(1))**2-c(2)*c(2)
       if(ad.ne.0.) go to 1
       n=1
       c1(1)=xm
       c1(2)=-cd/2./bd
       go to 8
  1    d=bd*bd-ad*cd
       if(d.lt.0.) return
       bt=bd/ad
       if(d.eq.0.) go to 5
       ct=cd/ad
       n=2
       c1(1)=xm
       c1(2)=-bt+sqrt(bt*bt-ct)
       c2(1)=xm
       c2(2)=-bt-sqrt(bt*bt-ct)
       go to 7
  2    a=(p1(1)-p2(1))/(p2(2)-p1(2))
       b=ym-a*xm
       aa=(2.*p1(1)-2.*c(1)+(2.*p1(2)-2.*c(2))*a)/2./c(3)
       bb=(c(1)*c(1)+c(2)*c(2)-p1(1)*p1(1)-p1(2)*p1(2)+c(3)*c(3)+
     *b*(2.*p1(2)-2.*c(2)))/2./c(3)
       ad=a*a+1.-aa*aa
       bd=c(1)-a*b+aa*bb+a*c(2)
       cd=c(1)*c(1)+b*b+c(2)*c(2)-bb*bb-2.*b*c(2)
       d=bd*bd-ad*cd
       if(d.lt.0.) return
       if(ad.ne.0.) go to 3
       n=1
       c1(1)=cd/2./bd
       c1(2)=a*c1(1)+b
       go to 8
  3    bt=bd/ad
       ct=cd/ad
       d=bt*bt-ct
       if(d.lt.0.) return
       if(d.eq.0.) go to 6
       n=2
       c1(1)=bt+sqrt(d)
       c1(2)=a*c1(1)+b
       c2(1)=bt-sqrt(d)
       c2(2)=a*c2(1)+b
       go to 7
 5     n=1
       c1(1)=xm
       c1(2)=-bt
       goto 8
  6    n=1
       c1(1)=bt
       c1(2)=a*c1(1)+b
       go to 8
  7    c2(3)=sqrt((c2(1)-p1(1))**2+(c2(2)-p1(2))**2)
  8    c1(3)=sqrt((c1(1)-p1(1))**2+(c1(2)-p1(2))**2)
       return
       end
