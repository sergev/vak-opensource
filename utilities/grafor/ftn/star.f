       subroutine star(r,m,alpha,phi)
C    subroutine   star    01/06/75
       common/gfbftr/xtr,ytr
       x0=xtr
       y0=ytr
       th0=180./m
       th1=th0*0.01745329
       if(ivest(alpha,0.,.000001)  ) 3,6,8
  6    th=270.+phi-2*th0
       if(r) 1,3,2
  1    sl=abs(r)
       r1=(sl*cos(th1))/sin(th1)
       call mova(r1,phi,0)
       go to 4
   2   call mova(r,phi,0)
       sl=r*(sin(th1)/cos(th1))
   4   do 7 i=1,m
       call mova(sl,th,1)
       th=th+2*th0
       call mova(sl,th,1)
       th=th-4*th0
  7    continue
       go to 14
   8   betta0=90.-th0-alpha/2
       betta=betta0*0.01745329
       if(betta0.le.0.) go to 30
       th=180.+phi+alpha/2
       th2=phi-2*th0-alpha/2
       if(r) 10,3,11
  10   sl=abs(r)
       r1=(sl*cos(betta))/sin(th1)
       call mova(r1,phi,0)
       go to 12
  11   call mova(r,phi,0)
       sl=(r*sin(th1))/cos(betta)
  12   do 13 j=1,m
       call mova(sl,th,1)
       call mova(sl,th2,1)
       th=th-2*th0
       th2=th2-2*th0
  13   continue
  14   call move(x0,y0,0)
       return
  30   call polg(r,m,phi)
   3   return
       end
