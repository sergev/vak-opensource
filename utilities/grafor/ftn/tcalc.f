      subroutine tcalc (x,y,t,n)
C...subroutine     tcalc     01/09/73
C...subroutine     tcalc     01/09/78
      dimension x(n),y(n),t(n)
      t(1)=1.
      t(2)=2.
      dx=x(2)-x(1)
      dy=y(2)-y(1)
      dz=dx*dx+dy*dy
      if (dz.eq.0.) goto 3
      do 2 i=3,n
      dx=x(i)-x(i-1)
      dy=y(i)-y(i-1)
   2  t(i)=t(i-1)+sqrt((dx*dx+dy*dy)/dz)
   3  return
      end
