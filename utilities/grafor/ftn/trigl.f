       subroutine trigl(x0,y0,w,h,theta,slope)
C       to draw a triangle by providing the base length less than height
C       and angle the side intersections at the start. point forms with
C       the base,x0, y0  x and y coordinates of the starting point of a
C       triangle.
C       w         length of the base,
C       h         height of the triangle
C       theta     angle which is formed by two sides,
C      slope     angle which the base of the triangle forms with the
C                positive direction of the x-axis.
       a=3.14159265/180.0
       x1=x0+w*cos(slope*a)
       y1=y0+w*sin(slope*a)
       s=h/sin(theta*a)
       x2=x0+s*cos(a*(slope+theta))
       y2=y0+s*sin(a*(slope+theta))
       call move(x0,y0,0)
       call move(x1,y1,1)
       call move(x2,y2,1)
       call move(x0,y0,1)
       return
       end
