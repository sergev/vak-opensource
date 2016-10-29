      subroutine ptlpc(c,p,n)
      common/gfgmp/p1(2),p2(2)
      dimension p(2),c(3)
      r=abs(c(3))
      delx=c(1)-p (1)
      dely=c(2)-p (2)
      d=sqrt(delx**2+dely**2)
      if(d.eq.0.or.d.lt.r) goto 4
      if(d.eq.r) go to 3
      st=dely/d
      ct=delx/d
      dl=sqrt(d**2-r**2)
      x1=(dl**2)/d
      h=sqrt(dl**2-x1**2)
      p1(1)=x1*ct-h*st+p(1)
      p1(2)=x1*st+h*ct+p(2)
      p2(1)=x1*ct+h*st+p(1)
      p2(2)=x1*st-h*ct+p(2)
       n=2
      goto 5
 3    p1(1)=p(1)
       p1(2)=p(2)
       n=1
      goto 5
  4    n=0
  5    return
      end
