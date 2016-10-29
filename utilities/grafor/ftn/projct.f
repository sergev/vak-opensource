      subroutine projct(xp,yp,zp,xpj,ypj)
C...01/09/78
      common /gftrg2/bitx,bity,t(4,4)
      r=t(4,1)*xp+t(4,2)*yp+t(4,3)*zp+t(4,4)
      xpj=(t(1,1)*xp+t(1,2)*yp+t(1,3)*zp+t(1,4))/r+bitx
      ypj=(t(2,1)*xp+t(2,2)*yp+t(2,3)*zp+t(2,4))/r+bity
      return
      end
