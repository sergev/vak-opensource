      subroutine cabin(i)
      common/gfmatr/t(32),nt,nt1
      nw=nt
         nt1=2
         call shear(2,1,float(i))
         call shear(2,3,1.)
         nt=nw
      return
      end
