      subroutine arcid(xc,yc,phi)
C     subroutine    arcid     01/06/75
      common/gfbftr/xtr,ytr
      delx=xtr-xc
      dely=ytr-yc
      r=sqrt(delx**2+dely**2)
      th0=angler(delx,dely)
      thf=th0+phi
      if(phi) 1,3,2
    1 r=-r
    2 call arcia(r,th0,thf)
    3 return
      end
