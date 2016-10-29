      subroutine exmima (z,m,n,zmi,zma)
      dimension z(m,n)
      zmi=z(1,1)
      zma=zmi
      do 1 i=1,m
      do 1 j=1,n
      if (zmi.gt.z(i,j)) zmi=z(i,j)
      if (zma.lt.z(i,j)) zma=z(i,j)
    1 continue
      return
      end
