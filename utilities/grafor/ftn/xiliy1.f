      function xiliy1 (z1,z2,arg1,arg2,zizol,i,j,ius,jus)
      common /gfnam1/ m,p(1)
      common /gfnam2/ key,pl(1)
      tga=(abs(z1)-abs(z2))/(arg1-arg2)
      if (abs(tga).le.1.e-5 ) tga=1.e-5
      delta=(zizol-abs(z2))/tga
C****   b   c eh e a pecob  o i,j macc ba p(i,j)
      lij=i+m*(j-1)
      lijus=i+ius+m*(j+jus-1)
      xiliy1=arg2+delta
      pl(key)=p(lij)+delta* (p(lijus)-p(lij))/(arg1-arg2)
      key=key+1
C
C
      z1=-abs(z1)
      z2=-abs(z2)
      return
      end
