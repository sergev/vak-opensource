      subroutine box (x,y,xl,yl)
      x1=x+xl
      y1=y+yl
      call move(x,y,0)
      call move(x,y1,1)
      call move(x1,y1,1)
      call move(x1,y,1)
      call move(x,y,1)
      return
      end
