	subroutine splitp(x,y,z)
C ********************************************************************
      common/gfmt/t(4,4),tt(4,4),tr(4,4),dpr1(3),itpprj
	common/gfcrdn/xp,yp,zp,dxcent,dycent,dzcent
      common/gfeps/eps,lengte
C   "x,y,z" - coords.of the point which defines splitting plane
C    (or planes). "call splitp()" must be before "call coordt()" or
C    "call hallne()".
C ********************************************************************
      dxcent=x
      dycent=y
      dzcent=z
      return
      end
