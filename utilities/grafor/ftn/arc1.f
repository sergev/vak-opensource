       subroutine arc1(xs1,ys1,xf1,yf1,xc1,yc1,ncw)
C***         for       h-10/mt        es-1022
C....................................................
C...to draw a circular arc not only from the coordinates of the str.
C   ,end point and center but also from the direction of the pen.
C   xs1,ys1    x and y coordinates of the string point of a cir.
C   xf1,yf1   x and y coord. of the end point of the circ. arc.
C   xc1,yc1   x and y coord. of the center of a circ. arc.
C   ncw   param. which designates the drawing dir.
C   ncw=1...clockwise       ncw=2...counterclockwise.
C **************adjustment center point arc1********************
C............................................................
      j=0
      yp=2*(yc1-yf1)+yf1
      xp=xf1+(2*(xc1-xf1))
      xq=xp
      r=-sqrt((xf1-xc1)**2+(yf1-yc1)**2)
      if(xf1.ge.xc1)goto 5
      if(yf1.ge.yc1)goto 3
      xq=xf1
      yq=yp
      jj=0
      goto 4
 3    yq=yf1
      jj=1
 4    if(ys1.ge.yq)goto 14
      if(jj.eq.0)goto 12
      goto 13
 5    if(yf1.lt.yc1)goto 6
      xq=xf1
      yq=yp
      jj=1
      goto 7
 6    yq=yf1
      jj=0
 7    if(ys1.le.yq)goto 14
      if(jj.eq.1)goto 13
 12   if(xs1.le.xq)goto 14
      goto 15
 13   if(xs1.ge.xq)goto 14
      goto 15
 14   j=1
  15  call move(xs1,ys1,0)
      if(ncw.eq.1)goto 16
      r=-r
      j=1-j
  16  call arcib(r,xf1,yf1,j)
      return
      end
