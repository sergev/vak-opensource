      subroutine roth(naxes,fi,dx,dy,dz,nlines,nbeg,nend)
      common/gfcrdn/xp,yp,zp
      dimension r(4,4)
      dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
C *********************************************************
C    rotate about the axes "naxes" on angle "fi" in right c.s.
C    clockwice if look from the end of the unit axes vector
C    to the origin.
C *********************************************************
      fi=fi*3.14159265/180.
      call strot3(naxes,cos(fi),sin(fi),r)
      do 5 j=nbeg,nend
      do 5 i=1,2
      call hcncor(dx(i,j),dy(i,j),dz(i,j),r)
      dx(i,j)=xp
      dy(i,j)=yp
      dz(i,j)=zp
5      continue
      return
      end
