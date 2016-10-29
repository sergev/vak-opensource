      subroutine secant(sl,alpha,x,y)
C.....subroutine secant  01/06/75    01/01/80
      common/gfgel/gf(28)
      common/gfbftr/xtr,ytr
      dimension store1(28)
      equivalence (xtr,xa),(ytr,ya)
      call rdmva(x0,y0,dl,th0)
      th=th0+alpha
      th1=th*gf(28)
      if(ivest(sl,0.,0.000001))   8,6,5
   5  x=xa+sl*cos(th1)
      y=ya+sl*sin(th1)
      return
   6  call wrstr(store1)
      gf(9)=abs(gf(9))
      call wrmva(10.,th)
      d=gf(5)*gf(14)-gf(15)*gf(4)
      if(d.eq.0.) goto 7
      x=(gf(6)*gf(15)-gf(16)*gf(5))/d
      y=(gf(16)*gf(4)-gf(6)*gf(14))/d
   9  call rdstr(store1)
   8  return
   7  call gffals(5)
      goto 9
      end
