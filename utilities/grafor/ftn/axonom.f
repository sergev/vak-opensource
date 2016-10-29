       subroutine axonom(x,y,z)
       common/gfmatr/t(32),nt,nt1
       nw=nt
       call hcrot1(x,y,z)
       nt=nw
       return
       end
