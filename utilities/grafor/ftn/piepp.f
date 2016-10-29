       subroutine piepp(p1,p2,a,b,k,p)
C......ha t  koop  hat  to k , otcto  e  ot  byx  a ahh x to ek
C......b  a ahhom otho eh   a k b
       dimension p1(2),p2(2),p(2)
       if(k.ne.1) go to 2
  1    p(1)=(a*p2(1)+b*p1(1))/(a+b)
       p(2)=(a*p2(2)+b*p1(2))/(a+b)
       goto 3
  2    if(a.eq.b) goto 3
       b=-b
       goto 1
  3    return
       end
