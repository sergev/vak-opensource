      subroutine sincl(xb,step,y,n)
C...subroutine     sincl     01/08/74
C...subroutine     sincl     01/09/78
      common /gftab/ irdb(5),rdb(17)
      dimension y(n),b(4)
      xbeg=xb
      dx=step
      if(irdb(2).lt.2) goto 6
      if(irdb(3).le.0) goto 6
      call tmf(xbeg,y(1),xf2,yf2)
      call move(xf2,yf2,0)
      if(n-2) 6,4,5
    4 x2=xbeg+dx
      call tmf(x2,y(2),xf2,yf2)
      call move(xf2,yf2,1)
      return
    5 f=1./(0.25*rdb(1)/rdb(17))
      x2=xbeg
      dy2=deriv5 (dx,y,n,1)
      do 2 i=2,n
      x1=x2
      dy1=dy2
      x2=x1+dx
      dy2=deriv5(dx,y,n,i)
      call cubpol (x1,x2,y(i-1),y(i),dy1,dy2,b)
      xf1=xf2
      yf1=yf2
      call tmf (x2,y(i),xf2,yf2)
      xj=x1
      k=sqrt((xf2-xf1)**2+(yf2-yf1)**2)*f+1.
      dx1=dx/(k+1)
      do 3 j=1,k
      xj=xj+dx1
      call pval(z,xj,b,4)
      call tmf(xj,z,xf1,yf1)
    3 call move(xf1,yf1,1)
    2 call move(xf2,yf2,1)
    6 return
      end
