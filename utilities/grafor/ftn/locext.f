      subroutine locext(phi,x,y,n1,n2,ikop)
C...subroutine locext 01/09/78
      dimension phi(n1,n2),x(n1),y(n2)
      common/gflmn/sz,ks,kp,met
      nb=n2-1
      na=n1-1
      do 8 i=2,na
      do 8 j=2,nb
      p=phi(i,j)
      d1=p-phi(i+1,j)
      d2=p-phi(i,j+1)
      d3=p-phi(i-1,j)
      d4=p-phi(i,j-1)
      d5=p-phi(i-1,j-1)
      d6=p-phi(i-1,j+1)
      d7=p-phi(i+1,j+1)
      d8=p-phi(i+1,j-1)
      if(d1*d5.le.0) goto 8
      if(d1*d6.le.0) goto 8
      if(d1*d7.le.0) goto 8
      if(d1*d8.le.0) goto 8
      if(d1*d2.le.0) goto 8
      if(d1*d3.le.0) goto 8
      if(d1*d4.le.0) goto 8
      jm=-3
      if(d1)4,8,5
4     jm=-14
5     xf=x(i)
      yf=y(j)
      if(ikop.ne.1) goto 52
      xf=xpol(x(i),y(j))
      yf=ypol(x(i),y(j))
52    call tmf(xf,yf,xx,yy)
      call move(xx,yy,0)
      call marker(jm)
      if(met.eq.999) goto 7
      sz=0.3
      kp=1
7     xx=xx+sz/2
       call number(xx,yy,sz,p,kp,0.)
8     continue
      return
      end
