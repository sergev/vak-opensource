      subroutine histgm(x0,dx,y0,yv,n,np1)
C   subroutine     histgm    01/09/78 01/02/81
      real l,m
      dimension yv(n)
      common/gftab/irdb(5),rdb(17)
      if(irdb(2).ne.2) goto 3
      call tmf(x0,y0,xn0,yn0)
      call move(xn0,yn0,0)
      x=x0
      n1=iabs(n)
      fnp1=np1
      coef=rdb(1)/rdb(17)
      do 4 i=1,n1
      y1=yv(i)
C     b cota o ho  cty eh  histgm
      call tmf(x,y1,xn0,yn1)
      call move(xn0,yn1,1)
      x1=x+dx
      call tmf(x1,y1,xn,yn1)
      call move(xn,yn1,1)
      call tmf(x1,y0,xn,yn0)
      call move(xn,yn0,1)
C      octpoeh e o ho  cty eh  histgm
      if(n) 2,3,4
  2   if(y1.eq.y0) go to 4
C     pac et  apametpob  tp xobk
      if(y1.ge.y0) goto 16
      call tmf(x,y1,xn,yn1)
      call move(xn,yn1,0)
      call tmf(x1,y0,xn1,yn0)
      call move(xn1,yn0,1)
      go to 5
 16   call tmf(x,y0,xn,yn0)
      call move(xn,yn0,0)
      call tmf(x1,y1,xn1,yn1)
      call move(xn1,yn1,1)
  5   y=abs(y1-y0)
      call tmf(dx,y,dxx,dyy)
      call tmf(0.,0.,dx0,dy0)
      dxx=dxx-dx0
      dyy=dyy-dy0
      dxx=dxx*coef
      dyy=dyy*coef
      l=dxx*fnp1
      m=dyy*fnp1
      f=amin1(l,m)
      dh=y/f
      dw=dx/f
      w2=0.0
      h1=0.0
      ih=f-1.0
      do 50 j=1,ih
      h1=h1+dh
      h2=y-h1
      w2=w2+dw
      w1=dx-w2
C      tp xobka o ho  cty eh  histgm
      if(y1.lt.y0) goto 21
      call tmf(x,y0+h2,xn0,yn0)
      call move(xn0,yn0,0)
      call tmf(x+w2,y1,xn0,yn1)
      call move(xn0,yn1,1)
      call tmf(x1,y0+h1,xn,yn0)
      call move(xn,yn0,0)
      call tmf(x+w1,y0,xn,yn0)
      call move(xn,yn0,1)
      go to 50
  21  call tmf(x+w2,y0,xn0,yn0)
      call move(xn0,yn0,0)
      call tmf(x,y0-h1,xn0,yn0)
      call move(xn0,yn0,1)
      call tmf(x+w1,y1,xn,yn1)
      call move(xn,yn1,0)
      call tmf(x1,y0-h2,xn,yn0)
      call move(xn,yn0,1)
  50  continue
   1  call tmf(x1,y0,xn,yn0)
      call move(xn,yn0,0)
C      epexo  k c e  cty eh  histgm
  4   x=x1
C      octpoeh e ypobh
      x=0.0
      call tmf(x,y0,xn,yn0)
      call move(xn,yn0,1)
  3   return
      end
