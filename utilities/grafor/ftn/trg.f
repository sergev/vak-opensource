      subroutine trg(l,ir,k,kl,kr,net,nodes,ibound,nb,x,y,xv,yv,ind)
C...01/09/78
      dimension net(1),nodes(1),ibound(2,1),x(1),y(1)
      common /gftrg1/job,xloc,yloc,step,dop,itrgl(3)
      idrwnl=1
      idrwnk=0
      idrwnr=0
      r1=cosin1(y(l)-y(ir),x(ir)-x(l),x(l)-xv,y(l)-yv)
      if(r1.lt.0) goto 32
      m=1
      l0=l
      ir0=ir
   25 k1=nodes(l0)
      k2=iabs(nodes(l0+1))-1
      i=k1
   14 m1=net(i)
      if(ir0.eq.iabs(m1)) goto 15
      i=i+1
      go to 14
   15 if(i.ne.k2) goto 17
      k0=net(k1)
      ki0=k1
      go to 18
   17 k0=net(i+1)
      ki0=i+1
   18 j=iabs(k0)
      r1=cosin1(y(j)-y(l0),x(l0)-x(j),x(l0)-xv,y(l0)-yv)
      if(r1.le.0) ki0=-ki0
      if(m.ne.1) goto 24
      k=iabs(k0)
      kl=ki0
      ir0=l0
      l0=k
      m=m+1
      go to 25
   24 kr=ki0
      j=iabs(kl)
      if(kl.lt.0) goto 51
      if(net(j).ge.0) goto 32
   51 j=iabs(kr)
      if(kr.le.0) goto 54
      if(net(j).ge.0) goto 32
   54 if(job.ne.0) goto 56
      callsee(l,ir,ibound,x,y,nb,ind,idrwnl,xv,yv)
      if(ind.le.0) goto 32
   56 if(kl.le.0) goto 58
      call see(k,l,ibound,x,y,nb,ind,idrwnk,xv,yv)
      if(ind.le.0) goto 32
   58 if(kr.le.0) goto 60
      call see(ir,k,ibound,x,y,nb,ind,idrwnr,xv,yv)
      if(ind.le.0) goto 32
   60 if(idrwnl.gt.0) l=-l
      if(idrwnk.gt.0) k=-k
      if(idrwnr.gt.0) ir=-ir
      ind=1
      go to 33
   32 ind=0
   33 return
      end
