      subroutine sdpg(step,eps,beta)
      real jj,kk
      common/gftab/ irdb(5),rdb(17)
      equivalence(a,rdb(5)),(b,rdb(6))
      al=beta*(3.141593/180.)
   18 sinal=sin(al)
      cosal=cos(al)
      if(sinal*cosal.ne.0.) go to 6
      al=al+0.00001
      go to 18
   6   if(sinal.gt.0.) go to 12
      al=al+3.141593
      go to 18
  12  jj=0.
      x1=0.
      x2=a*sinal
      x4=-b*cosal
      x3=x2+x4
      a1=amin1(x1,x2,x3,x4)
      a2=amax1(x1,x2,x3,x4)
      asc=a*sinal/cosal
      bcs=b*cosal/sinal
      j=((a1-eps)/step)
      if((a1-eps)/step.lt.0.) j=j-1
      k=(a2-a1)/step+1
      l=k+1
      do 8 i=1,l
      r=eps+(j+i-1)*step
      rs=r/sinal
      rc=r/cosal
      if(r.lt.a1) go to 8
      if(r-a2.gt.0.00001) go to 8
      if (cosal.le.0.) go to 14
      x1=amax1(0.,rs)
      x2=amin1(a,bcs+rs)
      y1=amax1(0.,-rc)
      y2=amin1(b,asc-rc)
      go to 11
  14  x1=amax1(0.,bcs+rs)
      x2=amin1(a,rs)
      y1=amin1(b,-rc)
      y2=amax1(0.,asc-rc)
  11  kk=1.-jj
      call move(jj*x1+kk*x2,jj*y1+kk*y2,0)
      call move(jj*x2+kk*x1,jj*y2+kk*y1,1)
      jj=1.-jj
   8  continue
      return
      end
