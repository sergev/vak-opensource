      subroutine shear(n1,n2,f)
      common/gfmatr/t(4,4),a(4,4),nt,nt1
      nt=2
      call hcunit(a)
      a(n1,n2)=f
      call hcmult(t,a)
      return
      end
