      subroutine  linfil(a,b,n,m)
C...subroutine     linfil    01/09/73
C...subroutine     linfil    01/09/78
      dimension  a(1),b(1)
      mm=m
      nn=n
      i=1
      k=mm+1
      if(nn) 10,70,15
   10 nn=-nn
      mm=mm-1
   15 do 30 j=1,nn
      s=0
      r=k-i+1
      do 20 l=i,k
  20  s=s+a(l)
      if(r.ne.0.) goto 25
      call grafer ('LNFL')
      return
  25  b(j)=s/r
      if (k.lt.nn) k=k+1
      if (j-i.ge.mm) i=i+1
  30  continue
      if(mm.le.1) b(nn)=b(nn-1)*2-b(nn-2)
   70 return
      end
