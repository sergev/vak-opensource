      subroutine fnroot(lx,ly,x,y,z,nroots,root)
C... fnroot 01/09/78
      dimension x(lx),y(ly),z(lx,ly)
      dimension root(1),u(5),v(5),w(5),u1(1),v1(1),w1(1)
      common/drinfn/in,xy,cont,k
      data nd,nh/4,15/
      nroots=0
      ndm1=nd-1
      if(in.eq.0) go to 1
      u1(1)=xy
      do 21 ik=1,nd
21    u(ik)=xy
      v(1)=y(ly)
      hy=(v(1)-y(1))/k
      hdy=hy/ndm1
      do 3 i=1,k
      v(nd)=v(1)
      do 22 ik=1,ndm1
      ik1=nd-ik
22    v(ik1)=v(ik1+1)-hdy
      call itplbv(lx,ly,x,y,z,nd,u,v,w)
      do 23 ik=1,ndm1
      pa=w(ik)-cont
      pb=w(ik+1)-cont
      if(pa*pb) 4,5,23
4     t1=v(ik)
      t2=v(ik+1)
      go to 100
5     if(pa.ne.0.) go to 23
      nroots=nroots+1
      root(nroots)=v(ik)
      go to 23
100    do 111 j=1,nh
      v1(1)=(t1+t2)*0.5
       call itplbv(lx,ly,x,y,z,1,u1,v1,w1)
      p=w1(1)-cont
      if(p*pa)  102,103,104
102   t2=v1(1)
      go to 111
104   t1=v1(1)
111   continue
103   nroots=nroots+1
      root(nroots)=v1(1)
23    continue
3     continue
       return
1     continue
      v1(1)=xy
      u(1)=x(lx)
      hx=(u(1)-x(1))/k
      do 421 ik=1,nd
421   v(ik)=xy
      hdx=hx/ndm1
      do 303 i=1,k
      u(nd)=u(1)
      do 422 ik=1,ndm1
      ik1=nd-ik
422   u(ik1)=u(ik1+1)-hdx
      call itplbv(lx,ly,x,y,z,nd,u,v,w)
      do 423 ik=1,ndm1
      pa=w(ik)-cont
      pb=w(ik+1)-cont
      if(pa*pb) 404,405,423
404   t1=u(ik)
      t2=u(ik+1)
      go to 3100
405   if(pa.ne.0.) go to 423
      nroots=nroots+1
      root(nroots)=u(ik)
      go to 423
3100   do 3111 j=1,nh
      u1(1)=(t1+t2)*0.5
       call itplbv(lx,ly,x,y,z,1,u1,v1,w1)
      p=w1(1)-cont
      if(p*pa) 3102,3103,3104
3102  t2=u1(1)
      go to 3111
3104  t1=u1(1)
3111  continue
3103  nroots=nroots+1
      root(nroots)=u1(1)
423   continue
303   continue
      return
      end
