      subroutine chensp(ym,n,yh,k,c,m)
C...subroutine     chensp    01/08/73
C...subroutine     chensp    01/09/78
      dimension ym(n),yh(k),c(m)
      if (m.le.0) goto 58
      if (n.le.0) goto 23
      pi1=3.141592/n
      do 11 i=1,m
      ir=i-1
      alamb=0.
      r=0.
      do 10 j=1,n
      x=cos(pi1*(n-j+1.))
      y=1.
      if(ir.le.0) goto 101
      h1=y
      y=x
      if(ir.le.1) goto 101
      h2=y
      f=h2+h2
      do 4 kot=2,ir
      h3=f*h2-h1
      h1=h2
    4 h2=h3
      y=h2
  101 alamb = alamb + y*y
   10 r=r+ym(j)*y
      if (alamb.le.0.) goto 58
   11 c(i)=r/alamb
   23  if(k.le.0) goto 58
      pi1=3.141592/k
      do 51 i=1,k
      if(m.ge.2) goto 8
       yh(i)=c(1)
      go to 51
    8  x=cos(pi1*(k-i+1.))
       f=x+x
       h2=0.
       h1=0.
      do 9 j=1,m
       kot=m-j
       h3=h2
       h2=h1
    9  h1=f*h2-h3+c(kot+1)
       yh(i)=0.5*(c(1)-h3+h1)
   51 continue
   58 return
      end
