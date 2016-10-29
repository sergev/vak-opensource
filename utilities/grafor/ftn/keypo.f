      subroutine keypo (phi,x,y,n1,n2,is1,is2,nm,sz,kp,ikop)
C...subroutine keypo 01/09/78
      dimension phi(n1,n2),x(n1),y(n2)
      do1 i=1,n1,is1
      do1 j=1,n2,is2
      xt=x(i)
      yt=y(j)
      if(ikop.ne.1) goto 5
      xt=xpol(x(i),y(j))
      yt=ypol(x(i),y(j))
5     call tmf(xt,yt,xf,yf)
      call move(xf,yf,0)
      call marker(nm)
      call number(xf+sz/2,yf,sz,phi(i,j),kp,0.)
1     continue
      return
      end
