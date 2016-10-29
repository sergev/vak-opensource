      subroutine pnordr(x,y,net,l,ib,ie)
C...01/09/78
      dimension x(1),y(1),net(1)
      i1=ib+1
      ir=iabs(net(ib))
      k1=iabs(net(i1))
      s1=cosin2(y(l)-y(ir),x(ir)-x(l),x(k1)-x(l),y(k1)-y(l))
      c1=cosin2(x(ir)-x(l),y(ir)-y(l),x(k1)-x(l),y(k1)-y(l))
      m=i1+1
      do 1 i=m,ie
      k2=iabs(net(i))
      s2=cosin2(y(l)-y(ir),x(ir)-x(l),x(k2)-x(l),y(k2)-y(l))
      c2=cosin2(x(ir)-x(l),y(ir)-y(l),x(k2)-x(l),y(k2)-y(l))
      if(c1.ge.0) goto 3
      if(c2.ge.0) goto 5
      if(s1-s2) 6,1,1
    6 s1=s2
      c1=c2
      i1=i
      go to 1
    5 if(s2) 1,1,6
    3 if(c2.ge.0) goto 8
      if(s1) 6,1,1
    8 if(s1.ge.0) goto 10
      if(s2.ge.0) goto 6
      if(s1-s2) 1,1,6
   10 if(s2.le.0) goto 1
      if(s1-s2) 1,6,6
    1 continue
      i=net(i1)
      net(i1)=net(ib+1)
      net(ib+1)=i
      return
      end
