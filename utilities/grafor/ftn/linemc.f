      subroutine linemc(x,y,n,nm,js)
C...subroutine	 linemc	  01/09/78
      common /gftab/irdb(5),rdb(17)
      dimension	x(n),y(n)
      if(irdb(2).lt.2) go to 26
      if(irdb(3).le.0) go to 26
      j	= 0
      xor=rdb(7)
      yor=rdb(8)
      xr=xor+rdb(9)
      yr=yor+rdb(10)
      k=0
      m=iabs(n)
      is=1
      if(js.ge.0) k=1
      ist=iabs(js)
      do 25  i=1,m
      call tmf(x(i),y(i),fx,fy)
      if(n.le.0) go to 18
      fx=amin1(xr,amax1(xor,fx))
      fy=amin1(yr,amax1(yor,fy))
 18   if(i.gt.1) go to 32
      fx1=fx
      fy1=fy
  32  call move(fx,fy,j)
      is=is-1
      if(is.gt.0) go to	25
      call marker(nm)
      is=ist
  25  j=k
      call move(fx1,fy1,j)
  26  return
      end
