      subroutine crossp(x,y,rc,t)
      common/gfeps/ eps,lengte
      dimension x(2),y(2),rc(2)
C ===================================================================
C    calc. of parameter "t" of the point "rc" on the line with end
C      points "x(1),y(1)" and "x(2),y(2)".
C ===================================================================
      d=x(2)-x(1)
C.....test: is the line vertical
      if(abs(d).le.eps) goto 1
      t=(rc(1)-x(1))/d
      return
1     continue
      t=(rc(2)-y(1))/(y(2)-y(1))
      return
      end
