      subroutine trsurf(x,y,z,n0,xv,yv,nodes,net0,
     1  iscr,scrn,idim1,idim2,net,ibound)
C...01/09/78
      common /gftrg1/job,xloc,yloc,step,dop,itrgl(3)
      dimension ibound(2,1),x(1),y(1),z(1),net0(1),net(1),nodes(1),
     1 iscr(1),scrn(idim1,idim2,2)
      job=0
      k=iabs(nodes(n0+1))-1
      do 7 i=1,k
    7 net(i)=net0(i)
      nb=0
      do 11 ir=1,n0
      k1=iabs(nodes(ir))
      k2=iabs(nodes(ir+1))-1
      do 10 j=k1,k2
      l=net(j)
      if(l.ge.0) goto 10
      l=iabs(l)
      r=cosin1(y(ir)-y(l),x(l)-x(ir),x(l)-xv,y(l)-yv)
      if(r.gt.0) goto 11
      nb=nb+1
      ibound(1,nb)=l
      ibound(2,nb)=ir
      go to 11
   10 continue
   11 continue
   12 k=iabs(nodes(n0+1))-1
      call nextrg(ibound,x,y,net,nodes,nb,xv,yv)
      call trdraw(iscr,scrn,idim1,idim2,x,y,z)
      if(nb.gt.0) goto 12
      do 15 i=1,n0
   15 nodes(i)=iabs(nodes(i))
      return
      end
