      subroutine  grid (x,y,xs,ys,m,n)
      y0 = y
      im = n+1
       xf=x+xs*	float(m)
      x0 = x
      call  move (x,y,0)
      do 10 i =	1,im
      call  move (x0,y0,1)
      call  move (xf,y0,1)
      y0 = y0 +	ys
      xt = xf
      xf = x0
   10 x0 = xt
      x0 = x
      y0 = y
      xf = y + ys* float(n)
      im = m + 1
      do 20 i= 1,im
      call move	(x0,xf,1)
      call move	(x0,y0,1)
      x0 = x0 +	xs
      xt = xf
      xf = y0
   20 y0 = xt
      return
	end
