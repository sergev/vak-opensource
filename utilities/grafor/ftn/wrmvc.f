      subroutine wrmvc(xm,ym,dl)
C.....subroutine  01/06/75   01/02/76   01/01/80
      common/gfbftr/xtr,ytr
      equivalence(xtr,x0),(ytr,y0)
      delx=xm-x0
      dely=ym-y0
      dl1=sqrt(delx**2+dely**2)
      if(dl1.le.0.) goto 2
      x=x0+dl*delx/dl1
      y=y0+dl*dely/dl1
      call wrmve(x,y)
  2   return
      end
