      subroutine wraeb(a,b,alpha,xf,yf)
C...wraeb      01/06/75   01/01/80
      common/gfbftr/xtr,ytr
      a1=abs(a)
      b1=abs(b)
      delx0=xf-xtr
      dely0=yf-ytr
      alpha1=alpha*0.01745329
      g=b1/a1
      delx=delx0*cos(alpha1)+dely0*sin(alpha1)
      dely=(-delx0*sin(alpha1)+dely0*cos(alpha1))/g
      j=(1.-abs(b)/b)/2.
      call arcang(a,delx,dely,j,th0,thf)
      call areb1(th0,thf,g,th02,thf2)
      call wraea(a,b1,alpha,th02,thf2)
      return
      end
