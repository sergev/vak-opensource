      subroutine see(l,ir,ibound,x,y,nb,ind,idrwn,xv,yv)
C...01/09/78
      dimension ibound(2,1),x(1),y(1)
      idrwn=0
      m=0
   12 m=m+1
      do 3 i=1,nb
      i3=iabs(ibound(1,i))
      i4=ibound(2,i)
      if(l.ne.i3) goto 13
      if(ir.ne.i4) goto 13
      if(ibound(1,i).ge.0) goto 14
      idrwn=1
      go to 11
   14 if(m-1) 3,12,3
   13 if(m.eq.1) goto 3
      call see1(l,ir,i3,i4,x,y,xv,yv,ind)
      if(ind.le.0) return
    3 continue
   11 ind=1
      return
      end
