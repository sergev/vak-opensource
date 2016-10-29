      subroutine wrmve(x,y)
C...wrmve      01/06/75   01/01/80
      common/gfgel/gf(28)
      common/gfbftr/xtr,ytr
      equivalence(xtr,x0),(ytr,y0)
      gf(10)=100.
      i=abs(gf(9))
      gf(i)=0.
      gf(i+1)=x0
      gf(i+2)=y0
      det=x*y0-x0*y
      if(abs(det).ge.1e-6) go to 2
      detx=x-x0
      if(abs(detx).ge.1e-6) go to 1
      gf(i+3)=100.
      gf(i+4)=0.
      gf(i+5)=0.
      go to 3
  1   dety=y-y0
      if(abs(dety).ge.1e-6) go to 4
      gf(i+3)=0.
      gf(i+4)=100.
      gf(i+5)=0.
      go to 3
  4   gf(i+3)=100.
      gf(i+4)=-100.*detx/dety
      gf(i+5)=0.
      go to 3
  2   gf(i+3)=(y-y0)*100./det
      gf(i+4)=(x0-x)*100./det
      gf(i+5)=100.
  3   gf(i+6)=x
      gf(i+7)=y
      if(gf(9).gt.0.)call strmod(0)
      return
      end
