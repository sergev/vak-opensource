       subroutine circ(r)
C     subroutine circ    01/06/75
       common/gfbftr/xtr,ytr
       x0=xtr
       y0=ytr
       call move(x0+r,y0,0)
       call arcia(r,0.,360.)
       call move(x0,y0,0)
       return
       end
