            subroutine isomet
      common/gfmatr/t(4,4),a(4,4),nt,nt1
      nw=nt
      nt1=2
         call tdrot(3,-45.)
         call tdrot(1,35.26439)
      nt=nw
         return
         end
