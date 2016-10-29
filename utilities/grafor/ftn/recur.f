      subroutine recur (z,m,n,nx,ny,kx,ky)
      dimension z(m,n)
      do 1 i=nx,kx
      do 1 j=ny,ky
      z(i,j)=abs(z(i,j))
    1 continue
      return
      end
