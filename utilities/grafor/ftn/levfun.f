      subroutine levfun(phi,n1,n2,c,ncn)
C...subroutine levfun 01/09/78
      dimension phi(1),c(1)
      n=n1*n2
      call minmax(phi,n,an,ax)
      a=(ax-an)/(ncn+1)
      c(1)=an+a
      do 1 i=2,ncn
  1   c(i)=c(i-1)+a
      return
      end
