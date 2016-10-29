      subroutine cilind(r,hmn,hmx,nh,nfi,dx,dy,dz,nlines,nbeg)
      dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
C *********************************************************
C  "nh+1" circles per "nfi"lines and "nfi" vertical lines.
C   the total number of lines is "(nh+2)*nfi".
C   ("nfi" - the number of lines per circle;"nh" - the number
C    of vertical line segments.)
C *********************************************************
      k=nbeg
      dh=(hmx-hmn)/nh
      dfi=2.*3.14159265/nfi
C.....gorizontal      circles.
      h=hmn
      nh1=nh+1
      do 1 j=1,nh1
      fi=0.
      do 2 i=1,nfi
      dy(1,k)=h
      dz(1,k)=r*cos(fi)
      dx(1,k)=r*sin(fi)
      fi=fi+dfi
      dy(2,k)=h
      dz(2,k)=r*cos(fi)
      dx(2,k)=r*sin(fi)
      k=k+1
2      continue
      h=h+dh
1      continue
C......vertical      lines.
      fi=0.
      do 3 i=1,nfi
      dy(1,k)=hmx
      dy(2,k)=hmn
      dz(1,k)=r*cos(fi)
      dz(2,k)=dz(1,k)
      dx(1,k)=r*sin(fi)
      dx(2,k)=dx(1,k)
      fi=fi+dfi
      k=k+1
3      continue
      return
      end
