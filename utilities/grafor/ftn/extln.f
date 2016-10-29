      subroutine extln(x,y,z,g)
      common/gfeps/ eps,lengte
      dimension x(2),y(2),z(2)
C
C ====================================================================
C    extend the line with end points "x(1),y(1),z(1)" and "x(2),y(2),
C    z(2)" at both ends a distance "g" on the picture plane.
C ====================================================================
C
C.....the end point "1" - with greater "y" value.
      am=y(1)-y(2)
      if(abs(am).le.eps) goto 1
      an=(z(1)-z(2))/am
      am=(x(1)-x(2))/am
      a=g/sqrt(1.+am*am)
      v=a*am
      u=a*an
C++   print 100,an,am,a,v,u
C##100   format(' s/p extln: an,am,a,v,u=',5f10.3)
C
C.....the end point "0".
      y(1)=y(1)+a
      x(1)=x(1)+v
      z(1)=z(1)+u
C.....point "3".
      y(2)=y(2)-a
      x(2)=x(2)-v
      z(2)=z(2)-u
      return
C..... case, when "y(1)=y(2)".
1     continue
      u=x(1)
      v=x(2)
      am=x(1)-x(2)
      if(am.ge.0.) goto 2
      x(1)=x(1)-g
      x(2)=x(2)+g
C----- y(1) and y(2) are without any change |
      goto 3
2     continue
      x(1)=x(1)+g
      x(2)=x(2)-g
C----- y(1) and y(2) are without any change |
3     continue
      if(abs(am).le.eps) return
      an=(z(1)-z(2))/am
      z(1)=z(1)+an*(x(1)-u)
      z(2)=z(2)+an*(x(2)-v)
C++   print 101,u,v,am
C##101   format(' unusual s/p *extln*: u,v,am=',3f10.3)
      return
      end
