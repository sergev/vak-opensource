      subroutine elps(a,b,alpha)
C     subroutine    elps      24/08/76
C     subroutine    elps      01/06/75
      common/gfbftr/xtr,ytr
      x0=xtr
      y0=ytr
      alpha1=alpha*0.01745329
      xa=x0+a*cos(alpha1)
      ya=y0+a*sin(alpha1)
      call elips(xa,ya,a,b,alpha,0,360.)
      call move(x0,y0,0)
      return
      end
