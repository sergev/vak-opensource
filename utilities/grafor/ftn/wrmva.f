       subroutine wrmva(dl,th)
C......subroutine 01/06/75
       common/gfgel/gf(28)
       common/gfbftr/xtr,ytr
       th1=th*gf(28)
       x=xtr+dl*cos(th1)
       y=ytr+dl*sin(th1)
       call wrmve(x,y)
       return
       end
