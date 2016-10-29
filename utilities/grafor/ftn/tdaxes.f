         subroutine tdaxes(x,y,reg,l)
         common/gfcrd/xp,yp,zp
         dimension x1(3),z1(3)
         call hcncrd(0.,0.,0.)
         x0=xp
         z0=zp
         xmn=xp
         xmx=xp
         zmn=zp
         zmx=zp
         do 7 j=1,3
         goto(71,72,73),j
   71 call hcncrd(1.,0.,0.)
         goto 74
   72 call hcncrd(0.,1.,0.)
         goto 74
   73 call hcncrd(0.,0.,1.)
   74 x1(j)=xp
         z1(j)=zp
         if(xp-xmn)101,102,102
  101 xmn=xp
         goto 104
  102 if(xp-xmx)104,104,103
  103 xmx=xp
  104 if(zp-zmn)105,106,106
  105  zmn=zp
         goto 7
  106 if(zp-zmx)7,7,107
  107 zmx=zp
    7 continue
       rx=xmx-xmn
       rz=zmx-zmn
       if(rx-rz)91,92,93
   91 dz=reg
       dx=dz*rx/rz
       goto 94
   92 dx=reg
       dz=reg
       goto 94
   93 dx=reg
       dz=dx*rz/rx
   94 call region(x,y,dx,dz,' ',1,l)
       call set(0)
         call limits(xmn,xmx,zmn,zmx)
         do 8 j=1,3
         call tmf(x0,z0,xr,zr)
         call move(xr,zr,0)
         call tmf(x1(j),z1(j),xr,zr)
       call move(xr,zr,1)
       goto(81,82,83),j
   81 call symbol(xr-.5,zr-1.2,.7,'x',1,0.)
       goto 8
   82 call symbol(xr-.5,zr+.2,.7,'y',1,0.)
       goto 8
   83 call symbol(xr+.5,zr-.8,.7,'z',1,0.)
    8 continue
         return
         end
