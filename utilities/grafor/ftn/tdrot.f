       subroutine tdrot(naxes,alpha)
C      subroutine tdrot  01.09.78
       common/gfmatr/t(4,4),a(4,4),nt,nt1
      dimension i(4)
      data i/2,3,1,2/
      nt=2
      naxe=iabs(naxes)
      alrad=alpha
      if (naxes.gt.0)alrad=3.141593/180.*alpha
      call hcunit(a)
      k=i(naxe)
      l=i(naxe+1)
      a(k,k)=cos(alrad)
      a(l,l)=cos(alrad)
      a(k,l)=sin(alrad)
      a(l,k)=-sin(alrad)
      call hcmult(t,a)
       return
       end
