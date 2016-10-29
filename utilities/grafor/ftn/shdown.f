      subroutine shdown(i)
      common/gfsv/te(15,2),nl,invsln
C.....shift down on one element from "i"-th(i.ge.1) to "nl"-th elements
C      of "te()".
      do 1 j=i,nl
      k=nl-j+i
      te(k+1,1)=te(k,1)
      te(k+1,2)=te(k,2)
1     continue
      nl=nl+1
C++   print 10,i,nl
C##10    format(' ==call shdown==;i=',i5,' nl=',i5)
      return
      end
