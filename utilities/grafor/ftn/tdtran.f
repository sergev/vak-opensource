       subroutine tdtran(dx,dy,dz)
       common/gfmatr/t(4,4),a(4,4),nt,nt1
       nt=2
       call hcunit(a)
       a(4,1)=dx
       a(4,2)=dy
       a(4,3)=dz
       call hcmult(t,a)
       return
       end
