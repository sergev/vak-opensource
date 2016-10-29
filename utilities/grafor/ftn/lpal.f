       subroutine lpal(p,a,l1,l)
       real p(2),l1(3),l(3)
      th1=180./3.141593*atan2(l1(1),l1(2))
      th=a+th1
      call lpax(p,th,l)
      return
      end
