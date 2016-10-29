      subroutine trig(x,y,n0,nodes,net0,ibound,net,nt1)
C...01/09/78
      dimension x(1),y(1),net(1),net0(1),ibound(2,1)
     1 , nodes(1),n(50),jp(2)
      do 91 i=1,n0
      k1=iabs(nodes(i))
      k2=iabs(nodes(i+1))-1
      do 93 j=k1,k2
      l=net0(j)
      if(l.lt.0) goto 92
   93 continue
      ibound(1,i)=0
      ibound(2,i)=0
      go to 91
   92 l=iabs(l)
      ibound(1,l)=i
      ibound(2,l)=0
   91 continue
      nt1=0
      l=1
      ir=1
   10 if(ibound(1,l).eq.ir) goto 7
      if(ibound(2,l).eq.ir) goto 7
      do 9 l=1,n0
      if(nodes(l).le.0) goto 9
      ir=ibound(1,l)
      if(ir.gt.0) goto 7
      ir=ibound(2,l)
      if(ir.gt.0) goto 7
    9 continue
      do 124 i=1,n0
  124 nodes(i)=iabs(nodes(i))
      return
    7 iend=0
      jp(1)=l
      jp(2)=ir
      do 125 i1=1,2
      i2=jp(i1)
      k1=iabs(nodes(i2))
      k2=iabs(nodes(i2+1))-1
      do 125 j1=k1,k2
      j2=iabs(net0(j1))
      k3=iabs(nodes(j2))
      k4=iabs(nodes(j2+1))-1
      do 125 i=k3,k4
      j=iabs(net0(i))
      if(j.eq.l) goto 125
      if(j.eq.ir) goto 125
      if(nodes(j).le.0) goto 125
      s=cosin1(y(l)-y(ir),x(ir)-x(l),x(j)-x(l),y(j)-y(l))
      if(s.le.0) goto 125
      if(iend.le.0) goto 126
      do 104 l1=1,iend
      if(j.eq.n(l1)) goto 125
  104 continue
  126 iend=iend+1
      n(iend)=j
  125 continue
      ind=-1
      r0=((x(ir)-x(l))**2+(y(ir)-y(l))**2)/4
      do 14 i=1,iend
      j=n(i)
      s=cosin2(y(l)-y(ir),x(ir)-x(l),x(j)-x(l),y(j)-y(l))
      s1=cosin2(y(ir)-y(j),x(j)-x(ir),x(l)-x(ir),y(l)-y(ir))
      if(s1.gt.s) s=s1
      r1=(x(j)-(x(l)+x(ir))/2)**2+(y(j)-(y(l)+y(ir))/2)**2
      if(r1.lt.r0) n(i)=-n(i)
      if((ind+1).ne.0) goto 22
      r=r1
      k=j
      ind=0
      if(s.ge.0.0025) ind=1
      go to 14
   22 if(ind.ne.0) goto 26
      if(s.le.0.0025) goto 28
      ind=1
   31 k=j
      r=r1
      go to 14
   26 if(s.le.0.0025) goto 14
   28 if(r.gt.r1) goto 31
   14 continue
      do 97 i=1,iend
      if(n(i).gt.0) goto 97
      k2=iabs(n(i))
      r1=cosin1(y(l)-y(k2),x(k2)-x(l),x(k)-x(l),y(k)-y(l))
      if(r1.lt.0) goto 97
      r1=cosin1(y(ir)-y(k),x(k)-x(ir),x(k2)-x(ir),y(k2)-y(ir))
      if(r1.ge.0) k=k2
   97 continue
      i=3*nt1
      net(i+1)=l
      net(i+2)=ir
      net(i+3)=k
      nt1=nt1+1
      l1=l
      ir1=ir
      k1=k
      if(ibound(1,ir1).eq.k1) goto 74
      if(ibound(2,ir1).ne.k1) goto 75
   74 ir1=-ir1
   75 if(ibound(1,k1).eq.l1) goto 77
      if(ibound(2,k1).ne.l1) goto 78
   77 k1=-k1
   78 l1=-l1
      n(1)=k1
      n(2)=l1
      n(3)=ir1
      n(4)=k1
      n(5)=l1
      do 32 i=2,4
      l1=iabs(n(i))
      ir1=iabs(n(i+1))
      k1=iabs(n(i-1))
      if(ibound(1,l1).ne.ir1) goto 33
      ibound(1,l1)=0
      go to 35
   33 if(ibound(2,l1).eq.ir1) ibound(2,l1)=0
   35 if(n(i-1).le.0) goto 41
      if(ibound(1,l1).ne.0) goto 63
      ibound(1,l1)=k1
      go to 41
   63 if(ibound(2,l1).ne.0) goto 65
      ibound(2,l1)=k1
      go to 41
    4 format(28hboundary is very complicated)
   65 write (*,4)
      stop
   41 if(ibound(1,l1).ne.0) goto 32
      if(ibound(2,l1).eq.0) nodes(l1)=-nodes(l1)
   32 continue
      if(ibound(1,k).eq.ir) goto 46
      if(ibound(2,k).eq.ir) goto 46
      ir=k
      go to 10
   46 l=k
      go to 10
      end
