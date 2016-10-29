       subroutine arcic(xm,ym,xf,yf,j)
C      subroutine arcic  01/06/75
       common/gfbftr/xtr,ytr
       delx=xf-xtr
       dely=yf-ytr
       call arcc1(xm,ym,xf,yf,j,r,jb)
       call arcang(r,delx,dely,jb,th0,thf)
       call arcia(r,th0,thf)
       return
       end
