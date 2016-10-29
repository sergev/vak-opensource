      subroutine spline (x,y,u,n,a,b,c,d,kode,ier)
C...subroutine     spline    01/09/73
C...subroutine     spline    01/09/78
      dimension x(n),y(n),u(n),a(n),b(n),c(n),d(n)
      a(2)=x(2)-x(1)
      if (kode.eq.2) goto 4
      call tdmp (x,y,n,a,b,c,d,kode)
      call tridig (u,n,a,b,c,d,kode,ier)
      if (ier.eq.0) goto 4
      call grafer ('SPLN')
      return
   4  do 5 i=2,n
      e=x(i)*y(i-1)
      f=x(i-1)*y(i)
      g=x(i-1)*x(i)
      r=u(i-1)*x(i)
      s=u(i)*x(i-1)
      h1=2./(x(i)-x(i-1))
      h2=h1*h1/4.
      d(i-1)=h2*(h1*g*(f-e)+e*x(i)+f*x(i-1)-g*(r+s))
      e=h1*(y(i)-y(i-1))
      f=x(i-1)+2.*x(i)
      h1=x(i)+2.*x(i-1)
      c(i-1)=h2*(-3.*e*g+r*h1+s*f)
      b(i-1)=1.5*h2*e*(f-x(i))-h2*(u(i-1)*f+u(i)*h1)
   5  a(i-1)=h2*(u(i)+u(i-1)-e)
      return
      end
