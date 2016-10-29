       subroutine wrmvb(deltax,deltay)
C...wrmvb      01/06/75   01/01/80
       common/gfbftr/xtr,ytr
       x=xtr+deltax
       y=ytr+deltay
       call wrmve(x,y)
       return
       end
