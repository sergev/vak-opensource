      subroutine fan2(x1,y1,x2,y2,x3,y3,x4,y4)
C**     for     h-10/mt           es-1022
C... to draw a fan shade by the coordinate value of four vertices of
C     x1,y1 ...
C     x2,y2   ,
C             .coordinate value of four vertices forming a fan shade.
C     x3,y3   ,
C     x4,y4 ...
C ********************************************************************
      a=x1*(y2-y3)+y1*(x3-x2)+x2*y3-x3*y2
      if(a.le.1.e-6)goto 50
       if(x4-x1) 200,100,200
C... x0,y0 coordinates of the circular arc.
  50   x0=(x2+x1)/2.
       y0=(y2+y1)/2.
       goto 500
C ********************************************************************
 100   x0=x1
       y0=(y3-y2)/(x3-x2)*(x0-x2)+y2
       goto 500
 200   if(x3-x2) 400,300,400
 300  x0=x2
       y0=(y4-y1)/(x4-x1)*(x0-x1)+y1
       goto 500
 400   x0=((y2-y1)+(y4-y1)/(x4-x1)*x1-(y3-y2)/(x3-x2)*x2)/
     1     ((y4-y1)/(x4-x1)-(y3-y2)/(x3-x2))
       y0=(y4-y1)/(x4-x1)*(x0-x1)+y1
500    continue
      call arc1(x1,y1,x2,y2,x0,y0,2)
      call move(x2,y2,0)
      call move(x3,y3,1)
      call arc1(x3,y3,x4,y4,x0,y0,1)
      call move(x4,y4,0)
      call move(x1,y1,1)
      return
      end
