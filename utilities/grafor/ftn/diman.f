       subroutine diman(x0,y0,r,a,b)
C **      for  h-10/mt           es-1022
C...      to draw an angle and dimension circular arc.
C      x0,y0     x,y  coordinates of the center ofangle.
C      r         radius of the dimension circular arc.
C      a         angle(degree) the angle shall be drawn down to one pl.
C                of decimal
C      b         angle between the line connecting the center with the
C                st. point of the angle forms with the positive dir.
      common /gftab/irdb(5),rdb(17)
       alph=a
       teta=3.14159265/180.0
       f=rdb(17)/rdb(1)
       ff=0.3*f
       pxs=x0+r*cos(b*teta)
       pys=y0+r*sin(b*teta)
       pxf=x0+r*cos((b+a)*teta)
       pyf=y0+r*sin((b+a)*teta)
       x1=pxs+ff*cos((b+75.)*teta)
       y1=pys+ff*sin((b+75.)*teta)
       x2=pxs+ff*cos((b+105.)*teta)
       y2=pys+ff*sin((b+105.)*teta)
      call move(pxs,pys,0)
      call move(x1,y1,1)
      call move(pxs,pys,0)
      call move(x2,y2,1)
      call move(pxs,pys,0)
      call arcia(r,b,a+b)
      x1=pxf+ff*cos((a+b-75.)*teta)
      y1=pyf+ff*sin((a+b-75.)*teta)
      x2=pxf+ff*cos((a+b-105.)*teta)
      y2=pyf+ff*sin((a+b-105.)*teta)
       call move(pxf,pyf,0)
       call move(x1,y1,1)
       call move(pxf,pyf,0)
       call move(x2,y2,1)
       pxc=x0+(r+ff)*cos((a/2.0+b)*teta)
       pyc=y0+(r+ff)*sin((a/2.0+b)*teta)
       call number(pxc,pyc,0.3*f,alph,1,b+a/2.)
       return
       end
