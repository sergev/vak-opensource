      subroutine prep(x,y,z,n0,xv,yv,zv,xpl,ypl,zpl,vx,vy,vz,
     1 iscr,idim1,rlytox)
C   01/09/78
      common /gftrg1/job,xloc,yloc,step,dop,itrgl(3)
      common /gftrg2/bitx,bity,t(4,4)
      dimension iscr(idim1),x(n0),y(n0),z(n0)
      call matevl(xv,yv,zv,xpl,ypl,zpl,vx,vy,vz)
      bitx=0.
      bity=0.
      call projct(x(1),y(1),z(1),xmin,ymin)
      xmax=xmin
      ymax=ymin
      do 3 i=1,n0
      call projct(x(i),y(i),z(i),x0,y0)
      if(x0.lt.xmin) xmin=x0
      if(x0.gt.xmax) xmax=x0
      if(y0.lt.ymin) ymin=y0
      if(y0.gt.ymax) ymax=y0
    3 continue
      rx=(xmax-xmin)/50
      xmin=xmin-rx
      xmax=xmax+rx
      bitx=-xmin
      bity=-ymin
      rlytox=(ymax-ymin)/(xmax-xmin)
      xmax=xmax-xmin
      ymax=ymax-ymin
      call limits(0.0,xmax,0.0,ymax)
      step=xmax/(idim1-1)
      dop=ymax/1000
      do 14 i=1,idim1
   14 iscr(i)=0
      xloc=0
      yloc=0
      return
      end
