      subroutine movc(xm,ym,dl,j)
C  01/02/76    10/02/79
      common /gfbftr/xtr,ytr
      equivalence (x,dx),(y,dy)
      dx = xm-xtr
      dy = ym-ytr
      dl1 = sqrt(dx**2+dy**2)
      if(dl1.lt.1.0e-9) goto 1
      x = xtr + dx*(dl/dl1)
      y = ytr + dy*(dl/dl1)
      call move(x,y,j)
 1    return
      end
