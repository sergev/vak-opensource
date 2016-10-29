      subroutine  xysort (x,y,n)
C...subroutine     xysort    01/09/73
C...subroutine     xysort    01/09/78
      dimension x(1),y(1)
      nn=iabs(n)
      n1= nn - 1
      do 30 i= 1,n1
      k= i
      am = x(i)
      i1= i+1
      do 20 j=i1,nn
      if (x(j).ge.am) goto 20
      k = j
      am = x(j)
   20 continue
      x(k)= x(i)
      x(i)= am
      if (n)  30,35,25
   25 am = y(k)
      y(k) = y(i)
      y(i) = am
   30 continue
   35 return
      end
