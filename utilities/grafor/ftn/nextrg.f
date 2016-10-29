      subroutine nextrg(ibound,x,y,net,nodes,nb,xv,yv)
C...01/09/78
      common /gftrg1/job,xloc,yloc,step,dop,itrgl(3)
      dimension ibound(2,1),x(1),y(1),net(1),nodes(1)
      if(job.eq.0) goto 105
      i=iabs(job)
      if(nodes(i)) 105,105,106
  105 k=iabs(ibound(1,1))
      r=(xv-x(k))**2+(yv-y(k))**2
      do 76 i=1,nb
      l=iabs(ibound(1,i))
      ir=ibound(2,i)
      vx=y(l)-y(ir)
      vy=x(ir)-x(l)
      r1=cosin1(yv-y(ir),x(ir)-xv,vx,vy)
      r2=cosin1(-vy,vx,x(l)-xv,y(l)-yv)
      if(r1*r2.lt.0) goto 73
      r1=cosin1(x(l)-xv,y(l)-yv,vx,vy)**2/(vx**2+vy**2)
      if(r1.ge.r) goto 76
      r=r1
      k=ir
      go to 76
   73 do 75 j=1,2
      k1=iabs(ibound(j,i))
      r1=(xv-x(k1))**2+(yv-y(k1))**2
      if(r1.ge.r) goto75
      r=r1
      k=k1
   75 continue
   76 continue
      job=0
      goto 107
  106 k=iabs(job)
  107 l2=k
      ir1=k
      k1=nodes(k)
      k2=iabs(nodes(k+1))-1
      m=0
      i=k1
   10 l=net(i)
      if(l) 11,7,7
    7 i=i+1
      if(i.gt.k2) i=k1
      go to 10
   11 l1=iabs(l)
   83 if(i.ne.k2) goto 13
      i=k1
      go to 84
   13 i=i+1
   84 if(net(i).eq.0) goto 83
      ir2=net(i)
      r1=cosin1(y(l1)-y(l2),x(l2)-x(l1),x(l1)-xv,y(l1)-yv)
      if(r1.lt.0) goto 15
      r1=cosin1(y(ir1)-y(ir2),x(ir2)-x(ir1),x(ir1)-xv,y(ir1)-yv)
      if(r1) 15,17,17
   15 if(m.ne.0) goto 17
      m=1
      go to 7
   17 l=l1
      ir=l2
      call trg(l,ir,k,kl,kr,net,nodes,ibound,nb,x,y,xv,yv,ind)
      if(ind.ne.0) goto 20
      l=ir1
      ir=ir2
      if(job.le.0) job=0
      i=0
      call trg(l,ir,k,kl,kr,net,nodes,ibound,nb,x,y,xv,yv,ind)
      if(ind.ne.0) goto 20
      k1=nodes(l1)
      k2=iabs(nodes(l1+1))-1
      if(l1.eq.ir2) goto 105
      if(l1.eq.ir1) goto 105
      i=k1
   23 m1=iabs(net(i))
      if(m1.eq.l2) goto 24
      i=i+1
      go to 23
   24 l2=l1
   93 if(i.ne.k1) goto 90
      i=k2
      go to 92
   90 i=i-1
   92 if(net(i).eq.0) goto 93
      l1=iabs(net(i))
      k1=nodes(ir2)
      k2=iabs(nodes(ir2+1))-1
      i=k1
   29 m1=iabs(net(i))
      if(m1.eq.ir1) goto 30
      i=i+1
      go to 29
   30 ir1=ir2
   99 if(i.ne.k2) goto 95
      i=k1
      go to 97
   95 i=i+1
   97 if(net(i).eq.0) goto 99
      ir2=net(i)
      job=0
      go to 17
   20 itrgl(1)=l
      itrgl(2)=ir
      itrgl(3)=k
      l=iabs(l)
      ir=iabs(ir)
      k=iabs(k)
       job=-k
      if(kl.ge.0) goto 102
      if(kr.ge.0) goto 103
      job=k
      go to 103
  102 job=-ir
  103 call ilimtn(l,ir,ibound,nb)
      if(kl.le.0) goto 35
      call ilimtn(k,l,ibound,nb)
      go to 36
   35 j=iabs(kl)
      if(net(j).le.0) goto 36
      nb=nb+1
      ibound(1,nb)=-l
      ibound(2,nb)=k
   36 if(kr.le.0) goto 37
      call ilimtn(ir,k,ibound,nb)
       go to 39
   37 j=iabs(kr)
      if(net(j).lt.0) goto 39
      nb=nb+1
      ibound(1,nb)=-k
      ibound(2,nb)=ir
   39 m=0
   58 k1=nodes(l)
      k2=iabs(nodes(l+1))-1
      i=k1
   42 m1=net(i)
      if(iabs(m1).eq.ir) goto 41
      i=i+1
      go to 42
   41 if(i.ne.k1) goto 43
      m1=net(k2)
      go to 45
   43 m1=net(i-1)
   45 if(m1.gt.0) goto 47
      net(i)=0
      go to 50
   47 net(i)=-net(i)
   50 if(i.ne.k2) goto 48
      i=k1
      go to 51
   48 i=i+1
   51 if(net(i).le.0) net(i)=0
      nodes(l)=-nodes(l)
      do 54 i=k1,k2
      if(net(i).eq.0) goto 54
      nodes(l)=-nodes(l)
      go to 57
   54 continue
   57 m1=l
      l=ir
      ir=k
      k=m1
      m=m+1
      if(m.lt.3) goto 58
      return
      end
