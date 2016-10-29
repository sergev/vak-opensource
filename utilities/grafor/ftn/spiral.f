       subroutine spiral(th0,thf,r0,rf)
C      subroutine spiral 01/06/75
       common/gfbftr/xtr,ytr
       x0=xtr
       y0=ytr
       call circle(x0,y0,th0,thf,r0,rf,1)
       return
       end
