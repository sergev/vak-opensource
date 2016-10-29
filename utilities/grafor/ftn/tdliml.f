       subroutine tdliml(x,y,z,n,s)
C      subroutine tdliml 01.09.78
       common/gfcrd/xp,yp,zp/gfliml/xmin,xmax,zmin,zmax
       dimension x(1),y(1),z(1)
       n1=iabs(n)
       call hcncrd(x(1),y(1),z(1))
      xmx=xp
      xmn=xmx
      zmn=zp
      zmx=zmn
       do 4 i=2,n1
       call hcncrd(x(i),y(i),z(i))
      if(zmn.gt.zp)zmn=zp
      if(zmx.lt.zp)zmx=zp
      if(xmn.gt.xp) xmn=xp
      if(xmx.lt.xp) xmx=xp
    4 continue
       if(n)20,31,30
  20  if(xmin.lt.xmn)xmn=xmin
      if (xmax.gt.xmx)xmx=xmax
      if(zmin.lt.zmn)zmn=zmin
      if (zmax.gt.zmx)zmx=zmax
   30 call limits(xmn,xmx,zmn,zmx)
       xmin=xmn
       xmax=xmx
       zmin=zmn
       zmax=zmx
       s=(xmax-xmin)/(zmax-zmin)
   31 return
      end
