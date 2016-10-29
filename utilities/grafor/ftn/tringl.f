      subroutine tringl(x,y,n0,ibound,idom,nodes,net,nt1)
C...01/09/78
      dimension x(1),y(1),net(1),ibound(2,1),n(5),nodes(1)
    4 format(28hboundary is very complicated)
      if(idom.ne.1) goto 90
      loc=0
  124 do 120 i=1,n0
      do 120 j=1,2
      k=ibound(j,i)
      if(k.gt.0) goto 121
  120 continue
      do 127 i=1,n0
      do 127 j=1,2
  127 ibound(j,i)=iabs(ibound(j,i))
   90 nt1=0
      iend=n0
      l=1
      ir=1
      do 51 i=1,n0
   51 nodes(i)=i
   10 if(ibound(1,l).eq.ir) goto 7
      if(ibound(2,l).eq.ir) goto 7
      do 9 i=1,iend
      l=nodes(i)
      ir=ibound(1,l)
      if(ir.ne.0) go to 10
      ir=ibound(2,l)
      if(ir.ne.0) go to 7
    9 continue
    7 ind=-1
      r0=((x(ir)-x(l))**2+(y(ir)-y(l))**2)/4
      do 14 i=1,iend
      if(nodes(i).ne.l) goto 15
      l1=i
      go to 14
   15 if(nodes(i).ne.ir) goto 17
      ir1=i
      go to 14
   17 j=nodes(i)
      s=cosin2(y(l)-y(ir),x(ir)-x(l),x(j)-x(l),y(j)-y(l))
      if(s.le.0) goto 14
      s1=cosin2(y(ir)-y(j),x(j)-x(ir),x(l)-x(ir),y(l)-y(ir))
      if(s1.gt.s) s=s1
      r1=(x(j)-(x(l)+x(ir))/2)**2+(y(j)-(y(l)+y(ir))/2)**2
      if(r1.lt.r0) nodes(i)=-nodes(i)
      if((ind+1).ne.0) goto 22
      r=r1
      k1=i
      ind=0
      if(s.ge.0.04) ind=1
      go to14
   22 if(ind.ne.0) goto 26
      if(s.le.0.04) goto 28
      ind=1
   31 k1=i
      r=r1
      go to14
   28 if(r-r1) 14,14,31
   26 if(s-0.04) 14,14,28
   14 continue
      k=iabs(nodes(k1))
      do 97 i=1,iend
      if(nodes(i).gt.0) goto 97
      k2=iabs(nodes(i))
      r1=cosin1(y(l)-y(k2),x(k2)-x(l),x(k)-x(l),y(k)-y(l))
      if(r1.lt.0) goto 97
      r1=cosin1(y(ir)-y(k),x(k)-x(ir),x(k2)-x(ir),y(k2)-y(ir))
      if(r1.lt.0) goto 97
      k1=i
      k=iabs(nodes(k1))
   97 nodes(i)=iabs(nodes(i))
      if(idom.ne.1) goto 102
      call tmf(x(l),y(l),xl,yl)
      call tmf(x(ir),y(ir),xir,yir)
      call tmf(x(k),y(k),xk,yk)
      if(ibound(1,ir).eq.k) goto 105
      if(ibound(2,ir).eq.k) goto 105
      if(loc.ne.ir) goto 107
      call move(xk,yk,1)
      call move(xl,yl,1)
      loc=l
      go to 102
  107 call move(xl,yl,0)
      call move(xk,yk,1)
      call move(xir,yir,1)
      loc=ir
      go to 102
  105 call move(xl,yl,0)
      call move(xk,yk,1)
      loc=k
  102 n(1)=l1
      n(2)=ir1
      n(3)=k1
      ish=1
      do 85 i=1,3
      do 87 j=1,3
      if(n(j)-i) 87,85,87
   87 continue
      do 88 j=ish,3
      j1=n(j)
      if(j1.le.3) goto 88
      nodes(j1)=nodes(i)
      ish=j+1
      go to 85
   88 continue
   85 continue
      nodes(1)=l
      l1=-1
      nodes(2)=ir
      ir1=2
      nodes(3)=k
      k1=3
      if(ibound(1,ir).eq.k) goto 74
      if(ibound(2,ir).ne.k) goto 75
   74 ir1=-ir1
   75 if(ibound(1,k).eq.l) goto 77
      if(ibound(2,k).ne.l) goto 78
   77 k1=-k1
   78 n(1)=k1
      n(2)=l1
      n(3)=ir1
      n(4)=k1
      n(5)=l1
      do 32 i=2,4
      k1=iabs(n(i))
      l1=nodes(k1)
      k1=iabs(n(i+1))
      ir1=nodes(k1)
      k1=iabs(n(i-1))
      k1=nodes(k1)
      if(ibound(1,l1).ne.ir1) goto 33
      ibound(1,l1)=0
      go to 35
   33 if(ibound(2,l1).eq.ir1) ibound(2,l1)=0
   35 if(n(i-1).le.0) goto 32
      if(ibound(1,l1).ne.0) goto 63
      ibound(1,l1)=k1
      go to 32
   63 if(ibound(2,l1).ne.0) goto 65
      ibound(2,l1)=k1
      go to 32
   65 write (*,4)
      stop
   32 continue
      i=3*nt1
      net(i+1)=l
      net(i+2)=ir
      net(i+3)=k
      nt1=nt1+1
      if(iend.le.3) goto 49
      do 80 i=1,3
      k1=nodes(i)
      if(ibound(1,k1).ne.0) goto 80
      if(ibound(2,k1).ne.0) goto 80
      nodes(i)=nodes(iend)
      iend=iend-1
   80 continue
      if(ibound(1,k).eq.ir) goto 46
      if(ibound(2,k).ne.ir) goto 47
   46 l=k
      go to 48
   47 ir=k
   48 if(iend.gt.0) goto 10
   49 return
  121 call tmf(x(i),y(i),r0,r1)
      call move(r0,r1,0)
  125 call tmf(x(k),y(k),r0,r1)
      ibound(j,i)=-k
      call move(r0,r1,1)
      i=k
      do 122 j=1,2
      if(ibound(j,i).gt.0) goto 123
  122 continue
      go to 124
  123 k=ibound(j,i)
      go to 125
      end
