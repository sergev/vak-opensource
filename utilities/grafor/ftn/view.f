      subroutine view(x,y,z)
      common/gfmatr/t(32),nt,nt1
       common/gfview/mac(20),xp1,yp1,zp1
      nw=nt
      nt1=3
      xp1=x
      yp1=y
      zp1=z
      if(nt.ne.1)
     1call hcinv(x,y,z,xp1,yp1,zp1)
      call hcrot1(x,y,z)
       r=sqrt(x**2+y**2+z**2)
      call hcprsp(r)
      nt=nw
      return
       end
