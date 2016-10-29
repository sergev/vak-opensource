      subroutine cntlll(l1,l2,l3,m)
        common/gfgmc/c1(3),c2(3),c3(3),c4(3),q(4)
       real l1(3),l2(3),l3(3),l6(3)
       real pp1(3),pp2(3),l4(3),l5(3),lp(3),lpar(3)
       dimension p12(3),p13(3),p23(3),p(2)
       call pill(l1,l2,p12,n12)
       call pill(l1,l3,p13,n13)
       call pill(l2,l3,p23,n23)
       if(n12.eq.0.or.n13.eq.0.or.n23.eq.0) goto 9
        m=0
        d1=l1(1)*l2(2)*l3(3)
        d2=l2(1)*l3(2)*l1(3)
        d3=l3(1)*l1(2)*l2(3)
        d4=l3(1)*l2(2)*l1(3)
        d5=l2(1)*l1(2)*l3(3)
        d6=l1(1)*l2(3)*l3(2)
        d=d1+d2+d3-(d4+d5+d6)
        if(d.eq.0.) return
        m=4
        ab1=sqrt(l1(1)*l1(1)+l1(2)*l1(2))
        ab2=sqrt(l2(1)*l2(1)+l2(2)*l2(2))
        ab3=sqrt(l3(1)*l3(1)+l3(2)*l3(2))
        al1=l1(1)/ab1+l2(1)/ab2
        al2=l1(1)/ab1-l2(1)/ab2
        al3=l1(1)/ab1+l3(1)/ab3
        al4=l1(1)/ab1-l3(1)/ab3
        be1=l1(2)/ab1+l2(2)/ab2
        be2=l1(2)/ab1-l2(2)/ab2
        be3=l1(2)/ab1+l3(2)/ab3
        be4=l1(2)/ab1-l3(2)/ab3
        ga1=l1(3)/ab1+l2(3)/ab2
        ga2=l1(3)/ab1-l2(3)/ab2
        ga3=l1(3)/ab1+l3(3)/ab3
        ga4=l1(3)/ab1-l3(3)/ab3
        div1=al1*be3-al3*be1
        xx1=be1*ga3-be3*ga1
        yy1=ga1*al3-ga3*al1
        div2=al2*be3-al3*be2
        xx2=be2*ga3-be3*ga2
        yy2=ga2*al3-ga3*al2
        div3=al1*be4-al4*be1
        xx3=be1*ga4-be4*ga1
        yy3=ga1*al4-ga4*al1
        div4=al2*be4-al4*be2
        xx4=be2*ga4-be4*ga2
        yy4=ga2*al4-ga4*al2
        c1(1)=xx1/div1
        c1(2)=yy1/div1
        c2(1)=xx2/div2
        c2(2)=yy2/div2
        c3(1)=xx3/div3
        c3(2)=yy3/div3
        c4(1)=xx4/div4
        c4(2)=yy4/div4
        return
 9     m=2
      if(n12.eq.0) go to 12
       if(n13.eq.0) go to 14
       do 10 i=1,3
       lpar(i)=l3(i)
       lp(i)=l1(i)
       pp1(i)=p12(i)
  10  pp2(i)=p13(i)
  11  s=sqrt(lpar(1)*lpar(1)+lpar(2)*lpar(2))
       d=abs((lpar(1)*pp1(1)+lpar(2)*pp1(2)+lpar(3))/s)
       p(1)=(pp1(1)+pp2(1))/2.
       p(2)=(pp1(2)+pp2(2))/2.
       call lparlp(lpar,p,l4)
       call lparld(lp,d/2.,l5)
       call pill(l4,l5,c1,n)
       call lparld(lp,-d/2.,l6)
       call pill(l4,l6,c2,n)
       c2(3)=d/2.
       c1(3)=d/2.
       return
  12  do 13 i=1,3
       lpar(i)=l2(i)
       lp(i)=l3(i)
       pp1(i)=p13(i)
  13  pp2(i)=p23(i)
       goto 11
  14  do 15 i=1,3
       lpar(i)=l3(i)
       lp(i)=l2(i)
       pp1(i)=p12(i)
  15  pp2(i)=p23(i)
       goto 11
       end
