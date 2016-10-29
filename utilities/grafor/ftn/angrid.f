       subroutine angrid(x0,y0,xs,ys,m1,n1,slope)
       m=m1
       n=n1
       sita=(slope)*3.14159265/180.
       ct=cos(sita)
       st=sin(sita)
   33  xm=x0+float(m)*xs*ct
       ym=y0+float(m)*xs*st
       xn=x0-float(n)*ys*st
       yn=y0+float(n)*ys*ct
       n=n+1
       do 10 i=1,n,2
       k=i-1
       xk=x0-float(k)*ys*st
       yk=y0+float(k)*ys*ct
       xm1=xm-float(k)*ys*st
       ym1=ym+float(k)*ys*ct
       call move(xk,yk,0)
       call move(xm1,ym1,1)
       if(i.eq.n)goto 1000
       xl=x0-float(i)*ys*st
       yl=y0+float(i)*ys*ct
       xm2=xm-float(i)*ys*st
       ym2=ym+float(i)*ys*ct
       call move(xm2,ym2,0)
       call move(xl,yl,1)
  10   continue
1000   m=m+1
       do 20 j=1,m,2
       k=j-1
       xk1=x0+float(k)*xs*ct
       yk1=y0+float(k)*xs*st
       xn1=xn+float(k)*xs*ct
       yn1=yn+float(k)*xs*st
       call move(xk1,yk1,0)
       call move(xn1,yn1,1)
       if(j.eq.m)goto 2000
       xl=x0+float(j)*xs*ct
       yl=y0+float(j)*xs*st
       xn2=xn+float(j)*xs*ct
       yn2=yn+float(j)*xs*st
       call move(xn2,yn2,0)
       call move(xl,yl,1)
   20  continue
 2000  return
       end
