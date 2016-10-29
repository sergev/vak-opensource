      subroutine pcnitc(p1,p2,p3,pt1,pt2,pt3,c)
C...........ha t  koop  hat   ehtpa   pa  yc b  cahho  b tpey"o  h k
C...........okpy hoct    to k  kacah   okpy hoct  co ctopoham
C...........tpey"o  h ka, ec     becth  bep  h  tpey"o  h ka
C...........pcn -  ehtp okpy hoct
C...........pt1 - t.kacah   okp-t  co ctopoho  tpey"o  h ka,  poxo   e
C........... epe  t. p1   p2
C...........pt2 -  epe  t. p2   p3
C...........pt3 -  epe  t.p1   p3
        dimension p1(2),p2(2),p3(2),  c(3),pt1(2),pt2(2),pt3(2),p23(2)
        dimension p13(2),p(2),pp(2),pt(2)
        real l1(3),l2(3),l(3),ll(3),l10(3)
       if((p2(1)-p1(1))*(p3(2)-p1(2))-(p3(1)-p1(1))*(p2(2)-p1(2)).
     *lt.1e-6) return
        d1=sqrt((p2(1)-p3(1))**2+(p2(2)-p3(2))**2)
        d2=sqrt((p1(1)-p3(1))**2+(p1(2)-p3(2))**2)
        d3=sqrt((p1(1)-p2(1))**2+(p1(2)-p2(2))**2)
        p23(1)=(d2*p2(1)+d3*p3(1))/(d2+d3)
        p23(2)=(d2*p2(2)+d3*p3(2))/(d2+d3)
        p13(1)=(d1*p1(1)+d3*p3(1))/(d1+d3)
        p13(2)=(d1*p1(2)+d3*p3(2))/(d1+d3)
        call lpp(p1,p23,l1)
        call lpp(p2,p13,l2)
        call pill(l1,l2, c ,n)
        k=1
        p(1)=p1(1)
        p(2)=p1(2)
        pp(1)=p2(1)
        pp(2)=p2(2)
  1     call lpp(p,pp,l)
        call lperlp(l, c ,ll)
        call pill(l,ll,pt,n)
        if(k.ne.1) go to 2
        pt1(1)=pt(1)
        pt1(2)=pt(2)
        p(1)=p3(1)
        p(2)=p3(2)
        k=2
        go to 1
  2     if(k.ne.2) go to 3
        pt2(1)=pt(1)
        pt2(2)=pt(2)
        k=3
        pp(1)=p1(1)
        pp(2)=p1(2)
        goto 1
  3     pt3(1)=pt(1)
        pt3(2)=pt(2)
        call lpp(p1,p2,l10)
        s=sqrt(l10(1)*l10(1)+l10(2)*l10(2))
       c(3)=abs(l10(1)*c(1)+l10(2)*c(2)+l10(3))/s
        return
        end
