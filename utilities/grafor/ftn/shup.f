      subroutine shup(i)
      common/gfsv/te(15,2),nl,invsln
C.....shift up on one element from "i"-th(i.ge.2) to "nl"-th elements
C      of "te()".
      if(i.gt.nl) goto 2
      do 1 k=i,nl
      te(k-1,1)=te(k,1)
      te(k-1,2)=te(k,2)
1     continue
2     nl=nl-1
C++   print 10,i,nl
C##10    format(' ++call shup++; i=',i5,' nl=',i5)
      return
      end
