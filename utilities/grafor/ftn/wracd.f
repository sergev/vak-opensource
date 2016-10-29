      subroutine wracd(xc,yc,phi)
C.....subroutine  wracd        01/06/75
      common/gfgel/gf(28)
      common/gfbftr/xtr,ytr
      gf(10)=100.
      delx=xtr-xc
      dely=ytr-yc
      r=sqrt(delx**2+dely**2)
      th0=angler(delx,dely)
      thf=th0+phi
      if(phi) 3,8,4
 3    r=-r
 4    i=abs(gf(9))
      gf(i)=20.
      gf(i+1)=xc
      gf(i+2)=yc
      gf(i+3)=r
      gf(i+6)=th0
      gf(i+7)=thf
      if(gf(9).gt.0.)call strmod(0)
 8    return
      end
