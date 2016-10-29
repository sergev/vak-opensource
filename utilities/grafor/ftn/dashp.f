      subroutine dashp(x,y,dl)
      common /gfbftr/xtr,ytr
      common /gftab/irdb(5),rdb(17)
      call move(xtr,ytr,0)
      f=rdb(17)/rdb(1)
      dx=x-xtr
      dy=y-ytr
      s=sqrt(dx*dx+dy*dy)
      if(s.lt.0.5*f) go to 1
      d=abs(dl)
      dx=d*(dx/s)
      dy=d*(dy/s)
      if(dl.lt.0) go to 3
 2    s=s-1.3*d
      if(s.le.0.) go to 1
      call move(xtr+dx,ytr+dy,1)
      call move(xtr+0.3*dx,ytr+0.3*dy,0)
      go to 2
 3    s=s-1.23*d
      if(s.le.0.) go to 1
      call move(xtr+dx,ytr+dy,1)
      call move(xtr+0.1*dx,ytr+0.1*dy,0)
      call move(xtr+0.03*dx,ytr+0.03*dy,1)
      call move(xtr+0.1*dx,ytr+0.1*dy,0)
      go to 3
 1    call move(x,y,1)
      return
      end
