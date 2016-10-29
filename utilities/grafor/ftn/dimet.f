       subroutine dimet
       common/gfmatr/t(32),nt,nt1
       nw=nt
       nt1=2
       call tdrot(3,-20.705)
       call tdrot(1,22.208)
       nt=nw
       return
       end
