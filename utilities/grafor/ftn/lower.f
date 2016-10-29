      subroutine lower (z,m,n,zmi)
      dimension z(m,n)
      if(zmi.eq.0.) return
      do 1 i=1,m
      do 1 j=1,n
      z(i,j)=z(i,j)-zmi
    1 continue
      return
      end
