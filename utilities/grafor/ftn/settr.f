         subroutine settr(a)
      dimension a(1)
      common/gfmatr/t(16),t1(16),nt,nt1
      common/gftr/n1,n2
      do 1 i=1,16
    1 t(i)=a(i)
      nt=n1
      nt1=n2
      return
      end
