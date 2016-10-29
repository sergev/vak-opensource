      subroutine forit (fnt,n,m,a,b,ier)
C...subroutine     forit     01/ 9/73
C...subroutine     forit     01/09/78
      dimension fnt(1),a(1),b(1)
C        check for parametr errors
      ier=0
      if(m.ge.0) goto 40
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
      fntz=fnt(1)
  70  u2=0.0
      u1=0.0
      i=2*n+1
C        form fourier coefficients recursively
  75  u0=fnt(i)+2.0*c*u1-u2
      u2=u1
      u1=u0
      i=i-1
      if (i.gt.1) goto 75
      a(j)=coef*(fntz+c*u1-u2)
      b(j)=coef*s*u1
      if (j.ge.(m+1)) goto 100
      q=c1*c-s1*s
      s=c1*s+s1*c
      c=q
      j=j+1
      goto 70
  100 a(1)=a(1)*0.5
      return
      end
