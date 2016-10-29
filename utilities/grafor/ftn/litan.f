      subroutine litan(xt,yt,j)
C...litan      01/06/75   01/01/80
      common/gfbftr/xtr,ytr
      call rdcrc(xc,yc,r)
      r1=abs(r)
      delx=xc-xtr
      dely=yc-ytr
      dl=delx**2+dely**2-r**2
      if(ivest(dl,0.,.000001)  .le.0) goto 4
      sl=sqrt(dl)
      th0=angler(delx,dely)
      z=r1/sl
      alpha=atan(z)/0.01745329
      if(j) 1,6,2
    1 th1=(th0+90.+alpha)*0.01745329
      go to 3
    2 th1=(th0+270.-alpha)*0.01745329
    3 xt=xc+r1*cos(th1)
      yt=yc+r1*sin(th1)
      return
    4 call gffals(5)
    6 return
      end
