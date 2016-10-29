      subroutine drawel(t,i)
      common/gflni/ xi(2),yi(2),zi(2)
C.....s/p: move or draw depending upon "i" ("i"="0" or "1").
      x=xi(1)+(xi(2)-xi(1))*t
      y=yi(1)+(yi(2)-yi(1))*t
C++   print 10,t,x,y,i
C##10    format('s/p *drawel*: t=',f10.5,' x,y=',2f15.3,' i=',i3)
C....transform matemat. coordinates to physical coordinates.
      call tmf(x,y,xf,yf)
C++   print 11,xf,yf
C##11    format(' phys.coord.=',2f10.5)
      call move(xf,yf,i)
      return
      end
