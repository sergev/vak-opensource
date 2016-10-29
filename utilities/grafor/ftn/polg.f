       subroutine polg(r,m,phi)
C   subroutine  polg     01/06/75
       common/gfbftr/xtr,ytr
       x0=xtr
       y0=ytr
       th1=3.141593/m
       if(r) 1,3,2
  1    sl=abs(r)
       r1=sl/(2*sin(th1))
       call mova(r1,phi,0)
       go to 4
   2   call mova(r,phi,0)
       sl=2*r*sin(th1)
       r1=r
  4    th=90.+180./m+phi
       do 5 i=1,m
       call mova(sl,th,1)
   5   th=th+360./m
       call mova(r1,phi+180.,0)
       call move(x0,y0,0)
  3    return
       end
