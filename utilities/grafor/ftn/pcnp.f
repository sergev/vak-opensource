      subroutine pcnp(p1,c,p,d)
      common/gfgmp/p2(2),p3(2)
      real p1(2),c(3),p(2),l1(3)
      call lpp(p1,c,l1)
      s=(p1(1)-c(1))**2+(p1(2)-c(2))**2-c(3)*c(3)
      if(abs(s).lt.1.e-6) return
      call pilc(l1,c,n)
      call spnp(p1,p)
      d=sqrt((p1(1)-p(1))**2+(p1(2)-p(2))**2)
      return
      end
