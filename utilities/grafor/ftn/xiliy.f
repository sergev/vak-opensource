      function  xiliy (z1,z2,arg1,arg2,zizol,i,j,ius,jus)
      tga=(abs(z1)-abs(z2))/(arg1-arg2)
      if(abs(tga).le.1.e-5) tga=1.e-5
      xiliy =arg2+(zizol-abs(z2))/tga
      z1=-abs(z1)
      z2=-abs(z2)
      return
      end
