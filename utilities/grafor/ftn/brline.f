      subroutine brline(x,y,n)
C...subroutine  brline  01/09/78
      common /gftab/irdb(5),rdb(17)
      dimension x(1),y(1)
       common/gfbro/a(4),met
      if(irdb(2) .lt.2) goto 26
      j=0
      xor=rdb(7)
      xr=xor+rdb(9)
      yor=rdb(8)
      yr=yor+rdb(10)
      m = n
      ic = 1
      if(m)  3,26,4
    3 ic = 0
      m = -m
    4 call where (fx,fy,f)
      call tmf(x(1),y(1),fx1,fy1)
      df=amax1(abs(fx1-fx),abs(fy1-fy))
      call tmf(x(m),y(m),fx1,fy1)
      dl=amax1(abs(fx1-fx),abs(fy1-fy))
      i=1
      ds=a(1)
      l=1
      inc=1
      if (df.le.dl) goto 7
      l=m
      inc=-1
    7 do 25 kk=1,m
      call tmf(x(l),y(l),fx,fy)
      l=l+inc
      fx=amin1(xr,amax1(xor,fx))
      fy=amin1(yr,amax1(yor,fy))
      if(met.ne.993) goto 16
      if(j.eq.0) goto 16
  15  dx=fx-xc
       dy=fy-yc
      s=sqrt(dx*dx+dy*dy)
      if(s.lt.ds) goto 23
      ds=ds/s
      dx=dx*ds
      dy=dy*ds
      xc=xc+dx
      yc=yc+dy
   37 call move(xc,yc,ic)
      i=i+1
      if(i.gt.4) i=1
      ds=a(i)
      ic = 1-ic
      go to 15
   23 ds=ds-s
      call move(fx,fy,ic)
      go to 21
   16 call move(fx,fy,j)
      j=1
   21 xc=fx
   25 yc=fy
   26 return
      end
