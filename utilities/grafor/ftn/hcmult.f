      subroutine hcmult(a,b)
      dimension a(4,4),b(4,4)
      dimension c(4)
      do 1 i=1,4
      do 2 j=1,4
      c(j)=0.
      do 2 k=1,4
    2 c(j)=c(j)+a(i,k)*b(k,j)
      do 3 j=1,4
    3 a(i,j)=c(j)
    1 continue
      return
      end
