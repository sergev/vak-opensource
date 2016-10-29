      subroutine pscale(x,y,r)
C     subroutine    pscale    01/09/79
      call atran2(r,0.,(1.-r)*x,0.,r,(1.-r)*y)
      return
      end
