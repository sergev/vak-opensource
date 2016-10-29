      subroutine matevl(xv,yv,zv,xpl,ypl,zpl,vx,vy,vz)
C...01/09/78
      common /gftrg2/bitx,bity,t(4,4)
      r=sqrt(vx**2+vy**2)
      t(1,1)=-vy/r
      t(1,2)=vx/r
      t(1,3)=0
      r=sqrt(vx**2+vy**2+vz**2)
      t(3,1)=vx/r
      t(3,2)=vy/r
      t(3,3)=vz/r
      t(2,1)=t(3,2)*t(1,3)-t(1,2)*t(3,3)
      t(2,2)=-t(3,1)*t(1,3)+t(1,1)*t(3,3)
      t(2,3)=t(3,1)*t(1,2)-t(1,1)*t(3,2)
      do 4 i=1,2
    4 t(i,4)=-t(i,1)*xv-t(i,2)*yv-t(i,3)*zv
      t(3,4)=-t(3,1)*xpl-t(3,2)*ypl-t(3,3)*zpl
      r=t(3,1)*xv+t(3,2)*yv+t(3,3)*zv+t(3,4)
      t(3,1)=t(3,1)/r
      t(3,2)=t(3,2)/r
      t(3,3)=t(3,3)/r
      t(3,4)=t(3,4)/r
      t(4,1)=-t(3,1)
      t(4,2)=-t(3,2)
      t(4,3)=-t(3,3)
      t(4,4)=1.0-t(3,4)
      return
      end
