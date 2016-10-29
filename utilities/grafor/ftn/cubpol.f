      subroutine cubpol (x1,x2,y1,y2,dy1,dy2,b)
C...subroutine     cubpol    01/09/73
      dimension b(4)
      h0=1/(x2-x1)
      h1=h0*h0
      dy=y2-y1
      s1=x2+2*x1
      s2=x1+2*x2
      b(1)=h1*(2*h0*x1*x2*(x1*y2-x2*y1)+y1*x2**2+y2*x1**2-x1*x2*(dy1*x2+
     1dy2*x1))
      b(2)=h1*(-6*h0*x1*x2*dy+dy1*x2*s1+dy2*x1*s2)
      b(3)=3*h0*h1*(dy*(x2+x1))-h1*(dy1*s2+dy2*s1)
      b(4)=h1*(dy2+dy1-2*h0*dy)
      return
      end
