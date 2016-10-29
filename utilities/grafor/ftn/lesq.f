      subroutine lesq (x,y,ro,m,b,n)
C...subroutine     lesq      01/09/73
C...subroutine     lesq      01/09/78
      dimension x(1),y(1),ro(1),a(36),b(6),v(11)
      ma=iabs(m)
      na=iabs(n)
      l=2*na-1
      if (n)  9,17,1
   1  do 2 k=1,l
   2  v(k)=0.
      do 6 j=1,ma
      rx=1.
      if (m.gt.0) rx=ro(j)
      do 6 k=1,l
      if (k.gt.1) rx=rx*x(j)
   6  v(k)=v(k)+rx
      j=1
      do 8 i=1,na
      l=i-j
      jk=j+na-1
      do 7 k=j,jk
   7  a(k)=v(k+l)
   8  j=j+na
      call sminv (a,v,na)
   9  continue
      do 10 k=1,na
  10  v(k)=0.
      do 14 j=1,ma
      rx=1.
      if(m.gt.0) rx=ro(j)
      do 14 k=1,na
      if (k.gt.1) rx=rx*x(j)
  14  v(k)=v(k)+y(j)*rx
      do 15 k=1,na
  15  b(k)=0.
      do 16 i=1,na
      do 16 j=1,na
  16  b(i)=b(i)+a(i+(j-1)*na)*v(j)
  17  return
      end
