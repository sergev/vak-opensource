       subroutine tdscal(naxes,scale)
C      subroutine tdscal  01.09.78
C  scaling at the direction of axe (if naxe=4 then at all directions)
         common/gfmatr/t(4,4),a(4,4),nt,nt1
      nt=2
       scal1=scale
       if(naxes.ge.4) scal1=1./scale
       call hcunit(a)
       a(naxes,naxes)=scal1
       call hcmult(t,a)
      return
      end
