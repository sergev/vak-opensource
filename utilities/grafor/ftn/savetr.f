       subroutine savetr(a)
      dimension a(1)
      common/gfmatr/t(16),t1(16),nt,nt1
      common/gftr/n1,n2
      do 1 i=1,16
    1 a(i)=t(i)
      n1=nt
      n2=nt1
      return
      end
