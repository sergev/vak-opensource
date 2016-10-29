      subroutine svlseg(al,au,i)
      common/gfsv/te(15,2),nl,invsln
C..... save line segment in "i"-th element of "te()".
      te(i,1)=al
      te(i,2)=au
C++   print 10
C##10    format(' ==call svlseg==')
      return
      end
