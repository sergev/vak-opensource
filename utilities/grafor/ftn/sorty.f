      subroutine sorty(dx,dy,dz,nlines,n1,n2)
      dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
C ======================================================================
C   sort end points of lines so as the end point with the greater "dy"
C    value has the index "1".
C ======================================================================
      do 1 i=n1,n2
      if(dy(1,i).ge.dy(2,i)) goto 1
C..... exchange "1" and "2" end points.
      s=dx(1,i)
      dx(1,i)=dx(2,i)
      dx(2,i)=s
      s=dy(1,i)
      dy(1,i)=dy(2,i)
      dy(2,i)=s
      s=dz(1,i)
      dz(1,i)=dz(2,i)
      dz(2,i)=s
1     continue
      return
      end
