      subroutine arcela(a,b,alpha,th0,thf)
C     subroutine arcela    01/06/75
      common/gfbftr/xtr,ytr
      x0=xtr
      y0=ytr
      al=abs(a)
      th=abs(th0-thf)
      if(ivest(th,360.,0.000001))   8,7,6
8     if(th0.gt.thf) goto 2
      if(a) 4,6,7
4     th02=th0+360.
      thf2=thf
      go to 3
2     if(a) 7,6,5
5     thf2=thf+360.
      th02=th0
      go to 3
7     th02=th0
      thf2=thf
3     call elips(x0,y0,al,b,alpha,th02,thf2)
      return
6     call gffals(5)
       return
       end
