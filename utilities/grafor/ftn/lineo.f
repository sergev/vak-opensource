      subroutine lineo(x,y,n)
C...subroutine	 lineo	  01/09/78
      common /gftab/irdb(5),rdb(17)
      dimension	x(n),y(n)
      if(irdb(2).lt.2) go to 26
      if(irdb(3).le.0) go to 26
      j	= 0
      xor=rdb(7)
      yor=rdb(8)
      xr=xor+rdb(9)
      yr=yor+rdb(10)
      m=iabs(n)
      call where(fx,fy,f)
      call tmf(x(1),y(1),fx1,fy1)
      df=amax1(abs(fx1-fx),abs(fy1-fy))
      call tmf(x(m),y(m),fx1,fy1)
      dl=amax1(abs(fx1-fx),abs(fy1-fy))
      l=1
      inc=1
      if(df.le.dl) go to 7
      l	= m
      inc=-1
   7  do 25  i=1,m
      call tmf(x(l),y(l),fx,fy)
       l=l+inc
      if(n.le.0) go to 18
      fx=amin1(xr,amax1(xor,fx))
      fy=amin1(yr,amax1(yor,fy))
  18  call move(fx,fy,j)
  25  j=1
  26  return
      end
