      subroutine spnp(p,pn)
      common/gfgmp/p1(2),p2(2)
      dimension p(2),pn(2)
      d1=sqrt((p1(1)-p(1))**2+(p1(2)-p(2))**2)
      d2=sqrt((p2(1)-p(1))**2+(p2(2)-p(2))**2)
      if(d1.ge.d2) go to 1
      pn(1)=p1(1)
      pn(2)=p1(2)
      return
  1   pn(1)=p2(1)
      pn(2)=p2(2)
      return
      end
