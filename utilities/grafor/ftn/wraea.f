      subroutine wraea(a,b,alpha,th0,thf)
C.....subroutine  wraea    01/06/75
      common/gfgel/gf(28)
      common/gfbftr/xtr,ytr
      if(a.eq.0.)goto 3
      gf(10)=100.
      alpha1=alpha*gf(28)
      th01=th0*gf(28)
      thf1=thf*gf(28)
      a1=abs(a)
      d=a1*b/sqrt((a1*sin(th01))**2+(b*cos(th01))**2)
      i=abs(gf(9))
      gf(i)=-20.
      gf(i+1)=xtr-d*cos(th01+alpha1)
      gf(i+2)=ytr-d*sin(th01+alpha1)
      gf(i+3)=a
      gf(i+4)=b
      gf(i+5)=alpha
      gf(i+6)=th0
      gf(i+7)=thf
      if(gf(9).gt.0.)call strmod(0)
      return
 3    call gffals(5)
      return
      end
