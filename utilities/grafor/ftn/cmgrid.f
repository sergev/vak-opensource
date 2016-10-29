       subroutine cmgrid(x,y,n1,n2,mx,my,sz,kp,m,n,ikop)
C...subroutine cmgrid 01/09/78
      dimension x(1),y(1)
      pk=57.2958
       h=0.2*sz
      ni=iabs(n1)
      nj=iabs(n2)
      if(n1.le.0) goto 10
      na=1
       ic=-1
       do23 i=1,ni
      xp=x(i)
      yp=y(na)
      if(ikop.ne.1) goto 31
      xp=xpol(x(i),y(na))
      yp=ypol(x(i),y(na))
31    call tmf(xp,yp,xm,ym)
       if(mod(i-1,mx).ne.0) goto 23
      ic=-ic
      na=na-ic
      if(ic.ge.0) goto 8
      tn=float(i-1)
      tp=float(i)
      if(m)4,8,5
4     tn=x(i-1)
      tp=x(i)
5     r=x(i)
      s=y(nj)
      if(ikop.ne.1) goto 33
      r=xpol(x(i),y(nj))
      s=ypol(x(i),y(nj))
33    call tmf(r,s,c,d)
      the=atan2(d-b,c-a)
       dp=h*cos(the)+h*sin(the)
       dm=h*cos(the)-h*sin(the)
      if(mod(i-2,iabs(m)).eq.0)
     *call number(a+dm,b+dp,sz,tn,kp,the*pk)
      if(mod(i-1,iabs(m)).eq.0)
     *call number(c+dm,d+dp,sz,tp,kp,the*pk)
8     call move(xm,ym,0)
      do2 j=1,nj
      na=na+ic
      o=x(i)
      p=y(na)
      if(ikop.ne.1) goto 35
      o=xpol(x(i),y(na))
      p=ypol(x(i),y(na))
35    call tmf(o,p,a,b)
2     call move(a,b,1)
23    continue
10    if(n2.le.0) goto 20
      na=1
       ic=-1
       do24 j=1,nj
      xp=x(na)
      yp=y(j)
      if(ikop.ne.1) goto 37
      xp=xpol(x(na),y(j))
      yp=ypol(x(na),y(j))
37    call tmf(xp,yp,xm,ym)
       if(mod(j-1,my).ne.0) goto 24
      ic=-ic
      na=na-ic
      if(ic.ge.0) goto 18
      tn=float(j-1)
      tp=float(j)
      if(n)14,18,15
14    tn=y(j-1)
      tp=y(j)
15    r=x(ni)
      s=y(j)
      if(ikop.ne.1) goto 39
      r=xpol(x(ni),y(j))
      s=ypol(x(ni),y(j))
39    call tmf(r,s,c,d)
      the=atan2(d-b,c-a)
       dm=h*cos(the)-h*sin(the)
       dp=h*cos(the)+h*sin(the)
      if(mod(j-2,iabs(n)).eq.0)
     *call number(a+dm,b+dp,sz,tn,kp,the*pk)
19    if(mod(j-1,iabs(n)).eq.0)
     *call number(c+dm,d+dp,sz,tp,kp,the*pk)
18    call move(xm,ym,0)
      do 12 i=1,ni
      na=na+ic
      o=x(na)
      p=y(j)
      if(ikop.ne.1) goto 41
      o=xpol(x(na),y(j))
      p=ypol(x(na),y(j))
41    call tmf(o,p,a,b)
12    call move(a,b,1)
24    continue
20    return
      end
