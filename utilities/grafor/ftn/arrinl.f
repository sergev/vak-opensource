      subroutine arrinl(x,y,a,ix,iy,ixb,ixe,iyb,iye,dx,dy,dz,nl,nlbeg)
      dimension x(ix),y(iy),a(ix,iy)
      dimension dx(2,nl),dy(2,nl),dz(2,nl)
C **********************************************************************
C   transfn from "grid" to "dx,dy,dz". the ordering for "south-west"
C  allocation of the center of proj. or direction of proj. is set |
C   the number of lines ("nl") must be:
C  nl=2*idx*idy-(idx+idy) ,where idx=ixe-ixb+1; idy=iye-iyb+1.
C *********************************************************************
      k=nlbeg
      iyb1=iyb+1
      ixe1=ixe-1
      do 2 j=iyb1,iye
C.... element of leading edge.
      dx(1,k)=x(ixb)
      dx(2,k)=x(ixb)
      dz(1,k)=y(j-1)
      dz(2,k)=y(j)
      dy(1,k)=a(ixb,j-1)
      dy(2,k)=a(ixb,j)
      k=k+1
C
      do 2 i=ixb,ixe1
      if(j.gt.iyb1) goto 3
C.... element of leading edge.
      dx(1,k)=x(i)
      dx(2,k)=x(i+1)
      dz(1,k)=y(iyb)
      dz(2,k)=y(iyb)
      dy(1,k)=a(i,iyb)
      dy(2,k)=a(i+1,iyb)
      k=k+1
3     continue
      dx(1,k)=x(i)
      dx(2,k)=x(i+1)
      dz(1,k)=y(j)
      dz(2,k)=y(j)
      dy(1,k)=a(i,j)
      dy(2,k)=a(i+1,j)
      k=k+1
C
      dx(1,k)=x(i+1)
      dx(2,k)=x(i+1)
      dz(1,k)=y(j)
      dz(2,k)=y(j-1)
      dy(1,k)=a(i+1,j)
      dy(2,k)=a(i+1,j-1)
      k=k+1
2     continue
      return
      end
