      subroutine rotate(x,y,psi)
C     subroutine    rotate    01/09/79
      ps=3.141593/180.*psi
      call atran2(cos(ps),-sin(ps),x*(1.-cos(ps))+y*sin(ps),
     csin(ps),cos(ps),-x*sin(ps)+y*(1.-cos(ps)))
      return
      end
