      subroutine ptlcc(c1,c2,n)
C............. n -   c o kacate  h x  p m x
      dimension c1(3),c2(3)
       common/gfgmc/p1(2),p2(2),p3(2),p4(2),p5(2),p6(2),p7(2),p8(2)
      delx=c2(1)-c1(1)
      dely=c2(2)-c1(2)
        d=sqrt(delx*delx+dely*dely)
      r1=abs(c1(3))
      r2=abs(c2(3))
      st=dely/d
      ct=delx/d
       if(d-abs(r1-r2)) 8,6,1
C.............bhe hee kacah e
  1    s=sqrt(d*d-(r1-r2)*(r1-r2))
      th=atan2(s,r1-r2)
      x1=r1*cos(th)
      h1=r1*sin(th)
      x2=r2*cos(th)+d
      h2=r2*sin(th)
      p1(1)=x1*ct-h1*st+c1(1)
      p1(2)=x1*st+h1*ct+c1(2)
      p2(1)=x2*ct-h2*st+c1(1)
      p2(2)=x2*st+h2*ct+c1(2)
      p3(1)=x1*ct+h1*st+c1(1)
      p3(2)=x1*st-h1*ct+c1(2)
      p4(1)=x2*ct+h2*st+c1(1)
      p4(2)=x2*st-h2*ct+c1(2)
      if(d-(r1+r2)) 4,3,2
  2       d1=(d*r1)/(r1+r2)
        d2=d-d1
      x3=r1*r1/d1
      h3=sqrt(r1*r1-x3*x3)
       x4=r2*r2/d2
      h4=-sqrt(r2*r2-x4*x4)
        x4=d-x4
       p5(1)=x3*ct-h3*st+c1(1)
       p5(2)=x3*st+h3*ct+c1(2)
       p6(1)=x4*ct-h4*st+c1(1)
      p6(2)=x4*st+h4*ct+c1(2)
      p7(1)=x3*ct+h3*st+c1(1)
       p7(2)=x3*st-h3*ct+c1(2)
      p8(1)=x4*ct+h4*st+c1(1)
      p8(2)=x4*st-h4*ct+c1(2)
      n=4
       goto 11
  3    p5(1)=r1*ct+c1(1)
       p5(2)=r1*st+c1(2)
       p6(1)=p5(1)
       p6(2)=p5(2)
       n=3
      goto 11
  4     n=2
        goto 11
  6     p1(1)=r1*ct+c1(1)
        p1(2)=r1*st+c1(2)
       p2(1)=p1(1)
       p2(2)=p1(2)
        n=1
        goto 11
  8     n=0
  11    return
        end
