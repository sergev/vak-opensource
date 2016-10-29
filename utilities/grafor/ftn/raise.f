      subroutine raise (z,m,n,zmi)
      dimension z(m,n)
      call exmima (z,m,n,zmi,zma)
      if(zmi) 10,10,20
   20 zmi=0.
      return
   10 zmi =abs(zmi)+1.
      do 1 j=1,n
      do 1 i=1,m
      z(i,j)=z(i,j)+zmi
    1 continue
      return
      end
