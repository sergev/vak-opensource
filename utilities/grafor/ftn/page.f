      subroutine  page (xl,yl,name,n,j)
C...subroutine	page	 01/09/78
      common /gftab/irdb(5),rdb(17)
      dimension	md(2),nd(3)
      dimension	name(1)
c     if(irdb(2).gt.0) call endpg(4hctph)
      irdb(2)=2
      irdb(3)=1
      f=rdb(17)/rdb(1)
      x=amin1(amax1(5.12*f,xl),f*rdb(3))
      y=amin1(amax1(5.12*f,yl),f*rdb(4))
      do 1 i=5,13,4
	  rdb(i)=x
	  rdb(i+1)=y
 1        continue
      call begin (int (x * rdb (1)), int (y * rdb (1)))
      call set(0)
      if(j.gt.0) call box(0.,0.,x,y)
      if(n.gt.0) call symbol(f*0.256,f*0.256,f*0.512,name,n,0.)
      return
      end
