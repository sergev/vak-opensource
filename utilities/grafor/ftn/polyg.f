      subroutine polyg (x,y,sl,m,th)
       n=m
      xn = x
      yn = y
      th0 = th * 0.01745
      call move	(x,y,0)
      if (n) 10,100,20
   10 th1=-6.2832 / n
      th2= -th1	* 2.0
      n= -n
      do 11 i =	1,n
      xn = xn +	sl* cos(th0)
      yn = yn +	sl* sin(th0)
      call move	(xn,yn,1)
      th0 = th0	+ th1
      xn = xn +sl * cos(th0)
      yn = yn +	sl * sin(th0)
      call move	(xn,yn,1)
   11 th0 = th0	+ th2
  100 return
   20 th1 =6.2832 / n
      do 21  i = 1,n
      xn = xn +	sl * cos(th0)
      yn = yn +sl * sin(th0)
      call  move (xn,yn,1)
   21 th0 = th0	+ th1
      return
      end
