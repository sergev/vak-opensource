       subroutine narrow(xs,ys,xf,yf,ss,iccc)
C **      for    h-10/mt        es-1022
C...to draw an arrow at an end (or both ends) of a straight line.
C   xs,ys   x and y coordinates of the starting point of the dimension
C   xf,yf  x and y coordinates of the end point of a dimension line.
C   ss     length of the arrow
C   iccc   code showing the position to draw the arrow
C   iccc=1 to draw the arrow at the starting point side.
C   iccc=2to draw the arrow at the end point side.
C   iccc=3 to draw the arrow at the both ends of the straight line
      icod=iccc
      s=ss
        alpha=15.0*3.14159265/180.0
      if(icod.le.0) icod=3
      if(icod.gt.3) icod=3
      if(xf.eq.xs) goto 240
      theta=atan((yf-ys)/(xf-xs))
      goto 250
 240  if(yf-ys) 241,500,242
 241  theta=-3.14159265/2.0
      goto 250
 242  theta=3.14159265/2.0
  250  isw=1
       go to (301,302,310),icod
 301  isw=0
 310  check=xf-xs
      px=xs
      s=abs(s)
      py=ys
       if(check.lt.0.) s=-s
       go to 340
  302  continue
      call move(xs,ys,0)
      call move(xf,yf,1)
  320  isw=0
       check=xs-xf
      px=xf
       py=yf
      s=abs(s)
       if(check.le.0.) s=-s
 340  qx=px+s*cos(theta+alpha)
      qy=py+s*sin(theta+alpha)
      call move(px,py,0)
      call move(qx,qy,1)
      qx=px+s*cos(theta-alpha)
      qy=py+s*sin(theta-alpha)
      call move(px,py,0)
      call move(qx,qy,1)
      if(icod.eq.2)goto 342
      call move(xs,ys,0)
      call move(xf,yf,1)
      icod=2
 342  continue
       if(isw.ne.0) go to 320
 500  return
      end
