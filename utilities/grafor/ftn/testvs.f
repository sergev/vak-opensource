      subroutine testvs
      common/gfsv/te(15,2),nl,invsln
      common/gfeps/eps,lengte
      if(nl.ne.1) return
C..... if "..." ,flag of visibility of line "i" is reset.
      if((abs(te(1,1)).le.eps).and.(abs(te(1,2)-1.).le.eps)) invsln=0
      return
      end
