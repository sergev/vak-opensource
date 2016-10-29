      subroutine fatlin(xf,yf,d)
C  10/02/79
      common /gfbftr/xtr,ytr  /gftab/irdb(5),rdb(17)
      x0 = xtr
      y0 = ytr
      sl = sqrt((xf-x0)**2+(yf-y0)**2)
      if(sl.lt.1.0e-3) return
      f = rdb(17)/rdb(1)
      f = (d*0.1-0.03)*f/sl
      dx = f*(yf-y0)
      dy = f*(x0-xf)
      call move(xf,yf,1)
      call move(xtr+dx/2,ytr+dy/2,1)
      call move(x0+dx/2,y0+dy/2,1)
      call move(xtr-dx,ytr-dy,1)
      call move(xf-dx/2,yf -dy/2,1)
      call move(xf,yf,1)
      return
      end
