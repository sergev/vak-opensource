      subroutine forif (fun,n,m,a,b,ier)
C...subroutine     forif     01/09/73
C...subroutine     forif     01/09/78
      dimension a(1),b(1)
	external fun
C        check for parametr errors
      ier=0
  20  if(m.ge.0) goto 40
      ier=2
      return
  40  if (m.le.n) goto 60
      ier=1
      return
C        compute and preset constants
  60  an=n
      coef=2.0/(2.0*an+1.0)
      const=3.141593*coef
      s1=sin(const)
      c1=cos(const)
      c=1.0
      s=0.0
      j=1
      funz=fun(0.0)
  70  u2=0.0
      u1=0.0
      ai=2*n
C        form fourier coefficients recursively
  75  x=ai*const
      u0=fun(x)+2.0*c*u1-u2
      u2=u1
      u1=u0
      ai=ai-1.0
      if (ai.gt.0.) goto 75
      a(j)=coef*(funz+c*u1-u2)
      b(j)=coef*s*u1
      if (j.ge.(m+1)) goto 100
      q=c1*c-s1*s
      s=c1*s+s1*c
      c=q
      j=j+1
      go to 70
 100  a(1)=a(1)*0.5
      return
      end
