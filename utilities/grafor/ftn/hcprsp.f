        subroutine  hcprsp(h)
C   perspective projection
      common/gfmatr/t(4,4),a(4,4),nt,nt1
      call hcunit(a)
      a(2,4)=1/h
      call hcmult(t,a)
      return
      end
