      subroutine wraca(r,th0,thf)
C.....subroutine wraca 01/06/75
      common/gfgel/gf(28)
      common/gfbftr/xtr,ytr
      if(abs(r).lt.1.e-6)  goto 3
      gf(10)=100.
      r1=abs(r)
      i=abs(gf(9))
      gf(i)=20.
      gf(i+1)=xtr-r1*cos(th0*0.01745329)
      gf(i+2)=ytr-r1*sin(th0*0.01745329)
      gf(i+3)=r
      gf(i+6)=th0
      gf(i+7)=thf
      if(gf(9).gt.0.) call strmod(0)
      return
 3    call gffals(5)
      return
      end
