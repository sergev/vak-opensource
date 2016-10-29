      subroutine  hcunit(a)
C     subroutine  hcunit  01.09.78
C  formation  of  unit matrix
      dimension a(4,4)
      do 1 i=1,4
      do 1 j=1,4
      a(i,j)=0.
      a(i,i)=1.
 1    continue
      return
      end
