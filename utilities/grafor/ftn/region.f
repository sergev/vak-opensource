      subroutine region(x,y,xl,yl,name,n,j)
C...subroutine	region	 01/09/78
      common /gftab/irdb(5),rdb(17)
      dimension	name(1)
      if(irdb(2).le.0) go to 14
      f=rdb(17)/rdb(1)
      zm=.3*f
      z=xl/amax1(float(n),1.)
      zh=1.5*amin1(z,zm)
      if(x.lt.0.) go to	1
      if(y.lt.0.) go to	1
      xlr=amin1((x+xl),rdb(5))-x
      ylr=amin1((y+yl),rdb(6))-y
      if(xlr.lt.f) go to 1
      if(ylr.lt.f) go to 1
      rdb(7)=x
      rdb(8)=y
      rdb(9)=xlr
      rdb(10)=ylr
      if(j.gt.0) call box(x,y,xlr,ylr)
      if(n.gt.0) call symbol(x,y+ylr+zm,zh,name,n,0.)
      irdb(2)=2
      return
 1    irdb(2)=1
   14 return
      end
