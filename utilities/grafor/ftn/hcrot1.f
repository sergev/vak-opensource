      subroutine hcrot1(x,y,z)
      pi=3.141593
      pi2=pi/2.
      r=sqrt(x**2+y**2)
      ax=atan2(abs(z),r)
      az=atan2(abs(x),abs(y))
      if(y.ne.0.) go to 2
      if(x.ne.0.) go to 1
      if(z.eq.0.) return
      ax=pi2
      az=0.
      go to 5
  1   az=pi2
  2   if(x.gt.0.) go to 3
      if(y.le.0.) go to 5
      if(x.eq.0.) go to 4
      az=pi-az
      go to 5
  3   if(y.ge.0.) go to 4
      az=-az
      go to 5
  4   az=az+pi
  5   if(z.lt.0.) ax=-ax
      call tdrot(-3,az)
      call tdrot(-1,ax)
      return
      end
