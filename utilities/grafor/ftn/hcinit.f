      subroutine hcinit(a)
      dimension a(4,4)
      do 1 i=1,4
      do 2 j=1,4
2     a(i,j)=0.
1     a(i,i)=1.
      return
      end
