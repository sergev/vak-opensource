      subroutine suplin(x,y,n,nm,js,l,ikop)
C...subroutine suplin 01/09/78
      dimension x(1),y(1)
      dimension xx(5),yy(5)
      kt=5
      ic=1
      na=mod(n,kt-1)
41    ic=ic-1
      do 42 ik=1,na
      ic=ic+1
      xx(ik)=x(ic)
      yy(ik)=y(ic)
      if(ikop.ne.0) goto 42
      xx(ik)=xpol(x(ic),y(ic))
      yy(ik)=ypol(x(ic),y(ic))
   42 continue
49    if(l.ne.1) goto 46
      call linemc(xx,yy,na,nm,js)
      go to 47
46    if(l.ne.2) goto 48
      call linemo(xx,yy,na,nm,js)
      go to 47
48    call brline(xx,yy,na)
47    na=kt
      if(n.gt.ic) goto 41
      return
      end
