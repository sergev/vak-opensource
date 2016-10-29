      subroutine cross(x,y,j)
C             ha ha eh e  po"pamm :o pe e  t   koop  hat   to k
C              epece eh    byx  "eometp  eck x   emehtob,  h opma    o
C             kotop x  a  caha b   oke
C              opma o pa eh  : call cross(x,y,j)
C             o  cah e  apametpob:
C             x,y - koop  hat  to k   epece eh
C             j - xapaktep ct ka to k   epece eh
C             j=1 -  epeceka8tc  cam    emeht
C             j=2 -  epb     emeht  epecekaet  po o  eh e btopo"o
C             j=3 - btopo    emeht  epecekaet  po o  eh e  epbo"o
C             j=4 -  epeceka8tc   po o  eh     emehtob
      common/gfgel/gf(28)
      dimension n(2)
      n(1)=0
      n(2)=0
      i=1
      if(gf(1)) 28,1,9
  1   if(gf(11)) 28,2,11
C              epece eh e  p m x
  2   d=gf(5)*gf(14)-gf(15)*gf(4)
      if(abs(d).lt.1e-8) go to 23
      gf(19)=(gf(6)*gf(15)-gf(16)*gf(5))/d
      gf(20)=(gf(16)*gf(4)-gf(6)*gf(14))/d
      gf(10)=1.
  3   continue
C              pobepka  p ha  e hoct  to k   epece eh    y"e okpy hoct
C             otpe ky  p mo
      do 7 k=1,2
      if(gf(i).ne.0.) go to 4
      if((gf(i+1)-gf(19))*(gf(19)-gf(i+6)).lt.0.) go to 6
       if((gf(i+2)-gf(20))*(gf(20)-gf(i+7))) 6,5,5
  4   if(gf(i).eq.2.) go to 5
      th=angler(gf(19)-gf(i+1),gf(20)-gf(i+2))
      if((gf(i+6)-th)*(th-gf(i+7)).lt.0.) go to 6
  5   n(k)=1
  6    if(i.gt.10) go to 24
      i=11
  7   continue
  8   return
  9   if(gf(11)) 28,10,18
  10  k=11
      l=1
      go to 12
  11  k=1
      l=11
  12  if(gf(10)) 8,30,13
  13  if(gf(10)-1.) 8,27,14
C              epece eh e okpy hoct  c  p mo
  14  s=sqrt(gf(k+3)**2+gf(k+4)**2)
      if(s.eq.0.) go to 23
      p=(gf(k+3)*gf(l+1)+gf(k+4)*gf(l+2)+gf(k+5))/s
      st=gf(k+3)/s
      ct=-gf(k+4)/s
      r=gf(l+3)
      if(abs(p)-abs(r)) 16,15,23
  15  gf(10)=1.
      gf(19)=-p*st+gf(l+1)
      gf(20)=p*ct+gf(l+2)
       go to 3
  16  gf(10)=2.
      gf(27)=21
      x=sqrt(gf(l+3)**2-p**2)
      gf(19)=x*ct-p*st+gf(l+1)
      gf(20)=x*st+p*ct+gf(l+2)
      gf(21)=-x*ct-p*st+gf(l+1)
      gf(22)=-x*st+p*ct+gf(l+2)
       go to 3
  18  if(gf(10)) 8,30,19
  19  if(gf(10)-1.) 8,27,20
C              epece eh e  byx okpy hocte       x  y"
  20  r1=abs(gf(4))
      r2=abs(gf(14))
      d1=sqrt((gf(12)-gf(2))**2+(gf(13)-gf(3))**2)
      if(d1.eq.0.) go to 23
      if(d1.gt.r1+r2) go to 23
      x1=(r1**2-r2**2+d1**2)/(2*d1)
      st=(gf(13)-gf(3))/d1
      ct=(gf(12)-gf(2))/d1
      h=sqrt(r1**2-x1**2)
      if(h) 8,21,22
  21  gf(10)=1.
      gf(19)=r1*ct+gf(2)
      gf(20)=r1*st+gf(3)
       go to 3
  22  gf(10)=2.
      gf(27)=21
      gf(19)=x1*ct-h*st+gf(2)
      gf(20)=x1*st+h*ct+gf(3)
      gf(21)=x1*ct+h*st+gf(2)
      gf(22)=x1*st-h*ct+gf(3)
       go to 3
  23  j=0
      go to 29
C              opm pobah e j - xapaktep ct k  to k   epece eh
  24  j=4-(2*n(1)+n(2))
      gf(10)=gf(10)-1
      if(gf(10)) 8,26,25
  25  j=-j
  26  x=gf(19)
      y=gf(20)
      return
  27  l=gf(27)
      gf(19)=gf(l)
      gf(20)=gf(l+1)
       go to 3
  28  call gffals(6)
      return
  29  call gffals(5)
      return
  30  call gffals(7)
      return
       end
