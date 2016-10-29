      subroutine trdraw(iscr,scrn,idim1,idim2,x,y,z)
C...01/09/78
      common /gftrg1/job,xloc,yloc,step,dop,l,ir,k
      dimension x(1),y(1),z(1),iscr(1),scrn(idim1,idim2,2),
     1x0(3,2),y0(3,2)
      i=iabs(l)
      call projct(x(i),y(i),z(i),xl,yl)
      i=iabs(k)
      call projct(x(i),y(i),z(i),xk,yk)
      i=iabs(ir)
      call projct(x(i),y(i),z(i),xir,yir)
      m=1
      if(l.le.0) goto 3
      x0(m,1)=xl
      y0(m,1)=yl
      x0(m,2)=xir
      y0(m,2)=yir
      m=m+1
    3 if(ir.le.0) goto 5
      x0(m,1)=xir
      y0(m,1)=yir
      x0(m,2)=xk
      y0(m,2)=yk
      m=m+1
    5 if(k.le.0) goto 7
      x0(m,1)=xk
      y0(m,1)=yk
      x0(m,2)=xl
      y0(m,2)=yl
      m=m+1
    7 r=(xloc-x0(1,1))**2+(yloc-y0(1,1))**2
      i1=1
      j1=1
      m=m-1
      do 9 i=1,m
      do 9 j=1,2
      r1=(xloc-x0(i,j))**2+(yloc-y0(i,j))**2
      if (r.le.r1) goto 9
      r=r1
      i1=i
      j1=j
    9 continue
      if(j1.ne.2) goto 12
      r1=x0(i1,1)
      x0(i1,1)=x0(i1,2)
      x0(i1,2)=r1
      r1=y0(i1,1)
      y0(i1,1)=y0(i1,2)
      y0(i1,2)=r1
  12   call pictur(iscr,scrn,idim1,idim2,x0(i1,1),
     1y0(i1,1),x0(i1,2),y0(i1,2))
      x0(i1,1)=x0(m,1)
      y0(i1,1)=y0(m,1)
      x0(i1,2)=x0(m,2)
      y0(i1,2)=y0(m,2)
      if(m.gt.1) goto 7
       call scrmod(iscr,scrn,idim1,idim2,xl,yl,xir,yir,xk,yk)
      return
      end
