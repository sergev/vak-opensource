      subroutine fan1(x0,y0,r1,r2,a1,a2)
C     for  h-10/mt          es-1022
C... to draw a fan shade by providing the center coordinate,two radii
C    side-angles of the fan shaded circular arc.
C    x0,y0    center coordinate of a fan shaded circular arc.
C   r1       radius of the outside circular arcof the fan shade.
C   r2      radius of the inside circular arc of the fan shade.
C    a1      angle of the right side of the fan shade.
C    a2      angle of the left side of the fan shade.
       b1=a1*3.14159265/180.
      b2=a2*3.14159265/180.
      x1=x0+r1*cos(b1)
      y1=y0+r1*sin(b1)
      x2=x0+r1*cos(b2)
      y2=y0+r1*sin(b2)
      x3=x0+r2*cos(b2)
      y3=y0+r2*sin(b2)
      x4=x0+r2*cos(b1)
      y4=y0+r2*sin(b1)
C... to draw the outside circular arc.
      call arc1(x1,y1,x2,y2,x0,y0,2)
      call move(x2,y2,0)
      call move(x3,y3,1)
C... to draw the inside circular arc.
      call arc1(x3,y3,x4,y4,x0,y0,1)
      call move(x4,y4,0)
      call move(x1,y1,1)
      return
      end
