      subroutine drawte
      common/gfsv/te(15,2),nl,invsln
      common/gfeps/eps,lengte
C************************************************
C     s/p: draw visible parts of "i"-th line.
C************************************************
C.....test: is the total "i"-th line visible
      if(nl.eq.0) goto 5
C
C.....test: is "min" of "te()" equal to "0."
      if(abs(te(1,1)).le.eps) goto 1
C.....end point "0." (i.e. xi(1),yi(1)) is visible |
      call drawel(0.,0)
      call drawel(te(1,1),1)
C
1     continue
      if(nl.le.1) goto 3
C.....draw that parts of line which are between the elements of "te()".
      do 2 k=2,nl
      call drawel(te(k-1,2),0)
      call drawel(te(k,1),1)
2     continue
C
3     continue
C.....test: is "max" of "te()" equal "1."
      if(abs(te(nl,2)-1.).le.eps) return
C.....end point "1." (i.e. xi(2),yi(2)) is visible |
      call drawel(te(nl,2),0)
      call drawel(1.,1)
      return
C
5     continue
C.....the total "i"-th line is visible |
      call drawel(0.,0)
      call drawel(1.,1)
      return
      end
