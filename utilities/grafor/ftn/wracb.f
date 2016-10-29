      subroutine wracb(r,xf,yf,j)
C.....subroutine wracb    01/06/75
      common/gfbftr/xtr,ytr
      delx=xf-xtr
      dely=yf-ytr
      call arcang(r,delx,dely,j,th0,thf)
      call wraca(r,th0,thf)
      return
      end
