      subroutine tranh(delx,dely,delz,dx,dy,dz,nlines,nbeg,nend)
      common/gfcrdn/xp,yp,zp
      dimension r(4,4)
      dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
C*********************************************
C       translate on vector (delx,dely,delz)
C*********************************************
      call hcinit(r)
      r(4,1)=delx
      r(4,2)=dely
      r(4,3)=delz
      do 5 j=nbeg,nend
      do 5 i=1,2
      call hcncor(dx(i,j),dy(i,j),dz(i,j),r)
      dx(i,j)=xp
      dy(i,j)=yp
      dz(i,j)=zp
5     continue
      return
      end
