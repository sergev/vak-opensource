      subroutine  elips	(x0,y0,a,b,alpha,theto,thetf)
C...subroutine	 elips	  01/09/78
      common /gftab/irdb(5),rdb(17)
      if((abs(a)+abs(b)).eq.0) go to 20
      alp=alpha*0.01745
      theo= theto*0.01745
      thef= thetf*0.01745
      d=a*b/sqrt((a*sin(theo))**2+(b*cos(theo))**2)
      xc = x0 -	d * cos(theo + alp)
      yc = y0 -	d * sin(theo + alp)
      bsq = b*b
      absq= a*a-bsq
      ab = a*b
      call move(x0,y0,0)
      fctr=rdb(17)/rdb(1)
      dthe=(0.1/(abs(a)+abs(b)))*fctr
      n	=(thef - theo)/dthe
      if(n) 6,5,7
   5  n	= -1
   6  n	= -n
      dthe = -dthe
   7  then = theo + dthe
      do 10 i=1,n
      st =sin(then)
      d	= ab/sqrt(absq*st*st+bsq)
      xf= xc+d*cos(then+alp)
      yf= yc+d*sin(then+alp)
      call move(xf,yf,1)
   10 then = then + dthe
      st= sin(thef)
      d= ab/sqrt(absq*st*st+bsq)
      xf= xc+d*cos(thef+alp)
      yf= yc+d*sin(thef+alp)
      call move(xf,yf,1)
   20 return
      end
